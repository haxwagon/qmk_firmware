#include QMK_KEYBOARD_H
#include "analog.h"
#include "print.h"

enum LAYERS {
    LAYER_QWERTY,  // default
    LAYER_FUNC,    // lower
    LAYER_NUMSYMS, // upper
    LAYER_MOVE,    // adjust
    // ** MODES **
    LAYER_GAMEPAD,
    LAYER_GAMEPAD2,
    LAYER_MEDIA,
    LAYER_JOYSTICK,
    LAYER_NUMPAD,
};
static const uint16_t LAYER_DEFAULT = LAYER_QWERTY;

#if defined(COMBO_ENABLE)
const uint16_t PROGMEM left_alt_combo[]      = {KC_X, KC_C, COMBO_END};
const uint16_t PROGMEM left_ctl_combo[]      = {KC_C, KC_V, COMBO_END};
const uint16_t PROGMEM left_ctl_alt_combo[]  = {KC_X, KC_C, KC_V, COMBO_END};
const uint16_t PROGMEM left_ctl_gui_combo[]  = {KC_C, KC_V, KC_B, COMBO_END};
const uint16_t PROGMEM left_gui_combo[]      = {KC_V, KC_B, COMBO_END};
const uint16_t PROGMEM right_alt_combo[]     = {KC_COMM, KC_DOT, COMBO_END};
const uint16_t PROGMEM right_ctl_combo[]     = {KC_M, KC_COMM, COMBO_END};
const uint16_t PROGMEM right_gui_combo[]     = {KC_N, KC_M, COMBO_END};
const uint16_t PROGMEM right_ctl_alt_combo[] = {KC_M, KC_COMM, KC_DOT, COMBO_END};
const uint16_t PROGMEM right_ctl_gui_combo[] = {KC_N, KC_M, KC_COMM, COMBO_END};
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

