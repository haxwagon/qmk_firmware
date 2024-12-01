#include QMK_KEYBOARD_H

// layers
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

// tap dances
enum {
    TD_BRACES,
    TD_BRACKETS,
    TD_COMMSCLN,
    TD_DOTEXLM,
    TD_PARENS,
    TD_PASTSLS,
    TD_PPLSMNS,
    TD_QUOTGRV,
    TD_SLASHES,
};

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
    if (state->count == 1) {
        if (state->interrupted || !state->pressed) return TD_SINGLE_TAP;
        // Key has not been interrupted, but the key is still held. Means you want to send a 'HOLD'.
        else return TD_SINGLE_HOLD;
    } else if (state->count == 2) {
        // TD_DOUBLE_SINGLE_TAP is to distinguish between typing "pepper", and actually wanting a double tap
        // action when hitting 'pp'. Suggested use case for this return value is when you want to send two
        // keystrokes of the key, and not the 'double tap' action/macro.
        if (state->interrupted) return TD_DOUBLE_SINGLE_TAP;
        else if (state->pressed) return TD_DOUBLE_HOLD;
        else return TD_DOUBLE_TAP;
    }

    // Assumes no one is trying to type the same letter three times (at least not quickly).
    // If your tap dance key is 'KC_W', and you want to type "www." quickly - then you will need to add
    // an exception here to return a 'TD_TRIPLE_SINGLE_TAP', and define that enum just like 'TD_DOUBLE_SINGLE_TAP'
    if (state->count == 3) {
        if (state->interrupted || !state->pressed) return TD_TRIPLE_TAP;
        else return TD_TRIPLE_HOLD;
    } else return TD_UNKNOWN;
}

