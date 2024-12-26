#include QMK_KEYBOARD_H

extern const char* LAYER_MAPS[];
extern const char* LAYER_NAMES[];
extern uint8_t dynamic_macro_recording;

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
