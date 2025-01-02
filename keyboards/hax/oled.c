#include QMK_KEYBOARD_H

__attribute__((weak)) uint8_t oled_get_macro_recording(void) { return 0; }
__attribute__((weak)) const char* oled_get_layer_map(void) { return ""; }
__attribute__((weak)) const char* oled_get_layer_name(void) { return ""; }

oled_rotation_t oled_init_user(oled_rotation_t rotation)
{
    return OLED_ROTATION_0;
}

bool oled_task_kb(void)
{
    dprintf("oled_task_user\n");
    oled_write_P(PSTR(oled_get_layer_name()), false);
    oled_write_P(PSTR("\n"), false);

    uint8_t macro_recording = oled_get_macro_recording();
    if (macro_recording == 0) {
        oled_write_P(PSTR("     "), false);
    } else {
        char buffer[8];
        sprintf(buffer, "REC%d ", macro_recording);
        oled_write_P(PSTR(buffer), false);
    }

    // Host Keyboard LED Status
    led_t led_state = host_keyboard_led_state();
    oled_write_P(led_state.num_lock ? PSTR("NUM ") : PSTR("    "), false);
    oled_write_P(led_state.caps_lock ? PSTR("CAP ") : PSTR("    "), false);
    oled_write_P(led_state.scroll_lock ? PSTR("SCR ") : PSTR("    "), false);

    oled_write_P(PSTR("\n"), false);

    // display keyboard map
    oled_write_P(PSTR(oled_get_layer_map()), false);

    return false;
}
