#include QMK_KEYBOARD_H

#include "cirque_pinnacles.h"

#include "drivers/sensors/cirque_pinnacle.h"
#include "spi_master.h"

__attribute__((weak)) cirque_pinnacles_config_t cirque_pinnacles_get_config(uint8_t cirque_index)
{
    cirque_pinnacles_config_t config = {
        .spi_cs_pin = 0,
        .flip_x = false,
        .flip_y = false,
        .swap_xy = false,
    };
    return config;
}

__attribute__((weak)) bool cirque_pinnacles_tapped(uint8_t cirque_index, uint8_t x, uint8_t y)
{
    return false;
}
__attribute__((weak)) bool cirque_pinnacles_moved(uint8_t cirque_index, int16_t x, int16_t y, int16_t dx, int16_t dy)
{
    return false;
}

#define CIRQUE_PINNACLES_TAP_ZONES_X_SIZE (UINT16_MAX / CIRQUE_PINNACLES_TAP_ZONES_X)
#define CIRQUE_PINNACLES_TAP_ZONES_Y_SIZE (UINT16_MAX / CIRQUE_PINNACLES_TAP_ZONES_Y)
#define CONSTRAIN(x, min, max) ((x) < min ? min : ((x) > max ? max : (x)))

static cirque_pinnacles_config_t cirque_pinnacles_configs[CIRQUE_PINNACLES_COUNT];
static uint8_t _current_spi_cs_pin = 0;

void cirque_pinnacles_select(uint8_t cirque_index)
{
    _current_spi_cs_pin = cirque_pinnacles_configs[cirque_index].spi_cs_pin;
}

uint8_t cirque_pinnacle_spi_get_cs_pin(void)
{
    return _current_spi_cs_pin;
}

void cirque_pinnacles_init(void)
{
    spi_init();

    for (uint8_t i = 0; i < CIRQUE_PINNACLES_COUNT; i++) {
        cirque_pinnacles_configs[i] = cirque_pinnacles_get_config(i);
        cirque_pinnacles_select(i);
        cirque_pinnacle_init();
    }
}

float linear_scale(float value, float min_in, float max_in, float min_out, float max_out)
{
    value = CONSTRAIN(value, min_in, max_in);
    value -= min_in;
    value = (value * (max_out - min_out) / (max_in - min_in)) + min_out;
    if (value > max_out) {
        value = max_out;
    }
    return value;
}

cirque_pinnacles_read_data_result_t cirque_pinnacles_read_data(uint8_t cirque_index, cirque_pinnacles_state_t* state)
{
    cirque_pinnacles_select(cirque_index);
    pinnacle_data_t data = cirque_pinnacle_read_data();
    if (!data.valid) {
        return NO_DATA_READ;
    }

    state->prev_x = state->x;
    state->prev_y = state->y;

    cirque_pinnacle_scale_data(&data, UINT16_MAX, UINT16_MAX);

    if (data.touchDown && data.zValue > 0) { // actively touching
        // handle scaling removing edge dead zones
        state->x = linear_scale(data.xValue, CIRQUE_PINNACLES_DEADZONE_LEFT, UINT16_MAX - CIRQUE_PINNACLES_DEADZONE_RIGHT, INT16_MIN, INT16_MAX);
        state->y = linear_scale(data.yValue, CIRQUE_PINNACLES_DEADZONE_TOP, UINT16_MAX - CIRQUE_PINNACLES_DEADZONE_BOTTOM, INT16_MIN, INT16_MAX);

        if (cirque_pinnacles_configs[cirque_index].swap_xy) {
            int16_t temp = state->x;
            state->x = state->y;
            state->y = temp;
        }
        if (cirque_pinnacles_configs[cirque_index].flip_x) {
            state->x *= -1;
        }
        if (cirque_pinnacles_configs[cirque_index].flip_y) {
            state->y *= -1;
        }
        if (state->x > -CIRQUE_PINNACLES_DEADZONE_CENTER && state->x < CIRQUE_PINNACLES_DEADZONE_CENTER) {
            state->x = 0;
        }
        if (state->y > -CIRQUE_PINNACLES_DEADZONE_CENTER && state->y < CIRQUE_PINNACLES_DEADZONE_CENTER) {
            state->y = 0;
        }

        if (!state->touching) {
            // started touching
            state->touched_at = timer_read();
            state->prev_x = state->x;
            state->prev_y = state->y;
        }
        state->tapped = false;
        state->tap_x = 0;
        state->tap_y = 0;
        state->touching = true;
    } else { // not actively touching
        state->tapped = false;
        if (state->touching && state->touched_at > 0 && timer_elapsed(state->touched_at) < CIRQUE_PINNACLES_TAP_TERM) {
            // stopped touching quickly enough to be a tap
            state->tapped = true;
            state->tap_x = CONSTRAIN(linear_scale(state->x, INT16_MIN, INT16_MAX, 0, UINT16_MAX) / CIRQUE_PINNACLES_TAP_ZONES_X_SIZE, 0, CIRQUE_PINNACLES_TAP_ZONES_X - 1);
            state->tap_y = CONSTRAIN(linear_scale(state->y, INT16_MIN, INT16_MAX, 0, UINT16_MAX) / CIRQUE_PINNACLES_TAP_ZONES_Y_SIZE, 0, CIRQUE_PINNACLES_TAP_ZONES_Y - 1);
        }
        state->touching = false;
        state->touched_at = 0;
        if (state->tapped) {
            dprintf("Cirque Pinnacles %d tapped at (%d, %d)\n", cirque_index, state->tap_x, state->tap_y);
            if (cirque_pinnacles_tapped(cirque_index, state->tap_x, state->tap_y)) {
                return DATA_HANDLED;
            }
        }
        state->x = 0;
        state->y = 0;
    }

    if (state->x == 0 && state->y == 0 && state->prev_x == 0 && state->prev_y == 0) {
        // nothing interesting this round, don't do anything
        return NO_DATA_READ;
    }

    if (cirque_pinnacles_moved(cirque_index, state->x, state->y, state->x - state->prev_x, state->y - state->prev_y)) {
        // already handled
        return DATA_HANDLED;
    }

    return DATA_UPDATED;
}

