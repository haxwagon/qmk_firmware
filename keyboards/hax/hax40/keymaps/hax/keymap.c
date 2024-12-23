#include QMK_KEYBOARD_H
#include "analog.h"
#include "print.h"

enum LAYERS {
    LAYER_QWERTY, // default
    LAYER_NUMSYMS, // upper
    LAYER_FUNC, // lower
    LAYER_MOVE,
    LAYER_GAMEPAD,
    LAYER_GAMEPAD2,
    LAYER_JOYSTICK,
    LAYER_JOYSTICK2,
    LAYER_MEDIA,
    LAYER_NUMPAD,
};
static const uint16_t LAYER_DEFAULT = LAYER_QWERTY;

const static char* LAYER_MAPS[] = {
    [LAYER_QWERTY] = "\
 Q  W  E  R  T      \n\
       Y  U  I  O  P\n\
 A  S  D  F  G      \n\
       H  J  K  L  '\n\
 Z  X  C  V  B      \n\
       N  M ,; .!  /\n\
",
    [LAYER_FUNC] = "\
ES GP ME NP JS      \n\
      R1 R2 P2 P1 <-\n\
-> MN HM FD         \n\
       <  v  ^  > EN\n\
LS '` =+ -_ /\\      \n\
      {} [] <> ()  \\\n\
",
    [LAYER_GAMEPAD] = "\
ES  Q  W  E  R      \n\
                    \n\
->  A  S  D  F      \n\
                    \n\
LS  Z  X  C  V      \n\
                    \n\
",
    [LAYER_GAMEPAD2] = "\
F1 F2 F3 F4 F5      \n\
                    \n\
 1  2  3  4  5      \n\
                     \n\
LS     T  G  B      \n\
                    \n\
",
    [LAYER_JOYSTICK] = "\
   NW  N NE         \n\
      CR LB SE ST RB\n\
    W  C  E         \n\
      CR  X  A  B  Y\n\
   SW  S SE         \n\
      CR L3 10 11 R3\n\
",
    [LAYER_JOYSTICK2] = "\
   NW  N NE         \n\
      CR LB SE ST RB\n\
    W  C  E         \n\
      CR  X  A  B  Y\n\
   SW  S SE         \n\
      CR L3 10 11 R3\n\
",
    [LAYER_MEDIA] = "\
                    \n\
         BD BU      \n\
                    \n\
      |< VD VU >| EN\n\
                    \n\
      << MU    >>   \n\
",
    [LAYER_MOVE] = "\
WU WL MU WR M2      \n\
               DE IN\n\
WD ML MD MR M1      \n\
       <  v  ^  > EN\n\
M7 M6 M5 M4 M3      \n\
      HM PD PU ED   \n\
",
    [LAYER_NUMPAD] = "\
                    \n\
     * /  7  8  9 <-\n\
                    \n\
      +-  4  5  6 EN\n\
                    \n\
       0  1  2  3  .\n\
",
    [LAYER_NUMSYMS] = "\
F1 F2 F3 F4 F5      \n\
      F6 F7 F8 F9 F0\n\
 1  2  3  4  5      \n\
       6  7  8  9  0\n\
 !  @  #  $  %      \n\
       ^  &  *  (  )\n\
",
};

const static char* LAYER_NAMES[] = {
    [LAYER_QWERTY] = "Default",
    [LAYER_FUNC] = "Functions",
    [LAYER_GAMEPAD] = "Gamepad",
    [LAYER_GAMEPAD2] = "Gamepad 2",
    [LAYER_JOYSTICK] = "Joystick",
    [LAYER_JOYSTICK2] = "Joystick 2",
    [LAYER_MEDIA] = "Media",
    [LAYER_MOVE] = "Move",
    [LAYER_NUMPAD] = "Numpad",
    [LAYER_NUMSYMS] = "Nums & Syms",
};

enum CUSTOM_KEYCODES {
    CKC_POINTING_DEVICE_INC_DPI = SAFE_RANGE,
    CKC_POINTING_DEVICE_DEC_DPI,
    CKC_JS_CENTER_RIGHT,
    CKC_JS_CENTER_RIGHT_X,
    CKC_JS_CENTER_RIGHT_Y,
};

