#include QMK_KEYBOARD_H
#include "joysticks.h"

static int16_t js_values[JOYSTICK_AXIS_COUNT] = { 0, 0, 0, 0, 0, 0 };

void move_joystick_axis(uint8_t axis, int16_t delta)
{
    int16_t value = js_values[axis];
    if (value > INT16_MAX - delta) {
        value = INT16_MAX;
    } else if (value < INT16_MIN + delta) {
        value = INT16_MIN;
    } else {
        value += delta;
    }
    set_joystick_axis(axis, value);
}

void set_joystick_axis(uint8_t axis, int16_t value)
{
    dprintf("Setting joystick %d axis to %d\n", axis, value);

    // js values should be between -512 and 512, divide by 64 to scale int16
    js_values[axis] = value / 32;
    if (js_values[axis] > 512) {
        js_values[axis] = 512;
    } else if (js_values[axis] < -512) {
        js_values[axis] = -512;
    }

    joystick_set_axis(axis, js_values[axis]);
}
