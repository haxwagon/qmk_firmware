#include QMK_KEYBOARD_H

#include "../cirque_pinnacles.h"
#if defined(JOYSTICK_ENABLE)
#include "../joysticks.h"
#endif

void keyboard_post_init_kb(void)
{
    dprintf("keyboard initializing...\n");
#if defined(CONSOLE_ENABLE)
    debug_enable = false;
    debug_matrix = false;
    debug_keyboard = false;
    debug_mouse = false;
#endif
#if defined(JOYSTICK_ENABLE)
    joysticks_init();
#endif
    keyboard_post_init_user();
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

const static cirque_pinnacles_config_t cirque_pinnacles_configs[CIRQUE_PINNACLES_COUNT] = {
    {
        .spi_cs_pin = CIRQUE_PINNACLES_SPI_CS_PIN_LEFT,
        .flip_x = false,
        .flip_y = true,
        .swap_xy = true,
    },
    {
        .spi_cs_pin = CIRQUE_PINNACLES_SPI_CS_PIN_RIGHT,
        .flip_x = false,
        .flip_y = true,
        .swap_xy = true,
    },
};
const static cirque_pinnacles_config_t cirque_pinnacles_flipped_configs[CIRQUE_PINNACLES_COUNT] = {
    {
        .spi_cs_pin = CIRQUE_PINNACLES_SPI_CS_PIN_RIGHT,
        .flip_x = true,
        .flip_y = true,
        .swap_xy = true,
    },
    {
        .spi_cs_pin = CIRQUE_PINNACLES_SPI_CS_PIN_LEFT,
        .flip_x = true,
        .flip_y = true,
        .swap_xy = true,
    },
};


__attribute__((weak)) bool haxtroller_is_flipped_user(void)
{
    return false;
}

const cirque_pinnacles_config_t* cirque_pinnacles_get_config(uint8_t cirque_index)
{
    if (haxtroller_is_flipped_user()) {
        return &cirque_pinnacles_flipped_configs[cirque_index];
    } else {
        return &cirque_pinnacles_configs[cirque_index];
    }
}

#if defined(OLED_ENABLE)
oled_rotation_t oled_init_kb(oled_rotation_t rotation)
{
    return OLED_ROTATION_180;
}
#endif

bool process_record_kb(uint16_t keycode, keyrecord_t* record)
{
#if defined(JOYSTICK_ENABLE)
    if (joysticks_handle_keycode(keycode, record->event.pressed)) {
        return false;
    }
#endif
    return process_record_user(keycode, record);
}

void housekeeping_task_kb(void)
{
#if defined(JOYSTICK_ENABLE)
    joysticks_housekeeping();
#endif
    return housekeeping_task_user();
}

