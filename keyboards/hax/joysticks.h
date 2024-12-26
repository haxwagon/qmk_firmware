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

void move_joystick_axis(uint8_t joystick, uint8_t axis, int16_t delta);
void set_joystick_axis(uint8_t joystick, uint8_t axis, int16_t value);
