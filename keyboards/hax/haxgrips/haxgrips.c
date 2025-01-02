#include QMK_KEYBOARD_H

#include "keyboards/hax/cirque_pinnacles.h"
#include "keyboards/hax/gamepad_qts.h"

#define GAMEPAD_QTS_COUNT 1

static gamepad_qt_device_t gamepad_qt_devices[GAMEPAD_QTS_COUNT];

void keyboard_post_init_user(void)
{
    dprintf("keyboard initializing...\n");
#if defined(CONSOLE_ENABLE)
    debug_enable = true;
    debug_matrix = false;
    debug_keyboard = false;
    debug_mouse = true;
#endif
    memset(gamepad_qt_devices, 0, sizeof(gamepad_qt_devices));
    gamepad_qt_devices[0].seesaw.address = 0x50;
    gamepad_qt_devices[0].seesaw.flow = -1;
    gamepad_qts_init(gamepad_qt_devices, 1);

    dprintf("keyboard initialized.\n");
}

void housekeeping_task_kb(void)
{
    gamepad_qts_update_states(gamepad_qt_devices, GAMEPAD_QTS_COUNT);
    // if (gamepad_qt_devices[0].state.x != 0 || gamepad_qt_devices[0].state.y != 0) {
    //     dprintf("Gamepad QT %d %d (%d, %d)\n", gamepad_qt_devices[0].seesaw.address, gamepad_qt_devices[0].seesaw.hardware_type, gam
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
