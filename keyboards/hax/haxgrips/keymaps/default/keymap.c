#include QMK_KEYBOARD_H


enum LAYERS {
    LAYER_DEFAULT,
};


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [LAYER_DEFAULT] = LAYOUT_ortho_1x1(
        KC_ENT
    )
};

static const uint16_t PROGMEM cirque_pinnacles_keymaps[][2][CIRQUE_PINNACLES_TAP_ZONES_Y][CIRQUE_PINNACLES_TAP_ZONES_X] = {
    [LAYER_DEFAULT] = {
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
