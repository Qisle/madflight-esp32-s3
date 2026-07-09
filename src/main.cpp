/*#########################################################################################################################

NOTICE: First get Quadcopter.ino to fly before attempting this program.

You can copy madflight_config.h from Quadcopter.ino to keep your settings.

###########################################################################################################################

See http://madflight.com for detailed description

Required Hardware

    IMU sensor (SPI or I2C)
    Web UI control over WiFi
    4 brushless motors with ESCs

Connecting Hardware

    SPI IMU: connect pin_imu_int, pin_imu_cs, pin_spi0_miso, pin_spi0_mosi, pin_spi0_sclk
    or for I2C IMU: connect pin_imu_int, pin_i2c1_scl, pin_i2c1_sda
    WiFi: configure credentials through the USB CDC CLI
    Motor MOSFET gates: PWM1..PWM4 / pin_out0..pin_out3 to right-bottom, right-top, left-top, left-bottom motors

Arming/disarming

    Use the Web Cockpit control UI. If the UI link times out, outputs disarm through failsafe.

LED State                              Meaning
---------                              -------
OFF                                    Not powered
ON (blue)                              Startup (don't move, running gyro calibration)
Blinking long OFF short ON (green)     DISARMED
Blinking long ON short OFF (red)       ARMED
Blink interval longer than 1 second    imu_loop() is taking too much time
Fast blinking                          Something is wrong, connect USB serial for info

MIT license
MIT license - Copyright (c) 2023-2026 https://madflight.com
##########################################################################################################################*/

// Vehicle specific madflight configuration
#define VEH_TYPE VEH_TYPE_COPTER
#define VEH_FLIGHTMODE_AP_IDS {AP_COPTER_FLIGHTMODE_ACRO, AP_COPTER_FLIGHTMODE_STABILIZE, AP_COPTER_FLIGHTMODE_ALT_HOLD, AP_COPTER_FLIGHTMODE_STABILIZE, AP_COPTER_FLIGHTMODE_STABILIZE, AP_COPTER_FLIGHTMODE_STABILIZE}
#define VEH_FLIGHTMODE_NAMES {"RATE", "ANGLE", "ALT", "ANGLE", "ANGLE", "ANGLE"}
enum flightmode_enum { RATE, ANGLE, ALT };
flightmode_enum rcl_to_flightmode_map[6] {RATE, ANGLE, ALT, ALT, ANGLE, ANGLE};

#include "madflight_config.h"
#include <madflight.h>
#include <math.h>
#include "usr/usr_mobile_ui.h"
#include "usr/usr_quad_tuning.h"

#define armed_min_throttle usr_quad_tuning.armed_min_throttle
#define ui_hover_throttle usr_quad_tuning.ui_hover_throttle
#define ui_vspeed_throttle_span usr_quad_tuning.ui_vspeed_throttle_span
#define i_limit usr_quad_tuning.i_limit
#define maxRoll usr_quad_tuning.maxRoll
#define maxPitch usr_quad_tuning.maxPitch
#define maxRollRate usr_quad_tuning.maxRollRate
#define maxPitchRate usr_quad_tuning.maxPitchRate
#define maxYawRate usr_quad_tuning.maxYawRate
#define attitude_lock_deg usr_quad_tuning.attitude_lock_deg
#define alt_hold_max_vspeed usr_quad_tuning.alt_hold_max_vspeed
#define alt_hold_kp usr_quad_tuning.alt_hold_kp
#define alt_hold_kv usr_quad_tuning.alt_hold_kv
#define alt_hold_ki usr_quad_tuning.alt_hold_ki
#define alt_hold_i_limit usr_quad_tuning.alt_hold_i_limit
#define alt_hold_max_corr usr_quad_tuning.alt_hold_max_corr
#define alt_hold_slew_rate usr_quad_tuning.alt_hold_slew_rate
#define alt_hold_v_lpf_hz usr_quad_tuning.alt_hold_v_lpf_hz
#define alt_baro_propwash_comp usr_quad_tuning.alt_baro_propwash_comp
#define alt_tilt_comp_max usr_quad_tuning.alt_tilt_comp_max
#define Kp_ro_pi_angle usr_quad_tuning.Kp_ro_pi_angle
#define Ki_ro_pi_angle usr_quad_tuning.Ki_ro_pi_angle
#define Kd_ro_pi_angle usr_quad_tuning.Kd_ro_pi_angle
#define Kp_yaw_angle usr_quad_tuning.Kp_yaw_angle
#define Kd_yaw_angle usr_quad_tuning.Kd_yaw_angle
#define Kp_ro_pi_rate usr_quad_tuning.Kp_ro_pi_rate
#define Ki_ro_pi_rate usr_quad_tuning.Ki_ro_pi_rate
#define Kd_ro_pi_rate usr_quad_tuning.Kd_ro_pi_rate
#define Kp_yaw_rate usr_quad_tuning.Kp_yaw_rate
#define Ki_yaw_rate usr_quad_tuning.Ki_yaw_rate
#define Kd_yaw_rate usr_quad_tuning.Kd_yaw_rate

