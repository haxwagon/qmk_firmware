#pragma once

#ifndef TURBO_DELAY
#define TURBO_DELAY 80
#endif

enum JS_AXES {
    JS_AXIS_X,
    JS_AXIS_Y,
    JS_AXIS_Z,
    JS_AXIS_RX,
    JS_AXIS_RY,
    JS_AXIS_RZ,
};

enum JS_DINPUTS { // xinput order
    JS_DINPUT_CROSS = JS_0,
    JS_DINPUT_CIRCLE,
    JS_DINPUT_SQUARE,
    JS_DINPUT_TRIANGLE,
    JS_DINPUT_LB,
    JS_DINPUT_RB,
    JS_DINPUT_LT,
    JS_DINPUT_RT,
    JS_DINPUT_SELECT,
    JS_DINPUT_START,
    JS_DINPUT_L3,
    JS_DINPUT_R3,
};

enum JOYSTICKS_KEYCODES {
    KC_JS_TURBO = 0x7F60,
    KC_JS_HAT_C,
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
bool joysticks_handle_keycode(uint16_t kc, bool pressed);
void joysticks_housekeeping(void);
void joysticks_set_axis(uint8_t axis, int16_t value);