#if defined(CIRQUE_PINNACLES_DUAL_MOUSE)

#define CIRQUE_PINNACLE_INCH_TO_PX(inch) (DIVIDE_UNSIGNED_ROUND((inch) * (uint32_t)CIRQUE_PINNACLE_DIAMETER_MM * 10, 254))
#define CIRQUE_PINNACLE_PX_TO_INCH(px) (DIVIDE_UNSIGNED_ROUND((px) * (uint32_t)254, CIRQUE_PINNACLE_DIAMETER_MM * 10))

static cirque_pinnacles_state_t cirque_pinnacles_states[CIRQUE_PINNACLES_COUNT];
static uint16_t _dpi = POINTING_DEVICE_DEFAULT_DPI;
static uint16_t pointing_device_tapped_at = 0;

uint16_t pointing_device_driver_get_cpi(void)
{
    return CIRQUE_PINNACLE_PX_TO_INCH(_dpi);
}

void pointing_device_driver_set_cpi(uint16_t cpi)
{
    _dpi = CIRQUE_PINNACLE_INCH_TO_PX(cpi);
}

void pointing_device_driver_init(void)
{
    printf("Initializing dual Cirque Pinnacles...\n");
    memset(cirque_pinnacles_states, 0, sizeof(cirque_pinnacles_states));
    cirque_pinnacles_init();
    printf("Done initializing dual Cirque Pinnacles.\n");
}

report_mouse_t pointing_device_driver_get_report(report_mouse_t mouse_report)
{
    if (cirque_pinnacles_read_data(0, &(cirque_pinnacles_states[0])) == DATA_UPDATED) {
        if (cirque_pinnacles_states[0].touching) {
            if (cirque_pinnacles_states[0].x - cirque_pinnacles_states[0].prev_x >= CIRQUE_PINNACLES_SCROLL_THRESHOLD) {
                mouse_report.h = 1;
            } else if (cirque_pinnacles_states[0].prev_x - cirque_pinnacles_states[0].x >= CIRQUE_PINNACLES_SCROLL_THRESHOLD) {
                mouse_report.h = -1;
            } else {
                mouse_report.h = 0;
            }
            if (cirque_pinnacles_states[0].y - cirque_pinnacles_states[0].prev_y >= CIRQUE_PINNACLES_SCROLL_THRESHOLD) {
                mouse_report.v = 1;
            } else if (cirque_pinnacles_states[0].prev_y - cirque_pinnacles_states[0].y >= CIRQUE_PINNACLES_SCROLL_THRESHOLD) {
                mouse_report.v = -1;
            } else {
                mouse_report.v = 0;
            }
#if CIRQUE_PINNACLES_SCROLL_REVERSE
            mouse_report.h *= -1;
            mouse_report.v *= -1;
#endif
        }
    }
    if (cirque_pinnacles_read_data(1, &(cirque_pinnacles_states[1])) == DATA_UPDATED) {
        if (cirque_pinnacles_states[1].touching) {
            pointing_device_tapped_at = timer_read();
            mouse_report.x = (cirque_pinnacles_states[1].x - cirque_pinnacles_states[1].prev_x) / 256 * pointing_device_driver_get_cpi();
            mouse_report.y = -(cirque_pinnacles_states[1].y - cirque_pinnacles_states[1].prev_y) / 256 * pointing_device_driver_get_cpi();
        }

        if (cirque_pinnacles_states[1].tapped) {
            pointing_device_tapped_at = timer_read();
            if (cirque_pinnacles_states[1].tap_x < 1) {
                mouse_report.buttons = pointing_device_handle_buttons(mouse_report.buttons, true, POINTING_DEVICE_BUTTON3);
            } else if (cirque_pinnacles_states[1].tap_x > 1) {
                mouse_report.buttons = pointing_device_handle_buttons(mouse_report.buttons, true, POINTING_DEVICE_BUTTON2);
            } else {
                mouse_report.buttons = pointing_device_handle_buttons(mouse_report.buttons, true, POINTING_DEVICE_BUTTON1);
            }
        }
    }

    if (timer_elapsed(pointing_device_tapped_at) > CIRQUE_PINNACLES_TAP_TERM && !cirque_pinnacles_states[1].touching) {
        // clear mouse down, have held off long enough
        mouse_report.buttons = pointing_device_handle_buttons(mouse_report.buttons, false, POINTING_DEVICE_BUTTON1);
        mouse_report.buttons = pointing_device_handle_buttons(mouse_report.buttons, false, POINTING_DEVICE_BUTTON2);
        mouse_report.buttons = pointing_device_handle_buttons(mouse_report.buttons, false, POINTING_DEVICE_BUTTON3);
    }

    return mouse_report;
}
#endif