// Prototypes (for PlatformIO, not needed for Arduino IDE)
uint32_t flightmode_led_color();
void led_Blink();
float degreeModulus(float v);
bool zero_integrators_requested();
void update_ui_vertical_mode();
bool ui_alt_hold_active();
float ui_tilt_collective_comp(float collective);
float ui_collective_command();
float effective_collective_command();
void update_attitude_lockout();
void control_Angle(bool zero_integrators);
void control_Rate(bool zero_integrators);
void out_KillSwitchAndFailsafe();
void out_Mixer();

//========================================================================================================================//
//                                               USER-SPECIFIED VARIABLES                                                 //
//========================================================================================================================//

// Quad tuning values live in src/usr/usr_quad_tuning.* so the web UI can edit them online.

// Yaw to keep in ANGLE mode when yaw stick is centered
float yaw_desired = 0;

struct ActivePilotInput {
  bool using_ui = false;
  bool connected = false;
  bool armed = false;
  uint8_t flightmode = 0;
  float throttle = 0;
  float vspeed = 0;
  float roll = 0;
  float pitch = 0;
  float yaw = 0;
};

ActivePilotInput g_control_input = {};

struct UiVerticalState {
  bool armed_prev = false;
  bool hover_mode = false;
  bool alt_hold_valid = false;
  bool alt_hold_position_locked = false;
  float arm_alt_m = 0;
  float alt_hold_target_m = 0;
  float alt_hold_v_lpf = 0;
  float alt_hold_i = 0;
  float alt_hold_collective = 0;
};

UiVerticalState g_ui_vertical = {};

bool g_attitude_lockout = false;
float g_last_collective_command = 0.0f;

bool usr_attitude_lockout_active() {
  return g_attitude_lockout;
}

void refresh_active_pilot_input() {
  UsrMobileUiControlState ui = {};
  if(usr_mobile_ui_control_snapshot(&ui)) {
    g_control_input.using_ui = true;
    g_control_input.connected = true;
    g_control_input.armed = ui.armed;
    g_control_input.flightmode = ui.flightmode;
    g_control_input.throttle = ui.throttle;
    g_control_input.vspeed = ui.vspeed;
    g_control_input.roll = ui.roll;
    g_control_input.pitch = ui.pitch;
    g_control_input.yaw = ui.yaw;
    return;
  }

  g_control_input.using_ui = false;
  g_control_input.connected = false;
  g_control_input.armed = false;
  g_control_input.flightmode = 0;
  g_control_input.throttle = 0;
  g_control_input.vspeed = 0;
  g_control_input.roll = 0;
  g_control_input.pitch = 0;
  g_control_input.yaw = 0;
}

bool zero_integrators_requested() {
  if(!g_control_input.connected || !g_control_input.armed) return true;
  if(!g_control_input.using_ui) return g_control_input.throttle <= 0.001f;
  if(!g_ui_vertical.hover_mode) return g_control_input.vspeed <= 0.05f;
  return false;
}

float first_order_lowpass_beta(float cutoff_hz, float dt) {
  if(cutoff_hz <= 0 || dt <= 0) return 1.0f;
  const float rc = 1.0f / (2.0f * PI * cutoff_hz);
  return constrain(dt / (rc + dt), 0.0f, 1.0f);
}

