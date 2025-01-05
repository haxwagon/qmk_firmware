#pragma once

void oled_newline(void);
void oled_render_flag(bool flag, const char* name, uint8_t width);
void oled_render_locks(void);
void oled_render_mods(bool verbose);
void oled_render_padding(uint8_t padding);
void oled_render_pointing_dpi(void);
void oled_render_u16(uint16_t n);