#if defined(KEY_OVERRIDE_ENABLE)
const key_override_t backspace_del_key_override = ko_make_basic(MOD_MASK_SHIFT, KC_BSPC, KC_DEL);
const key_override_t f11_key_override = ko_make_basic(MOD_MASK_SHIFT, KC_F1, KC_F11);
const key_override_t f12_key_override = ko_make_basic(MOD_MASK_SHIFT, KC_F2, KC_F12);
const key_override_t f13_key_override = ko_make_basic(MOD_MASK_SHIFT, KC_F3, KC_F13);
const key_override_t f14_key_override = ko_make_basic(MOD_MASK_SHIFT, KC_F4, KC_F14);
const key_override_t f15_key_override = ko_make_basic(MOD_MASK_SHIFT, KC_F5, KC_F15);
const key_override_t f16_key_override = ko_make_basic(MOD_MASK_SHIFT, KC_F6, KC_F16);
const key_override_t f17_key_override = ko_make_basic(MOD_MASK_SHIFT, KC_F7, KC_F17);
const key_override_t f18_key_override = ko_make_basic(MOD_MASK_SHIFT, KC_F8, KC_F18);
const key_override_t f19_key_override = ko_make_basic(MOD_MASK_SHIFT, KC_F9, KC_F19);
const key_override_t f20_key_override = ko_make_basic(MOD_MASK_SHIFT, KC_F10, KC_F20);
const key_override_t* key_overrides[] = {
    &backspace_del_key_override,
    &f11_key_override,
    &f12_key_override,
    &f13_key_override,
    &f14_key_override,
    &f15_key_override,
    &f16_key_override,
    &f17_key_override,
    &f18_key_override,
    &f19_key_override,
    &f20_key_override,
};
#endif

#if defined(TAP_DANCE_ENABLE)

#if defined(TAP_DANCE_QUAD_SUPPORT)
// #region Tap Dance Quad Support
typedef enum {
    TD_NONE,
    TD_UNKNOWN,
    TD_SINGLE_TAP,
    TD_SINGLE_HOLD,
    TD_DOUBLE_TAP,
    TD_DOUBLE_HOLD,
    TD_DOUBLE_SINGLE_TAP, // Send two single taps
    TD_TRIPLE_TAP,
    TD_TRIPLE_HOLD
} td_quad_state_t;

typedef struct
{
    uint16_t kc_tap;
    uint16_t kc_hold;
    uint16_t kc_double_tap;
    uint16_t kc_double_tap_hold;
} td_quad_keycodes_t;

td_quad_state_t current_dance_quad_state(tap_dance_state_t* state)
{
    switch (state->count) {
    case 1:
        if (state->interrupted || !state->pressed)
            return TD_SINGLE_TAP;
        return TD_SINGLE_HOLD;
    case 2:
        if (state->interrupted)
            return TD_DOUBLE_SINGLE_TAP;
        else if (state->pressed)
            return TD_DOUBLE_HOLD;
        return TD_DOUBLE_TAP;
    case 3:
        if (state->interrupted || !state->pressed)
            return TD_TRIPLE_TAP;
        return TD_TRIPLE_HOLD;
    default:
        return TD_UNKNOWN;
    }
}

void td_quad_finished(tap_dance_state_t* state, void* user_data)
{
    td_quad_keycodes_t* kcs = (td_quad_keycodes_t*)user_data;
    switch (current_dance_quad_state(state)) {
    case TD_SINGLE_TAP:
        if (kcs->kc_tap) {
            register_code(kcs->kc_tap);
        }
        break;
    case TD_SINGLE_HOLD:
        if (kcs->kc_hold) {
            register_code(kcs->kc_hold);
        }
        break;
    case TD_DOUBLE_TAP:
        if (kcs->kc_double_tap) {
            register_code(kcs->kc_double_tap);
        }
        break;
    case TD_DOUBLE_HOLD:
        if (kcs->kc_double_tap_hold) {
            register_code(kcs->kc_double_tap_hold);
        }
        break;
    case TD_DOUBLE_SINGLE_TAP:
        if (kcs->kc_tap) {
            tap_code(kcs->kc_tap);
            register_code(kcs->kc_tap);
        }
        break;
    default:
        break;
    }
}