float slew_limit_value(float current, float target, float rate_per_s, float dt) {
  if(rate_per_s <= 0 || dt <= 0) return target;
  const float step = rate_per_s * dt;
  return current + constrain(target - current, -step, step);
}

void update_ui_vertical_mode() {
  const bool ui_active = g_control_input.using_ui && g_control_input.connected && g_control_input.armed;
  const float hover = constrain(ui_hover_throttle, 0.05f, 0.90f);

  if(!ui_active) {
    g_ui_vertical.armed_prev = false;
    g_ui_vertical.hover_mode = false;
    g_ui_vertical.alt_hold_valid = false;
    g_ui_vertical.alt_hold_position_locked = false;
    g_ui_vertical.arm_alt_m = alt.getH();
    g_ui_vertical.alt_hold_target_m = g_ui_vertical.arm_alt_m;
    g_ui_vertical.alt_hold_v_lpf = 0;
    g_ui_vertical.alt_hold_i = 0;
    g_ui_vertical.alt_hold_collective = hover;
    return;
  }

  if(!g_ui_vertical.armed_prev) {
    g_ui_vertical.arm_alt_m = alt.getH();
    g_ui_vertical.alt_hold_target_m = g_ui_vertical.arm_alt_m;
    g_ui_vertical.hover_mode = false;
    g_ui_vertical.alt_hold_valid = false;
    g_ui_vertical.alt_hold_position_locked = false;
    g_ui_vertical.alt_hold_v_lpf = 0;
    g_ui_vertical.alt_hold_i = 0;
    g_ui_vertical.alt_hold_collective = hover;
  }

  const float rel_alt_m = alt.getH() - g_ui_vertical.arm_alt_m;

  if(!g_ui_vertical.hover_mode) {
    // Keep the motors near idle right after arming. Once the pilot actually commands
    // a takeoff, switch to centered up/down control.
    if(g_control_input.vspeed > 0.35f || rel_alt_m > 0.30f) {
      g_ui_vertical.hover_mode = true;
    }
  } else {
    // Near touchdown, a full-down stick command returns to ground mode so re-arming
    // does not jump straight to the hover throttle estimate.
    if(g_control_input.vspeed < -0.85f && rel_alt_m < 0.12f) {
      g_ui_vertical.hover_mode = false;
    }
  }

  if(ui_alt_hold_active()) {
    const bool vertical_stick_active = fabs(g_control_input.vspeed) > 0.05f;
    if(!g_ui_vertical.alt_hold_valid) {
      g_ui_vertical.alt_hold_target_m = alt.getH();
      g_ui_vertical.alt_hold_v_lpf = alt.getV();
      g_ui_vertical.alt_hold_i = 0;
      g_ui_vertical.alt_hold_position_locked = false;
      g_ui_vertical.alt_hold_collective = constrain(
        hover + g_control_input.vspeed * constrain(ui_vspeed_throttle_span, 0.02f, 0.90f),
        0.0f,
        1.0f
      );
      g_ui_vertical.alt_hold_valid = true;
    }
    if(vertical_stick_active) {
      g_ui_vertical.alt_hold_position_locked = false;
      g_ui_vertical.alt_hold_target_m = alt.getH();
    } else if(!g_ui_vertical.alt_hold_position_locked) {
      g_ui_vertical.alt_hold_target_m = alt.getH();
      if(fabs(alt.getV()) < 0.12f) {
        g_ui_vertical.alt_hold_position_locked = true;
      }
    }
    const float min_target_m = g_ui_vertical.arm_alt_m + 0.08f;
    if(g_ui_vertical.alt_hold_target_m < min_target_m) g_ui_vertical.alt_hold_target_m = min_target_m;
  } else {
    g_ui_vertical.alt_hold_valid = false;
    g_ui_vertical.alt_hold_position_locked = false;
    g_ui_vertical.alt_hold_target_m = alt.getH();
    g_ui_vertical.alt_hold_v_lpf = alt.getV();
    g_ui_vertical.alt_hold_i = 0;
    g_ui_vertical.alt_hold_collective = hover + g_control_input.vspeed * constrain(ui_vspeed_throttle_span, 0.02f, 0.90f);
  }

  g_ui_vertical.armed_prev = true;
}

