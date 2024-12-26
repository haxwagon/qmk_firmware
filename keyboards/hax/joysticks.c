#include QMK_KEYBOARD_H
#include "joysticks.h"

static const int16_t js_maxvalue = 512;
// static const uint8_t js_precisions[2][3] = { { 10, 10, 10 }, { 10, 10, 10 } };
static uint16_t js_values[2][3] = { { 0, 0, 0 }, { 0, 0, 0 } };

enum JS_AXES {
    JS_AXIS_0_X,
    JS_AXIS_0_Y,
    JS_AXIS_0_Z,
    JS_AXIS_1_X,
    JS_AXIS_1_Y,
    JS_AXIS_1_Z,
};

static uint8_t joystick_axis(uint8_t joystick, uint8_t axis)
{
    switch (joystick) {
    case 0:
        switch (axis) {
        case 0:
            return JS_AXIS_0_X;
        case 1:
            return JS_AXIS_0_Y;
        case 2:
            return JS_AXIS_0_Z;
        default:
            break;
        }
        break;
    case 1:
        switch (axis) {
        case 0:
            return JS_AXIS_1_X;
        case 1:
            return JS_AXIS_1_Y;
        case 2:
            return JS_AXIS_1_Z;
        default:
            break;
        }
        break;
    default:
        break;
    }
    return 0;
}

void move_joystick_axis(uint8_t joystick, uint8_t axis, int16_t delta)
{
    int16_t value = js_values[joystick][axis];
    value += delta;
    if (value > js_maxvalue) {
        value = js_maxvalue;
    }
    if (value < -js_maxvalue) {
        value = -js_maxvalue;
    }
    set_joystick_axis(joystick, axis, value);
}

void set_joystick_axis(uint8_t joystick, uint8_t axis, int16_t value)
{
    dprintf("Setting joystick %d axis %d to %d\n", joystick, axis, value);

    js_values[joystick][axis] = value;
    joystick_set_axis(joystick_axis(joystick, axis), js_values[joystick][axis]);
}
