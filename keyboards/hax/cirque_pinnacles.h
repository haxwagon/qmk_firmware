#pragma once


#ifndef CIRQUE_PINNACLES_TOUCH_ZONES_X
#    define CIRQUE_PINNACLES_TOUCH_ZONES_X 1
#endif

#ifndef CIRQUE_PINNACLES_TOUCH_ZONES_Y
#    define CIRQUE_PINNACLES_TOUCH_ZONES_Y 1
#endif

#ifndef CIRQUE_PINNACLES_DEADZONE
#    define CIRQUE_PINNACLES_DEADZONE 0
#endif

#ifndef CIRQUE_PINNACLES_COUNT
#    define CIRQUE_PINNACLES_COUNT 1
#endif

typedef uint8_t cirque_pinnacles_touches_t[CIRQUE_PINNACLES_TOUCH_ZONES_Y][CIRQUE_PINNACLES_TOUCH_ZONES_X];

typedef struct {
    int16_t                    x;
    int16_t                    y;
    int16_t                    prev_x;
    int16_t                    prev_y;
    bool                       flip_x;
    bool                       flip_y;
    bool                       swap_xy;
    cirque_pinnacles_touches_t touches;
} cirque_pinnacles_state_t;


void cirque_pinnacles_init(uint8_t spi_cs_pin);

typedef enum {
    NO_DATA_READ,
    DATA_HANDLED,
    DATA_UPDATED,
} cirque_pinnacles_read_data_result_t;
cirque_pinnacles_read_data_result_t cirque_pinnacles_read_data(uint8_t spi_cs_pin, cirque_pinnacles_state_t* state);
