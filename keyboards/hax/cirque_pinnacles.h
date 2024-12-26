#pragma once

#ifndef CIRQUE_PINNACLES_COUNT
#   define CIRQUE_PINNACLES_COUNT 1
#endif

#ifndef CIRQUE_PINNACLES_TOUCH_ZONES_X
#    define CIRQUE_PINNACLES_TOUCH_ZONES_X 3
#endif

#ifndef CIRQUE_PINNACLES_TOUCH_ZONES_Y
#    define CIRQUE_PINNACLES_TOUCH_ZONES_Y 3
#endif

typedef uint8_t cirque_pinnacles_touches_t[CIRQUE_PINNACLES_TOUCH_ZONES_Y][CIRQUE_PINNACLES_TOUCH_ZONES_X];

typedef struct {
    int16_t x;
    int16_t y;
    int16_t dx;
    int16_t dy;
    cirque_pinnacles_touches_t touches;
    // uint8_t touches[CIRQUE_PINNACLES_TOUCH_ZONES_Y][CIRQUE_PINNACLES_TOUCH_ZONES_X];
} cirque_pinnacles_state_t;

typedef struct {
    uint8_t address;
    bool    absolute;
    bool    flip_x;
    bool    flip_y;
    bool    swap_xy;
    cirque_pinnacles_state_t state;
} cirque_pinnacles_device_t;

void cirque_pinnacles_init(cirque_pinnacles_device_t devices[CIRQUE_PINNACLES_COUNT]);
bool cirque_pinnacles_update_state(cirque_pinnacles_device_t* device);
bool cirque_pinnacles_update_states(cirque_pinnacles_device_t device[CIRQUE_PINNACLES_COUNT]);
