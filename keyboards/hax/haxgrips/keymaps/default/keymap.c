#include QMK_KEYBOARD_H
#include "cirque_pinnacles.h"
#if defined(JOYSTICK_ENABLE)
#include "joysticks.h"
#endif
#if defined(OLED_ENABLE)
#include "oled.h"
#endif
#include "print.h"
#include "tap_dance_quad.h"

#ifndef POINTING_DEVICE_MAX_DPI
#    define POINTING_DEVICE_MAX_DPI 16
#endif
#ifndef POINTING_DEVICE_MIN_DPI
#    define POINTING_DEVICE_MIN_DPI 1
#endif

enum LAYERS {
    LAYER_QWERTY,  // default
    LAYER_NUMSYMS, // upper
    LAYER_FUNC,    // lower
    LAYER_MOVE,    // adjust
#if defined(JOYSTICK_ENABLE)
    LAYER_JOYSTICK,
#endif
    LAYER_MEDIA,
    LAYER_MENU,
    LAYER_NUMPAD,
    LAYER_APP_FPS,
    LAYER_APP_MOBA,
};

static const uint16_t LAYER_DEFAULT = LAYER_QWERTY;

static const char* LAYER_MAPS[] = {
    [LAYER_QWERTY]   = "\
 Q  W  E  R  T      \n\
       Y  U  I  O  P\n\
 A  S  D  F  G      \n\
       H  J  K  L  '\n\
 Z  X  C  V  B      \n\
       N  M ,; .!  /\n\
",
    [LAYER_FUNC]     = "\
ES          MO      \n\
      R1 R2 P2 P1 <-\n\
-> MN HM FD ME      \n\
       <  v  ^  > EN\n\
LS '` =+ -_ /\\      \n\
      {} [] <> ()  \\\n\
",
#if defined(JOYSTICK_ENABLE)
    [LAYER_JOYSTICK] = "\
   NW  N NE         \n\
         LB LT RT RB\n\
DP  W  C  E         \n\
      BT  X  A  B  Y\n\
   SW  S SE         \n\
      RA L3 SE ST R3\n\
",
#endif
    [LAYER_MEDIA]   = "\
                    \n\
         BD BU      \n\
                    \n\
      |< VD VU >| EN\n\
                    \n\
      << MU    >>   \n\
",
    [LAYER_MOVE]    = "\
WU WL MU WR M2      \n\
               DE IN\n\
WD ML MD MR M1      \n\
       <  v  ^  > EN\n\
M7 M6 M5 M4 M3      \n\
      HM PD PU ED   \n\
",
    [LAYER_NUMPAD]  = "\
                    \n\
      */  6  8  9 <-\n\
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
    [LAYER_APP_FPS]  = "\
 1  2  3  4  5      \n\
      SS HL         \n\
LS  C SP  Q  E      \n\
                    \n\
cZ  X  V Rt Fg      \n\
                    \n\
",
    [LAYER_APP_MOBA] = "\
 1  2  3  4  5      \n\
      SS HL         \n\
 Q  W  E  R  A      \n\
                    \n\
tE  T  D  F  B      \n\
                    \n\
",
};

static const char* LAYER_NAMES[] = {
    [LAYER_QWERTY] = "Default",
    [LAYER_FUNC] = "Functions",
#if defined(JOYSTICK_ENABLE)
    [LAYER_JOYSTICK] = "Joystick",
#endif
    [LAYER_MEDIA] = "Media",
    [LAYER_MOVE] = "Move",
    [LAYER_NUMPAD] = "Numpad",
    [LAYER_NUMSYMS] = "Nums & Syms",
    [LAYER_APP_FPS] = "App: FPS",
    [LAYER_APP_MOBA] = "App: MOBA",
};

enum CUSTOM_KEYCODES {
    CKC_POINTING_DEVICE_INC_DPI = SAFE_RANGE,
    CKC_POINTING_DEVICE_DEC_DPI,
    CKC_JS_LEFT_DPAD_TOGGLE,
    CKC_JS_RIGHT_BUTTONS_TOGGLE,
    CKC_JS_RIGHT_AXES_TOGGLE,
    CKC_MENU_TOGGLE,
    CKC_MENU_SELECT,
    CKC_MENU_HIGHLIGHT_NEXT,
    CKC_MENU_HIGHLIGHT_PREV,
};

