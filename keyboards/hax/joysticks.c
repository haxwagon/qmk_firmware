#include QMK_KEYBOARD_H
#include "joysticks.h"

static int16_t js_values[JOYSTICK_AXIS_COUNT];

static const int16_t AXIS_SCALE = 32;

void joysticks_init(void)
{
    for (uint8_t i = 0; i < JOYSTICK_AXIS_COUNT; i++) {
        js_values[i] = 0;
    }
}

uint8_t joysticks_map_js_hat_keycode(uint16_t kc) {
    switch (kc) {
    case KC_JS_HAT_C: return JOYSTICK_HAT_CENTER;
    case KC_JS_HAT_DL: return JOYSTICK_HAT_SOUTHWEST;
    case KC_JS_HAT_D: return JOYSTICK_HAT_SOUTH;
    case KC_JS_HAT_DR: return JOYSTICK_HAT_SOUTHEAST;
    case KC_JS_HAT_L: return JOYSTICK_HAT_WEST;
    case KC_JS_HAT_R: return JOYSTICK_HAT_EAST;
    case KC_JS_HAT_UL: return JOYSTICK_HAT_NORTHWEST;
    case KC_JS_HAT_U: return JOYSTICK_HAT_NORTH;
    case KC_JS_HAT_UR: return JOYSTICK_HAT_NORTHEAST;
    default: return JOYSTICK_HAT_CENTER;
    }
}

void joysticks_move_axis(uint8_t axis, int16_t delta)
{
    int16_t value = js_values[axis] * AXIS_SCALE;
    if (delta >= 0 && INT16_MAX - value < delta) {
        value = INT16_MAX;
    } else if (delta < 0 && INT16_MIN - value > delta) {
        value = INT16_MIN;
    } else {
        value += delta;
    }
    dprintf("new axis %d with delta %d value is %d\n", axis, delta, value);
    joysticks_set_axis(axis, value);
}

static bool is_turbo_active = false;
static uint16_t turbo_down_at = 0;
static uint16_t turbo_up_at = 0;
static uint16_t turbo_button = 0;

bool joysticks_handle_keycode(uint16_t kc, bool pressed) {
    switch (kc) {
    case JS_0...JS_31:
        if (pressed) {
            register_joystick_button(kc);
            if (is_turbo_active) {
                if (turbo_button > 0) {
                    unregister_joystick_button(turbo_button);
                }
                turbo_button = kc;
                turbo_down_at = timer_read();
                turbo_up_at = 0;
            }
        } else {
            unregister_joystick_button(kc);
            if (is_turbo_active) {
                if (turbo_button > 0) {
                    unregister_joystick_button(turbo_button);
                }
                turbo_button = 0;
                turbo_down_at = 0;
                turbo_up_at = 0;
            }
        }
        return true;
    case KC_JS_TURBO:
        is_turbo_active = pressed;
        if (!is_turbo_active) {
            turbo_button = 0;
            turbo_down_at = 0;
            turbo_up_at = 0;
        }
        return true;
    case KC_JS_HAT_C:
    case KC_JS_HAT_DL:
    case KC_JS_HAT_D:
    case KC_JS_HAT_DR:
    case KC_JS_HAT_L:
    case KC_JS_HAT_R:
    case KC_JS_HAT_UL:
    case KC_JS_HAT_U:
    case KC_JS_HAT_UR:
        if (pressed) {
            joystick_set_hat(joysticks_map_js_hat_keycode(kc));
        } else {
            joystick_set_hat(JOYSTICK_HAT_CENTER);
        }
        return true;
    case KC_JS_LEFT_TRIGGER:
        if (pressed) {
            joysticks_set_axis(JS_AXIS_RX, INT16_MIN);
            register_joystick_button(JS_XINPUT_BUTTON_LT - QK_JOYSTICK);
        } else {
            joysticks_set_axis(JS_AXIS_RX, 0);
            unregister_joystick_button(JS_XINPUT_BUTTON_LT - QK_JOYSTICK);
        }
        return true;
    case KC_JS_RIGHT_TRIGGER:
        if (pressed) {
            joysticks_set_axis(JS_AXIS_RX, INT16_MAX);
            register_joystick_button(JS_XINPUT_BUTTON_RT - QK_JOYSTICK);
        } else {
            joysticks_set_axis(JS_AXIS_RX, 0);
            unregister_joystick_button(JS_XINPUT_BUTTON_RT - QK_JOYSTICK);
        }
        return true;
    default:
        return false;
    }
}

void joysticks_housekeeping(void)
{
    if (is_turbo_active && turbo_button > 0) {
        // we are holding down a joystick button with turbo mode active
        if (turbo_up_at > 0 && timer_elapsed(turbo_up_at) >= TURBO_DELAY / 2) {
            register_joystick_button(turbo_button);
            turbo_down_at = timer_read();
            turbo_up_at = 0;
        } else if (turbo_down_at > 0 && timer_elapsed(turbo_down_at) >= TURBO_DELAY / 2) {
            unregister_joystick_button(turbo_button);
            turbo_up_at = timer_read();
            turbo_down_at = 0;
        }
    }
}

void joysticks_set_axis(uint8_t axis, int16_t value)
{
    dprintf("Setting joystick %d axis to %d\n", axis, value);

    // js values should be between -512 and 512, divide by 64 to scale int16
    js_values[axis] = value / AXIS_SCALE;
    if (js_values[axis] > 512) {
        js_values[axis] = 512;
    } else if (js_values[axis] < -512) {
        js_values[axis] = -512;
    }

    joystick_set_axis(axis, js_values[axis]);
}
