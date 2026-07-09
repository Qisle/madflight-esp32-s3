# madflight ESP32-S3 indoor flight controller

This repository contains the key project code for an ESP32-S3 indoor quadcopter flight controller based on madflight 2.3.0.

It is intentionally a small source release, not a full copy of the upstream madflight repository. The included files are the project-specific firmware entry point, board/sensor configuration, Web Cockpit control path, PID control code, online tuning code, WS2812 status LED code, and PlatformIO/library metadata needed to understand the build target.

## Project Scope

- Target MCU: ESP32-S3
- Build system: PlatformIO
- Vehicle type: quadcopter
- Pilot input: embedded Web Cockpit over WiFi
- Control modes: RATE, ANGLE, ALT
- IMU: BMI270 over SPI
- Barometer: BMP580 over I2C
- Magnetometer: QMC6309 over I2C
- Status LED: WS2812 on GPIO45
- Motor outputs: GPIO4, GPIO5, GPIO6, GPIO7

## Included Code

- `platformio.ini`: ESP32-S3 PlatformIO environment, USB CDC, PSRAM, and 16 MB flash configuration.
- `library.properties`: Arduino library metadata inherited from the madflight project layout.
- `src/main.cpp`: quadcopter control program, Web Cockpit input integration, flight mode handling, ALT control, arming/failsafe path, and motor mixing.
- `src/madflight_config.h`: board, sensor, bus, pin, and output configuration for the current ESP32-S3 hardware.
- `src/usr/usr_mobile_ui.cpp`: ESP32 WiFi/WebServer/API/UDP control logic and telemetry endpoints.
- `src/usr/usr_mobile_ui.h`: Web Cockpit control-state interface used by the flight controller.
- `src/usr/usr_mobile_ui_page.h`: embedded HTML/CSS/JS Web Cockpit page served from firmware.
- `src/usr/usr_quad_tuning.cpp` and `src/usr/usr_quad_tuning.h`: online tunable quadcopter parameters and defaults.
- `src/pid/*`: PID controller state, controller implementation, and low-pass filter helper used by the control loop.
- `src/ws2812_status_led.cpp` and `src/ws2812_status_led.h`: WS2812 status LED helper.

## Not Included

This repository does not include:

- Local assistant/workspace configuration files.
- Development notes, rendered reports, logs, generated caches, or build output.
- Large upstream madflight modules, board databases, generated protocol libraries, examples, images, PDFs, or tooling not specific to this ESP32-S3 build.

To build a complete firmware image, place these files into a compatible madflight 2.3.0 source tree or restore the required upstream madflight library modules through your normal dependency workflow.

## Safety Notice

Flight-control firmware can damage hardware or cause injury if configured or tested incorrectly. Verify sensor orientation, pin mapping, motor order, arming behavior, failsafe behavior, and output limits on the bench before any powered flight test.
