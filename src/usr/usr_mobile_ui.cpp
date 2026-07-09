#include "usr_mobile_ui.h"

#include "usr_mobile_ui_page.h"
#include "usr_quad_tuning.h"

#include "../madflight_modules.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#if defined(ARDUINO_ARCH_ESP32)
  #include <ESPmDNS.h>
  #include <NetBIOS.h>
  #include <Preferences.h>
  #include <WebServer.h>
  #include <WiFi.h>
  #include <WiFiUdp.h>
#endif

#if !defined(ARDUINO_ARCH_ESP32)

void usr_mobile_ui_preinit() {}
void usr_mobile_ui_begin() {}
void usr_mobile_ui_loop() {}
const char *usr_mobile_ui_host() { return "madflight.local"; }
bool usr_mobile_ui_control_snapshot(UsrMobileUiControlState *state) {
  if(state) *state = {};
  return false;
}

#else

namespace {

constexpr char kUsrHostName[] = "madflight";
constexpr char kUsrUiPrefsNs[] = "usr-ui";
constexpr uint32_t kUsrWifiRetryMs = 10000;
constexpr uint32_t kUsrControlTimeoutMs = 450;
constexpr uint16_t kUsrControlUdpPort = 4210;
constexpr uint8_t kUsrControlUdpDrainMax = 6;
constexpr size_t kUsrControlUdpPacketMax = 160;
constexpr uint32_t kUsrUdpDisarmDebounceMs = 80;
constexpr uint8_t kUsrMaxFlightmode = 2;
constexpr uint32_t kUsrWaveSampleIntervalMs = 25;
constexpr uint16_t kUsrWaveSampleCount = 96;
constexpr char kUsrRequiredUiVersion[] = "0528-footer-throttle-v1";
constexpr char kUsrHotspotDomainNote[] = "Phone hotspot mode often does not resolve .local names. Use the aircraft IP address first.";

enum UsrControlSource : uint8_t {
  USR_CONTROL_SOURCE_NONE = 0,
  USR_CONTROL_SOURCE_HTTP = 1,
  USR_CONTROL_SOURCE_UDP = 2,
};

struct WaveSample {
  float roll;
  float pitch;
  float yaw;
  float gx;
  float gy;
  float gz;
  float throttle;
};

struct RawUsrControlState {
  bool has_input = false;
  bool armed = false;
  uint8_t flightmode = 0;
  float throttle = 0;
  float vspeed = 0;
  float roll = 0;
  float pitch = 0;
  float yaw = 0;
  uint32_t updated_ms = 0;
  uint32_t client_id = 0;
  uint32_t seq = 0;
  uint8_t source = USR_CONTROL_SOURCE_NONE;
};

struct EffectiveFlightInput {
  bool using_ui = false;
  bool ui_connected = false;
  bool rc_connected = false;
  bool connected = false;
  bool armed = false;
  uint8_t flightmode = 0;
  float throttle = 0;
  float vspeed = 0;
  float roll = 0;
  float pitch = 0;
  float yaw = 0;
  uint8_t source = USR_CONTROL_SOURCE_NONE;
};

Preferences g_usr_ui_prefs;
WebServer g_usr_ui_server(80);
WiFiUDP g_usr_control_udp;
portMUX_TYPE g_usr_control_mux = portMUX_INITIALIZER_UNLOCKED;

String g_usr_wifi_ssid;
String g_usr_wifi_password;

bool g_usr_routes_ready = false;
bool g_usr_server_started = false;
bool g_usr_udp_started = false;
bool g_usr_mdns_started = false;
bool g_usr_nbns_started = false;

uint32_t g_usr_last_wifi_retry_ms = 0;
wl_status_t g_usr_last_wifi_status = WL_IDLE_STATUS;

WaveSample g_usr_wave_buffer[kUsrWaveSampleCount] = {};
uint16_t g_usr_wave_head = 0;
uint16_t g_usr_wave_count = 0;
uint32_t g_usr_last_wave_sample_ms = 0;
RawUsrControlState g_usr_control_state = {};
bool g_usr_udp_rearm_blocked = false;
bool g_usr_udp_disarm_pending = false;
uint32_t g_usr_udp_disarm_pending_ms = 0;
uint32_t g_usr_udp_disarm_pending_client_id = 0;
uint32_t g_usr_udp_disarm_pending_seq = 0;

const char *control_source_text(uint8_t source) {
  switch(source) {
    case USR_CONTROL_SOURCE_HTTP: return "ui";
    case USR_CONTROL_SOURCE_UDP: return "udp";
    default: return "none";
  }
}

void ui_log_println(const String &line) {
  Serial.println(line);
}

void ui_log_printf(const char *fmt, ...) {
  char buffer[256];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);
  Serial.print(buffer);
}

String masked_secret(const String &secret) {
  if(secret.isEmpty()) return "";

  if(secret.length() <= 2) {
    String masked;
    masked.reserve(secret.length());
    for(size_t i = 0; i < secret.length(); i++) masked += '*';
    return masked;
  }

  String out;
  out.reserve(secret.length());
  out += secret[0];
  for(size_t i = 1; i + 1 < secret.length(); i++) out += '*';
  out += secret[secret.length() - 1];
  return out;
}

String json_escape(const String &input) {
  String out;
  out.reserve(input.length() + 8);
  for(size_t i = 0; i < input.length(); i++) {
    const char c = input[i];
    switch(c) {
      case '\\': out += "\\\\"; break;
      case '"': out += "\\\""; break;
      case '\n': out += "\\n"; break;
      case '\r': out += "\\r"; break;
      case '\t': out += "\\t"; break;
      default: out += c; break;
    }
  }
  return out;
}

String wifi_status_text(wl_status_t status) {
  switch(status) {
    case WL_CONNECTED: return "connected";
    case WL_NO_SSID_AVAIL: return "ssid_not_found";
    case WL_CONNECT_FAILED: return "connect_failed";
    case WL_CONNECTION_LOST: return "connection_lost";
    case WL_DISCONNECTED: return "disconnected";
    case WL_IDLE_STATUS: return "idle";
    default: return "unknown";
  }
}

