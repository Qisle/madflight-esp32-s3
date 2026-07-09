#include "ws2812_status_led.h"

#include "led/led.h"
#include "out/out.h"
#include "rcl/rcl.h"

static uint8_t triwave8(uint8_t v) {
  return (v & 0x80) ? (uint8_t)(255 - ((v & 0x7F) << 1)) : (uint8_t)((v & 0x7F) << 1);
}

static uint32_t color_wheel(uint8_t pos) {
  pos = 255 - pos;
  if(pos < 85) {
    return ((uint32_t)(255 - pos * 3) << 16) | (uint32_t)(pos * 3);
  }
  if(pos < 170) {
    pos -= 85;
    return ((uint32_t)(pos * 3) << 8) | (uint32_t)(255 - pos * 3);
  }
  pos -= 170;
  return ((uint32_t)(pos * 3) << 16) | ((uint32_t)(255 - pos * 3) << 8);
}

static uint32_t scale_rgb(uint32_t rgb, uint8_t scale) {
  uint32_t r = ((rgb >> 16) & 0xFF) * scale / 255;
  uint32_t g = ((rgb >> 8) & 0xFF) * scale / 255;
  uint32_t b = (rgb & 0xFF) * scale / 255;
  return (r << 16) | (g << 8) | b;
}

void ws2812_status_led_begin() {
  if(!led.installed()) return;
  led.color(0x001020);
}

void ws2812_status_led_update() {
  if(!led.installed()) return;

  uint32_t now = millis();

  if(out.armed()) {
    // Armed: vivid red heartbeat to keep the state unmistakable.
    uint8_t pulse = 48 + ((uint16_t)triwave8((uint8_t)(now >> 2)) * 176U) / 255U;
    led.color(scale_rgb(0xFF1800, pulse));
    return;
  }

  if(rcl.installed() && !rcl.connected()) {
    // Receiver installed but not connected: warm amber pulse.
    uint8_t pulse = 24 + ((uint16_t)triwave8((uint8_t)(now >> 3)) * 140U) / 255U;
    led.color(scale_rgb(0xFF7000, pulse));
    return;
  }

  // Normal idle: smooth color cycle with a soft breathing envelope.
  uint8_t hue = (uint8_t)(now >> 4);
  uint8_t breathe = 36 + ((uint16_t)triwave8((uint8_t)(now >> 3)) * 156U) / 255U;
  led.color(scale_rgb(color_wheel(hue), breathe));
}
