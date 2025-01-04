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