float clampf(float value, float min_value, float max_value) {
  if(value < min_value) return min_value;
  if(value > max_value) return max_value;
  return value;
}

float ui_collective_from_vspeed(float vspeed) {
  const float hover = clampf(usr_quad_tuning.ui_hover_throttle, 0.05f, 0.90f);
  const float span = clampf(usr_quad_tuning.ui_vspeed_throttle_span, 0.02f, 0.90f);
  return clampf(hover + clampf(vspeed, -1.0f, 1.0f) * span, 0.0f, 1.0f);
}

bool parse_bool_arg(const String &value) {
  return value == "1" || value == "true" || value == "on" || value == "yes";
}

bool parse_bool_token(const char *value) {
  if(!value) return false;
  return strcmp(value, "1") == 0 || strcmp(value, "true") == 0 || strcmp(value, "on") == 0 || strcmp(value, "yes") == 0;
}

char *trim_token(char *value) {
  if(!value) return value;
  while(*value == ' ' || *value == '\t' || *value == '\r' || *value == '\n') value++;

  char *end = value + strlen(value);
  while(end > value) {
    char c = *(end - 1);
    if(c != ' ' && c != '\t' && c != '\r' && c != '\n') break;
    end--;
  }
  *end = '\0';
  return value;
}

bool parse_uint32_token(const char *value, uint32_t *out) {
  if(!value || !*value || !out) return false;
  char *end = nullptr;
  unsigned long parsed = strtoul(value, &end, 10);
  if(end == value || *end != '\0') return false;
  *out = (uint32_t)parsed;
  return true;
}

bool parse_float_token(const char *value, float *out) {
  if(!value || !*value || !out) return false;
  char *end = nullptr;
  float parsed = strtof(value, &end);
  if(end == value || *end != '\0') return false;
  *out = parsed;
  return true;
}

bool parse_bool_control_token(const char *value, bool *out) {
  if(!value || !out) return false;
  if(parse_bool_token(value)) {
    *out = true;
    return true;
  }
  if(strcmp(value, "0") == 0 || strcmp(value, "false") == 0 || strcmp(value, "off") == 0 || strcmp(value, "no") == 0) {
    *out = false;
    return true;
  }
  return false;
}

bool apply_control_state(
  uint8_t source,
  uint32_t client_id,
  uint32_t seq,
  bool armed,
  uint8_t flightmode,
  float throttle,
  float vspeed,
  float roll,
  float pitch,
  float yaw,
  RawUsrControlState *current
) {
  const uint32_t now = millis();
  bool accepted = false;

  portENTER_CRITICAL(&g_usr_control_mux);
  const bool timed_out = !g_usr_control_state.has_input || (uint32_t)(now - g_usr_control_state.updated_ms) > kUsrControlTimeoutMs;
  const bool udp_active = !timed_out && g_usr_control_state.source == USR_CONTROL_SOURCE_UDP;
  const bool blocked_by_udp = source == USR_CONTROL_SOURCE_HTTP && udp_active && armed;
  const bool blocked_udp_rearm = source == USR_CONTROL_SOURCE_UDP && g_usr_udp_rearm_blocked && armed;
  const bool source_changed = timed_out || source != g_usr_control_state.source || client_id != g_usr_control_state.client_id;
  const bool can_update_state = !blocked_by_udp && !blocked_udp_rearm && (source_changed || seq >= g_usr_control_state.seq);

  const bool debounce_udp_disarm =
    can_update_state
    && source == USR_CONTROL_SOURCE_UDP
    && !armed
    && !timed_out
    && g_usr_control_state.source == USR_CONTROL_SOURCE_UDP
    && !source_changed
    && g_usr_control_state.armed;
  if(debounce_udp_disarm) {
    const bool same_pending =
      g_usr_udp_disarm_pending
      && g_usr_udp_disarm_pending_client_id == client_id;
    const bool confirmed =
      same_pending
      && (seq > g_usr_udp_disarm_pending_seq
          || (uint32_t)(now - g_usr_udp_disarm_pending_ms) >= kUsrUdpDisarmDebounceMs);
    if(!confirmed) {
      g_usr_udp_disarm_pending = true;
      g_usr_udp_disarm_pending_ms = now;
      g_usr_udp_disarm_pending_client_id = client_id;
      g_usr_udp_disarm_pending_seq = seq;
      if(current) *current = g_usr_control_state;
      portEXIT_CRITICAL(&g_usr_control_mux);
      return false;
    }
  } else if(source != USR_CONTROL_SOURCE_UDP || armed) {
    g_usr_udp_disarm_pending = false;
  }

  if(can_update_state) {
    g_usr_control_state.has_input = true;
    g_usr_control_state.armed = armed;
    g_usr_control_state.flightmode = flightmode;
    g_usr_control_state.throttle = throttle;
    g_usr_control_state.vspeed = vspeed;
    g_usr_control_state.roll = roll;
    g_usr_control_state.pitch = pitch;
    g_usr_control_state.yaw = yaw;
    g_usr_control_state.updated_ms = now;
    g_usr_control_state.client_id = client_id;
    g_usr_control_state.seq = seq;
    g_usr_control_state.source = source;
    if(source == USR_CONTROL_SOURCE_HTTP && !armed) g_usr_udp_rearm_blocked = true;
    if(source == USR_CONTROL_SOURCE_HTTP && armed) g_usr_udp_rearm_blocked = false;
    if(source == USR_CONTROL_SOURCE_UDP && !armed) g_usr_udp_rearm_blocked = false;
    if(source == USR_CONTROL_SOURCE_UDP) g_usr_udp_disarm_pending = false;
    accepted = true;
  }

  if(current) *current = g_usr_control_state;
  portEXIT_CRITICAL(&g_usr_control_mux);
  return accepted;
}

