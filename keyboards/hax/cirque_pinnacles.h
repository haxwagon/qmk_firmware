#pragma once

#ifndef CIRQUE_PINNACLES_COUNT
#define CIRQUE_PINNACLES_COUNT 1
#endif

#ifndef CIRQUE_PINNACLES_DEADZONE_CENTER
#define CIRQUE_PINNACLES_DEADZONE_CENTER 0
#endif

#ifndef CIRQUE_PINNACLES_DEADZONE_EDGES
#define CIRQUE_PINNACLES_DEADZONE_EDGES 0
#endif

#ifndef CIRQUE_PINNACLES_DEADZONE_BOTTOM
#define CIRQUE_PINNACLES_DEADZONE_BOTTOM CIRQUE_PINNACLES_DEADZONE_EDGES
#endif

#ifndef CIRQUE_PINNACLES_DEADZONE_LEFT
#define CIRQUE_PINNACLES_DEADZONE_LEFT CIRQUE_PINNACLES_DEADZONE_EDGES
#endif

#ifndef CIRQUE_PINNACLES_DEADZONE_RIGHT
#define CIRQUE_PINNACLES_DEADZONE_RIGHT CIRQUE_PINNACLES_DEADZONE_EDGES
#endif

#ifndef CIRQUE_PINNACLES_DEADZONE_TOP
#define CIRQUE_PINNACLES_DEADZONE_TOP CIRQUE_PINNACLES_DEADZONE_EDGES
#endif

#ifndef CIRQUE_PINNACLES_SCROLL_THRESHOLD
#define CIRQUE_PINNACLES_SCROLL_THRESHOLD 256
#endif

#ifndef CIRQUE_PINNACLES_TAP_TERM
#define CIRQUE_PINNACLES_TAP_TERM 200
#endif

#ifndef CIRQUE_PINNACLES_TAP_ZONES_X
#define CIRQUE_PINNACLES_TAP_ZONES_X 1
#endif

#ifndef CIRQUE_PINNACLES_TAP_ZONES_Y
#define CIRQUE_PINNACLES_TAP_ZONES_Y 1
#endif

typedef struct {
    int16_t x;
    int16_t y;
    bool touching;
    uint16_t touched_at;
    uint16_t tapped;
    uint16_t tap_x;
    uint16_t tap_y;
    int16_t prev_x;
    int16_t prev_y;
} cirque_pinnacles_state_t;

typedef struct {
    pin_t spi_cs_pin;
    bool flip_x;
    bool flip_y;
    bool swap_xy;
} cirque_pinnacles_config_t;

void cirque_pinnacles_init(void);

typedef enum {
    NO_DATA_READ,
    DATA_HANDLED,
    DATA_UPDATED,
} cirque_pinnacles_read_data_result_t;
cirque_pinnacles_read_data_result_t cirque_pinnacles_read_data(uint8_t spi_cs_pin, cirque_pinnacles_state_t* state);

void cirque_pinnacles_clear_active_keys(uint8_t cirque_index);

#if defined(CIRQUE_PINNACLES_POINTING_DEVICE)
typedef enum {
    POINTING_DEVICE_BUTTONS,
    POINTING_DEVICE_HV,
    POINTING_DEVICE_XY,
} cirque_pinnacles_pointing_device_mode_t;
#endif
