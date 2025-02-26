#pragma once

#define MAX_MENU_ITEMS 16

typedef union {
    uint16_t u16;
    void*    context;
} oled_on_menu_item_selected_context_t;
typedef void (*oled_on_menu_item_selected)(uint8_t index, oled_on_menu_item_selected_context_t context);

typedef struct {
    const char*                          name;
    oled_on_menu_item_selected           on_selected;
    oled_on_menu_item_selected_context_t context;
} oled_menu_item_t;

typedef struct {
    const char*      title;
    oled_menu_item_t items[MAX_MENU_ITEMS];
    uint8_t          items_count;
} oled_menu_t;

bool oled_in_menu(void);
void oled_menu_activate(const oled_menu_t* menu);
void oled_menu_highlight_next(void);
void oled_menu_highlight_prev(void);
void oled_menu_select(void);
void oled_newline(void);
void oled_render_flag(bool flag, const char* name, uint8_t width);
void oled_render_locks(void);
void oled_render_menu(uint8_t lines);
void oled_render_mods(bool verbose);
void oled_render_padding(uint8_t padding);
void oled_render_pointing_dpi(void);
void oled_render_u16(uint16_t n);