void td_quad_reset(tap_dance_state_t* state, void* user_data)
{
    td_quad_keycodes_t* kcs = (td_quad_keycodes_t*)user_data;
    switch (current_dance_quad_state(state)) {
    case TD_SINGLE_TAP:
        if (kcs->kc_tap) {
            unregister_code(kcs->kc_tap);
        }
        break;
    case TD_SINGLE_HOLD:
        if (kcs->kc_hold) {
            unregister_code(kcs->kc_hold);
        }
        break;
    case TD_DOUBLE_TAP:
        if (kcs->kc_double_tap) {
            unregister_code(kcs->kc_double_tap);
        }
        break;
    case TD_DOUBLE_HOLD:
        if (kcs->kc_double_tap_hold) {
            unregister_code(kcs->kc_double_tap_hold);
        }
        break;
    case TD_DOUBLE_SINGLE_TAP:
        if (kcs->kc_tap) {
            unregister_code(kcs->kc_tap);
        }
        break;
    default:
        break;
    }
}

#define ACTION_TAP_DANCE_QUAD(kc_tap, kc_hold, kc_double_tap, kc_double_tap_hold)                           \
    {                                                                                                       \
        .fn = { NULL, td_quad_finished, td_quad_reset, NULL },                                              \
        .user_data = (void*)&((td_quad_keycodes_t) { kc_tap, kc_hold, kc_double_tap, kc_double_tap_hold }), \
    }
#endif

enum {
    TD_ANGLES,
    TD_BRACES,
    TD_BRACKETS,
    TD_COMMSCLN,
    TD_DOTCOLN,
    TD_DOTEXLM,
    TD_EQLPLUS,
    TD_MINSUNDS,
    TD_PARENS,
    TD_PASTSLS,
    TD_PPLSMNS,
    TD_QUOTGRV,
    TD_SLASHES,
};

tap_dance_action_t tap_dance_actions[] = {
    [TD_ANGLES] = ACTION_TAP_DANCE_DOUBLE(KC_LABK, KC_RABK),
    [TD_BRACES] = ACTION_TAP_DANCE_DOUBLE(KC_LCBR, KC_RCBR),
    [TD_BRACKETS] = ACTION_TAP_DANCE_DOUBLE(KC_LBRC, KC_RBRC),
    [TD_COMMSCLN] = ACTION_TAP_DANCE_DOUBLE(KC_COMM, KC_SCLN),
    [TD_DOTCOLN] = ACTION_TAP_DANCE_QUAD(KC_DOT, KC_COLN, 0, 0),
    [TD_DOTEXLM] = ACTION_TAP_DANCE_DOUBLE(KC_DOT, KC_EXLM),
    [TD_EQLPLUS] = ACTION_TAP_DANCE_DOUBLE(KC_EQL, KC_PLUS),
    [TD_MINSUNDS] = ACTION_TAP_DANCE_DOUBLE(KC_MINS, KC_UNDS),
    [TD_PARENS] = ACTION_TAP_DANCE_DOUBLE(KC_LPRN, KC_RPRN),
    [TD_PASTSLS] = ACTION_TAP_DANCE_DOUBLE(KC_PAST, KC_PSLS),
    [TD_PPLSMNS] = ACTION_TAP_DANCE_DOUBLE(KC_PPLS, KC_PMNS),
    [TD_QUOTGRV] = ACTION_TAP_DANCE_DOUBLE(KC_QUOT, KC_GRV),
    [TD_SLASHES] = ACTION_TAP_DANCE_QUAD(KC_SLSH, KC_QUES, KC_BSLS, KC_PIPE),
};
#endif

