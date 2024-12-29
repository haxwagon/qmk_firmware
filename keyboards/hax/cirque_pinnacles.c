#include QMK_KEYBOARD_H

#include "cirque_pinnacles.h"
#include "drivers/sensors/cirque_pinnacle.h"
#include "spi_master.h"

__attribute__((weak)) bool    cirque_pinnacles_tapped(uint8_t spi_cs_pin, uint8_t x, uint8_t y) { return false; }
__attribute__((weak)) bool    cirque_pinnacles_moved(uint8_t spi_cs_pin, int16_t x, int16_t y, int16_t dx, int16_t dy) { return false; }

#define CIRQUE_PINNACLES_TOUCH_ZONES_X_SIZE (UINT16_MAX / CIRQUE_PINNACLES_TOUCH_ZONES_X)
#define CIRQUE_PINNACLES_TOUCH_ZONES_Y_SIZE (UINT16_MAX / CIRQUE_PINNACLES_TOUCH_ZONES_Y)
#define CONSTRAIN(x, min, max) ((x) < min ? min : ((x) > max ? max : (x)))

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

float linear_scale(float value, float min_in, float max_in, float min_out, float max_out) {
    value = CONSTRAIN(value, min_in, max_in);
    value -= min_in;
    return (value * (max_out - min_out) / (max_in - min_in)) + min_out;
}

cirque_pinnacles_read_data_result_t cirque_pinnacles_read_data(uint8_t spi_cs_pin, cirque_pinnacles_state_t* state) {
    cirque_pinnacles_select(spi_cs_pin);
    pinnacle_data_t data = cirque_pinnacle_read_data();
    if (!data.valid) {
        return NO_DATA_READ;
    }

    state->prev_x = state->x;
    state->prev_y = state->y;

    cirque_pinnacle_scale_data(&data, UINT16_MAX, UINT16_MAX);

    if (data.touchDown && data.zValue > 0) { // actively touching
        // handle scaling removing edge dead zones
        state->x = linear_scale(data.xValue, CIRQUE_PINNACLES_LEFT_DEADZONE, UINT16_MAX - CIRQUE_PINNACLES_RIGHT_DEADZONE, INT16_MIN, INT16_MAX);
        state->y = linear_scale(data.yValue, CIRQUE_PINNACLES_TOP_DEADZONE, UINT16_MAX - CIRQUE_PINNACLES_BOTTOM_DEADZONE, INT16_MIN, INT16_MAX);

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
        if (state->x > -CIRQUE_PINNACLES_CENTER_DEADZONE && state->x < CIRQUE_PINNACLES_CENTER_DEADZONE) {
            state->x = 0;
        }
        if (state->y > -CIRQUE_PINNACLES_CENTER_DEADZONE && state->y < CIRQUE_PINNACLES_CENTER_DEADZONE) {
            state->y = 0;
        }

        state->touch_x = CONSTRAIN(
            linear_scale(state->x, INT16_MIN, INT16_MAX, 0, UINT16_MAX) / CIRQUE_PINNACLES_TOUCH_ZONES_X_SIZE,
            0, CIRQUE_PINNACLES_TOUCH_ZONES_X - 1);
        state->touch_y = CONSTRAIN(
            linear_scale(state->y, INT16_MIN, INT16_MAX, 0, UINT16_MAX) / CIRQUE_PINNACLES_TOUCH_ZONES_Y_SIZE,
            0, CIRQUE_PINNACLES_TOUCH_ZONES_Y - 1);

        if (!state->touching) {
            // started touching
            state->touched_at = timer_read();
        }
        state->touching = true;
    } else { // not actively touching
        bool tapped = false;
        if (state->touching && state->touched_at > 0 && timer_elapsed(state->touched_at) < CIRQUE_PINNACLES_TAP_TERM) {
            // stopped touching quickly enough to be a tap
            tapped = true;
        }
        state->touching = false;
        state->touched_at = 0;
        if (tapped) {
            dprintf("Cirque Pinnacles (%d) tapped at (%d, %d)\n", spi_cs_pin, state->touch_x, state->touch_y);
            if (cirque_pinnacles_tapped(spi_cs_pin, state->touch_x, state->touch_y)) {
                return DATA_HANDLED;
            }
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