void apply_due_udp_disarm(uint32_t now) {
  if(!g_usr_udp_disarm_pending) return;
  if(!g_usr_control_state.has_input
      || g_usr_control_state.source != USR_CONTROL_SOURCE_UDP
      || !g_usr_control_state.armed
      || g_usr_control_state.client_id != g_usr_udp_disarm_pending_client_id
      || g_usr_control_state.seq > g_usr_udp_disarm_pending_seq) {
    g_usr_udp_disarm_pending = false;
    return;
  }
  if((uint32_t)(now - g_usr_udp_disarm_pending_ms) < kUsrUdpDisarmDebounceMs) return;

  g_usr_control_state.armed = false;
  g_usr_control_state.seq = g_usr_udp_disarm_pending_seq;
  g_usr_control_state.updated_ms = now;
  g_usr_udp_rearm_blocked = false;
  g_usr_udp_disarm_pending = false;
}

bool fill_control_snapshot(UsrMobileUiControlState *state) {
  if(!state) return false;

  const uint32_t now = millis();
  RawUsrControlState raw;
  portENTER_CRITICAL(&g_usr_control_mux);
  apply_due_udp_disarm(now);
  raw = g_usr_control_state;
  portEXIT_CRITICAL(&g_usr_control_mux);

  const uint32_t age_ms = raw.has_input ? (uint32_t)(now - raw.updated_ms) : UINT32_MAX;
  const bool connected = raw.has_input && age_ms <= kUsrControlTimeoutMs;

  state->connected = connected;
  state->age_ms = connected ? age_ms : kUsrControlTimeoutMs + 1;
  state->armed = connected ? raw.armed : false;
  state->flightmode = connected ? raw.flightmode : 0;
  state->throttle = connected ? raw.throttle : 0.0f;
  state->vspeed = connected ? raw.vspeed : 0.0f;
  state->roll = connected ? raw.roll : 0.0f;
  state->pitch = connected ? raw.pitch : 0.0f;
  state->yaw = connected ? raw.yaw : 0.0f;
  state->source = connected ? raw.source : USR_CONTROL_SOURCE_NONE;
  return connected;
}

EffectiveFlightInput current_effective_flight_input() {
  EffectiveFlightInput input;
  UsrMobileUiControlState ui = {};
  input.ui_connected = fill_control_snapshot(&ui);

  if(input.ui_connected) {
    input.using_ui = true;
    input.connected = true;
    input.armed = ui.armed;
    input.flightmode = ui.flightmode;
    input.throttle = ui.throttle;
    input.vspeed = ui.vspeed;
    input.roll = ui.roll;
    input.pitch = ui.pitch;
    input.yaw = ui.yaw;
    input.source = ui.source;
    return input;
  }

  return input;
}

const char *alt_estimator_text() {
  #if ALT_USE == ALT_USE_KALMAN3
    return "KALMAN3";
  #elif ALT_USE == ALT_USE_KALMAN2
    return "KALMAN2";
  #elif ALT_USE == ALT_USE_COMP
    return "COMP";
  #elif ALT_USE == ALT_USE_BAR
    return "BAR";
  #else
    return "NONE";
  #endif
}

String current_ip_text() {
  return WiFi.status() == WL_CONNECTED ? WiFi.localIP().toString() : String("0.0.0.0");
}

String current_preferred_url() {
  return WiFi.status() == WL_CONNECTED ? String("http://") + current_ip_text() + "/" : String("http://") + kUsrHostName + ".local/";
}

void load_wifi_config() {
  if(!g_usr_ui_prefs.begin(kUsrUiPrefsNs, true)) return;
  g_usr_wifi_ssid = g_usr_ui_prefs.getString("ssid", "");
  g_usr_wifi_password = g_usr_ui_prefs.getString("pass", "");
  g_usr_ui_prefs.end();
}

void save_wifi_config() {
  if(!g_usr_ui_prefs.begin(kUsrUiPrefsNs, false)) return;
  g_usr_ui_prefs.putString("ssid", g_usr_wifi_ssid);
  g_usr_ui_prefs.putString("pass", g_usr_wifi_password);
  g_usr_ui_prefs.end();
}

void clear_wifi_config() {
  if(!g_usr_ui_prefs.begin(kUsrUiPrefsNs, false)) return;
  g_usr_ui_prefs.remove("ssid");
  g_usr_ui_prefs.remove("pass");
  g_usr_ui_prefs.end();
  g_usr_wifi_ssid = "";
  g_usr_wifi_password = "";
}

void print_wifi_help() {
  ui_log_println("[USR:UI] USB serial WiFi commands:");
  ui_log_println("[USR:UI]   wifi show");
  ui_log_println("[USR:UI]   wifi set \"Your Hotspot\" \"password\"");
  ui_log_println("[USR:UI]   wifi reconnect");
  ui_log_println("[USR:UI]   wifi clear");
  ui_log_printf("[USR:UI] Fixed URL: http://%s.local/\n", kUsrHostName);
  ui_log_println("[USR:UI] Hotspot note: phone hotspots often cannot resolve .local, so use the printed IP address first.");
}

void print_wifi_status() {
  const String masked_password = g_usr_wifi_password.length() ? masked_secret(g_usr_wifi_password) : String("<not-set>");
  const String ip = current_ip_text();

  ui_log_printf(
    "[USR:UI] SSID:%s PASS:%s HOST:%s.local WIFI:%s IP:%s\n",
    g_usr_wifi_ssid.length() ? g_usr_wifi_ssid.c_str() : "<not-set>",
    masked_password.c_str(),
    kUsrHostName,
    wifi_status_text(WiFi.status()).c_str(),
    ip.c_str()
  );
}