#if defined(COMBO_ENABLE)
const uint16_t PROGMEM left_alt_combo[] = { KC_Q, KC_W, COMBO_END };
const uint16_t PROGMEM left_ctl_combo[] = { KC_W, KC_E, COMBO_END };
const uint16_t PROGMEM left_ctl_alt_combo[] = { KC_Q, KC_W, KC_E, COMBO_END };
const uint16_t PROGMEM left_ctl_gui_combo[] = { KC_W, KC_E, KC_R, COMBO_END };
const uint16_t PROGMEM left_gui_combo[] = { KC_E, KC_R, COMBO_END };
const uint16_t PROGMEM right_alt_combo[] = { KC_O, KC_P, COMBO_END };
const uint16_t PROGMEM right_ctl_combo[] = { KC_I, KC_O, COMBO_END };
const uint16_t PROGMEM right_gui_combo[] = { KC_U, KC_I, COMBO_END };
const uint16_t PROGMEM right_ctl_alt_combo[] = { KC_I, KC_O, KC_P, COMBO_END };
const uint16_t PROGMEM right_ctl_gui_combo[] = { KC_U, KC_I, KC_O, COMBO_END };
combo_t key_combos[] = {
    COMBO(left_alt_combo, KC_LALT),
    COMBO(left_ctl_combo, KC_LCTL),
    COMBO(left_gui_combo, KC_LGUI),
    COMBO(left_ctl_alt_combo, LCTL(KC_LALT)),
    COMBO(left_ctl_gui_combo, LCTL(KC_LGUI)),
    COMBO(right_alt_combo, KC_RALT),
    COMBO(right_ctl_combo, KC_RCTL),
    COMBO(right_gui_combo, KC_RGUI),
    COMBO(right_ctl_alt_combo, RCTL(KC_RALT)),
    COMBO(right_ctl_gui_combo, RCTL(KC_RGUI)),
};
#endif

#if defined(JOYSTICK_ENABLE)
static const int16_t js_maxvalue = 512;
static const uint8_t js_precisions[2][3] = { { 10, 10, 10 }, { 10, 10, 10 } };
static uint16_t js_values[2][3] = { { 0, 0, 0 }, { 0, 0, 0 } };

joystick_config_t joystick_axes[JOYSTICK_AXIS_COUNT] = {
    JOYSTICK_AXIS_IN(GP27, 0, 200, 512),
    JOYSTICK_AXIS_VIRTUAL,
    JOYSTICK_AXIS_VIRTUAL,
    JOYSTICK_AXIS_VIRTUAL,
    JOYSTICK_AXIS_VIRTUAL,
    JOYSTICK_AXIS_VIRTUAL,
};