bool ui_alt_hold_active() {
  return g_control_input.using_ui
    && g_control_input.connected
    && g_control_input.armed
    && g_ui_vertical.hover_mode
    && veh.getFlightmode() == ALT;
}

float ui_tilt_collective_comp(float collective) {
  const float max_comp = constrain(alt_tilt_comp_max, 0.0f, 0.15f);
  if(max_comp <= 0.0f) return 0.0f;

  const float deg_to_rad_local = 0.01745329252f;
  const float roll_rad = constrain(ahr.roll, -35.0f, 35.0f) * deg_to_rad_local;
  const float pitch_rad = constrain(ahr.pitch, -35.0f, 35.0f) * deg_to_rad_local;
  const float cos_tilt = constrain(cosf(roll_rad) * cosf(pitch_rad), 0.50f, 1.0f);
  const float tilt_comp = collective * (1.0f / cos_tilt - 1.0f);
  return constrain(tilt_comp, 0.0f, max_comp);
}

float ui_collective_command() {
  if(!g_control_input.connected || !g_control_input.armed) return 0.0f;

  if(!g_ui_vertical.hover_mode) {
    const float takeoff = (g_control_input.vspeed - 0.05f) / 0.95f;
    return constrain(takeoff, 0.0f, 1.0f);
  }

  const float hover = constrain(ui_hover_throttle, 0.05f, 0.90f);
  const float span = constrain(ui_vspeed_throttle_span, 0.02f, 0.90f);
  if(ui_alt_hold_active()) {
    const float error_h = g_ui_vertical.alt_hold_position_locked ? (g_ui_vertical.alt_hold_target_m - alt.getH()) : 0.0f;
    const float max_vspeed = constrain(alt_hold_max_vspeed, 0.05f, 2.0f);
    const float stick_vspeed = fabs(g_control_input.vspeed) > 0.05f ? g_control_input.vspeed * max_vspeed : 0.0f;
    const float v_target = constrain(stick_vspeed + alt_hold_kp * error_h, -max_vspeed, max_vspeed);
    const float v_lpf_beta = first_order_lowpass_beta(constrain(alt_hold_v_lpf_hz, 1.0f, 20.0f), imu.dt);
    g_ui_vertical.alt_hold_v_lpf += v_lpf_beta * (alt.getV() - g_ui_vertical.alt_hold_v_lpf);
    const float v_error = v_target - g_ui_vertical.alt_hold_v_lpf;
    const float alt_i_limit = constrain(alt_hold_i_limit, 0.0f, 0.20f);
    g_ui_vertical.alt_hold_i += constrain(alt_hold_ki, 0.0f, 0.30f) * v_error * imu.dt;
    g_ui_vertical.alt_hold_i = constrain(g_ui_vertical.alt_hold_i, -alt_i_limit, alt_i_limit);
    const float corr = alt_hold_kv * v_error + g_ui_vertical.alt_hold_i;
    const float max_corr = constrain(alt_hold_max_corr, 0.02f, 0.60f);
    const float base_target = constrain(hover + constrain(corr, -max_corr, max_corr), 0.0f, 1.0f);
    const float target = constrain(base_target + ui_tilt_collective_comp(base_target), 0.0f, 1.0f);
    const float slew_rate = constrain(alt_hold_slew_rate, 0.05f, 5.0f);
    g_ui_vertical.alt_hold_collective = constrain(
      slew_limit_value(g_ui_vertical.alt_hold_collective, target, slew_rate, imu.dt),
      0.0f,
      1.0f
    );
    return g_ui_vertical.alt_hold_collective;
  }
  const float collective = constrain(hover + g_control_input.vspeed * span, 0.0f, 1.0f);
  g_ui_vertical.alt_hold_collective = collective;
  return collective;
}

float effective_collective_command() {
  const float collective = g_control_input.using_ui ? ui_collective_command() : g_control_input.throttle;
  g_last_collective_command = constrain(collective, 0.0f, 1.0f);
  return g_last_collective_command;
}

//========================================================================================================================//
//                                                       SETUP()                                                          //
//========================================================================================================================//