#if defined(KEY_OVERRIDE_ENABLE)
const key_override_t backspace_del_key_override = ko_make_basic(MOD_MASK_SHIFT, KC_BSPC, KC_DEL);
const key_override_t f11_key_override           = ko_make_basic(MOD_MASK_SHIFT, KC_F1, KC_F11);
const key_override_t f12_key_override           = ko_make_basic(MOD_MASK_SHIFT, KC_F2, KC_F12);
const key_override_t f13_key_override           = ko_make_basic(MOD_MASK_SHIFT, KC_F3, KC_F13);
const key_override_t f14_key_override           = ko_make_basic(MOD_MASK_SHIFT, KC_F4, KC_F14);
const key_override_t f15_key_override           = ko_make_basic(MOD_MASK_SHIFT, KC_F5, KC_F15);
const key_override_t f16_key_override           = ko_make_basic(MOD_MASK_SHIFT, KC_F6, KC_F16);
const key_override_t f17_key_override           = ko_make_basic(MOD_MASK_SHIFT, KC_F7, KC_F17);
const key_override_t f18_key_override           = ko_make_basic(MOD_MASK_SHIFT, KC_F8, KC_F18);
const key_override_t f19_key_override           = ko_make_basic(MOD_MASK_SHIFT, KC_F9, KC_F19);
const key_override_t f20_key_override           = ko_make_basic(MOD_MASK_SHIFT, KC_F10, KC_F20);
const key_override_t *key_overrides[]           = {
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
//#region Tap Dance Quad Support
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

typedef struct {
    uint16_t kc_tap;
    uint16_t kc_hold;
    uint16_t kc_double_tap;
    uint16_t kc_double_tap_hold;
} td_quad_keycodes_t;

td_quad_state_t current_dance_quad_state(tap_dance_state_t *state) {
    switch (state->count) {
        case 1:
            if (state->interrupted || !state->pressed) return TD_SINGLE_TAP;
            // Key has not been interrupted, but the key is still held. Means you want to send a 'HOLD'.
            return TD_SINGLE_HOLD;
        case 2:
            if (state->interrupted) return TD_DOUBLE_SINGLE_TAP;
            else if (state->pressed) return TD_DOUBLE_HOLD;
            return TD_DOUBLE_TAP;
        case 3:
            if (state->interrupted || !state->pressed) return TD_TRIPLE_TAP;
            return TD_TRIPLE_HOLD;
        default:
            return TD_UNKNOWN;
    }
}

void td_quad_finished(tap_dance_state_t *state, void *user_data) {
    td_quad_keycodes_t* kcs = (td_quad_keycodes_t*)user_data;
    switch (current_dance_quad_state(state)) {
        case TD_SINGLE_TAP: if (kcs->kc_tap) { register_code(kcs->kc_tap); } break;
        case TD_SINGLE_HOLD: if (kcs->kc_hold) { register_code(kcs->kc_hold); } break;
        case TD_DOUBLE_TAP: if (kcs->kc_double_tap) { register_code(kcs->kc_double_tap); } break;
        case TD_DOUBLE_HOLD: if (kcs->kc_double_tap_hold) { register_code(kcs->kc_double_tap_hold); } break;
        case TD_DOUBLE_SINGLE_TAP: if (kcs->kc_tap) { tap_code(kcs->kc_tap); register_code(kcs->kc_tap); } break;
        default: break;
    }
}

void td_quad_reset(tap_dance_state_t *state, void *user_data) {
    td_quad_keycodes_t* kcs = (td_quad_keycodes_t*)user_data;
    switch (current_dance_quad_state(state)) {
        case TD_SINGLE_TAP: if (kcs->kc_tap) { unregister_code(kcs->kc_tap); } break;
        case TD_SINGLE_HOLD: if (kcs->kc_hold) { unregister_code(kcs->kc_hold); } break;
        case TD_DOUBLE_TAP: if (kcs->kc_double_tap) { unregister_code(kcs->kc_double_tap); } break;
        case TD_DOUBLE_HOLD: if (kcs->kc_double_tap_hold) { unregister_code(kcs->kc_double_tap_hold); } break;
        case TD_DOUBLE_SINGLE_TAP: if (kcs->kc_tap) { unregister_code(kcs->kc_tap); } break;
        default: break;
    }
}

#define ACTION_TAP_DANCE_QUAD(kc_tap, kc_hold, kc_double_tap, kc_double_tap_hold) \
    { .fn = {NULL, td_quad_finished, td_quad_reset, NULL}, \
      .user_data = (void *)&((td_quad_keycodes_t){kc_tap, kc_hold, kc_double_tap, kc_double_tap_hold}), }
#endif

enum {
    TD_BRACES,
    TD_BRACKETS,
    TD_COMMSCLN,
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
    // Tap once for Escape, twice for Caps Lock
    [TD_BRACES]   = ACTION_TAP_DANCE_DOUBLE(KC_LCBR, KC_RCBR),
    [TD_BRACKETS] = ACTION_TAP_DANCE_DOUBLE(KC_LBRC, KC_RBRC),
    [TD_COMMSCLN] = ACTION_TAP_DANCE_DOUBLE(KC_COMM, KC_SCLN),
    [TD_DOTEXLM]  = ACTION_TAP_DANCE_DOUBLE(KC_DOT, KC_EXLM),
    [TD_EQLPLUS]  = ACTION_TAP_DANCE_DOUBLE(KC_EQL, KC_PLUS),
    [TD_MINSUNDS] = ACTION_TAP_DANCE_DOUBLE(KC_MINS, KC_UNDS),
    [TD_PARENS]   = ACTION_TAP_DANCE_DOUBLE(KC_LPRN, KC_RPRN),
    [TD_PASTSLS]  = ACTION_TAP_DANCE_DOUBLE(KC_PAST, KC_PSLS),
    [TD_PPLSMNS]  = ACTION_TAP_DANCE_DOUBLE(KC_PPLS, KC_PMNS),
    [TD_QUOTGRV]  = ACTION_TAP_DANCE_DOUBLE(KC_QUOT, KC_GRV),
    [TD_SLASHES]  = ACTION_TAP_DANCE_QUAD(KC_SLSH, KC_QUES, KC_BSLS, KC_PIPE)
};
#endif

#if defined(JOYSTICK_ENABLE)
// joystick settings
joystick_config_t joystick_axes[JOYSTICK_AXIS_COUNT] = {
    JOYSTICK_AXIS_IN(GP26, 0, 127, 256),
    JOYSTICK_AXIS_IN(GP27, 0, 127, 256),
    JOYSTICK_AXIS_VIRTUAL,
    JOYSTICK_AXIS_VIRTUAL,
    JOYSTICK_AXIS_VIRTUAL,
    JOYSTICK_AXIS_VIRTUAL,
};
static const uint8_t js_maxvalue = 127;
static const uint8_t js_precisions[2][3] = { {10, 10, 10}, {10, 10, 10} };
static uint16_t js_values[2][3] = { {0, 0, 0}, {0, 0, 0} };
enum JS_AXES {
    JS_AXIS_0_X,
    JS_AXIS_0_Y,
    JS_AXIS_0_Z,
    JS_AXIS_1_X,
    JS_AXIS_1_Y,
    JS_AXIS_1_Z
};
uint8_t joystick_axis(uint8_t joystick, uint8_t axis) {
    switch (joystick) {
        case 0:
            switch (axis) {
                case 0: return JS_AXIS_0_X;
                case 1: return JS_AXIS_0_Y;
                case 2: return JS_AXIS_0_Z;
                default: break;
            }
            break;
        case 1:
            switch (axis) {
                case 0: return JS_AXIS_1_X;
                case 1: return JS_AXIS_1_Y;
                case 2: return JS_AXIS_1_Z;
                default: break;
            }
            break;
        default:
            break;
    }
    return 0;
}
void update_joystick_axis(uint8_t joystick, uint8_t axis, uint8_t delta) {
    js_values[joystick][axis] += delta;
    if (js_values[joystick][axis] > js_maxvalue) {
        js_values[joystick][axis] = js_maxvalue;
    }
    if (js_values[joystick][axis] < -js_maxvalue) {
        js_values[joystick][axis] = -js_maxvalue;
    }
    joystick_set_axis(joystick_axis(joystick, axis), js_values[joystick][axis]);
}
#endif

#if defined(ENCODER_ENABLE)
bool encoder_update_user(uint8_t encoder, bool clockwise) {
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
            break;
    }
    return false;
}
#endif

