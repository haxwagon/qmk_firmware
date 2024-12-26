#pragma once

#ifndef POINTING_DEVICE_DEFAULT_DPI
#    define POINTING_DEVICE_DEFAULT_DPI 5
#endif
#ifndef POINTING_DEVICE_MAX_DPI
#    define POINTING_DEVICE_MAX_DPI 16
#endif
#ifndef POINTING_DEVICE_MIN_DPI
#    define POINTING_DEVICE_MIN_DPI 1
#endif

uint8_t get_pointing_dpi(void);
void increase_pointing_dpi(uint8_t delta);
void set_pointing_dpi(uint8_t dpi);