void setup() {
  usr_mobile_ui_preinit();

  // Setup madflight modules, start madflight RTOS tasks, Serial.begin(11520)
  madflight_setup();

  // STOP if imu is not installed
  if(!imu.installed()) madflight_panic("This program needs an IMU.");

  // PWM1..PWM4 map to right-bottom, right-top, left-top, left-bottom motors.
  // The matching pins are configured as pin_out0..pin_out3 in madflight_config.h.
  int motor_outputs[] = {0, 1, 2, 3};

  // AO3416 low-side MOSFETs directly switch two-wire motors, so use true 0..100% duty cycle.
  bool success = out.setup_brushed(4, motor_outputs, 5000);            // Brushed motors: 5000Hz with 0-100% duty cycle
  // ESC alternatives, not for the current MOSFET direct-drive hardware:
  //bool success = out.setup_motors(4, motor_outputs, 400, 950, 2000);  // Standard PWM: 400Hz, 950-2000 us
  //bool success = out.setup_motors(4, motor_outputs, 2000, 125, 250); // Oneshot125: 2000Hz, 125-250 us
  //bool success = out.setup_dshot(4, motor_outputs, 300);             // Dshot300
  //bool success = out.setup_dshot_bidir(4, motor_outputs, 300);       // Dshot300 Bi-Directional

  out.print();
  if(!success) madflight_panic("Motor init failed.");

  usr_mobile_ui_begin();

  // Set initial desired yaw
  yaw_desired = ahr.yaw;

  Serial.println("Setup completed, CLI started - Type 'help' for help, or 'diff' to debug");
}

//========================================================================================================================//
//                                                            LOOP()                                                      //
//========================================================================================================================//

void loop() {
  usr_mobile_ui_loop();
  delay(1);
}

//========================================================================================================================//
//                                                   IMU UPDATE LOOP                                                      //
//========================================================================================================================//

// This is the __MAIN__ part of this program. It is called from the IMU FreeRTOS task when new IMU data is available.
void imu_loop() {
  static MsgSubscription<BarState> alt_bar_sub("altbar", &bar.topic);
  static BarState alt_bar_state;

  // Blink LED
  led_Blink();

  // Sensor fusion: update ahr.roll, ahr.pitch, and ahr.yaw angle estimates (degrees) from IMU data
  ahr.update();

  // Keep altitude estimation on the IMU task so accel-up and baro samples
  // are fused in one execution context.
  alt.updateAccelUp(ahr.getAccelUp(), ahr.ts);
  if(alt_bar_sub.pull_updated(&alt_bar_state)) {
    alt.updateBarAlt(
      alt_bar_state.alt,
      alt_bar_state.ts,
      g_last_collective_command,
      constrain(ui_hover_throttle, 0.05f, 0.90f),
      alt_baro_propwash_comp
    );
  }

  refresh_active_pilot_input();

  // Update flight mode
  if(g_control_input.connected && veh.setFlightmode(rcl_to_flightmode_map[g_control_input.flightmode])) {
    Serial.printf("Flightmode:%s\n", veh.flightmode_name());
  }

  update_ui_vertical_mode();
  update_attitude_lockout();

  // PID Controller
  const bool zero_integrators = zero_integrators_requested();
  switch(veh.getFlightmode()) {
    case ALT:
    case ANGLE:
      control_Angle(zero_integrators);
      break;
    default:
      control_Rate(zero_integrators);
  }

  // Updates out.arm, the output armed flag
  out_KillSwitchAndFailsafe();

  // Actuator mixing
  out_Mixer();
}

//========================================================================================================================
//                      IMU UPDATE LOOP FUNCTIONS - in same order as they are called from imu_loop()
//========================================================================================================================

uint32_t flightmode_led_color() {
  switch(veh.getFlightmode()) {
    case RATE:
      return 0xff0000;
    case ANGLE:
      return 0x0060ff;
    case ALT:
      return 0x9000ff;
    default:
      return 0xffffff;
  }
}

void led_Blink() {
  // Blink LED once per second, if LED blinks slower then the loop takes too much time, use CLI 'pimu' to investigate.
  // DISARMED: mode color short on. ARMED: mode color long on.
  uint32_t modulus = imu.update_cnt % imu.getSampleRate();
  const uint32_t mode_color = flightmode_led_color();
  if(modulus == 0) led.color((out.armed() ? 0 : mode_color));
  if(modulus == imu.getSampleRate() / 10) led.color((out.armed() ? mode_color : 0));
}

