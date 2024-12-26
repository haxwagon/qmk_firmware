#include QMK_KEYBOARD_H
#include "pointing_dpi.h"

static uint8_t pointing_device_dpi = POINTING_DEVICE_DEFAULT_DPI;

uint8_t get_pointing_dpi(void) {
    return pointing_device_dpi;
}

void increase_pointing_dpi(uint8_t delta) {
    set_pointing_dpi(pointing_device_dpi + delta);
}

void set_pointing_dpi(uint8_t dpi) {
    pointing_device_dpi = dpi;
    if (pointing_device_dpi < POINTING_DEVICE_MIN_DPI) {
        pointing_device_dpi = POINTING_DEVICE_MIN_DPI;
    } else if (pointing_device_dpi > POINTING_DEVICE_MAX_DPI) {
        pointing_device_dpi = POINTING_DEVICE_MAX_DPI;
    }
    dprintf("DPI: %d\n", pointing_device_dpi);
}
