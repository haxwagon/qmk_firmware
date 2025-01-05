#include QMK_KEYBOARD_H
#include "oled.h"

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

void oled_newline(void)
{
    oled_write_P(PSTR("\n"), false);
}

void oled_render_flag(bool flag, const char* name, uint8_t width)
{
    if (flag) {
        oled_write_P(PSTR(name), false);
        if (strlen(name) < width) {
            oled_render_padding(width - strlen(name));
        }
    } else {
        oled_render_padding(width);
    }
}

void oled_render_locks(void)
{
    led_t led_state = host_keyboard_led_state();
    oled_render_flag(led_state.num_lock, "NUM", 4);
    oled_render_flag(led_state.caps_lock, "CAP", 4);
    oled_render_flag(led_state.scroll_lock, "SCR", 4);
}

void oled_render_mods(bool verbose)
{
    uint8_t modifiers = get_mods() | get_oneshot_mods();
    bool is_caps = host_keyboard_led_state().caps_lock;
    if (verbose) {
        oled_render_flag((modifiers & MOD_BIT(KC_LSFT)) || is_caps, "LS", 3);
        oled_render_flag(modifiers & MOD_BIT(KC_LCTL), "LC", 3);
        oled_render_flag(modifiers & MOD_BIT(KC_LGUI), "LG", 3);
        oled_render_flag(modifiers & MOD_BIT(KC_LALT), "LA", 3);
        oled_render_flag(modifiers & MOD_BIT(KC_RALT), "RA", 3);
        oled_render_flag(modifiers & MOD_BIT(KC_RGUI), "RG", 3);
        oled_render_flag(modifiers & MOD_BIT(KC_RCTL), "RC", 3);
        oled_render_flag((modifiers & MOD_BIT(KC_RSFT)) || is_caps, "RS", 3);
    } else {
        oled_render_flag((modifiers & MOD_MASK_SHIFT) || is_caps, "SHFT", 5);
        oled_render_flag(modifiers & MOD_MASK_CTRL, "CTL", 4);
        oled_render_flag(modifiers & MOD_MASK_GUI, "GUI", 4);
        oled_render_flag(modifiers & MOD_MASK_ALT, "ALT", 4);
    }
}

void oled_render_padding(uint8_t padding)
{
    for (uint8_t n = padding - 1; n > 0; n--) {
        oled_write_P(PSTR(" "), false);
    }
}

#if defined(POINTING_DEVICE_ENABLE)
void oled_render_pointing_dpi(void)
{
    oled_write_P(PSTR("CPI:"), false);
    oled_render_padding(1);
    oled_write(get_u16_str(pointing_device_get_cpi(), ' '), false);
}
#endif

void oled_render_u16(uint16_t n)
{
    oled_write(get_u16_str(n, ' '), false);
}