void td_quad_finished(tap_dance_state_t *state, void *user_data) {
    td_quad_keycodes_t* kcs = (td_quad_keycodes_t*)user_data;
    switch (current_dance_quad_state(state)) {
        case TD_SINGLE_TAP: if (kcs->kc_tap) { register_code(kcs->kc_tap); } break;
        case TD_SINGLE_HOLD: if (kcs->kc_hold) { register_code(kcs->kc_hold); } break;
        case TD_DOUBLE_TAP: if (kcs->kc_double_tap) { register_code(kcs->kc_double_tap); } break;
        case TD_DOUBLE_HOLD: if (kcs->kc_double_tap_hold) { register_code(kcs->kc_double_tap_hold); } break;
        // Last case is for fast typing. Assuming your key is `f`:
        // For example, when typing the word `buffer`, and you want to make sure that you send `ff` and not `Esc`.
        // In order to type `ff` when typing fast, the next character will have to be hit within the `TAPPING_TERM`, which by default is 200ms.
        case TD_DOUBLE_SINGLE_TAP:
            if (kcs->kc_tap) {
                tap_code(kcs->kc_tap);
                register_code(kcs->kc_tap);
            }
            break;
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

tap_dance_action_t tap_dance_actions[] = {
    // Tap once for Escape, twice for Caps Lock
    [TD_BRACES]   = ACTION_TAP_DANCE_DOUBLE(KC_LCBR, KC_RCBR),
    [TD_BRACKETS] = ACTION_TAP_DANCE_DOUBLE(KC_LBRC, KC_RBRC),
    [TD_COMMSCLN] = ACTION_TAP_DANCE_DOUBLE(KC_COMM, KC_SCLN),
    [TD_DOTEXLM]  = ACTION_TAP_DANCE_DOUBLE(KC_DOT, KC_EXLM),
    [TD_PARENS]   = ACTION_TAP_DANCE_DOUBLE(KC_LPRN, KC_RPRN),
    [TD_PASTSLS]  = ACTION_TAP_DANCE_DOUBLE(KC_PAST, KC_PSLS),
    [TD_PPLSMNS]  = ACTION_TAP_DANCE_DOUBLE(KC_PPLS, KC_PMNS),
    [TD_QUOTGRV]  = ACTION_TAP_DANCE_DOUBLE(KC_QUOT, KC_GRV),
    [TD_SLASHES]  = ACTION_TAP_DANCE_QUAD(KC_SLSH, KC_QUES, KC_BSLS, KC_PIPE)
};

// key combos
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

// key overrides
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

// joystick settings
joystick_config_t joystick_axes[JOYSTICK_AXIS_COUNT] = {
//    JOYSTICK_AXIS_IN(PINNAME, LOWVAL, 0VAL, HIGHVAL) or JOYSTICK_AXIS_VIRTUAL
    JOYSTICK_AXIS_VIRTUAL,
    JOYSTICK_AXIS_VIRTUAL,
    JOYSTICK_AXIS_VIRTUAL,
    JOYSTICK_AXIS_VIRTUAL,
    JOYSTICK_AXIS_VIRTUAL,
    JOYSTICK_AXIS_VIRTUAL,
};
static bool joystick_precision = false;
static uint16_t joystick_precision_mod = 64;
static uint16_t joystick_axis_val = 127;

enum JS_AXES {
    JS_AXIS_0_X,
    JS_AXIS_0_Y,
    JS_AXIS_0_Z,
    JS_AXIS_1_X,
    JS_AXIS_1_Y,
    JS_AXIS_1_Z
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [LAYER_QWERTY] = LAYOUT_split_3x5_2(
        KC_Q,         KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I,            KC_O,           KC_P,
        KC_A,         KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K,            KC_L,           KC_SCLN,
        LSFT_T(KC_Z), KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, TD(TD_COMMSCLN), TD(TD_DOTEXLM), RSFT_T(KC_SLSH),
        LT(TL_UPPR, KC_SPACE), LT(TL_LOWR, KC_SPACE), LT(TL_LOWR, KC_SPACE), LT(TL_UPPR, KC_SPACE)
    ),
    [LAYER_FUNC] = LAYOUT_split_3x5_2(
        KC_ESC,  TT(LAYER_GAMEPAD), TT(LAYER_MEDIA),  TT(LAYER_NUMPAD), TT(LAYER_JOYSTICK), KC_UNDS,       KC_MINS,         KC_PLUS,       KC_EQL,         KC_BSPC,
        KC_TAB,  KC_MENU,           KC_HOME,          KC_FIND,          KC_NO,              KC_LEFT,       KC_DOWN,         KC_UP,         KC_RGHT,        KC_ENT,
        KC_LSFT, KC_NO,             KC_NO,            KC_NO,            KC_NO,              TD(TD_BRACES), TD(TD_BRACKETS), TD(TD_PARENS), TD(TD_QUOTGRV), RSFT_T(KC_BSLS),
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),
    [LAYER_GAMEPAD] = LAYOUT_split_3x5_2(
        KC_ESC,  KC_Q, KC_W, KC_E, KC_R, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_TAB,  KC_A, KC_S, KC_D, KC_F, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        MO(LAYER_GAMEPAD2), KC_SPACE, TG(LAYER_GAMEPAD), TG(LAYER_GAMEPAD)
    ),
    [LAYER_GAMEPAD2] = LAYOUT_split_3x5_2(
        KC_F1,   KC_F2, KC_F3, KC_F4, KC_F5, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_1,    KC_2,  KC_3,  KC_4,  KC_5,  KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_LSFT, KC_NO, KC_T,  KC_G,  KC_B,  KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_TRNS, KC_SPACE, KC_TRNS, KC_TRNS
    ),
    [LAYER_JOYSTICK] = LAYOUT_split_3x5_2(
        JS_10, KC_P7, KC_P8, KC_P9, JS_8, JS_3, JS_2, JOYSTICK_HAT_NORTHWEST, JOYSTICK_HAT_NORTH,  JOYSTICK_HAT_NORTHEAST,
        JS_6,  KC_P4, KC_P5, KC_P6, JS_7, JS_1, JS_0, JOYSTICK_HAT_WEST,      JOYSTICK_HAT_CENTER, JOYSTICK_HAT_EAST,
        JS_11, KC_P1, KC_P2, KC_P3, JS_9, JS_5, JS_4, JOYSTICK_HAT_SOUTHWEST, JOYSTICK_HAT_SOUTH,  JOYSTICK_HAT_SOUTHEAST,
        TG(LAYER_JOYSTICK), TG(LAYER_JOYSTICK), TG(LAYER_JOYSTICK), TG(LAYER_JOYSTICK)
    ),
    [LAYER_MEDIA] = LAYOUT_split_3x5_2(
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,   KC_BRID, KC_BRIU, KC_NO,   KC_NO,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_MPRV, KC_VOLD, KC_VOLU, KC_MNXT, KC_MPLY,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_MRWD, KC_MUTE, KC_NO,   KC_MFFD, KC_NO,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),
    [LAYER_MOVE] = LAYOUT_split_3x5_2(
        MS_WHLU, MS_WHLL, MS_UP,   MS_WHLR, MS_BTN2, KC_NO,   KC_NO,    KC_NO,   KC_DEL,  KC_INS,
        MS_WHLD, MS_LEFT, MS_DOWN, MS_RGHT, MS_BTN1, KC_LEFT, KC_DOWN,  KC_UP,   KC_RGHT, KC_ENT,
        MS_BTN7, MS_BTN6, MS_BTN5, MS_BTN4, MS_BTN3, KC_HOME, KC_PGDN,  KC_PGUP, KC_END,  KC_NO,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),
    [LAYER_NUMPAD] = LAYOUT_split_3x5_2(
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, TD(TD_PASTSLS), KC_P7, KC_P8, KC_P9, KC_BSPC,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, TD(TD_PPLSMNS), KC_P4, KC_P5, KC_P6, KC_PENT,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_P0,          KC_P1, KC_P2, KC_P3, KC_PDOT,
        TG(LAYER_NUMPAD), TG(LAYER_NUMPAD), TG(LAYER_NUMPAD), TG(LAYER_NUMPAD)
    ),
    [LAYER_NUMSYMS] = LAYOUT_split_3x5_2(
        KC_F1,           KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,
        KC_1,            KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,
        LSFT_T(KC_EXLM), KC_AT,   KC_HASH, KC_DLR,  KC_PERC, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, RSFT_T(KC_RPRN),
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    int16_t precision_val = joystick_axis_val;
    if (joystick_precision) {
        precision_val -= joystick_precision_mod;
    }

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

#if defined(ENCODER_ENABLE)
bool encoder_0_update(bool clockwise) {
    switch (get_highest_layer(layer_state)) {
        case LAYER_MEDIA:
            if (clockwise) {
                tap_code(KC_VOLD);
            } else {
                tap_code(KC_VOLU);
            }
            break;
        case LAYER_JOYSTICK:
            int16_t precision_val = joystick_axis_val;
            if (clockwise) {
                joystick_set_axis(2, precision_val);
            } else {
                joystick_set_axis(2, -precision_val);
            }
            break;
        default:
            if (clockwise) {
                tap_code(MS_WHLD);
            } else {
                tap_code(MS_WHLU);
            }
            break;
    }
    return false;
}
bool encoder_update_user(uint8_t encoder_index, bool clockwise) {
    switch (encoder_index) {
        case 0:
            return encoder_0_update(clockwise);
            break;
        default:
            break;
    }
    return false;
}
#endif

#if defined(OLED_ENABLE)
bool oled_task_user(void) {
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