static const uint16_t* _cirque_pinnacles_nineboxes[CIRQUE_PINNACLES_COUNT];
bool cirque_pinnacles_ninebox_active(uint8_t cirque_index) {
    return _cirque_pinnacles_nineboxes[cirque_index] != NULL;
}

uint16_t cirque_pinnacles_get_ninebox(uint8_t cirque_index, uint8_t ninebox_index) {
    return _cirque_pinnacles_nineboxes[cirque_index][ninebox_index];
}

#if defined(KEY_OVERRIDE_ENABLE)
const key_override_t  backspace_del_key_override = ko_make_basic(MOD_MASK_SHIFT, KC_BSPC, KC_DEL);
const key_override_t  shift1_f1_key_override     = ko_make_basic(MOD_MASK_SHIFT, KC_1, KC_F1);
const key_override_t  shift2_f2_key_override     = ko_make_basic(MOD_MASK_SHIFT, KC_2, KC_F2);
const key_override_t  shift3_f3_key_override     = ko_make_basic(MOD_MASK_SHIFT, KC_3, KC_F3);
const key_override_t  shift4_f4_key_override     = ko_make_basic(MOD_MASK_SHIFT, KC_4, KC_F4);
const key_override_t  shift5_f5_key_override     = ko_make_basic(MOD_MASK_SHIFT, KC_5, KC_F5);
const key_override_t  f11_key_override           = ko_make_basic(MOD_MASK_SHIFT, KC_F1, KC_F11);
const key_override_t  f12_key_override           = ko_make_basic(MOD_MASK_SHIFT, KC_F2, KC_F12);
const key_override_t  f13_key_override           = ko_make_basic(MOD_MASK_SHIFT, KC_F3, KC_F13);
const key_override_t  f14_key_override           = ko_make_basic(MOD_MASK_SHIFT, KC_F4, KC_F14);
const key_override_t  f15_key_override           = ko_make_basic(MOD_MASK_SHIFT, KC_F5, KC_F15);
const key_override_t  f16_key_override           = ko_make_basic(MOD_MASK_SHIFT, KC_F6, KC_F16);
const key_override_t  f17_key_override           = ko_make_basic(MOD_MASK_SHIFT, KC_F7, KC_F17);
const key_override_t  f18_key_override           = ko_make_basic(MOD_MASK_SHIFT, KC_F8, KC_F18);
const key_override_t  f19_key_override           = ko_make_basic(MOD_MASK_SHIFT, KC_F9, KC_F19);
const key_override_t  f20_key_override           = ko_make_basic(MOD_MASK_SHIFT, KC_F10, KC_F20);
const key_override_t* key_overrides[]            = {
    &backspace_del_key_override,
    &shift1_f1_key_override,
    &shift2_f2_key_override,
    &shift3_f3_key_override,
    &shift4_f4_key_override,
    &shift5_f5_key_override,
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
enum {
    TD_ANGLES,
    TD_BRACES,
    TD_BRACKETS,
    TD_COMMSCLN,
    TD_DOTCOLN,
    TD_DOTEXLM,
    TD_EQLPLUS,
    TD_F_G,
    TD_M_N,
    TD_MINSUNDS,
    TD_PARENS,
    TD_PASTSLS,
    TD_PPLSMNS,
    TD_QUOTGRV,
    TD_R_T,
    TD_SLASHES,
    TD_TAB_ESC,
};

tap_dance_action_t tap_dance_actions[] = {
    [TD_ANGLES]   = ACTION_TAP_DANCE_DOUBLE(KC_LABK, KC_RABK),
    [TD_BRACES]   = ACTION_TAP_DANCE_DOUBLE(KC_LCBR, KC_RCBR),
    [TD_BRACKETS] = ACTION_TAP_DANCE_DOUBLE(KC_LBRC, KC_RBRC),
    [TD_COMMSCLN] = ACTION_TAP_DANCE_DOUBLE(KC_COMM, KC_SCLN),
    [TD_DOTCOLN]  = ACTION_TAP_DANCE_QUAD(KC_DOT, KC_COLN, 0, 0),
    [TD_DOTEXLM]  = ACTION_TAP_DANCE_DOUBLE(KC_DOT, KC_EXLM),
    [TD_EQLPLUS]  = ACTION_TAP_DANCE_DOUBLE(KC_EQL, KC_PLUS),
    [TD_F_G]      = ACTION_TAP_DANCE_DOUBLE(KC_F, KC_G),
    [TD_M_N]      = ACTION_TAP_DANCE_DOUBLE(KC_M, KC_N),
    [TD_MINSUNDS] = ACTION_TAP_DANCE_DOUBLE(KC_MINS, KC_UNDS),
    [TD_PARENS]   = ACTION_TAP_DANCE_DOUBLE(KC_LPRN, KC_RPRN),
    [TD_PASTSLS]  = ACTION_TAP_DANCE_DOUBLE(KC_PAST, KC_PSLS),
    [TD_PPLSMNS]  = ACTION_TAP_DANCE_DOUBLE(KC_PPLS, KC_PMNS),
    [TD_QUOTGRV]  = ACTION_TAP_DANCE_DOUBLE(KC_QUOT, KC_GRV),
    [TD_R_T]      = ACTION_TAP_DANCE_DOUBLE(KC_R, KC_T),
    [TD_SLASHES]  = ACTION_TAP_DANCE_QUAD(KC_SLSH, KC_QUES, KC_BSLS, KC_PIPE),
    [TD_TAB_ESC]  = ACTION_TAP_DANCE_DOUBLE(KC_TAB, KC_ESC),
};
#endif

#if defined(JOYSTICK_ENABLE)
static const uint16_t js_left_dpad_ninebox[9] = {
    KC_JS_HAT_UL, KC_JS_HAT_U, KC_JS_HAT_UR,
    KC_JS_HAT_L, KC_JS_HAT_C, KC_JS_HAT_R,
    KC_JS_HAT_DL, KC_JS_HAT_D, KC_JS_HAT_DR
};
static const uint16_t js_right_buttons_ninebox[9] = {
    KC_NO, JS_3, KC_NO,
    JS_2, JS_9, JS_1,
    KC_NO, JS_0, KC_NO
};

bool cirque_pinnacles_joysticks_active(void) {
    return get_highest_layer(layer_state) == LAYER_JOYSTICK;
}

static bool js_right_alt_axes_active = false;
bool cirque_pinnacles_joysticks_right_alt_axes_active(void) {
    return js_right_alt_axes_active;
}
#endif

#if defined(DYNAMIC_MACRO_ENABLE)
static int8_t dynamic_macro_direction = 0;
bool dynamic_macro_record_start_user(int8_t direction)
{
    dynamic_macro_direction = direction;
    dprintf("Started recording macro %d\n", dynamic_macro_direction);
    return true;
}

bool dynamic_macro_record_end_user(int8_t direction) {
    dprintf("Stopped recording macro %d\n", dynamic_macro_direction);
    dynamic_macro_direction = 0;
    dynamic_macro_led_blink();
    return true;
}
#endif

#if defined(OLED_ENABLE)
void menu_activate_layer(uint8_t item_index, oled_on_menu_item_selected_context_t context)
{
    uint8_t layer = (uint8_t)context.u16;
    dprintf("Selecting mode %d=>%d...\n", item_index, layer);
    layer_move(TO(layer));
    oled_menu_activate(NULL);
}

static const oled_menu_t MODE_SELECT_MENU = {
    .title = "Mode Select",
    .items = {
        {
            .name = "Joystick",
            .on_selected = menu_activate_layer,
            .context = { u16: LAYER_JOYSTICK },
        },
        {
            .name = "Numpad",
            .on_selected = menu_activate_layer,
            .context = { u16: LAYER_NUMPAD },
        },
        {
            .name = "Media",
            .on_selected = menu_activate_layer,
            .context = { u16: LAYER_MEDIA },
        },
        {
            .name = "App: FPS",
            .on_selected = menu_activate_layer,
            .context = { u16: LAYER_APP_FPS },
        },
        {
            .name = "App: MOBA",
            .on_selected = menu_activate_layer,
            .context = { u16: LAYER_APP_MOBA },
        },
    },
    .items_count = 5,
};

bool oled_task_user(void)
{
    if (oled_in_menu()) {
        oled_render_menu();
        return false;
    }

    oled_clear();
    oled_write_P(PSTR(LAYER_NAMES[get_highest_layer(layer_state)]), false);
    oled_newline();
    oled_set_cursor(16, 0);
    oled_write_P(PSTR("HaxTT"), false);
    oled_set_cursor(0, 1);

    switch (get_highest_layer(layer_state)) {
    case LAYER_JOYSTICK: {
        oled_render_flag((_cirque_pinnacles_nineboxes[0] == js_left_dpad_ninebox), "DPAD", 4);
        oled_write_P((_cirque_pinnacles_nineboxes[1] == js_right_buttons_ninebox) ? PSTR("BTNS ") : (js_right_alt_axes_active ? PSTR("ALT  ") : PSTR("     ")), false);
    } break;
    case LAYER_MOVE: {
        oled_render_pointing_dpi();
    } break;
    default: {
        // oled_render_locks();
        oled_render_mods(false);

        if (dynamic_macro_direction == 0) {
            oled_render_padding(4);
        } else {
            oled_write_P("REC", false);
            oled_write_P("1", false);
            // oled_render_u16(dynamic_macro_recording);
        }
    } break;
    }

    oled_newline();
    oled_write_P(PSTR(LAYER_MAPS[get_highest_layer(layer_state)]), false);
    oled_render_dirty(true);
    return false;
}
#endif



const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [LAYER_QWERTY]   = LAYOUT_ortho_4x10(
        KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P,
        KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_QUOT,
        LSFT_T(KC_Z), KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, TD(TD_COMMSCLN), TD(TD_DOTCOLN), RSFT_T(KC_SLSH),
        KC_LCTL, KC_LGUI, KC_LALT, TL_LOWR, KC_SPACE, KC_SPACE, TL_UPPR, KC_RALT, KC_RGUI, KC_RCTL
    ),
    [LAYER_FUNC]     = LAYOUT_ortho_4x10(
        KC_ESC, KC_NO, KC_NO, KC_NO, CKC_MENU_TOGGLE, DM_REC1, DM_REC2, DM_PLY2, DM_PLY1, KC_BSPC,
        KC_TAB, KC_MENU, KC_HOME, KC_FIND, TT(LAYER_MEDIA), KC_LEFT, KC_DOWN, KC_UP, KC_RGHT, KC_ENT,
        KC_LSFT, TD(TD_QUOTGRV), TD(TD_EQLPLUS), TD(TD_MINSUNDS), TD(TD_SLASHES), TD(TD_BRACES), TD(TD_BRACKETS), TD(TD_ANGLES), TD(TD_PARENS), RSFT_T(KC_BSLS),
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_ESC, KC_ENT, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),
#if defined(JOYSTICK_ENABLE)
    [LAYER_JOYSTICK] = LAYOUT_ortho_4x10(
        KC_NO, KC_JS_HAT_UL, KC_JS_HAT_U, KC_JS_HAT_UR, KC_NO, KC_NO, JS_XINPUT_BUTTON_LB, KC_JS_LEFT_TRIGGER, KC_JS_RIGHT_TRIGGER, JS_XINPUT_BUTTON_RB,
        KC_NO, KC_JS_HAT_L, KC_JS_HAT_D, KC_JS_HAT_R, CKC_JS_LEFT_DPAD_TOGGLE, CKC_JS_RIGHT_BUTTONS_TOGGLE, JS_XINPUT_BUTTON_X, JS_XINPUT_BUTTON_A, JS_XINPUT_BUTTON_B, JS_XINPUT_BUTTON_Y,
        KC_NO, KC_JS_HAT_DL, KC_JS_HAT_D, KC_JS_HAT_DR, KC_NO, CKC_JS_RIGHT_AXES_TOGGLE, JS_XINPUT_BUTTON_L3, JS_XINPUT_BUTTON_SELECT, JS_XINPUT_BUTTON_START, JS_XINPUT_BUTTON_R3,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, TO(LAYER_DEFAULT), KC_NO, KC_NO, KC_NO
    ),
#endif
    [LAYER_MEDIA]       = LAYOUT_ortho_4x10(
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_BRID, KC_BRIU, KC_NO, KC_NO,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_MPRV, KC_VOLD, KC_VOLU, KC_MNXT, KC_MPLY,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_MRWD, KC_MUTE, KC_NO, KC_MFFD, KC_NO,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, TO(LAYER_DEFAULT), KC_NO, KC_NO, KC_NO
    ),
    [LAYER_MENU] = LAYOUT_ortho_4x10(
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, CKC_MENU_HIGHLIGHT_NEXT, CKC_MENU_HIGHLIGHT_PREV, KC_NO, CKC_MENU_SELECT,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, CKC_MENU_TOGGLE, KC_NO, KC_NO, KC_NO
    ),
    [LAYER_MOVE]        = LAYOUT_ortho_4x10(
        MS_WHLU, MS_WHLL, MS_UP, MS_WHLR, MS_BTN2, KC_NO, CKC_POINTING_DEVICE_DEC_DPI, CKC_POINTING_DEVICE_INC_DPI, KC_DEL, KC_INS,
        MS_WHLD, MS_LEFT, MS_DOWN, MS_RGHT, MS_BTN1, KC_LEFT, KC_DOWN, KC_UP, KC_RGHT, KC_ENT,
        MS_BTN7, MS_BTN6, MS_BTN5, MS_BTN4, MS_BTN3, KC_HOME, KC_PGDN, KC_PGUP, KC_END, KC_NO,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),
    [LAYER_NUMPAD]      = LAYOUT_ortho_4x10(
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, TD(TD_PASTSLS), KC_P7, KC_P8, KC_P9, KC_BSPC,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, TD(TD_PPLSMNS), KC_P4, KC_P5, KC_P6, KC_PENT,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_P0, KC_P1, KC_P2, KC_P3, KC_PDOT,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_NO, KC_NO, KC_NO, TO(LAYER_DEFAULT), KC_TRNS, KC_TRNS, KC_TRNS
    ),
    [LAYER_NUMSYMS]     = LAYOUT_ortho_4x10(
        KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10,
        KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0,
        LSFT_T(KC_EXLM), KC_AT, KC_HASH, KC_DLR, KC_PERC, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, RSFT_T(KC_RPRN),
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, MO(LAYER_MOVE), KC_ENT, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),
    [LAYER_APP_FPS]  = LAYOUT_ortho_4x10(
        KC_1, KC_2, KC_3, KC_4, KC_5, LALT(KC_F1), LALT(KC_F10), KC_NO, KC_NO, KC_NO,
        KC_LSFT, KC_C, KC_SPACE, KC_Q, KC_E, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        LCTL_T(KC_Z), KC_X, KC_V, TD(TD_R_T), TD(TD_F_G), KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_TRNS, KC_TRNS, KC_TRNS, TD(TD_M_N), TD(TD_TAB_ESC), KC_SPACE, TO(LAYER_DEFAULT), KC_TRNS, KC_TRNS, KC_TRNS
    ),
    [LAYER_APP_MOBA]  = LAYOUT_ortho_4x10(
        KC_1, KC_2, KC_3, KC_4, KC_5, LALT(KC_F1), LALT(KC_F10), KC_NO, KC_NO, KC_NO,
        KC_Q, KC_W, KC_E, KC_R, KC_A, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        TD(TD_TAB_ESC), KC_T, KC_D, KC_F, KC_B, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_LCTL, KC_SPACE, KC_SPACE, TO(LAYER_DEFAULT), KC_TRNS, KC_TRNS, KC_TRNS
    ),
};

