# madflight ESP32-S3 indoor flight controller

This repository contains the key project code for an ESP32-S3 indoor quadcopter flight controller based on madflight 2.3.0.

It is intentionally a source-code release focused on the firmware project, not a full workspace dump. The repository includes the complete `src/` directory from the current ESP32-S3 project, plus the root metadata needed to understand the PlatformIO target.

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
- `src/`: complete firmware source directory from the current project.

Important project-specific files inside `src/` include:

- `src/main.cpp`: quadcopter control program, Web Cockpit input integration, flight mode handling, ALT control, arming/failsafe path, and motor mixing.
- `src/madflight_config.h`: board, sensor, bus, pin, and output configuration for the current ESP32-S3 hardware.
- `src/usr/*`: ESP32 Web Cockpit, WiFi/WebServer/API/UDP control logic, telemetry endpoints, embedded HTML/CSS/JS page, and online tuning defaults.
- `src/pid/*`: PID controller state, controller implementation, and low-pass filter helper used by the control loop.
- `src/ws2812_status_led.*`: WS2812 status LED helper.

## Not Included

This repository does not include:

- Local assistant/workspace configuration files.
- Development notes, rendered reports, logs, generated caches, or build output.
- Non-source workspace material such as examples, images, PDFs, reports, and auxiliary tooling outside `src/`.

To build a complete firmware image, use this repository as a PlatformIO project source tree and restore any required external toolchain dependencies through your normal PlatformIO setup.

## Safety Notice

Flight-control firmware can damage hardware or cause injury if configured or tested incorrectly. Verify sensor orientation, pin mapping, motor order, arming behavior, failsafe behavior, and output limits on the bench before any powered flight test.
