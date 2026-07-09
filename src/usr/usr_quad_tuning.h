#pragma once

#include <Arduino.h>

struct UsrQuadTuning {
  float armed_min_throttle;
  float ui_hover_throttle;
  float ui_vspeed_throttle_span;

  float i_limit;
  float maxRoll;
  float maxPitch;
  float maxRollRate;
  float maxPitchRate;
  float maxYawRate;

  float Kp_ro_pi_angle;
  float Ki_ro_pi_angle;
  float Kd_ro_pi_angle;
  float Kp_yaw_angle;
  float Kd_yaw_angle;

  float Kp_ro_pi_rate;
  float Ki_ro_pi_rate;
  float Kd_ro_pi_rate;
  float Kp_yaw_rate;
  float Ki_yaw_rate;
  float Kd_yaw_rate;

  float attitude_lock_deg;
  float alt_hold_max_vspeed;
  float alt_hold_kp;
  float alt_hold_kv;
  float alt_hold_max_corr;
  float alt_hold_ki;
  float alt_hold_i_limit;
  float alt_hold_slew_rate;
  float alt_hold_v_lpf_hz;
  float alt_baro_propwash_comp;
  float alt_tilt_comp_max;
};

extern UsrQuadTuning usr_quad_tuning;

void usr_quad_tuning_begin();
void usr_quad_tuning_reset_defaults();
void usr_quad_tuning_save();

uint16_t usr_quad_tuning_count();
bool usr_quad_tuning_get(uint16_t index, String *name, float *value);
bool usr_quad_tuning_set(const String &name, const String &value);
bool usr_quad_tuning_setf(const String &name, float value);