enum JS_AXES {
    JS_AXIS_0_X,
    JS_AXIS_0_Y,
    JS_AXIS_0_Z,
    JS_AXIS_1_X,
    JS_AXIS_1_Y,
    JS_AXIS_1_Z,
};
enum JS_XINPUT_BUTTONS { // xinput order
    JS_XINPUT_BUTTON_A,
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
uint8_t joystick_axis(uint8_t joystick, uint8_t axis)
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
void set_joystick_axis(uint8_t joystick, uint8_t axis, int16_t value)
{
    dprintf("Setting joystick %d axis %d to %d\n", joystick, axis, value);

    js_values[joystick][axis] = value;
    joystick_set_axis(joystick_axis(joystick, axis), js_values[joystick][axis]);
}
void update_joystick_axis(uint8_t joystick, uint8_t axis, int16_t delta)
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
#endif

#if defined(DYNAMIC_MACRO_ENABLE)
static uint8_t dynamic_macro_recording = 0;
#endif

#if defined(ENCODER_ENABLE)
bool encoder_update_user(uint8_t encoder, bool clockwise)
{
    dprintf("Encoder: %d, Clockwise: %d", encoder, clockwise);
    switch (get_highest_layer(layer_state)) {
#if defined(JOYSTICK_ENABLE)
    case LAYER_JOYSTICK:
        uint8_t delta = clockwise ? js_precisions[encoder][2] : -js_precisions[encoder][2];
        update_joystick_axis(encoder, 2, delta);
        break;
#endif
    default:
        switch (encoder) {
        case 0:
            clockwise ? tap_code(MS_WHLD) : tap_code(MS_WHLU);
            break;
        case 1:
            clockwise ? tap_code(KC_VOLD) : tap_code(KC_VOLU);
            break;
        default:
            break;
        }
        break;
    }
    return false;
}
#endif

#if defined(OLED_ENABLE)
oled_rotation_t oled_init_user(oled_rotation_t rotation)
{
    return OLED_ROTATION_0;
}

bool oled_task_user(void)
{
    oled_write_P(PSTR(LAYER_NAMES[get_highest_layer(layer_state)]), false);
    oled_write_P(PSTR("\n"), false);

#if defined(DYNAMIC_MACRO_ENABLE)
    switch (dynamic_macro_recording) {
    case 1:
        oled_write_P(PSTR("REC1 "), false);
        break;
    case 2:
        oled_write_P(PSTR("REC2 "), false);
        break;
    default:
        oled_write_P(PSTR("     "), false);
        break;
    }
#endif
    // Host Keyboard LED Status
    led_t led_state = host_keyboard_led_state();
    oled_write_P(led_state.num_lock ? PSTR("NUM ") : PSTR("    "), false);
    oled_write_P(led_state.caps_lock ? PSTR("CAP ") : PSTR("    "), false);
    oled_write_P(led_state.scroll_lock ? PSTR("SCR ") : PSTR("    "), false);

    oled_write_P(PSTR("\n"), false);

    // display keyboard map
    oled_write_P(PSTR(LAYER_MAPS[get_highest_layer(layer_state)]), false);

    return false;
}
#endif

#if defined(POINTING_DEVICE_ENABLE)

void           dual_cirque_pinnacle_init(void) { }
report_mouse_t dual_cirque_pinnacle_get_report(report_mouse_t mouse_report) { return mouse_report; }
uint16_t       dual_cirque_pinnacle_get_cpi(void) { return 0; }
void           dual_cirque_pinnacle_set_cpi(uint16_t cpi) {}

const pointing_device_driver_t cirque_pinnacle_pointing_device_driver = {
    .init       = dual_cirque_pinnacle_init,
    .get_report = dual_cirque_pinnacle_get_report,
    .get_cpi    = dual_cirque_pinnacle_get_cpi,
    .set_cpi    = dual_cirque_pinnacle_set_cpi,
};

static uint8_t pointing_device_dpi = POINTING_DEVICE_DEFAULT_DPI;
static const uint16_t pointing_device_js_dpi = POINTING_DEVICE_DEFAULT_JS_DPI;
report_mouse_t pointing_device_task_user(report_mouse_t mouse_report)
{
    if (mouse_report.x != 0 || mouse_report.y != 0 || mouse_report.buttons != 0) {
        dprintf("Pointing Device: X: %d, Y: %d, Buttons: %d\n", mouse_report.x, mouse_report.y, mouse_report.buttons);
    }
    switch (get_highest_layer(layer_state)) {
#if defined(JOYSTICK_ENABLE)
    case LAYER_JOYSTICK:
        update_joystick_axis(1, 0, mouse_report.x * pointing_device_js_dpi);
        update_joystick_axis(1, 1, mouse_report.y * pointing_device_js_dpi);
        mouse_report.x = 0;
        mouse_report.y = 0;
        if (mouse_report.buttons > 0) {
            register_joystick_button(JS_XINPUT_BUTTON_R3);
            mouse_report.buttons = 0;
        }
        break;
    case LAYER_JOYSTICK2:
        update_joystick_axis(0, 2, mouse_report.y * pointing_device_js_dpi);
        mouse_report.x = 0;
        mouse_report.y = 0;
        if (mouse_report.buttons > 0) {
            register_joystick_button(JS_XINPUT_BUTTON_R3);
            mouse_report.buttons = 0;
        }
        break;
#endif
    case LAYER_MOVE:
        if (abs(mouse_report.y) > abs(mouse_report.x)) {
            // scrolling vertically
            if (mouse_report.y > 0) {
                tap_code(MS_WHLD);
                mouse_report.x = 0;
                mouse_report.y = 0;
            } else if (mouse_report.y < 0) {
                tap_code(MS_WHLU);
                mouse_report.x = 0;
                mouse_report.y = 0;
            }
        } else if (abs(mouse_report.x) > abs(mouse_report.y)) {
            if (mouse_report.x > 0) {
                tap_code(MS_WHLR);
                mouse_report.x = 0;
                mouse_report.y = 0;
            } else if (mouse_report.x < 0) {
                tap_code(MS_WHLL);
                mouse_report.x = 0;
                mouse_report.y = 0;
            }
        }
        break;
    default:
        mouse_report.x *= pointing_device_dpi;
        mouse_report.y *= pointing_device_dpi;
        break;
    }
    return mouse_report;
}
#endif

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [LAYER_QWERTY] = LAYOUT_ortho_4x10(
        KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P,
        KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_QUOT,
        LSFT_T(KC_Z), KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, TD(TD_COMMSCLN), TD(TD_DOTCOLN), RSFT_T(KC_SLSH),
        KC_NO, KC_NO, KC_NO, TL_LOWR, KC_SPACE, KC_SPACE, TL_UPPR, KC_NO, KC_NO, KC_NO),
    [LAYER_FUNC] = LAYOUT_ortho_4x10(
        KC_ESC, TT(LAYER_GAMEPAD), TT(LAYER_MEDIA), TT(LAYER_NUMPAD), TT(LAYER_JOYSTICK), DM_REC1, DM_REC2, DM_PLY2, DM_PLY1, KC_BSPC,
        KC_TAB, KC_MENU, KC_HOME, KC_FIND, KC_NO, KC_LEFT, KC_DOWN, KC_UP, KC_RGHT, KC_ENT,
        KC_LSFT, TD(TD_QUOTGRV), TD(TD_EQLPLUS), TD(TD_MINSUNDS), TD(TD_SLASHES), TD(TD_BRACES), TD(TD_BRACKETS), TD(TD_ANGLES), TD(TD_PARENS), RSFT_T(KC_BSLS),
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_ESC, KC_ENT, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS),
    [LAYER_GAMEPAD] = LAYOUT_ortho_4x10(
        KC_ESC, KC_Q, KC_W, KC_E, KC_R, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_TAB, KC_A, KC_S, KC_D, KC_F, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_NO, KC_NO, KC_NO, MO(LAYER_GAMEPAD2), KC_SPACE, KC_SPACE, TO(LAYER_DEFAULT), KC_NO, KC_NO, KC_NO),
    [LAYER_GAMEPAD2] = LAYOUT_ortho_4x10(
        KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_1, KC_2, KC_3, KC_4, KC_5, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_LSFT, KC_NO, KC_T, KC_G, KC_B, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS),
    [LAYER_JOYSTICK] = LAYOUT_ortho_4x10(
        KC_NO, JOYSTICK_HAT_NORTHWEST, JOYSTICK_HAT_NORTH, JOYSTICK_HAT_NORTHEAST, KC_NO, CKC_JS_CENTER_RIGHT_Y, JS_4, JS_6, JS_7, JS_5,
        MO(LAYER_JOYSTICK2), JOYSTICK_HAT_WEST, JOYSTICK_HAT_SOUTH, JOYSTICK_HAT_EAST, KC_NO, CKC_JS_CENTER_RIGHT, JS_2, JS_0, JS_1, JS_3,
        KC_NO, JOYSTICK_HAT_SOUTHWEST, JOYSTICK_HAT_SOUTH, JOYSTICK_HAT_SOUTHEAST, KC_NO, CKC_JS_CENTER_RIGHT_X, JS_8, JS_10, JS_11, JS_9,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, TO(LAYER_DEFAULT), KC_NO, KC_NO, KC_NO),
    [LAYER_JOYSTICK2] = LAYOUT_ortho_4x10(
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS),
    [LAYER_MEDIA] = LAYOUT_ortho_4x10(
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_BRID, KC_BRIU, KC_NO, KC_NO,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_MPRV, KC_VOLD, KC_VOLU, KC_MNXT, KC_MPLY,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_MRWD, KC_MUTE, KC_NO, KC_MFFD, KC_NO, KC_NO,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, TO(LAYER_DEFAULT), KC_NO, KC_NO, KC_NO),
    [LAYER_MOVE] = LAYOUT_ortho_4x10(
        MS_WHLU, MS_WHLL, MS_UP, MS_WHLR, MS_BTN2, KC_NO, CKC_POINTING_DEVICE_DEC_DPI, CKC_POINTING_DEVICE_INC_DPI, KC_DEL, KC_INS,
        MS_WHLD, MS_LEFT, MS_DOWN, MS_RGHT, MS_BTN1, KC_LEFT, KC_DOWN, KC_UP, KC_RGHT, KC_ENT,
        MS_BTN7, MS_BTN6, MS_BTN5, MS_BTN4, MS_BTN3, KC_HOME, KC_PGDN, KC_PGUP, KC_END, KC_NO,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS),
    [LAYER_NUMPAD] = LAYOUT_ortho_4x10(
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, TD(TD_PASTSLS), KC_P7, KC_P8, KC_P9, KC_BSPC,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, TD(TD_PPLSMNS), KC_P4, KC_P5, KC_P6, KC_PENT,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_P0, KC_P1, KC_P2, KC_P3, KC_PDOT,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, TO(LAYER_DEFAULT), KC_NO, KC_NO, KC_NO),
    [LAYER_NUMSYMS] = LAYOUT_ortho_4x10(
        KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10,
        KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0,
        LSFT_T(KC_EXLM), KC_AT, KC_HASH, KC_DLR, KC_PERC, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, RSFT_T(KC_RPRN),
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_ESC, KC_ENT, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS),
};

