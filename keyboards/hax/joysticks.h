#pragma once

enum JS_AXES {
    JS_AXIS_X,
    JS_AXIS_Y,
    JS_AXIS_Z,
    JS_AXIS_RX,
    JS_AXIS_RY,
    JS_AXIS_RZ,
};

enum JS_XINPUT_BUTTONS { // xinput order
    JS_XINPUT_BUTTON_A = JS_0,
    JS_XINPUT_BUTTON_B,
    JS_XINPUT_BUTTON_X,
    JS_XINPUT_BUTTON_Y,
    JS_XINPUT_BUTTON_LB,
    JS_XINPUT_BUTTON_RB,
    JS_XINPUT_BUTTON_LT,
    JS_XINPUT_BUTTON_RT,
    JS_XINPUT_BUTTON_SELECT,
    JS_XINPUT_BUTTON_START,
    JS_XINPUT_BUTTON_L3,
    JS_XINPUT_BUTTON_R3,
};

enum JS_HAT_KEYCODES {
    KC_JS_HAT_C = 0x7F60,
    KC_JS_HAT_D,
    KC_JS_HAT_DL,
    KC_JS_HAT_DR,
    KC_JS_HAT_L,
    KC_JS_HAT_R,
    KC_JS_HAT_U,
    KC_JS_HAT_UL,
    KC_JS_HAT_UR,
    KC_JS_LEFT_TRIGGER,
    KC_JS_RIGHT_TRIGGER,
};

void joysticks_init(void);
void joysticks_move_axis(uint8_t axis, int16_t delta);
bool joysticks_process_keycode(uint16_t kc, bool pressed);
void joysticks_set_axis(uint8_t axis, int16_t value);