int tokenize_quoted(const String &line, String out_tokens[], int max_tokens) {
  bool in_quote = false;
  char quote_char = 0;
  String token;
  int count = 0;

  for(size_t i = 0; i < line.length(); i++) {
    const char c = line[i];

    if(in_quote) {
      if(c == quote_char) {
        in_quote = false;
      } else {
        token += c;
      }
      continue;
    }

    if(c == '"' || c == '\'') {
      in_quote = true;
      quote_char = c;
      continue;
    }

    if(c == ' ' || c == '\t') {
      if(token.length() > 0 && count < max_tokens) {
        out_tokens[count++] = token;
        token = "";
      }
      continue;
    }

    token += c;
  }

  if(token.length() > 0 && count < max_tokens) {
    out_tokens[count++] = token;
  }

  return count;
}

void begin_wifi_connect(const char *reason) {
  if(g_usr_wifi_ssid.isEmpty()) {
    ui_log_println("[USR:UI] No WiFi config stored yet. Use `wifi set \"SSID\" \"PASSWORD\"`.");
    return;
  }

  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.setAutoReconnect(true);
  WiFi.setHostname(kUsrHostName);
  WiFi.begin(g_usr_wifi_ssid.c_str(), g_usr_wifi_password.c_str());
  g_usr_last_wifi_retry_ms = millis();

  ui_log_printf(
    "[USR:UI] WiFi connect (%s) -> SSID:%s HOST:%s.local\n",
    reason,
    g_usr_wifi_ssid.c_str(),
    kUsrHostName
  );
}

void handle_wifi_command_line(const String &line) {
  String tokens[6];
  int token_count = tokenize_quoted(line, tokens, 6);
  if(token_count <= 0) return;

  String command = tokens[0];
  command.toLowerCase();

  if(command == "help") {
    print_wifi_help();
    return;
  }

  if(command != "wifi") {
    ui_log_printf("[USR:UI] Unknown command: %s\n", line.c_str());
    print_wifi_help();
    return;
  }

  if(token_count == 1 || tokens[1] == "show") {
    print_wifi_status();
    return;
  }

  String sub = tokens[1];
  sub.toLowerCase();

  if(sub == "help") {
    print_wifi_help();
    return;
  }

  if(sub == "set") {
    if(token_count < 4) {
      ui_log_println("[USR:UI] Usage: wifi set \"SSID\" \"PASSWORD\"");
      return;
    }
    g_usr_wifi_ssid = tokens[2];
    g_usr_wifi_password = tokens[3];
    save_wifi_config();
    ui_log_println("[USR:UI] WiFi credentials saved.");
    begin_wifi_connect("usb-cli-set");
    return;
  }

  if(sub == "reconnect") {
    begin_wifi_connect("usb-cli-reconnect");
    return;
  }

  if(sub == "clear") {
    WiFi.disconnect(true);
    clear_wifi_config();
    ui_log_println("[USR:UI] WiFi credentials cleared.");
    return;
  }

  ui_log_printf("[USR:UI] Unknown wifi command: %s\n", sub.c_str());
  print_wifi_help();
}

bool handle_wifi_cli_command(String arg1, String arg2) {
  String line = "wifi";
  arg1.trim();
  arg2.trim();
  if(arg1.length()) line += " " + arg1;
  if(arg2.length()) line += " " + arg2;
  handle_wifi_command_line(line);
  return true;
}

void capture_wave_sample() {
  uint32_t now = millis();
  if(now - g_usr_last_wave_sample_ms < kUsrWaveSampleIntervalMs) return;
  g_usr_last_wave_sample_ms = now;
  EffectiveFlightInput input = current_effective_flight_input();

  WaveSample &sample = g_usr_wave_buffer[g_usr_wave_head];
  sample.roll = ahr.roll;
  sample.pitch = ahr.pitch;
  sample.yaw = ahr.yaw;
  sample.gx = imu.gx;
  sample.gy = imu.gy;
  sample.gz = imu.gz;
  sample.throttle = input.throttle;

  g_usr_wave_head = (g_usr_wave_head + 1) % kUsrWaveSampleCount;
  if(g_usr_wave_count < kUsrWaveSampleCount) g_usr_wave_count++;
}

void send_json(const String &json) {
  g_usr_ui_server.sendHeader("Cache-Control", "no-store");
  g_usr_ui_server.sendHeader("Pragma", "no-cache");
  g_usr_ui_server.sendHeader("Expires", "0");
  g_usr_ui_server.send(200, "application/json; charset=utf-8", json);
}

void handle_root() {
  g_usr_ui_server.sendHeader("Cache-Control", "no-store");
  g_usr_ui_server.sendHeader("Pragma", "no-cache");
  g_usr_ui_server.sendHeader("Expires", "0");
  g_usr_ui_server.send_P(200, "text/html; charset=utf-8", USR_MOBILE_UI_PAGE);
}