static const uint16_t PROGMEM cirque_pinnacles_keymaps[][2][CIRQUE_PINNACLES_TAP_ZONES_Y][CIRQUE_PINNACLES_TAP_ZONES_X] = {
    [LAYER_QWERTY] = {
        {
            { JOYSTICK_HAT_NORTHWEST, JOYSTICK_HAT_NORTH, JOYSTICK_HAT_NORTHEAST },
            { JOYSTICK_HAT_WEST, JS_8, JOYSTICK_HAT_EAST },
            { JOYSTICK_HAT_SOUTHWEST, JOYSTICK_HAT_SOUTH, JOYSTICK_HAT_SOUTHEAST },
        },
        {
            { KC_NO, JS_3, KC_NO },
            { JS_2, JS_9, JS_1 },
            { KC_NO, JS_0, KC_NO },
        },
    },
};

bool cirque_pinnacles_touchdown(uint8_t cirque_index, int16_t x, int16_t y, int16_t dx, int16_t dy)
{
    switch (cirque_index) {
    case 0: // left pad
        // switch (get_highest_layer(layer_state)) {
        // case LAYER_JOYSTICK:
        // case LAYER_JOYSTICK2:
        //     set_joystick_axis(-1, 0, x);
        //     set_joystick_axis(-1, 1, y);
        //     return true;
        // default:
        //     break;
        // }
        break;
    case 1: // right pad
        // switch (get_highest_layer(layer_state)) {
        // case LAYER_JOYSTICK:
        //     set_joystick_axis(0, 0, x);
        //     set_joystick_axis(0, 1, y);
        //     return true;
        // case LAYER_JOYSTICK2:
        //     move_joystick_axis(-1, 2, y / 10);
        //     return true;
        // case LAYER_MOVE:
        //     if (abs(y) > abs(x)) {
        //         // scrolling vertically
        //         if (y > -1) {
        //             tap_code(MS_WHLD);
        //             return true;
        //         } else if (y < -1) {
        //             tap_code(MS_WHLU);
        //             return true;
        //         }
        //     } else if (abs(x) > abs(y)) {
        //         // scrolling horizontally
        //         if (x > -1) {
        //             tap_code(MS_WHLR);
        //             return true;
        //         } else if (x < -1) {
        //             tap_code(MS_WHLL);
        //             return true;
        //         }
        //     }
        //     break;
        // }
        break;
    }
    return false;
}

