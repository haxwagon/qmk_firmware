#include QMK_KEYBOARD_H
#include "oled.h"

static const oled_menu_t* _active_menu = NULL;
static uint8_t _menu_highlighted_index = 0;

bool oled_in_menu(void)
{
    return _active_menu != NULL;
}

void oled_menu_activate(const oled_menu_t* menu)
{
    _active_menu = menu;

    if (!oled_in_menu()) {
        return;
    }
    _menu_highlighted_index = 0;
}

void oled_menu_highlight_next(void)
{
    if (!oled_in_menu()) {
        return;
    }
    if (_menu_highlighted_index < _active_menu->items_count - 1) {
        _menu_highlighted_index++;
    } else {
        _menu_highlighted_index = 0;
    }
}

void oled_menu_highlight_prev(void)
{
    if (!oled_in_menu()) {
        return;
    }

    if (_menu_highlighted_index > 0) {
        _menu_highlighted_index--;
    } else {
        _menu_highlighted_index = _active_menu->items_count - 1;
    }
}

void oled_menu_select(void)
{
    if (!oled_in_menu()) {
        return;
    }

    oled_on_menu_item_selected on_selected = _active_menu->items[_menu_highlighted_index].on_selected;
    if (on_selected != NULL) {
        oled_on_menu_item_selected_context_t context = _active_menu->items[_menu_highlighted_index].context;
        on_selected(_menu_highlighted_index, context);
    }
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

void oled_render_menu(void)
{
    if (!oled_in_menu()) {
        return;
    }

    oled_clear();
    oled_write_ln_P(PSTR(_active_menu->title), false);
    oled_newline();

    for (int8_t i = -2; i <= 2; i++) {
        if (_menu_highlighted_index + i < 0 || _menu_highlighted_index + i > _active_menu->items_count - 1) {
            oled_newline();
            continue;
        }

        if (i == 0) {
            oled_write_P(PSTR("> "), false);
        } else {
            oled_write_P(PSTR("  "), false);
        }

        oled_write_P(PSTR(_active_menu->items[_menu_highlighted_index + i].name), false);
        oled_newline();
    }
    oled_render_dirty(true);
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
