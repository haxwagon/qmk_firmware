#pragma once

enum JS_XINPUT_BUTTONS { // xinput order
    JS_XINPUT_BUTTON_A = JS_0,
    JS_XINPUT_BUTTON_B,
    JS_XINPUT_BUTTON_X,
    JS_XINPUT_BUTTON_Y,
    JS_XINPUT_BUTTON_LB,
    JS_XINPUT_BUTTON_RB,
    JS_XINPUT_BUTTON_SELECT,
    JS_XINPUT_BUTTON_START,
    JS_XINPUT_BUTTON_L3,
    JS_XINPUT_BUTTON_R3,
};

enum JS_AXES {
    JS_AXIS_0_X,
    JS_AXIS_0_Y,
    JS_AXIS_1_X,
    JS_AXIS_RX,
    JS_AXIS_RY,
    JS_AXIS_1_Y,
};

void move_joystick_axis(uint8_t axis, int16_t delta);
void set_joystick_axis(uint8_t axis, int16_t value);