void handle_overview() {
  String json;
  json.reserve(4096);
  EffectiveFlightInput input = current_effective_flight_input();
  UsrMobileUiControlState ui = {};
  fill_control_snapshot(&ui);

  const bool wifi_connected = (WiFi.status() == WL_CONNECTED);
  const String ip = current_ip_text();
  const String hostname_mdns = String(kUsrHostName) + ".local";
  const String hostname_nbns = String(kUsrHostName);
  const String preferred_url = wifi_connected ? String("http://") + ip + "/" : String("http://") + hostname_mdns + "/";

  json += "{";
  json += "\"hostname\":\"" + json_escape(hostname_mdns) + "\",";
  json += "\"hostname_mdns\":\"" + json_escape(hostname_mdns) + "\",";
  json += "\"hostname_nbns\":\"" + json_escape(hostname_nbns) + "\",";
  json += "\"ip\":\"" + json_escape(ip) + "\",";
  json += "\"preferred_url\":\"" + json_escape(preferred_url) + "\",";
  json += "\"hotspot_note\":\"" + json_escape(String(kUsrHotspotDomainNote)) + "\",";
  json += "\"wifi_ssid\":\"" + json_escape(g_usr_wifi_ssid) + "\",";
  json += "\"wifi_connected\":";
  json += wifi_connected ? "true" : "false";
  json += ",";

  json += "\"video\":{";
  json += "\"transport\":\"reserved\",";
  json += "\"stream_url\":\"\",";
  json += "\"attach_api\":\"MadflightUI.attachVideoSource(url, type)\"";
  json += "},";

  json += "\"flight\":{";
  json += "\"armed\":";
  json += out.armed() ? "true" : "false";
  json += ",";
  json += "\"rcl_connected\":";
  json += input.rc_connected ? "true" : "false";
  json += ",";
  json += "\"ui_connected\":";
  json += ui.connected ? "true" : "false";
  json += ",";
  json += "\"input_source\":\"";
  json += input.connected ? control_source_text(input.source) : "none";
  json += "\",";
  json += "\"control_udp_port\":" + String((unsigned int)kUsrControlUdpPort) + ",";
  json += "\"attitude_lockout\":";
  json += usr_attitude_lockout_active() ? "true" : "false";
  json += ",";
  json += "\"mode\":\"" + json_escape(String(veh.flightmode_name())) + "\",";
  json += "\"mode_index\":" + String((unsigned int)input.flightmode) + ",";
  json += "\"throttle\":" + String(input.throttle, 3) + ",";
  json += "\"vspeed\":" + String(input.vspeed, 3) + ",";
  json += "\"roll\":" + String(input.roll, 3) + ",";
  json += "\"pitch\":" + String(input.pitch, 3) + ",";
  json += "\"yaw\":" + String(input.yaw, 3);
  json += "},";

  json += "\"imu\":{";
  json += "\"name\":\"" + json_escape(String(imu.name())) + "\",";
  json += "\"installed\":";
  json += imu.installed() ? "true" : "false";
  json += ",";
  json += "\"sample_rate_hz\":" + String(imu.getSampleRate()) + ",";
  json += "\"ax\":" + String(imu.ax, 3) + ",";
  json += "\"ay\":" + String(imu.ay, 3) + ",";
  json += "\"az\":" + String(imu.az, 3) + ",";
  json += "\"gx\":" + String(imu.gx, 3) + ",";
  json += "\"gy\":" + String(imu.gy, 3) + ",";
  json += "\"gz\":" + String(imu.gz, 3);
  json += "},";

  json += "\"ahr\":{";
  json += "\"ax\":" + String(ahr.ax, 3) + ",";
  json += "\"ay\":" + String(ahr.ay, 3) + ",";
  json += "\"az\":" + String(ahr.az, 3) + ",";
  json += "\"gx\":" + String(ahr.gx, 3) + ",";
  json += "\"gy\":" + String(ahr.gy, 3) + ",";
  json += "\"gz\":" + String(ahr.gz, 3) + ",";
  json += "\"mx\":" + String(ahr.mx, 2) + ",";
  json += "\"my\":" + String(ahr.my, 2) + ",";
  json += "\"mz\":" + String(ahr.mz, 2) + ",";
  json += "\"roll_deg\":" + String(ahr.roll, 3) + ",";
  json += "\"pitch_deg\":" + String(ahr.pitch, 3) + ",";
  json += "\"yaw_deg\":" + String(ahr.yaw, 3);
  json += "},";

  json += "\"alt\":{";
  json += "\"estimator\":\"" + json_escape(String(alt_estimator_text())) + "\",";
  json += "\"h_m\":" + String(alt.getH(), 3) + ",";
  json += "\"v_ms\":" + String(alt.getV(), 3) + ",";
  json += "\"bar_m\":" + String(bar.alt, 3);
  json += "},";

  json += "\"bar\":{";
  json += "\"name\":\"" + json_escape(String(bar.name())) + "\",";
  json += "\"installed\":";
  json += bar.installed() ? "true" : "false";
  json += ",";
  json += "\"alt_m\":" + String(bar.alt, 3) + ",";
  json += "\"press_pa\":" + String(bar.press, 1) + ",";
  json += "\"temp_c\":" + String(bar.temp, 2);
  json += "},";

  json += "\"mag\":{";
  json += "\"name\":\"" + json_escape(String(mag.name())) + "\",";
  json += "\"installed\":";
  json += mag.installed() ? "true" : "false";
  json += ",";
  json += "\"mx_ut\":" + String(mag.mx, 2) + ",";
  json += "\"my_ut\":" + String(mag.my, 2) + ",";
  json += "\"mz_ut\":" + String(mag.mz, 2);
  json += "},";

  json += "\"outputs\":{";
  json += "\"m1\":" + String(out.get_output(0), 3) + ",";
  json += "\"m2\":" + String(out.get_output(1), 3) + ",";
  json += "\"m3\":" + String(out.get_output(2), 3) + ",";
  json += "\"m4\":" + String(out.get_output(3), 3);
  json += "}";

  json += "}";
  send_json(json);
}