#if defined(OLED_ENABLE)
oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    dprint("oled_init_user");
    return OLED_ROTATION_0;
}

bool oled_task_user(void) {
    dprint("oled_task_user");
    oled_set_cursor(0, 1);
    oled_write("Hello", false);
    oled_set_cursor(1, 1);

    // Host Keyboard LED Status
    led_t led_state = host_keyboard_led_state();
    oled_write_P(led_state.num_lock ? PSTR("NUM ") : PSTR("    "), false);
    oled_write_P(led_state.caps_lock ? PSTR("CAP ") : PSTR("    "), false);
    oled_write_P(led_state.scroll_lock ? PSTR("SCR ") : PSTR("    "), false);

    // Host Keyboard Layer Status
    oled_write_P(PSTR("Layer: "), false);
    switch (get_highest_layer(layer_state)) {
        case LAYER_QWERTY:
            oled_write_P(PSTR("Default\n"), false);
            oled_write_P(PSTR(" Q  W  E  R  T  Y  U  I  O  P\n"), false);
            oled_write_P(PSTR(" A  S  D  F  G  H  J  K  L  ;\n"), false);
            oled_write_P(PSTR(" Z  X  C  V  B  N  M ,; .!  /\n"), false);
            break;
        case LAYER_FUNC:
            oled_write_P(PSTR("Functions\n"), false);
            oled_write_P(PSTR("ES    ME NP JS  _  -  +  = <-\n"), false);
            oled_write_P(PSTR("->              <  v  ^  > EN\n"), false);
            oled_write_P(PSTR("               {} [] () '`  \\\n"), false);
            break;
        case LAYER_GAMEPAD:
            oled_write_P(PSTR("Gamepad\n"), false);
            oled_write_P(PSTR("ES  Q  W  E  R               \n"), false);
            oled_write_P(PSTR("->  A  S  D  F               \n"), false);
            oled_write_P(PSTR("LS  Z  X  C  V               \n"), false);
            break;
        case LAYER_GAMEPAD2:
            oled_write_P(PSTR("Gamepad 2\n"), false);
            oled_write_P(PSTR("F1 F2 F3 F4 F5               \n"), false);
            oled_write_P(PSTR(" 1  2  3  4  5               \n"), false);
            oled_write_P(PSTR("LS     T  G  B               \n"), false);
            break;
        case LAYER_JOYSTICK:
            oled_write_P(PSTR("Joystick\n"), false);
            oled_write_P(PSTR("10 z< /\\ z>  8  3  2 NW  N NE\n"), false);
            oled_write_P(PSTR(" 6 <= \\/ =>  7  1  0  W  C  E\n"), false);
            oled_write_P(PSTR("11    \\/     9  5  4 SW  S SE\n"), false);
            break;
        case LAYER_MEDIA:
            oled_write_P(PSTR("Media\n"), false);
            oled_write_P(PSTR("                  BD BU      \n"), false);
            oled_write_P(PSTR("               |< VD VU >| EN\n"), false);
            oled_write_P(PSTR("               << MU    >>   \n"), false);
            break;
        case LAYER_MOVE:
            oled_write_P(PSTR("Move\n"), false);
            oled_write_P(PSTR("WU WL MU WR M2          DE IN\n"), false);
            oled_write_P(PSTR("WD ML MD MR M1  <  v  ^  > EN\n"), false);
            oled_write_P(PSTR("M7 M6 M5 M4 M3 HM PD PU ED   \n"), false);
            break;
        case LAYER_NUMPAD:
            oled_write_P(PSTR("Numpad\n"), false);
            oled_write_P(PSTR("               */  7  8  9 <-\n"), false);
            oled_write_P(PSTR("               +-  4  5  6 EN\n"), false);
            oled_write_P(PSTR("                0  1  2  3  .\n"), false);
            break;
        case LAYER_NUMSYMS:
            oled_write_P(PSTR("Numbers & Symbols\n"), false);
            oled_write_P(PSTR("F1 F2 F3 F4 F5 F6 F7 F8 F9 F0\n"), false);
            oled_write_P(PSTR(" 1  2  3  4  5  6  7  8  9  0\n"), false);
            oled_write_P(PSTR(" !  @  #  $  %  ^  &  *  (  )\n"), false);
            break;
        default:
            // Or use the write_ln shortcut over adding '\n' to the end of your string
            oled_write_ln_P(PSTR("Undefined"), false);
    }

    return false;
}
#endif

