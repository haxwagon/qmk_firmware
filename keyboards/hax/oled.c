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
        oled_write_P(name, false);
        if (width > 0) {
            if (strlen(name) < width) {
                oled_render_padding(width - strlen(name));
            }
        }
    } else if (width > 0) {
        oled_render_padding(width);
    }
}

void oled_render_locks(void)
{
    led_t led_state = host_keyboard_led_state();
    oled_render_flag(led_state.num_lock, PSTR("NUM"), 4);
    oled_render_flag(led_state.caps_lock, PSTR("CAP"), 4);
    oled_render_flag(led_state.scroll_lock, PSTR("SCR"), 4);
}

void oled_render_menu(uint8_t lines)
{
    if (!oled_in_menu()) {
        return;
    }

    oled_clear();
    oled_write_ln_P(PSTR(_active_menu->title), false);

    for (int8_t i = -(lines / 2); i <= (lines / 2); i++) {
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
    uint8_t mods = get_mods() | get_oneshot_mods();
    bool is_caps = host_keyboard_led_state().caps_lock;
    if (verbose) {
        oled_render_flag((mods & MOD_BIT(KC_LSFT)) || is_caps, PSTR("LS"), 3);
        oled_render_flag(mods & MOD_BIT(KC_LCTL), PSTR("LC"), 3);
        oled_render_flag(mods & MOD_BIT(KC_LGUI), PSTR("LG"), 3);
        oled_render_flag(mods & MOD_BIT(KC_LALT), PSTR("LA"), 3);
        oled_render_flag(mods & MOD_BIT(KC_RALT), PSTR("RA"), 3);
        oled_render_flag(mods & MOD_BIT(KC_RGUI), PSTR("RG"), 3);
        oled_render_flag(mods & MOD_BIT(KC_RCTL), PSTR("RC"), 3);
        oled_render_flag((mods & MOD_BIT(KC_RSFT)) || is_caps, PSTR("RS"), 3);
    } else {
        oled_render_flag((mods & MOD_BIT(KC_LSFT)) || (mods & MOD_BIT(KC_RSFT)) || is_caps, PSTR("SFT"), 3);
        oled_render_flag((mods & MOD_BIT(KC_LCTL)) || (mods & MOD_BIT(KC_RCTL)), PSTR("CTL"), 3);
        oled_render_flag((mods & MOD_BIT(KC_LGUI)) || (mods & MOD_BIT(KC_RGUI)), PSTR("GUI"), 3);
        oled_render_flag((mods & MOD_BIT(KC_LALT)) || (mods & MOD_BIT(KC_RALT)), PSTR("ALT"), 3);
    }
}

void oled_render_padding(uint8_t padding)
{
    for (int8_t n = padding - 1; n >= 0; n--) {
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

static const char *depad_str(const char *depad_str, char depad_char) {
    while (*depad_str == depad_char) {
        ++depad_str;
    }
    return depad_str;
}

void oled_render_u16(uint16_t n)
{
    oled_write(depad_str(get_u16_str(n, ' '), ' '), false);
}
