#include QMK_KEYBOARD_H

#include "keyboards/hax/cirque_pinnacles.h"
#include "keyboards/hax/gamepad_qts.h"

#define NUM_GAMEPAD_QTS 1


static cirque_pinnacles_state_t cirque_pinnacles_states[CIRQUE_PINNACLES_COUNT];
static gamepad_qt_device_t      gamepad_qt_devices[NUM_GAMEPAD_QTS];

void keyboard_post_init_user(void)
{
    print("Initializing pointing devices...\n");
    memset(cirque_pinnacles_states, 0, sizeof(cirque_pinnacles_states));
    cirque_pinnacles_init(CIRQUE_PINNACLE_SPI_CS_PIN_LEFT);
    // cirque_pinnacles_init(CIRQUE_PINNACLE_SPI_CS_PIN_RIGHT);

    memset(gamepad_qt_devices, 0, sizeof(gamepad_qt_devices));
    gamepad_qt_devices[0].seesaw.address = 0x50;
    gamepad_qt_devices[0].seesaw.flow = -1;
    gamepad_qts_init(gamepad_qt_devices, 1);
    printf("Done initializing pointing devices.\n");

#if defined(CONSOLE_ENABLE)
    debug_enable = true;
    debug_matrix = false;
    debug_keyboard = false;
    debug_mouse = true;
#endif
    dprintf("keyboard initialized.");
}

report_mouse_t pointing_device_task_user(report_mouse_t mouse_report)
{
    gamepad_qts_update_states(gamepad_qt_devices, NUM_GAMEPAD_QTS);
    if (cirque_pinnacles_read_data(CIRQUE_PINNACLE_SPI_CS_PIN_LEFT, &(cirque_pinnacles_states[0])) != NO_DATA_READ) {
        dprintf("Left: (%d, %d)\n", cirque_pinnacles_states[0].x, cirque_pinnacles_states[0].y);
        if (cirque_pinnacles_states[0].touched) {
            dprintf("Left Touch: (%d, %d)\n", cirque_pinnacles_states[0].touch_x, cirque_pinnacles_states[0].touch_y);
        }
    }
    if (cirque_pinnacles_read_data(CIRQUE_PINNACLE_SPI_CS_PIN_RIGHT, &(cirque_pinnacles_states[1])) != NO_DATA_READ) {
        dprintf("Right: (%d, %d)\n", cirque_pinnacles_states[1].x, cirque_pinnacles_states[1].y);
        if (cirque_pinnacles_states[1].touched) {
            dprintf("Right Touch: (%d, %d)\n", cirque_pinnacles_states[1].touch_x, cirque_pinnacles_states[1].touch_y);
        }
    }

    // if (gamepad_qt_devices[0].state.x != 0 || gamepad_qt_devices[0].state.y != 0) {
    //     dprintf("Gamepad QT %d %d (%d, %d)\n", gamepad_qt_devices[0].seesaw.address, gamepad_qt_devices[0].seesaw.hardware_type, gamepad_qt_devices[0].state.x, gamepad_qt_devices[0].state.y);
    // }

    if (mouse_report.x != 0 || mouse_report.y != 0 || mouse_report.buttons != 0) {
        dprintf("Pointing Device: X: %d, Y: %d, Buttons: %d\n", mouse_report.x, mouse_report.y, mouse_report.buttons);
    }

    return mouse_report;
}