#if defined(POINTING_DEVICE_ENABLE)
report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    dprintf("Pointing Device: X: %d, Y: %d, Buttons: %d\n", mouse_report.x, mouse_report.y, mouse_report.buttons);
    /* TODO: intercept mouse movement to turn into JS1 x/y if in JS mode
    if (set_scrolling) {
        mouse_report.h = mouse_report.x;
        mouse_report.v = mouse_report.y;
        mouse_report.x = 0;
        mouse_report.y = 0;
    }

// modify buttons
 // @brief Handles pointing device buttons
 //
 // Returns modified button bitmask using bool pressed and selected pointing_device_buttons_t button in uint8_t buttons bitmask.
 //
 // @param buttons[in] uint8_t bitmask
 // @param pressed[in] bool
 // @param button[in] pointing_device_buttons_t value
 // @return Modified uint8_t bitmask buttons
// __attribute__((weak)) uint8_t pointing_device_handle_buttons(uint8_t buttons, bool pressed, pointing_device_buttons_t button) {
    mouse_report.buttons = pointing_device_handle_buttons(mouse_report.buttons, data.button, POINTING_DEVICE_BUTTON1);

    */
    return mouse_report;
}
#endif

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [LAYER_QWERTY] = LAYOUT_ortho_4x10(
        KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P,
        KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN,
        LSFT_T(KC_Z), KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, TD(TD_COMMSCLN), TD(TD_DOTEXLM), RSFT_T(KC_SLSH),
        KC_NO, KC_NO, KC_NO, LT(TL_UPPR, KC_SPACE), LT(TL_LOWR, KC_SPACE), LT(TL_LOWR, KC_SPACE), LT(TL_UPPR, KC_SPACE), KC_NO, KC_NO, KC_NO
    ),
    [LAYER_FUNC] = LAYOUT_ortho_4x10(
        KC_ESC, TT(LAYER_GAMEPAD), TT(LAYER_MEDIA), TT(LAYER_NUMPAD), TT(LAYER_JOYSTICK), DM_REC1, DM_REC2, DM_PLY2, DM_PLY1, KC_BSPC,
        KC_TAB, KC_MENU, KC_HOME, KC_FIND, KC_NO, KC_LEFT, KC_DOWN, KC_UP, KC_RGHT, KC_ENT,
        KC_LSFT, TD(TD_EQLPLUS), TD(TD_MINSUNDS), TD(TD_SLASHES), KC_NO, TD(TD_BRACES), TD(TD_BRACKETS), TD(TD_PARENS), TD(TD_QUOTGRV), RSFT_T(KC_BSLS),
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),
    [LAYER_GAMEPAD] = LAYOUT_ortho_4x10(
        KC_ESC, KC_Q, KC_W, KC_E, KC_R, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_TAB, KC_A, KC_S, KC_D, KC_F, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_NO, KC_NO, KC_NO, MO(LAYER_GAMEPAD2), KC_SPACE, TO(LAYER_DEFAULT), TO(LAYER_DEFAULT), KC_NO, KC_NO, KC_NO
    ),
    [LAYER_GAMEPAD2] = LAYOUT_ortho_4x10(
        KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_1, KC_2, KC_3, KC_4, KC_5, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_LSFT, KC_NO, KC_T, KC_G, KC_B, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_SPACE, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),
    [LAYER_JOYSTICK] = LAYOUT_ortho_4x10(
        JS_10, KC_P7, KC_P8, KC_P9, JS_8, JS_4, JS_2, JOYSTICK_HAT_NORTHWEST, JOYSTICK_HAT_NORTH, JOYSTICK_HAT_NORTHEAST,
        JS_6, KC_P4, KC_P5, KC_P6, JS_7, JS_1, JS_0, JOYSTICK_HAT_WEST, JOYSTICK_HAT_CENTER, JOYSTICK_HAT_EAST,
        JS_11, KC_P1, KC_P2, KC_P3, JS_9, JS_5, JS_3, JOYSTICK_HAT_SOUTHWEST, JOYSTICK_HAT_SOUTH, JOYSTICK_HAT_SOUTHEAST,
        KC_NO, KC_NO, KC_NO, TO(LAYER_DEFAULT), TO(LAYER_DEFAULT), TO(LAYER_DEFAULT), TO(LAYER_DEFAULT), KC_NO, KC_NO, KC_NO
    ),
    [LAYER_MEDIA] = LAYOUT_ortho_4x10(
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_BRID, KC_BRIU, KC_NO, KC_NO,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_MPRV, KC_VOLD, KC_VOLU, KC_MNXT, KC_MPLY,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_MRWD, KC_MUTE, KC_NO, KC_MFFD, KC_NO,
        KC_NO, KC_NO, KC_NO, TO(LAYER_DEFAULT), TO(LAYER_DEFAULT), TO(LAYER_DEFAULT), TO(LAYER_DEFAULT), KC_NO, KC_NO, KC_NO
    ),
    [LAYER_MOVE] = LAYOUT_ortho_4x10(
        MS_WHLU, MS_WHLL, MS_UP, MS_WHLR, MS_BTN2, KC_NO, KC_NO, KC_NO, KC_DEL, KC_INS,
        MS_WHLD, MS_LEFT, MS_DOWN, MS_RGHT, MS_BTN1, KC_LEFT, KC_DOWN, KC_UP, KC_RGHT, KC_ENT,
        MS_BTN7, MS_BTN6, MS_BTN5, MS_BTN4, MS_BTN3, KC_HOME, KC_PGDN, KC_PGUP, KC_END, KC_NO,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),
    [LAYER_NUMPAD] = LAYOUT_ortho_4x10(
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, TD(TD_PASTSLS), KC_P7, KC_P8, KC_P9, KC_BSPC,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, TD(TD_PPLSMNS), KC_P4, KC_P5, KC_P6, KC_PENT,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_P0, KC_P1, KC_P2, KC_P3, KC_PDOT,
        KC_NO, KC_NO, KC_NO, TO(LAYER_DEFAULT), TO(LAYER_DEFAULT), TO(LAYER_DEFAULT), TO(LAYER_DEFAULT), KC_NO, KC_NO, KC_NO
    ),
    [LAYER_NUMSYMS] = LAYOUT_ortho_4x10(
        KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10,
        KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0,
        LSFT_T(KC_EXLM), KC_AT, KC_HASH, KC_DLR, KC_PERC, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, RSFT_T(KC_RPRN),
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_SPACE, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),
};