void handle_control() {
  const String ui_version = g_usr_ui_server.arg("ui_version");
  if(ui_version != kUsrRequiredUiVersion) {
    String json;
    json.reserve(160);
    json += "{";
    json += "\"ok\":false,";
    json += "\"error\":\"stale_ui\",";
    json += "\"required_ui_version\":\"";
    json += kUsrRequiredUiVersion;
    json += "\"";
    json += "}";
    g_usr_ui_server.send(409, "application/json; charset=utf-8", json);
    return;
  }

  const bool has_vspeed = g_usr_ui_server.hasArg("vspeed");
  const float legacy_throttle = clampf(g_usr_ui_server.arg("throttle").toFloat(), 0.0f, 1.0f);
  const float vspeed = has_vspeed
    ? clampf(g_usr_ui_server.arg("vspeed").toFloat(), -1.0f, 1.0f)
    : clampf(legacy_throttle * 2.0f - 1.0f, -1.0f, 1.0f);
  const float throttle = has_vspeed ? ui_collective_from_vspeed(vspeed) : legacy_throttle;
  const float roll = clampf(g_usr_ui_server.arg("roll").toFloat(), -1.0f, 1.0f);
  const float pitch = clampf(g_usr_ui_server.arg("pitch").toFloat(), -1.0f, 1.0f);
  const float yaw = clampf(g_usr_ui_server.arg("yaw").toFloat(), -1.0f, 1.0f);
  const bool armed_requested = parse_bool_arg(g_usr_ui_server.arg("armed"));
  const uint8_t flightmode = (uint8_t)constrain(g_usr_ui_server.arg("flightmode").toInt(), 0, kUsrMaxFlightmode);
  const uint32_t client_id = (uint32_t)strtoul(g_usr_ui_server.arg("client_id").c_str(), nullptr, 10);
  const uint32_t seq = (uint32_t)strtoul(g_usr_ui_server.arg("seq").c_str(), nullptr, 10);

  RawUsrControlState current;
  bool accepted = apply_control_state(
    USR_CONTROL_SOURCE_HTTP,
    client_id,
    seq,
    armed_requested,
    flightmode,
    throttle,
    vspeed,
    roll,
    pitch,
    yaw,
    &current
  );

  String json;
  json.reserve(256);
  json += "{";
  json += "\"ok\":true,";
  json += "\"accepted\":";
  json += accepted ? "true" : "false";
  json += ",";
  json += "\"connected\":true,";
  json += "\"armed\":";
  json += current.armed ? "true" : "false";
  json += ",";
  json += "\"flightmode\":";
  json += String((unsigned int)current.flightmode);
  json += "}";
  send_json(json);
}

void handle_video_config() {
  String json;
  json.reserve(384);
  json += "{";
  json += "\"ok\":true,";
  json += "\"hostname\":\"";
  json += kUsrHostName;
  json += ".local\",";
  json += "\"preferred_url\":\"";
  json += json_escape(current_preferred_url());
  json += "\",";
  json += "\"transport\":\"reserved\",";
  json += "\"stream_url\":\"\",";
  json += "\"attach_api\":\"MadflightUI.attachVideoSource(url, type)\"";
  json += "}";
  send_json(json);
}

void handle_params() {
  String json;
  json.reserve(40000);
  json += "{\"items\":[";

  bool first = true;
  for(uint16_t i = 0; i < usr_quad_tuning_count(); i++) {
    String name;
    float value = 0;
    if(!usr_quad_tuning_get(i, &name, &value)) continue;
    if(!first) json += ",";
    first = false;
    json += "{";
    json += "\"scope\":\"usr\",";
    json += "\"name\":\"" + json_escape(name) + "\",";
    json += "\"value\":\"" + String(value, 6) + "\"";
    json += "}";
  }

  for(uint16_t i = 0; i < cfg.paramCount(); i++) {
    String name;
    float value = 0;
    if(!cfg.getNameAndValue(i, &name, &value)) continue;
    if(!first) json += ",";
    first = false;
    json += "{";
    json += "\"scope\":\"cfg\",";
    json += "\"name\":\"" + json_escape(name) + "\",";
    json += "\"value\":\"" + String(value, 6) + "\"";
    json += "}";
  }

  json += "]}";
  send_json(json);
}

void handle_param_set() {
  String scope = g_usr_ui_server.hasArg("scope") ? g_usr_ui_server.arg("scope") : "";
  String name = g_usr_ui_server.hasArg("name") ? g_usr_ui_server.arg("name") : "";
  String value = g_usr_ui_server.hasArg("value") ? g_usr_ui_server.arg("value") : "";

  bool ok = false;
  if(scope == "usr" || name.startsWith("USR_")) {
    ok = usr_quad_tuning_set(name, value);
  } else {
    ok = cfg.setParam(name, value);
  }

  String json;
  json.reserve(256);
  json += "{";
  json += "\"ok\":";
  json += ok ? "true" : "false";
  json += ",";
  json += "\"scope\":\"" + json_escape(scope) + "\",";
  json += "\"name\":\"" + json_escape(name) + "\",";
  json += "\"value\":\"" + json_escape(value) + "\"";
  json += "}";
  send_json(json);
}

void handle_param_save() {
  cfg.writeToEeprom();
  usr_quad_tuning_save();
  send_json("{\"ok\":true}");
}

String build_wave_channel_json(const char *name, const char *color, int channel) {
  String json;
  json.reserve(1600);
  json += "{\"name\":\"";
  json += name;
  json += "\",\"color\":\"";
  json += color;
  json += "\",\"samples\":[";

  for(uint16_t i = 0; i < g_usr_wave_count; i++) {
    uint16_t pos = (g_usr_wave_head + kUsrWaveSampleCount - g_usr_wave_count + i) % kUsrWaveSampleCount;
    const WaveSample &sample = g_usr_wave_buffer[pos];
    float value = 0;
    switch(channel) {
      case 0: value = sample.roll; break;
      case 1: value = sample.pitch; break;
      case 2: value = sample.yaw; break;
      case 3: value = sample.gx; break;
      case 4: value = sample.gy; break;
      case 5: value = sample.gz; break;
      case 6: value = sample.throttle; break;
      default: value = 0; break;
    }
    if(i) json += ",";
    json += String(value, 3);
  }

  json += "]}";
  return json;
}

void handle_waveform() {
  String json;
  json.reserve(12000);
  json += "{";
  json += "\"sample_ms\":";
  json += String(kUsrWaveSampleIntervalMs);
  json += ",";
  json += "\"channels\":[";
  json += build_wave_channel_json("roll", "#ff9b38", 0);
  json += ",";
  json += build_wave_channel_json("pitch", "#63f5e9", 1);
  json += ",";
  json += build_wave_channel_json("yaw", "#c09cff", 2);
  json += ",";
  json += build_wave_channel_json("gx", "#7fe38b", 3);
  json += ",";
  json += build_wave_channel_json("gy", "#ffe56e", 4);
  json += ",";
  json += build_wave_channel_json("gz", "#ff7f7f", 5);
  json += ",";
  json += build_wave_channel_json("throttle", "#f4f7fb", 6);
  json += "]";
  json += "}";
  send_json(json);
}

