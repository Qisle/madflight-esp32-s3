#pragma once

#include <stdint.h>

struct UsrMobileUiControlState {
  bool connected = false;
  bool armed = false;
  uint8_t flightmode = 0;
  float throttle = 0;
  float vspeed = 0;
  float roll = 0;
  float pitch = 0;
  float yaw = 0;
  uint32_t age_ms = 0;
  uint8_t source = 0;
};

void usr_mobile_ui_preinit();
void usr_mobile_ui_begin();
void usr_mobile_ui_loop();

const char *usr_mobile_ui_host();
bool usr_mobile_ui_control_snapshot(UsrMobileUiControlState *state);
bool usr_attitude_lockout_active();
