#include QMK_KEYBOARD_H

#include "cirque_pinnacles.h"
#include "drivers/sensors/cirque_pinnacle.h"
#include "spi_master.h"

__attribute__((weak)) bool    cirque_pinnacles_tapped(uint8_t spi_cs_pin, uint8_t x, uint8_t y) { return false; }
__attribute__((weak)) bool    cirque_pinnacles_moved(uint8_t spi_cs_pin, int16_t x, int16_t y, int16_t dx, int16_t dy) { return false; }

static const uint16_t TOUCH_ZONE_X_SIZE = UINT16_MAX / CIRQUE_PINNACLES_TOUCH_ZONES_X;
static const uint16_t TOUCH_ZONE_Y_SIZE = UINT16_MAX / CIRQUE_PINNACLES_TOUCH_ZONES_Y;

static uint8_t _spi_cs_pin = 0;

void cirque_pinnacles_select(uint8_t spi_cs_pin) {
    _spi_cs_pin = spi_cs_pin;
}

uint8_t cirque_pinnacle_spi_get_cs_pin(void) { return _spi_cs_pin; }

void cirque_pinnacles_init(uint8_t spi_cs_pin) {
    spi_init();

    for (uint8_t i = 0; i < CIRQUE_PINNACLES_COUNT; i++) {
        cirque_pinnacles_select(i);
        cirque_pinnacle_init();
    }
}

cirque_pinnacles_read_data_result_t cirque_pinnacles_read_data(uint8_t spi_cs_pin, cirque_pinnacles_state_t* state) {
    pinnacle_data_t data = cirque_pinnacle_read_data();
    if (!data.valid) {
        return NO_DATA_READ;
    }

    state->prev_x = state->x;
    state->prev_y = state->y;

    cirque_pinnacle_scale_data(&data, UINT16_MAX, UINT16_MAX);
    state->x = ((int16_t)(data.xValue / 2) - (INT16_MAX / 4)) * 2;
    state->y = ((int16_t)(data.yValue / 2) - (INT16_MAX / 4)) * 2;
    if (state->swap_xy) {
        int16_t temp = state->x;
        state->x = state->y;
        state->y = temp;
    }
    if (state->flip_x) {
        state->x *= -1;
    }
    if (state->flip_y) {
        state->y *= -1;
    }
    if (state->x > -CIRQUE_PINNACLES_DEADZONE && state->x < CIRQUE_PINNACLES_DEADZONE) {
        state->x = 0;
    }
    if (state->y > -CIRQUE_PINNACLES_DEADZONE && state->y < CIRQUE_PINNACLES_DEADZONE) {
        state->y = 0;
    }

    state->touched = false;
    state->touch_x = 0;
    state->touch_y = 0;
    if (data.touchDown) {
        state->touched = true;
        state->touch_x = data.xValue / TOUCH_ZONE_X_SIZE;
        state->touch_y = data.yValue / TOUCH_ZONE_Y_SIZE;
        if (cirque_pinnacles_tapped(spi_cs_pin, state->touch_x, state->touch_y)) {
            return DATA_HANDLED;
        }
    }

    if (state->x == 0 && state->y == 0 && state->prev_x == 0 && state->prev_y == 0) {
        // nothing interesting this round, don't do anything
        return NO_DATA_READ;
    }

    if (cirque_pinnacles_moved(spi_cs_pin, state->x, state->y, state->x - state->prev_x, state->y - state->prev_y)) {
        // already handled
        return DATA_HANDLED;
    }

    return DATA_UPDATED;
}

// uint16_t cirque_pinnacle_get_cpi(void) {
//     return CIRQUE_PINNACLE_PX_TO_INCH(cirque_pinnacle_get_scale());
// }
// void cirque_pinnacle_set_cpi(uint16_t cpi) {
//     cirque_pinnacle_set_scale(CIRQUE_PINNACLE_INCH_TO_PX(cpi));
// }

// // clang-format off
// const pointing_device_driver_t cirque_pinnacle_pointing_device_driver = {
//     .init       = cirque_pinnacle_init,
//     .get_report = cirque_pinnacle_get_report,
//     .set_cpi    = cirque_pinnacle_set_cpi,
//     .get_cpi    = cirque_pinnacle_get_cpi
// };