bool parse_udp_control_packet(char *packet, RawUsrControlState *parsed) {
  if(!packet || !parsed) return false;

  char *tokens[9] = {};
  uint8_t count = 0;
  for(char *token = strtok(packet, ","); token && count < 9; token = strtok(nullptr, ",")) {
    tokens[count++] = trim_token(token);
  }

  bool tagged = false;
  uint8_t index = 0;
  if(count > 0 && strcmp(tokens[0], "MFUDP1") == 0) {
    tagged = true;
    index = 1;
  }

  const uint8_t expected = tagged ? 9 : 7;
  if(count != expected) return false;

  uint32_t client_id = 0;
  uint32_t seq = 0;
  bool armed = false;
  uint32_t mode = 0;
  float vspeed = 0;
  float roll = 0;
  float pitch = 0;
  float yaw = 0;

  if(tagged) {
    if(!parse_uint32_token(tokens[index++], &client_id)) return false;
  }
  if(!parse_uint32_token(tokens[index++], &seq)) return false;
  if(!parse_bool_control_token(tokens[index++], &armed)) return false;
  if(!parse_uint32_token(tokens[index++], &mode)) return false;
  if(!parse_float_token(tokens[index++], &vspeed)) return false;
  if(!parse_float_token(tokens[index++], &roll)) return false;
  if(!parse_float_token(tokens[index++], &pitch)) return false;
  if(!parse_float_token(tokens[index++], &yaw)) return false;

  parsed->client_id = client_id;
  parsed->seq = seq;
  parsed->armed = armed;
  parsed->flightmode = (uint8_t)constrain((int)mode, 0, kUsrMaxFlightmode);
  parsed->vspeed = clampf(vspeed, -1.0f, 1.0f);
  parsed->roll = clampf(roll, -1.0f, 1.0f);
  parsed->pitch = clampf(pitch, -1.0f, 1.0f);
  parsed->yaw = clampf(yaw, -1.0f, 1.0f);
  parsed->throttle = ui_collective_from_vspeed(parsed->vspeed);
  return true;
}

bool parse_udp_telemetry_request(char *packet, uint32_t *client_id, uint32_t *seq) {
  if(!packet || !client_id || !seq) return false;

  char *tokens[3] = {};
  uint8_t count = 0;
  for(char *token = strtok(packet, ","); token && count < 3; token = strtok(nullptr, ",")) {
    tokens[count++] = trim_token(token);
  }

  if(count != 3 || strcmp(tokens[0], "MFTEL1") != 0) return false;
  if(!parse_uint32_token(tokens[1], client_id)) return false;
  if(!parse_uint32_token(tokens[2], seq)) return false;
  return true;
}

void append_csv_float(String *packet, float value, uint8_t digits) {
  if(!packet) return;
  *packet += ",";
  *packet += String(value, (unsigned int)digits);
}

void send_udp_telemetry(IPAddress remote_ip, uint16_t remote_port, uint32_t client_id, uint32_t seq) {
  EffectiveFlightInput input = current_effective_flight_input();
  const uint8_t source = input.connected ? input.source : USR_CONTROL_SOURCE_NONE;

  String packet;
  packet.reserve(320);
  packet += "MFTEL1,";
  packet += String((unsigned long)client_id);
  packet += ",";
  packet += String((unsigned long)seq);
  packet += ",";
  packet += out.armed() ? "1" : "0";
  packet += ",";
  packet += String((unsigned int)source);
  packet += ",";
  packet += String((unsigned int)input.flightmode);
  packet += ",";
  packet += String((unsigned int)kUsrControlUdpPort);
  append_csv_float(&packet, input.vspeed, 3);
  append_csv_float(&packet, input.roll, 3);
  append_csv_float(&packet, input.pitch, 3);
  append_csv_float(&packet, input.yaw, 3);
  append_csv_float(&packet, ahr.roll, 3);
  append_csv_float(&packet, ahr.pitch, 3);
  append_csv_float(&packet, ahr.yaw, 3);
  packet += ",";
  packet += String(imu.getSampleRate());
  append_csv_float(&packet, ahr.gx, 3);
  append_csv_float(&packet, ahr.gy, 3);
  append_csv_float(&packet, ahr.gz, 3);
  append_csv_float(&packet, ahr.ax, 3);
  append_csv_float(&packet, ahr.ay, 3);
  append_csv_float(&packet, ahr.az, 3);
  append_csv_float(&packet, alt.getH(), 3);
  append_csv_float(&packet, alt.getV(), 3);
  append_csv_float(&packet, out.get_output(0), 3);
  append_csv_float(&packet, out.get_output(1), 3);
  append_csv_float(&packet, out.get_output(2), 3);
  append_csv_float(&packet, out.get_output(3), 3);

  g_usr_control_udp.beginPacket(remote_ip, remote_port);
  g_usr_control_udp.write((const uint8_t *)packet.c_str(), packet.length());
  g_usr_control_udp.endPacket();
}

void service_control_udp() {
  if(!g_usr_udp_started) return;

  for(uint8_t i = 0; i < kUsrControlUdpDrainMax; i++) {
    int packet_size = g_usr_control_udp.parsePacket();
    if(packet_size <= 0) return;

    char packet[kUsrControlUdpPacketMax + 1] = {};
    int len = g_usr_control_udp.read(packet, kUsrControlUdpPacketMax);
    if(len <= 0) continue;
    packet[len] = '\0';

    char telemetry_packet[kUsrControlUdpPacketMax + 1] = {};
    strncpy(telemetry_packet, packet, kUsrControlUdpPacketMax);
    telemetry_packet[kUsrControlUdpPacketMax] = '\0';
    uint32_t telemetry_client_id = 0;
    uint32_t telemetry_seq = 0;
    if(parse_udp_telemetry_request(telemetry_packet, &telemetry_client_id, &telemetry_seq)) {
      send_udp_telemetry(g_usr_control_udp.remoteIP(), g_usr_control_udp.remotePort(), telemetry_client_id, telemetry_seq);
      continue;
    }

    RawUsrControlState parsed;
    if(!parse_udp_control_packet(packet, &parsed)) continue;

    apply_control_state(
      USR_CONTROL_SOURCE_UDP,
      parsed.client_id,
      parsed.seq,
      parsed.armed,
      parsed.flightmode,
      parsed.throttle,
      parsed.vspeed,
      parsed.roll,
      parsed.pitch,
      parsed.yaw,
      nullptr
    );
  }
}