void keyboard_post_init_user(void)
{
#if defined(CONSOLE_ENABLE)
    debug_enable = true;
    debug_matrix = false;
    debug_keyboard = false;
    debug_mouse = true;
#endif
}

bool process_record_user(uint16_t keycode, keyrecord_t* record)
{
    dprintf("Pressed keycode: %d on layer %d\n", keycode, get_highest_layer(layer_state));

    dprintf("GP26: %d, GP27: %d\n", analogReadPin(GP26), analogReadPin(GP27));

    switch (keycode) {
#if defined(DYNAMIC_MACRO_ENABLE)
    case DM_REC1:
        switch (dynamic_macro_recording) {
        case 0:
            dynamic_macro_recording = 1;
            break;
        case 1:
            dynamic_macro_recording = 0;
            break;
        default:
            break;
        }
        break;
    case DM_REC2:
        switch (dynamic_macro_recording) {
        case 0:
            dynamic_macro_recording = 2;
            break;
        case 2:
            dynamic_macro_recording = 0;
            break;
        default:
            break;
        }
        break;
#endif
#if defined(POINTING_DEVICE_ENABLE)
    case CKC_POINTING_DEVICE_DEC_DPI:
        if (pointing_device_dpi > POINTING_DEVICE_MIN_DPI) {
            pointing_device_dpi--;
            dprintf("DPI: %d\n", pointing_device_dpi);
        }
        break;
    case CKC_POINTING_DEVICE_INC_DPI:
        if (pointing_device_dpi < POINTING_DEVICE_MAX_DPI) {
            pointing_device_dpi++;
            dprintf("DPI: %d\n", pointing_device_dpi);
        }
        break;
#endif
#if defined(JOYSTICK_ENABLE)
    case CKC_JS_CENTER_RIGHT:
        set_joystick_axis(1, 0, 0);
        set_joystick_axis(1, 1, 0);
        return false;
    case CKC_JS_CENTER_RIGHT_X:
        set_joystick_axis(1, 0, 0);
        return false;
    case CKC_JS_CENTER_RIGHT_Y:
        set_joystick_axis(1, 1, 0);
        return false;
#endif
    default:
        break;
    }
    return true;
}
