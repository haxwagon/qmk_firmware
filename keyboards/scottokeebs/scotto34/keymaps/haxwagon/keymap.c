/*
Copyright 2023 Joe Scotto

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include QMK_KEYBOARD_H

// key combos
const uint16_t PROGMEM left_alt_combo[] = {KC_S, KC_D, COMBO_END};
const uint16_t PROGMEM left_ctl_combo[] = {KC_A, KC_S, COMBO_END};
const uint16_t PROGMEM left_ctl_alt_combo[] = {KC_A, KC_S, KC_D, COMBO_END};
const uint16_t PROGMEM left_ctl_gui_combo[] = {KC_A, KC_S, KC_D, KC_F, COMBO_END};
const uint16_t PROGMEM left_gui_combo[] = {KC_D, KC_F, COMBO_END};
const uint16_t PROGMEM right_alt_combo[] = {KC_K, KC_L, COMBO_END};
const uint16_t PROGMEM right_ctl_combo[] = {KC_L, KC_SCLN, COMBO_END};
const uint16_t PROGMEM right_gui_combo[] = {KC_J, KC_K, COMBO_END};
const uint16_t PROGMEM right_ctl_alt_combo[] = {KC_K, KC_L, KC_SCLN, COMBO_END};
const uint16_t PROGMEM right_ctl_gui_combo[] = {KC_J, KC_K, KC_L, KC_SCLN, COMBO_END};
const uint16_t PROGMEM edit_combo[] = {KC_X, KC_C, COMBO_END};
const uint16_t PROGMEM mouse_combo[] = {KC_C, KC_V, COMBO_END};
const uint16_t PROGMEM media_combo[] = {KC_X, KC_C, KC_V, COMBO_END};
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
    COMBO(edit_combo, MO(3)),
    COMBO(media_combo, MO(4)),
    COMBO(mouse_combo, MO(5)),
};

// key overrides
const key_override_t f11_key_override = ko_make_basic(MOD_MASK_SHIFT, KC_F1, KC_F11);
const key_override_t f12_key_override = ko_make_basic(MOD_MASK_SHIFT, KC_F2, KC_F12);
const key_override_t *key_overrides[] = {
	&f11_key_override,
	&f12_key_override
};

// joystick settings
joystick_config_t joystick_axes[JOYSTICK_AXIS_COUNT] = {
    JOYSTICK_AXIS_VIRTUAL,
    JOYSTICK_AXIS_VIRTUAL,
    JOYSTICK_AXIS_VIRTUAL,
    JOYSTICK_AXIS_VIRTUAL
};
static bool joystick_precision = false;
static uint16_t joystick_precision_mod = 64;
static uint16_t joystick_axis_val = 127;

enum LAYERS {
    LAYER_QWERTY,
    LAYER_SPECIAL,
    LAYER_NUMSYMS,
    LAYER_EDIT,
    LAYER_MEDIA,
    LAYER_MOUSE,
    LAYER_JOYSTICK
};


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [LAYER_QWERTY] = LAYOUT_split_3x5_2(
        KC_Q,         KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I,     KC_O,   KC_P,
        KC_A,         KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K,     KC_L,   KC_SCLN,
        LSFT_T(KC_Z), KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMMA, KC_DOT, RSFT_T(KC_SLSH),
                LT(LAYER_NUMSYMS, KC_SPACE), LT(LAYER_SPECIAL, KC_SPACE), LT(LAYER_SPECIAL, KC_SPACE), LT(LAYER_NUMSYMS, KC_SPACE)
    ),
    [LAYER_SPECIAL] = LAYOUT_split_3x5_2(
        KC_ESC,         KC_NO,   KC_NO,   KC_NO,   TG(LAYER_JOYSTICK), KC_MINS, KC_PLUS, KC_EQL,  KC_DEL,  KC_BSPC,
        KC_TAB,         KC_NO,   KC_NO,   KC_NO,   KC_NO,              KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, KC_ENT,
        LSFT_T(KC_GRV), KC_NO,   KC_LCBR, KC_RCBR, KC_NO,              KC_NO,   KC_LBRC, KC_RBRC, KC_QUOT, RSFT_T(KC_BSLS),
                KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),
    [LAYER_NUMSYMS] = LAYOUT_split_3x5_2(
        KC_F1,           KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,
        KC_1,            KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,
        LSFT_T(KC_EXLM), KC_AT,   KC_HASH, KC_DLR,  KC_PERC, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, RSFT_T(KC_RPRN),
                KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),
    [LAYER_EDIT] = LAYOUT_split_3x5_2(
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, LCTL(KC_X), LCTL(KC_Z), KC_UP,   KC_DEL,  KC_INS,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, LCTL(KC_C), KC_LEFT,    KC_DOWN, KC_RGHT, KC_ENT,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, LCTL(KC_V), KC_PGDN,    KC_PGUP, KC_HOME, KC_END,
                KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),
    [LAYER_MEDIA] = LAYOUT_split_3x5_2(
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,   KC_MRWD, KC_VOLU, KC_MFFD, KC_NO,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_MPLY, KC_MPRV, KC_VOLD, KC_MNXT, KC_NO,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,   KC_NO,   KC_MUTE, KC_NO,   KC_NO,
                KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),
    [LAYER_MOUSE] = LAYOUT_split_3x5_2(
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, MS_BTN5, MS_BTN2, MS_BTN3, MS_BTN4, MS_BTN6,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, MS_LEFT, MS_DOWN, MS_UP,   MS_RGHT, MS_BTN1,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, MS_WHLL, MS_WHLD, MS_WHLU, MS_WHLR, MS_BTN7,
                KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),
    [LAYER_JOYSTICK] = LAYOUT_split_3x5_2(
        JS_10, KC_P7, KC_P8, KC_P9, JS_8, JS_3, JS_2, JOYSTICK_HAT_NORTHWEST, JOYSTICK_HAT_NORTH,  JOYSTICK_HAT_NORTHEAST,
        JS_6,  KC_P4, KC_P5, KC_P6, JS_7, JS_1, JS_0, JOYSTICK_HAT_WEST,      JOYSTICK_HAT_CENTER, JOYSTICK_HAT_EAST,
        JS_11, KC_P1, KC_P2, KC_P3, JS_9, JS_5, JS_4, JOYSTICK_HAT_SOUTHWEST, JOYSTICK_HAT_SOUTH,  JOYSTICK_HAT_SOUTHEAST,
                TG(LAYER_JOYSTICK), TG(LAYER_JOYSTICK), TG(LAYER_JOYSTICK), TG(LAYER_JOYSTICK)
    )
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    int16_t precision_val = joystick_axis_val;
    if (joystick_precision) {
        precision_val -= joystick_precision_mod;
    }

    switch (keycode) {
        case KC_P4:
            joystick_set_axis(0, record->event.pressed ? -precision_val : 0);
            return false;
        case KC_P6:
            joystick_set_axis(0, record->event.pressed ? precision_val : 0);
            return false;
        case KC_P8:
            joystick_set_axis(1, record->event.pressed ? -precision_val : 0);
            return false;
        case KC_P2:
        case KC_P5:
            joystick_set_axis(1, record->event.pressed ? precision_val : 0);
            return false;
        case KC_P7:
            joystick_set_axis(2, record->event.pressed ? -precision_val : 0);
            return false;
        case KC_P1:
            joystick_set_axis(2, record->event.pressed ? precision_val : 0);
            return false;
        case KC_P9:
            joystick_set_axis(3, record->event.pressed ? -precision_val : 0);
            return false;
        case KC_P3:
            joystick_set_axis(3, record->event.pressed ? precision_val : 0);
            return false;
        case KC_P0:
            joystick_precision = record->event.pressed;
            return false;
    }
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
            oled_write_P(PSTR(" Z  X  C  V  B  N  M  ,  .  /\n"), false);
            break;
        case LAYER_SPECIAL:
            oled_write_P(PSTR("Special\n"), false);
            oled_write_P(PSTR("ES          JS  -  +  = DE <-\n"), false);
            oled_write_P(PSTR("->              <  v  ^  > EN\n"), false);
            oled_write_P(PSTR(" `     {  }        [  ]  '  \\\n"), false);
            break;
        case LAYER_NUMSYMS:
            oled_write_P(PSTR("Numbers & Symbols\n"), false);
            oled_write_P(PSTR("F1 F2 F3 F4 F5 F6 F7 F8 F9 F0\n"), false);
            oled_write_P(PSTR(" 1  2  3  4  5  6  7  8  9  0\n"), false);
            oled_write_P(PSTR(" !  @  #  $  %  ^  &  *  (  )\n"), false);
            break;
        case LAYER_EDIT:
            oled_write_P(PSTR("Edit\n"), false);
            break;
        case LAYER_MEDIA:
            oled_write_P(PSTR("Media\n"), false);
            break;
        case LAYER_MOUSE:
            oled_write_P(PSTR("Mouse\n"), false);
            break;
        case LAYER_JOYSTICK:
            oled_write_P(PSTR("Joystick\n"), false);
            oled_write_P(PSTR("10     ^     8  3  2 NW  N NE\n"), false);
            oled_write_P(PSTR(" 6  <  v  >  7  1  0  W  C  E\n"), false);
            oled_write_P(PSTR("11     v     9  5  4 SW  S SE\n"), false);
            break;
        default:
            // Or use the write_ln shortcut over adding '\n' to the end of your string
            oled_write_ln_P(PSTR("Undefined"), false);
    }

    return false;
}
#endif