// Returns angle in range -180 to 180
float degreeModulus(float v) {
  if(v >= 180) {
    return fmod(v + 180, 360) - 180;
  } else if(v < -180.0) {
    return fmod(v - 180, 360) + 180;
  }
  return v;
}

void update_attitude_lockout() {
  const float threshold = attitude_lock_deg;
  if(threshold > 0.0f && g_control_input.armed && (fabs(ahr.roll) >= threshold || fabs(ahr.pitch) >= threshold)) {
    g_attitude_lockout = true;
    if(out.armed()) {
      out.set_armed(false);
      Serial.printf("OUT: DISARMED due to attitude lockout roll=%.1f pitch=%.1f limit=%.1f\n", ahr.roll, ahr.pitch, threshold);
      if(bbx.gizmo) bbx.stop();
    }
  }

  if(!g_control_input.armed) {
    g_attitude_lockout = false;
  }
}

void control_Angle(bool zero_integrators) {
  // DESCRIPTION: Computes control commands based on angle error
  // Basic PID control to stabilize on angle setpoints.

  // inputs: roll_des, pitch_des, yawRate_des
  // outputs: pid.roll, pid.pitch, pid.yaw

  // Desired values
  float roll_des = g_control_input.roll * maxRoll;
  float pitch_des = -g_control_input.pitch * maxPitch;
  float yawRate_des = g_control_input.yaw * maxYawRate;

  // State vars
  static float integral_roll, integral_pitch, error_yawRate_prev, integral_yawRate;

  // Zero the integrators
  if(zero_integrators) {
    integral_roll = 0;
    integral_pitch = 0;
    integral_yawRate = 0;
  }

  // Roll PID
  float error_roll = roll_des - ahr.roll;
  integral_roll += error_roll * imu.dt;
  integral_roll = constrain(integral_roll, -i_limit, i_limit);
  float derivative_roll = ahr.gx;
  pid.roll = 0.01 * (Kp_ro_pi_angle * error_roll + Ki_ro_pi_angle * integral_roll - Kd_ro_pi_angle * derivative_roll);

  // Pitch PID
  float error_pitch = pitch_des - ahr.pitch;
  integral_pitch += error_pitch * imu.dt;
  integral_pitch = constrain(integral_pitch, -i_limit, i_limit);
  float derivative_pitch = ahr.gy;
  pid.pitch = 0.01 * (Kp_ro_pi_angle * error_pitch + Ki_ro_pi_angle * integral_pitch - Kd_ro_pi_angle * derivative_pitch);

  // Yaw PID
  if(-0.02 < g_control_input.yaw && g_control_input.yaw < 0.02) {
    if(zero_integrators) yaw_desired = ahr.yaw;

    // Yaw stick centered: hold yaw_desired
    float error_yaw = degreeModulus(yaw_desired - ahr.yaw);
    float desired_yawRate = error_yaw / 0.5;
    float derivative_yaw = desired_yawRate - ahr.gz;
    pid.yaw = 0.01 * (Kp_yaw_angle * error_yaw + Kd_yaw_angle * derivative_yaw);

    error_yawRate_prev = 0;
  } else {
    // Yaw stick not centered: stabilize on rate from GyroZ
    float error_yawRate = yawRate_des - ahr.gz;
    integral_yawRate += error_yawRate * imu.dt;
    integral_yawRate = constrain(integral_yawRate, -i_limit, i_limit);
    float derivative_yawRate = (error_yawRate - error_yawRate_prev) / imu.dt;
    pid.yaw = 0.01 * (Kp_yaw_rate * error_yawRate + Ki_yaw_rate * integral_yawRate + Kd_yaw_rate * derivative_yawRate);

    error_yawRate_prev = error_yawRate;
    yaw_desired = ahr.yaw;
  }
}

