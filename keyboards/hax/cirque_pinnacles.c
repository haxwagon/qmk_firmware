#include QMK_KEYBOARD_H

#include "cirque_pinnacles.h"
#if defined(CIRQUE_PINNACLES_DUAL_JOYSTICKS)
#include "joysticks.h"
#endif

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

__attribute__((weak)) bool cirque_pinnacles_touchdown(uint8_t cirque_index, int16_t x, int16_t y, int16_t dx, int16_t dy)
{
    return false;
}

__attribute__((weak)) bool cirque_pinnacles_touchup(uint8_t cirque_index)
{
    return false;
}

#define CIRQUE_PINNACLES_TAP_ZONES_X_SIZE (UINT16_MAX / CIRQUE_PINNACLES_TAP_ZONES_X)
#define CIRQUE_PINNACLES_TAP_ZONES_Y_SIZE (UINT16_MAX / CIRQUE_PINNACLES_TAP_ZONES_Y)
#define CONSTRAIN(x, min, max) ((x) < min ? min : ((x) > max ? max : (x)))

static cirque_pinnacles_config_t cirque_pinnacles_configs[CIRQUE_PINNACLES_COUNT];
static uint8_t _current_spi_cs_pin = 0;
static uint16_t _ninebox_keysdown[CIRQUE_PINNACLES_COUNT][2];

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
        for (uint8_t j = 0; j < 2; j++) {
            _ninebox_keysdown[i][j] = 0;
        }
        cirque_pinnacles_configs[i] = cirque_pinnacles_get_config(i);
        cirque_pinnacles_select(i);
        cirque_pinnacle_init();
    }
}

#if defined(CIRQUE_PINNACLES_DUAL_JOYSTICKS)

__attribute__((weak)) bool cirque_pinnacles_joysticks_active(void) {
    return false;
}

__attribute__((weak)) bool cirque_pinnacles_joysticks_right_alt_axes_active(void) {
    return false;
}

bool cirque_pinnacles_joystick_touchdown(uint8_t cirque_index, int16_t x, int16_t y, int16_t dx, int16_t dy) {
    if (!cirque_pinnacles_joysticks_active()) {
        return false;
    }

    switch (cirque_index) {
    case 0: { // left pad
        joysticks_set_axis(JS_AXIS_X, x);
        joysticks_set_axis(JS_AXIS_Y, -y);
        return true;
    } break;
    case 1: { // right pad
        if (cirque_pinnacles_joysticks_right_alt_axes_active()) {
            joysticks_set_axis(JS_AXIS_RX, x);
            joysticks_move_axis(JS_AXIS_RY, dy / 8);
        } else {
            joysticks_set_axis(JS_AXIS_Z, x);
            joysticks_set_axis(JS_AXIS_RZ, -y);
        }
        return true;
    } break;
    default:
        break;
    }
    return false;
}

bool cirque_pinnacles_joystick_touchup(uint8_t cirque_index) {
    switch (cirque_index) {
    case 0: { // left pad
        joysticks_set_axis(JS_AXIS_X, 0);
        joysticks_set_axis(JS_AXIS_Y, 0);
        return true;
    } break;
    case 1: { // right pad
        if (cirque_pinnacles_joysticks_right_alt_axes_active()) {
            joysticks_set_axis(JS_AXIS_RX, 0);
        } else {
            joysticks_set_axis(JS_AXIS_Z, 0);
            joysticks_set_axis(JS_AXIS_RZ, 0);
        }
        return true;
    } break;
    default:
        break;
    }
    return false;
}

#endif

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

static const uint16_t NINEBOX_CENTER_SIZE = 512;
uint8_t cirque_pinnacles_ninebox_index(int16_t x, int16_t y) {
    if (abs(x) < NINEBOX_CENTER_SIZE && abs(y) < NINEBOX_CENTER_SIZE) {
        return 4;
    } else if (abs(x) >= 2 * abs(y)) { // right or left
        return x > 0 ? 5 : 3;
    } else if (abs(y) >= 2 * abs(x)) { // up or down
        return y > 0 ? 1 : 7;
    } else if (x >= 0 && y >= 0) {
        return 2;
    } else if (x >= 0 && y < 0) {
        return 8;
    } else if (y >= 0) {
        return 0;
    } else {
        return 6;
    }
}

__attribute__((weak)) bool cirque_pinnacles_ninebox_active(uint8_t cirque_index) {
    return false;
}

__attribute__((weak)) uint16_t cirque_pinnacles_get_ninebox(uint8_t cirque_index, uint8_t ninebox_index) {
    return KC_NO;
}

void cirque_pinnacles_set_keydown(uint16_t kc)
{
#if defined(CIRQUE_PINNACLES_DUAL_JOYSTICKS)
    if (joysticks_process_keycode(kc, true)) {
        return;
    }
#endif
    register_code16(kc);
}

void cirque_pinnacles_set_keyup(uint16_t kc)
{
#if defined(CIRQUE_PINNACLES_DUAL_JOYSTICKS)
    if (joysticks_process_keycode(kc, false)) {
        return;
    }
#endif
    unregister_code16(kc);
}