bool cirque_pinnacles_tapped(uint8_t cirque_index, uint8_t x, uint8_t y)
{
    uint16_t kc = cirque_pinnacles_keymaps[get_highest_layer(layer_state)][cirque_index][CIRQUE_PINNACLES_TAP_ZONES_Y - y - 1][x];
    if (kc == KC_NO || kc <= 0) {
        return false;
    }
    tap_code(kc);
    dprintf("Pressed keycode: %d on cirque %d (%d, %d)\n", kc, cirque_index, x, y);
    return true;
}

bool process_record_user(uint16_t keycode, keyrecord_t* record)
{
    dprintf("Pressed keycode: %d on layer %d\n", keycode, get_highest_layer(layer_state));

    switch (keycode) {
// #if defined(DYNAMIC_MACRO_ENABLE)
//     case DM_REC1:
//         switch (dynamic_macro_recording) {
//         case 0:
//             dynamic_macro_recording = 1;
//             break;
//         case 1:
//             dynamic_macro_recording = 0;
//             break;
//         default:
//             break;
//         }
//         break;
//     case DM_REC2:
//         switch (dynamic_macro_recording) {
//         case 0:
//             dynamic_macro_recording = 2;
//             break;
//         case 2:
//             dynamic_macro_recording = 0;
//             break;
//         default:
//             break;
//         }
//         break;
// #endif
// #if defined(POINTING_DEVICE_ENABLE)
//     case CKC_POINTING_DEVICE_DEC_DPI: {
//         uint16_t cpi = pointing_device_get_cpi();
//         if (cpi > POINTING_DEVICE_MIN_DPI) {
//             pointing_device_set_cpi(cpi - 1);
//         }
//     } break;
//     case CKC_POINTING_DEVICE_INC_DPI: {
//         uint16_t cpi = pointing_device_get_cpi();
//         if (cpi < POINTING_DEVICE_MAX_DPI) {
//             pointing_device_set_cpi(cpi + 1);
//         }
//     } break;
// #endif
// #if defined(JOYSTICK_ENABLE)
//     case CKC_JS_CENTER_RIGHT:
//         set_joystick_axis(1, 0, 0);
//         set_joystick_axis(1, 1, 0);
//         return false;
//     case CKC_JS_CENTER_RIGHT_X:
//         set_joystick_axis(1, 0, 0);
//         return false;
//     case CKC_JS_CENTER_RIGHT_Y:
//         set_joystick_axis(1, 1, 0);
//         return false;
// #endif
    default:
        break;
    }
    return true;
}

// uint8_t oled_get_macro_recording(void)
// {
//     return dynamic_macro_recording;
// }
// const char* oled_get_layer_map(void)
// {
//     return LAYER_MAPS[get_highest_layer(layer_state)];
// }
// const char* oled_get_layer_name(void)
// {
//     return LAYER_NAMES[get_highest_layer(layer_state)];
// }
