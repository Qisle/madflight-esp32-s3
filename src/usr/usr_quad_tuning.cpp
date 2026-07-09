#include "usr_quad_tuning.h"

#if defined(ARDUINO_ARCH_ESP32)
  #include <Preferences.h>
#endif

UsrQuadTuning usr_quad_tuning = {};

namespace {

struct ParamBinding {
  const char *name;
  float UsrQuadTuning::*member;
  float default_value;
};

const ParamBinding kBindings[] = {
  {"USR_ARMED_MIN_THROTTLE", &UsrQuadTuning::armed_min_throttle, 0.10f},
  {"USR_UI_HOVER_THROTTLE", &UsrQuadTuning::ui_hover_throttle, 0.30f},
  {"USR_UI_VSPEED_THROTTLE_SPAN", &UsrQuadTuning::ui_vspeed_throttle_span, 0.24f},
  {"USR_I_LIMIT", &UsrQuadTuning::i_limit, 25.0f},
  {"USR_MAX_ROLL", &UsrQuadTuning::maxRoll, 30.0f},
  {"USR_MAX_PITCH", &UsrQuadTuning::maxPitch, 30.0f},
  {"USR_MAX_ROLL_RATE", &UsrQuadTuning::maxRollRate, 60.0f},
  {"USR_MAX_PITCH_RATE", &UsrQuadTuning::maxPitchRate, 60.0f},
  {"USR_MAX_YAW_RATE", &UsrQuadTuning::maxYawRate, 160.0f},
  {"USR_KP_RO_PI_ANGLE", &UsrQuadTuning::Kp_ro_pi_angle, 0.2f},
  {"USR_KI_RO_PI_ANGLE", &UsrQuadTuning::Ki_ro_pi_angle, 0.1f},
  {"USR_KD_RO_PI_ANGLE", &UsrQuadTuning::Kd_ro_pi_angle, 0.05f},
  {"USR_KP_YAW_ANGLE", &UsrQuadTuning::Kp_yaw_angle, 1.00f},
  {"USR_KD_YAW_ANGLE", &UsrQuadTuning::Kd_yaw_angle, 0.1f},
  {"USR_KP_RO_PI_RATE", &UsrQuadTuning::Kp_ro_pi_rate, 0.15f},
  {"USR_KI_RO_PI_RATE", &UsrQuadTuning::Ki_ro_pi_rate, 0.2f},
  {"USR_KD_RO_PI_RATE", &UsrQuadTuning::Kd_ro_pi_rate, 0.0002f},
  {"USR_KP_YAW_RATE", &UsrQuadTuning::Kp_yaw_rate, 0.3f},
  {"USR_KI_YAW_RATE", &UsrQuadTuning::Ki_yaw_rate, 0.05f},
  {"USR_KD_YAW_RATE", &UsrQuadTuning::Kd_yaw_rate, 0.00015f},
  {"USR_ATTITUDE_LOCK_DEG", &UsrQuadTuning::attitude_lock_deg, 60.0f},
  {"USR_ALT_HOLD_MAX_VSPEED", &UsrQuadTuning::alt_hold_max_vspeed, 0.80f},
  {"USR_ALT_HOLD_KP", &UsrQuadTuning::alt_hold_kp, 0.80f},
  {"USR_ALT_HOLD_KV", &UsrQuadTuning::alt_hold_kv, 0.25f},
  {"USR_ALT_HOLD_KI", &UsrQuadTuning::alt_hold_ki, 0.04f},
  {"USR_ALT_HOLD_I_LIMIT", &UsrQuadTuning::alt_hold_i_limit, 0.08f},
  {"USR_ALT_HOLD_MAX_CORR", &UsrQuadTuning::alt_hold_max_corr, 0.18f},
  {"USR_ALT_HOLD_SLEW_RATE", &UsrQuadTuning::alt_hold_slew_rate, 1.20f},
  {"USR_ALT_HOLD_V_LPF_HZ", &UsrQuadTuning::alt_hold_v_lpf_hz, 5.0f},
  {"USR_ALT_BARO_PROPWASH_COMP", &UsrQuadTuning::alt_baro_propwash_comp, 0.0f},
  {"USR_ALT_TILT_COMP_MAX", &UsrQuadTuning::alt_tilt_comp_max, 0.08f},
};

#if defined(ARDUINO_ARCH_ESP32)
Preferences g_usr_quad_prefs;
#endif

void load_defaults() {
  for(const ParamBinding &binding : kBindings) {
    usr_quad_tuning.*(binding.member) = binding.default_value;
  }
}

bool find_binding(const String &name, const ParamBinding **out_binding) {
  String upper = name;
  upper.trim();
  upper.toUpperCase();

  for(const ParamBinding &binding : kBindings) {
    if(upper == binding.name) {
      if(out_binding) *out_binding = &binding;
      return true;
    }
  }
  return false;
}

} // namespace

void usr_quad_tuning_reset_defaults() {
  load_defaults();
}

void usr_quad_tuning_begin() {
  load_defaults();

#if defined(ARDUINO_ARCH_ESP32)
  if(!g_usr_quad_prefs.begin("usr-quad", true)) return;

  const size_t stored_len = g_usr_quad_prefs.getBytesLength("blob");
  if(stored_len > 0 && stored_len <= sizeof(UsrQuadTuning)) {
    UsrQuadTuning loaded = {};
    loaded = usr_quad_tuning;
    if(g_usr_quad_prefs.getBytes("blob", &loaded, stored_len) == stored_len) {
      usr_quad_tuning = loaded;
    }
  }

  g_usr_quad_prefs.end();
#endif
}

void usr_quad_tuning_save() {
#if defined(ARDUINO_ARCH_ESP32)
  if(!g_usr_quad_prefs.begin("usr-quad", false)) return;
  g_usr_quad_prefs.putBytes("blob", &usr_quad_tuning, sizeof(usr_quad_tuning));
  g_usr_quad_prefs.end();
#endif
}

uint16_t usr_quad_tuning_count() {
  return (uint16_t)(sizeof(kBindings) / sizeof(kBindings[0]));
}

bool usr_quad_tuning_get(uint16_t index, String *name, float *value) {
  if(index >= usr_quad_tuning_count()) return false;
  if(name) *name = kBindings[index].name;
  if(value) *value = usr_quad_tuning.*(kBindings[index].member);
  return true;
}

bool usr_quad_tuning_setf(const String &name, float value) {
  const ParamBinding *binding = nullptr;
  if(!find_binding(name, &binding) || !binding) return false;
  usr_quad_tuning.*(binding->member) = value;
  return true;
}

bool usr_quad_tuning_set(const String &name, const String &value) {
  return usr_quad_tuning_setf(name, value.toFloat());
}
