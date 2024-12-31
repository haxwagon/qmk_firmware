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

static const cirque_pinnacles_config_t cirque_pinnacles_configs[CIRQUE_PINNACLES_COUNT] = {
    {
        .spi_cs_pin = CIRQUE_PINNACLES_SPI_CS_PIN_LEFT,
        .flip_x = false,
        .flip_y = false,
        .swap_xy = true,
    },
    {
        .spi_cs_pin = CIRQUE_PINNACLES_SPI_CS_PIN_RIGHT,
        .flip_x = false,
        .flip_y = false,
        .swap_xy = true,
    }
};

cirque_pinnacles_config_t cirque_pinnacles_get_config(uint8_t cirque_index)
{
    return cirque_pinnacles_configs[cirque_index];
}
