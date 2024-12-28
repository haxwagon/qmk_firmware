#pragma once

#define CIRQUE_PINNACLE_CIRCULAR_SCROLL_ENABLE
#define CIRQUE_PINNACLE_CUSTOM
#define CIRQUE_PINNACLE_DIAMETER_MM 35
#define CIRQUE_PINNACLE_TAP_ENABLE
#define CIRQUE_PINNACLES_DEADZONE 16 // up to half of int16_t
#define CIRQUE_PINNACLES_TOUCH_ZONES_X 3
#define CIRQUE_PINNACLES_TOUCH_ZONES_Y 3
// #define DYNAMIC_MACRO_NO_NESTING
// #define DYNAMIC_MACRO_SIZE 256
// #define ENCODER_A_PINS {GP4, GP6 }
// #define ENCODER_B_PINS { GP5, GP7 }
#define I2C_DRIVER I2CD1
#define I2C1_SCL_PIN GP27
#define I2C1_SDA_PIN GP26
#define JOYSTICK_AXIS_COUNT 6
#define JOYSTICK_AXIS_RESOLUTION 10
#define JOYSTICK_BUTTON_COUNT 12
#define JOYSTICK_HAS_HAT
// #define OLED_BRIGHTNESS 128
// #define OLED_DISPLAY_128X64
// #define OLED_DISPLAY_ADDRESS 0x3C
#define PERMISSIVE_HOLD
// #define POINTING_DEVICE_DEFAULT_DPI 5
// #define POINTING_DEVICE_MAX_DPI 16
// #define POINTING_DEVICE_MIN_DPI 1

// // --- I2C ---
// #ifndef PICO_DEFAULT_I2C
// #define PICO_DEFAULT_I2C 1
// #endif
// #ifndef PICO_DEFAULT_I2C_SDA_PIN
// #define PICO_DEFAULT_I2C_SDA_PIN 2
// #endif
// #ifndef PICO_DEFAULT_I2C_SCL_PIN
// #define PICO_DEFAULT_I2C_SCL_PIN 3
// #endif

// // --- SPI ---
// #ifndef PICO_DEFAULT_SPI
// #define PICO_DEFAULT_SPI 0
// #endif
// #ifndef PICO_DEFAULT_SPI_SCK_PIN
// #define PICO_DEFAULT_SPI_SCK_PIN 6
// #endif
// #ifndef PICO_DEFAULT_SPI_TX_PIN
// #define PICO_DEFAULT_SPI_TX_PIN 7
// #endif
// #ifndef PICO_DEFAULT_SPI_RX_PIN
// #define PICO_DEFAULT_SPI_RX_PIN 4
// #endif
// #ifndef PICO_DEFAULT_SPI_CSN_PIN
// #define PICO_DEFAULT_SPI_CSN_PIN 5
// #endif
#define SPI_MODE 3
#define SPI_LSB_FIRST false
#define SPI_DRIVER SPID0
#define SPI_SCK_PIN GP6
#define SPI_SCK_PAL_MODE 5
#define SPI_MOSI_PIN GP7
#define SPI_MOSI_PAL_MODE 5
#define SPI_MISO_PIN GP4
#define SPI_MISO_PAL_MODE 5
#define SPI_DIVISOR 16
#define CIRQUE_PINNACLE_SPI_DIVISOR 8
#define CIRQUE_PINNACLE_SPI_CS_PIN GP5

#define CIRQUE_PINNACLE_TAP_ENABLE
#define POINTING_DEVICE_GESTURES_SCROLL_ENABLE

// #define TAPPING_TERM 150
// #define TAPPING_TERM_PER_KEY
// #define TAPPING_TOGGLE 2
