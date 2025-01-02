#include QMK_KEYBOARD_H

#include "keyboards/hax/cirque_pinnacles.h"

void keyboard_post_init_user(void)
{
    dprintf("keyboard initializing...\n");
#if defined(CONSOLE_ENABLE)
    debug_enable = true;
    debug_matrix = false;
    debug_keyboard = false;
    debug_mouse = true;
#endif
    dprintf("keyboard initialized.\n");
}

#if defined(JOYSTICK_ENABLE)
joystick_config_t joystick_axes[JOYSTICK_AXIS_COUNT] = {
    JOYSTICK_AXIS_VIRTUAL,
    JOYSTICK_AXIS_VIRTUAL,
    JOYSTICK_AXIS_VIRTUAL,
    JOYSTICK_AXIS_VIRTUAL,
    JOYSTICK_AXIS_VIRTUAL,
    JOYSTICK_AXIS_VIRTUAL,
};
#endif

static const cirque_pinnacles_config_t cirque_pinnacles_configs[CIRQUE_PINNACLES_COUNT] = {
    {
        .spi_cs_pin = CIRQUE_PINNACLES_SPI_CS_PIN_LEFT,
        .flip_x = true,
        .flip_y = false,
        .swap_xy = false,
    },
    {
        .spi_cs_pin = CIRQUE_PINNACLES_SPI_CS_PIN_RIGHT,
        .flip_x = false,
        .flip_y = true,
        .swap_xy = false,
    }
};

cirque_pinnacles_config_t cirque_pinnacles_get_config(uint8_t cirque_index)
{
    return cirque_pinnacles_configs[cirque_index];
}

#if defined(OLED_ENABLE)
oled_rotation_t oled_init_kb(oled_rotation_t rotation)
{
    return OLED_ROTATION_0;
}
#endif
