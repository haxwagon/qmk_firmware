#pragma once

#define CIRQUE_PINNACLES_COUNT 2
#define CIRQUE_PINNACLES_DEADZONE_CENTER 2
#define CIRQUE_PINNACLES_DEADZONE_EDGES 4096
#define CIRQUE_PINNACLES_DUAL_JOYSTICKS
#define CIRQUE_PINNACLES_POINTING_DEVICE
#define CIRQUE_PINNACLES_SCROLL_REVERSE 1
#define CIRQUE_PINNACLES_SCROLL_THRESHOLD 256
#define CIRQUE_PINNACLES_SPI_CS_PIN_LEFT GP5
#define CIRQUE_PINNACLES_SPI_CS_PIN_RIGHT GP17
#define CIRQUE_PINNACLES_TAP_ZONES_X 3
#define CIRQUE_PINNACLES_TAP_ZONES_Y 3
#define CIRQUE_PINNACLE_ATTENUATION EXTREG__TRACK_ADCCONFIG__ADC_ATTENUATE_2X
#define CIRQUE_PINNACLE_CIRCULAR_SCROLL_ENABLE
#define CIRQUE_PINNACLE_CLOCK_SPEED 10000000
#define CIRQUE_PINNACLE_CURVED_OVERLAY
#define CIRQUE_PINNACLE_DIAMETER_MM 35
#define CIRQUE_PINNACLE_POSITION_MODE CIRQUE_PINNACLE_ABSOLUTE_MODE
#define CIRQUE_PINNACLE_SPI_CS_PIN CIRQUE_PINNACLES_SPI_CS_PIN_RIGHT
#define CIRQUE_PINNACLE_SPI_DIVISOR 8
#define CIRQUE_PINNACLE_SPI_LSB_FIRST false
#define CIRQUE_PINNACLE_SPI_MODE 1
#define CIRQUE_PINNACLE_TAP_ENABLE
#define DYNAMIC_MACRO_NO_NESTING
#define DYNAMIC_MACRO_SIZE 256
#define I2C1_SCL_PIN GP27
#define I2C1_SDA_PIN GP26
#define I2C_DRIVER I2CD1
#define JOYSTICK_AXIS_COUNT 6
#define JOYSTICK_AXIS_RESOLUTION 10
#define JOYSTICK_BUTTON_COUNT 12
#define JOYSTICK_HAS_HAT
#define OLED_BRIGHTNESS 128
#define OLED_DISPLAY_128X64
#define OLED_DISPLAY_ADDRESS 0x3C
#define PERMISSIVE_HOLD
#define POINTING_DEVICE_DEFAULT_DPI 5
#define POINTING_DEVICE_DRIVER_cirque_pinnacle_spi
#define POINTING_DEVICE_GESTURES_SCROLL_ENABLE
#define POINTING_DEVICE_MAX_DPI 16
#define POINTING_DEVICE_MIN_DPI 1
#define SPI_DRIVER SPID0
#define SPI_MISO_PIN GP4
#define SPI_MOSI_PIN GP3
#define SPI_SCK_PIN GP2
#define TAPPING_TERM 150
#define TAPPING_TERM_PER_KEY
#define TAPPING_TOGGLE 2