void control_Rate(bool zero_integrators) {
  // Computes control commands based on rate error.

  // inputs: roll_des, pitch_des, yawRate_des
  // outputs: pid.roll, pid.pitch, pid.yaw

  // Desired values
  float rollRate_des = g_control_input.roll * maxRollRate;
  float pitchRate_des = -g_control_input.pitch * maxPitchRate;
  float yawRate_des = g_control_input.yaw * maxYawRate;

  // State vars
  static float integral_roll, error_roll_prev;
  static float integral_pitch, error_pitch_prev;
  static float integral_yaw, error_yaw_prev;

  // Zero the integrators
  if(zero_integrators) {
    integral_roll = 0;
    integral_pitch = 0;
    integral_yaw = 0;
  }

  // Roll
  float error_roll = rollRate_des - ahr.gx;
  integral_roll += error_roll * imu.dt;
  integral_roll = constrain(integral_roll, -i_limit, i_limit);
  float derivative_roll = (error_roll - error_roll_prev) / imu.dt;
  pid.roll = 0.01 * (Kp_ro_pi_rate * error_roll + Ki_ro_pi_rate * integral_roll + Kd_ro_pi_rate * derivative_roll);

  // Pitch
  float error_pitch = pitchRate_des - ahr.gy;
  integral_pitch += error_pitch * imu.dt;
  integral_pitch = constrain(integral_pitch, -i_limit, i_limit);
  float derivative_pitch = (error_pitch - error_pitch_prev) / imu.dt;
  pid.pitch = 0.01 * (Kp_ro_pi_rate * error_pitch + Ki_ro_pi_rate * integral_pitch + Kd_ro_pi_rate * derivative_pitch);

  // Yaw
  float error_yaw = yawRate_des - ahr.gz;
  integral_yaw += error_yaw * imu.dt;
  integral_yaw = constrain(integral_yaw, -i_limit, i_limit);
  float derivative_yaw = (error_yaw - error_yaw_prev) / imu.dt;
  pid.yaw = 0.01 * (Kp_yaw_rate * error_yaw + Ki_yaw_rate * integral_yaw + Kd_yaw_rate * derivative_yaw);

  // Update derivative variables
  error_roll_prev = error_roll;
  error_pitch_prev = error_pitch;
  error_yaw_prev = error_yaw;
}

void out_KillSwitchAndFailsafe() {
  // Change to ARMED when the active control link requests it.
  if(!out.armed() && g_control_input.armed && !g_attitude_lockout) {
    out.set_armed(true);
    Serial.println("OUT: ARMED");
    if(bbx.gizmo) bbx.start();
  }

  // Change to DISARMED when the active control link disarms or times out.
  if(out.armed() && (!g_control_input.armed || !g_control_input.connected)) {
    out.set_armed(false);
    if(!g_control_input.armed) {
      Serial.println("OUT: DISARMED");
      if(bbx.gizmo) bbx.stop();
    } else {
      Serial.println("OUT: DISARMED due to lost control link");
      // Keep on logging to document the crash.
    }
  }
}

void out_Mixer() {
  // DESCRIPTION: Mixes scaled commands from PID controller to actuator outputs.

  /*
  Motor order diagram

        front
   CW -->   <-- CCW
       3     2
        \ ^ /
         |X|
        / - \
       4     1
  CCW -->   <-- CW

                                          M1234
  Pitch up (stick back)   (front+ back-)   -++-
  Roll right              (left+ right-)   --++
  Yaw right               (CCW+ CW-)       -+-+
  */

  // IMPORTANT: This is a safety feature to remind the pilot to disarm.
  // Set motor outputs to at least armed_min_throttle to keep at least one prop spinning when armed.
  const float collective = effective_collective_command();
  float thr = armed_min_throttle + (1 - armed_min_throttle) * collective;

  if(collective <= 0.001f) {
    // If throttle idle, then run props at low speed without applying PID. This allows stick commands for arm/disarm.
    out.set_output(0, thr);
    out.set_output(1, thr);
    out.set_output(2, thr);
    out.set_output(3, thr);
  } else {
    // Quad mixing
    out.set_output(0, thr - pid.pitch - pid.roll - pid.yaw); // PWM1 / M1 Right-bottom CW, GPIO4
    out.set_output(1, thr + pid.pitch - pid.roll + pid.yaw); // PWM2 / M2 Right-top CCW, GPIO5
    out.set_output(2, thr + pid.pitch + pid.roll - pid.yaw); // PWM3 / M3 Left-top CW, GPIO6
    out.set_output(3, thr - pid.pitch + pid.roll + pid.yaw); // PWM4 / M4 Left-bottom CCW, GPIO7
  }
}
