#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_ortho_1x1(
        KC_ENT)
};

bool oled_task_user(void)
{
    printf("oled_task_user\n");
    oled_write_P(PSTR("SUCCESS!\n"), false);
    return false;
}