void keyboard_post_init_user(void) {
  // Customise these values to desired behaviour
  debug_enable=true;
  debug_matrix=true;
  debug_keyboard=true;
  debug_mouse=true;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // int16_t precision_val = joystick_axis_val;
    // if (joystick_precision) {
    //     precision_val -= joystick_precision_mod;
    // }
    // switch (keycode) {
    //     case KC_P4:
    //         joystick_set_axis(JS_AXIS_0_X, record->event.pressed ? -precision_val : 0);
    //         return false;
    //     case KC_P6:
    //         joystick_set_axis(JS_AXIS_0_X, record->event.pressed ? precision_val : 0);
    //         return false;
    //     case KC_P8:
    //         joystick_set_axis(JS_AXIS_0_Y, record->event.pressed ? -precision_val : 0);
    //         return false;
    //     case KC_P2:
    //     case KC_P5:
    //         joystick_set_axis(JS_AXIS_0_Y, record->event.pressed ? precision_val : 0);
    //         return false;
    //     case KC_P7:
    //         joystick_set_axis(JS_AXIS_Z, record->event.pressed ? -precision_val : 0);
    //         return false;
    //     case KC_P9:
    //         joystick_set_axis(JS_AXIS_Z, record->event.pressed ? precision_val : 0);
    //         return false;
    //     case KC_P0:
    //         joystick_precision = record->event.pressed;
    //         return false;
    // }
    return true;
}
