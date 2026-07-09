/*========================================================================================================================
                                              MADFLIGHT CONFIG
==========================================================================================================================

The configuration is loaded from two strings: first from madflight_board in from the board header in /brd/, and then from 
madflight_config in this file. If the same setting occurs more than once, the last one is applied.

The strings are multi-line raw strings in with a key-value list. Anything after '#' or '/' is ignored as comment.

You have 3 options to setup the flight controller:

  1) Default - Keep #define MF_BOARD "brd/default.h", this defines the default pinout as shown on https://madflight.com 
     for the supported processor families. Now edit madflight_config below, and uncomment and configure imu_gizmo, 
     imu_bus_type, rcl_gizmo, and other lines as needed. (Do not change the pin_xxx_yyy and xxx_yyy_bus settings.)
 
  2) BetaFlight - Change "brd/default.h" to the BetaFlight flight controller you want to use, for example: 
     #define MF_BOARD "brd/betaflight/MTKS-MATEKH743.h". See library/madflight/src for all available boards. Edit 
     madflight_config to fine-tune the configuration.
 
  3) Bare Metal - Comment out "#define MF_BOARD", and set the full configuration in madflight_config below.

Pins and spi/i2c/serial busses use zero-based numbering, i.e. "gps_ser_bus 0" connects the GPS to the first serial bus
with pins pin_ser0_tx and pin_ser0_rx. Pins use GPIO numbers, not physical pin numbers. Use -1 to disable a pin or bus.

You can also modify the configuration from the CLI, for example "set imu_gizmo MPU6500" or "set imu_spi_bus 1", then
use "save" to save the config to eeprom and reboot to use the new config.

If things do not work as expected, have a good look at the startup messages!

========================================================================================================================*/

// define the board header to use, or comment out for none
#define MF_BOARD "brd/default.h"

const char madflight_config[] = R""(

//--- IMU --- Inertial Measurement Unit  (use spi -OR- i2c bus)
imu_gizmo      BMI270 // options: NONE, BMI270, ICM42688, ICM45686, MPU6000, MPU6050, MPU6500, MPU9150, MPU9250
imu_bus_type   SPI    // options: SPI, I2C (not all combinations of gizmo and bus_type are supported)
//imu_align      CW0   // options: CW0, CW90, CW180, CW270, CW0FLIP, CW90FLIP, CW180FLIP, CW270FLIP
imu_spi_bus    0      // spi
pin_imu_cs     12     // spi
pin_imu_int    8      // spi and i2c
imu_i2c_bus    -1     // i2c
//imu_i2c_adr     0    // i2c: enter decimal i2c address, not hex (use 0 for default i2c address)

// IMPORTANT: the IMU sensor should be the ONLY sensor on the selected bus, interrupt pin is required

pin_spi0_miso  9
pin_spi0_mosi  10
pin_spi0_sclk  11
pin_i2c0_sda   1
pin_i2c0_scl   2
pin_i2c1_sda  -1
pin_i2c1_scl  -1


//--- RCL --- Remote Controller Link  (use serial bus -OR- ppm pin)
rcl_gizmo      NONE  // Web UI is the only pilot control link for this build.
rcl_ser_bus   -1
pin_rcl_ppm   -1
rcl_num_ch     8
rcl_deadband   0

//--- BAR --- Barometer
bar_gizmo      BMP580 // options: NONE, BMP280, BMP388, BMP390, BMP580, HP203B, MS5611
bar_i2c_adr    70
bar_i2c_bus    0

//--- MAG --- Magnetometer
mag_gizmo      QMC6309 // options: NONE, QMC6309, QMC5883L, QMC5883P, RM3100
//mag_align      CW0   // options: CW0, CW90, CW180, CW270, CW0FLIP, CW90FLIP, CW180FLIP, CW270FLIP
//mag_i2c_adr    0
mag_i2c_bus    0

//--- BAT --- Battery Monitor  (use i2c bus -OR- adc pins)
bat_gizmo      NONE  // options: NONE, ADC, INA226, INA228
bat_i2c_adr    0
bat_i2c_bus   -1
pin_bat_i     -1
pin_bat_v     -1
bat_cal_v      1
bat_cal_i      1

//--- GPS ---
gps_gizmo      NONE  // Indoor DWM3000 build does not use GPS.
gps_baud       0
gps_ser_bus   -1

//--- BBX --- Black Box Data Logger  (use spi -OR- mmc)
bbx_gizmo      NONE  // options: NONE, SDSPI, SDMMC
pin_bbx_cs    -1
bbx_spi_bus   -1
pin_mmc_dat   -1
pin_mmc_clk   -1
pin_mmc_cmd   -1

//--- RDR --- Radar (use serial bus -OR- trig+echo pins)
rdr_gizmo      NONE  // options: NONE, DTS6012M, LD2411S, LD2413, SR04, USD1
rdr_ser_bus   -1
rdr_baud       0
pin_rdr_trig  -1
pin_rdr_echo  -1

//--- OFL --- Optical FLow (use serial bus -OR- spi bus)
ofl_gizmo      NONE  // options: NONE, PMW3901, PMW3901U
ofl_ser_bus   -1
ofl_baud       0
ofl_spi_bus   -1
pin_ofl_cs    -1

//--- LED ---
led_gizmo     RGB    // options: NONE, HIGH_IS_ON, LOW_IS_ON, RGB
pin_led       45

//--- AHR --- AHRS (keep MAHONY, unless you want to experiment)
//ahr_gizmo      MAHONY // options: MAHONY, MAHONY_BF, MADGWICK, VQF

//--- Serial bus 0 ---
//pin_ser0_rx   -1
//pin_ser0_tx   -1

//--- Serial bus 1 ---
//pin_ser1_rx   -1
//pin_ser1_tx   -1 

//--- SPI bus 0 ---
//pin_spi0_miso -1
//pin_spi0_mosi -1
//pin_spi0_sclk -1

//--- SPI bus 1 ---
//pin_spi1_miso -1
//pin_spi1_mosi -1
//pin_spi1_sclk -1

//--- I2C Bus 0 ---
//pin_i2c0_sda  -1
//pin_i2c0_scl  -1

//--- I2C Bus 1 ---
//pin_i2c1_sda  -1
//pin_i2c1_scl  -1

//--- OUT Pins ---
pin_out0       4  // PWM1 / M1: right-bottom
pin_out1       5  // PWM2 / M2: right-top
pin_out2       6  // PWM3 / M3: left-top
pin_out3       7  // PWM4 / M4: left-bottom
pin_out4      -1
pin_out5      -1
//pin_out6      -1
//pin_out7      -1
//pin_out8      -1
//pin_out9      -1
//pin_out10     -1
//pin_out11     -1
//pin_out12     -1
//pin_out13     -1
//pin_out14     -1
//pin_out15     -1

)""; // End of madflight_config


//========================================================================================================================//
//                                               COMPILER OPTIONS                                                         //
//========================================================================================================================//

// Reset config eeprom to defaults (uncomment this, upload, execute, then comment out, and upload again)
//#define MF_CONFIG_CLEAR

// Uncomment to reduce startup delay
//#define MF_DEBUG