bool cirque_pinnacles_set_keys_down(uint8_t cirque_index, uint16_t kc1, uint16_t kc2) {
    bool handled = false;
    int8_t kc1_down_pos = -1;
    int8_t kc2_down_pos = -1;
    uint8_t next_open_pos = 0;
    for (uint8_t i = 0; i < 2; i++) {
        if (_ninebox_keysdown[cirque_index][i] > 0 && _ninebox_keysdown[cirque_index][i] != kc1 && _ninebox_keysdown[cirque_index][i] != kc2) {
            cirque_pinnacles_set_keyup(_ninebox_keysdown[cirque_index][i]);
            _ninebox_keysdown[cirque_index][i] = 0;
        } else if (_ninebox_keysdown[cirque_index][i] == kc1) {
            kc1_down_pos = i;
            next_open_pos = (i + 1) % 2;
            handled = true;
        } else if (_ninebox_keysdown[cirque_index][i] == kc2) {
            kc2_down_pos = i;
            next_open_pos = (i + 1) % 2;
            handled = true;
        }
    }
    if (kc1 != KC_NO && kc1_down_pos < 0) {
        cirque_pinnacles_set_keydown(kc1);
        _ninebox_keysdown[cirque_index][next_open_pos] = kc1;
        next_open_pos = (next_open_pos + 1) % 2;
        handled = true;
    }
    if (kc2 != KC_NO && kc2_down_pos < 0) {
        cirque_pinnacles_set_keydown(kc2);
        _ninebox_keysdown[cirque_index][next_open_pos] = kc2;
        next_open_pos = (next_open_pos + 1) % 2;
        handled = true;
    }
    return handled;
}

bool cirque_pinnacles_ninebox_touchdown(uint8_t cirque_index, int16_t x, uint16_t y) {
    if (!cirque_pinnacles_ninebox_active(cirque_index)) {
        return false;
    }

    // have a mapping, try to use it
    uint8_t ninebox_pos = cirque_pinnacles_ninebox_index(x, y);
    uint16_t kc = cirque_pinnacles_get_ninebox(cirque_index, ninebox_pos);
    if (kc != KC_NO || ninebox_pos == 1 || ninebox_pos == 3 || ninebox_pos == 5 || ninebox_pos == 7) {
        // defined key or single cardinal direction
        return cirque_pinnacles_set_keys_down(cirque_index, kc, KC_NO);
    } else {
        // corner direction and we don't have a kc for it explicitly, try the corners near it
        switch (ninebox_pos) {
            case 0: // upper left
                return cirque_pinnacles_set_keys_down(cirque_index, cirque_pinnacles_get_ninebox(cirque_index, 1), cirque_pinnacles_get_ninebox(cirque_index, 3));
            case 2: // upper right
                return cirque_pinnacles_set_keys_down(cirque_index, cirque_pinnacles_get_ninebox(cirque_index, 1), cirque_pinnacles_get_ninebox(cirque_index, 5));
            case 6: // lower left
                return cirque_pinnacles_set_keys_down(cirque_index, cirque_pinnacles_get_ninebox(cirque_index, 3), cirque_pinnacles_get_ninebox(cirque_index, 7));
            case 8: // lower right
                return cirque_pinnacles_set_keys_down(cirque_index, cirque_pinnacles_get_ninebox(cirque_index, 5), cirque_pinnacles_get_ninebox(cirque_index, 7));
            default:
                return false;
        }
    }
}

bool cirque_pinnacles_ninebox_touchup(uint8_t cirque_index)
{
    cirque_pinnacles_clear_active_keys(cirque_index);
    return cirque_pinnacles_ninebox_active(cirque_index);
}

void cirque_pinnacles_clear_active_keys(uint8_t cirque_index)
{
    for (uint8_t i = 0; i < 2; i++) {
        if (_ninebox_keysdown[cirque_index][i] > 0) {
            cirque_pinnacles_set_keyup(_ninebox_keysdown[cirque_index][i]);
            _ninebox_keysdown[cirque_index][i] = 0;
        }
    }
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

        int16_t dx = state->x - state->prev_x;
        int16_t dy = state->y - state->prev_y;
        if (cirque_pinnacles_ninebox_touchdown(cirque_index, state->x, state->y)) {
            return DATA_HANDLED;
        } else if (cirque_pinnacles_joystick_touchdown(cirque_index, state->x, state->y, dx, dy)) {
            return DATA_HANDLED;
        } else if (cirque_pinnacles_touchdown(cirque_index, state->x, state->y, dx, dy)) {
            return DATA_HANDLED;
        }
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
        state->prev_x = 0;
        state->prev_y = 0;
        state->x = 0;
        state->y = 0;

        if (cirque_pinnacles_ninebox_touchup(cirque_index)) {
            return DATA_HANDLED;
        } else if (cirque_pinnacles_joystick_touchup(cirque_index)) {
            return DATA_HANDLED;
        } else if (cirque_pinnacles_touchup(cirque_index)) {
            return DATA_HANDLED;
        }
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