void ensure_routes() {
  if(g_usr_routes_ready) return;
  g_usr_routes_ready = true;

  g_usr_ui_server.on("/", HTTP_GET, handle_root);
  g_usr_ui_server.on("/index.html", HTTP_GET, handle_root);
  g_usr_ui_server.on("/api/overview", HTTP_GET, handle_overview);
  g_usr_ui_server.on("/api/control", HTTP_POST, handle_control);
  g_usr_ui_server.on("/api/video/config", HTTP_GET, handle_video_config);
  g_usr_ui_server.on("/api/params", HTTP_GET, handle_params);
  g_usr_ui_server.on("/api/param/set", HTTP_GET, handle_param_set);
  g_usr_ui_server.on("/api/param/set", HTTP_POST, handle_param_set);
  g_usr_ui_server.on("/api/param/save", HTTP_POST, handle_param_save);
  g_usr_ui_server.on("/api/waveform", HTTP_GET, handle_waveform);
  g_usr_ui_server.on("/favicon.ico", HTTP_GET, []() {
    g_usr_ui_server.send(204, "image/x-icon", "");
  });

  g_usr_ui_server.onNotFound([]() {
    if(g_usr_ui_server.uri().startsWith("/api/")) {
      g_usr_ui_server.send(404, "application/json; charset=utf-8", "{\"ok\":false,\"error\":\"not_found\"}");
      return;
    }
    handle_root();
  });
}

void ensure_network_services() {
  if(!g_usr_server_started) {
    g_usr_ui_server.begin();
    g_usr_server_started = true;
    ui_log_printf(
      "[USR:UI] HTTP ready -> host=http://%s.local/ ip=http://%s/\n",
      kUsrHostName,
      WiFi.localIP().toString().c_str()
    );
    ui_log_println("[USR:UI] Hotspot tip -> if the phone cannot open .local, open the IP address above.");
  }

  if(!g_usr_udp_started) {
    if(g_usr_control_udp.begin(kUsrControlUdpPort)) {
      g_usr_udp_started = true;
      ui_log_printf("[USR:UI] UDP control ready -> %s:%u\n", WiFi.localIP().toString().c_str(), (unsigned int)kUsrControlUdpPort);
    } else {
      ui_log_printf("[USR:UI] UDP control start failed on port %u\n", (unsigned int)kUsrControlUdpPort);
    }
  }

  if(!g_usr_mdns_started) {
    if(MDNS.begin(kUsrHostName)) {
      MDNS.addService("http", "tcp", 80);
      g_usr_mdns_started = true;
      ui_log_printf("[USR:UI] mDNS ready -> %s.local\n", kUsrHostName);
    } else {
      ui_log_println("[USR:UI] mDNS start failed, use the IP address instead.");
    }
  }

  if(!g_usr_nbns_started) {
    if(NBNS.begin(kUsrHostName)) {
      g_usr_nbns_started = true;
      ui_log_printf("[USR:UI] NetBIOS ready -> http://%s/ (best effort)\n", kUsrHostName);
    } else {
      ui_log_println("[USR:UI] NetBIOS start failed.");
    }
  }
}

void service_wifi() {
  const wl_status_t status = WiFi.status();
  if(status != g_usr_last_wifi_status) {
    const wl_status_t previous = g_usr_last_wifi_status;
    g_usr_last_wifi_status = status;
    ui_log_printf("[USR:UI] WiFi state -> %s\n", wifi_status_text(status).c_str());
    if(status == WL_CONNECTED) {
      ui_log_printf("[USR:UI] Connected IP -> %s\n", WiFi.localIP().toString().c_str());
    } else if(previous == WL_CONNECTED) {
      if(g_usr_mdns_started) {
        MDNS.end();
        g_usr_mdns_started = false;
      }
      if(g_usr_nbns_started) {
        NBNS.end();
        g_usr_nbns_started = false;
      }
      if(g_usr_udp_started) {
        g_usr_control_udp.stop();
        g_usr_udp_started = false;
      }
      ui_log_println("[USR:UI] WiFi link lost, network name services will restart after reconnect.");
    }
  }

  if(status == WL_CONNECTED) {
    ensure_network_services();
    return;
  }

  if(!g_usr_wifi_ssid.isEmpty() && millis() - g_usr_last_wifi_retry_ms >= kUsrWifiRetryMs) {
    begin_wifi_connect("retry");
  }
}

} // namespace

void usr_mobile_ui_preinit() {
  usr_quad_tuning_begin();
}

void usr_mobile_ui_begin() {
  ensure_routes();
  load_wifi_config();
  ui_log_println("");
  ui_log_println("[USR:UI] Mobile cockpit bootstrap");
  print_wifi_help();
  print_wifi_status();
  if(!g_usr_wifi_ssid.isEmpty()) {
    begin_wifi_connect("boot");
  }
}

void usr_mobile_ui_loop() {
  capture_wave_sample();
  service_wifi();
  service_control_udp();

  if(g_usr_server_started) {
    g_usr_ui_server.handleClient();
  }
}

const char *usr_mobile_ui_host() {
  return "madflight.local";
}

bool usr_mobile_ui_control_snapshot(UsrMobileUiControlState *state) {
  return fill_control_snapshot(state);
}

bool cli_execute(String cmd, String arg1, String arg2) {
  if(cmd != "wifi") return false;
  return handle_wifi_cli_command(arg1, arg2);
}

#endif
