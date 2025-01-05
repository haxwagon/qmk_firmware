#include QMK_KEYBOARD_H

__attribute__((weak)) uint8_t oled_get_macro_recording(void) { return 0; }
__attribute__((weak)) const char* oled_get_layer_name(void) { return ""; }
__attribute__((weak)) const char* oled_write_layer_details(void) { return ""; }

bool oled_task_kb(void)
{
    // Defer to the keymap if they want to override
    if(!oled_task_user()) { return false; }

    oled_write_P(PSTR(oled_get_layer_name()), false);
    oled_write_P(PSTR("\n"), false);

    oled_write_layer_details();

    return false;
}
