#include QMK_KEYBOARD_H

#include "keyboards/hax/cirque_pinnacles.h"
#include "keyboards/hax/gamepad_qts.h"

#define NUM_GAMEPAD_QTS 1

#define CIRQUE_PINNACLE_INCH_TO_PX(inch) (DIVIDE_UNSIGNED_ROUND((inch) * (uint32_t)CIRQUE_PINNACLE_DIAMETER_MM * 10, 254))
#define CIRQUE_PINNACLE_PX_TO_INCH(px) (DIVIDE_UNSIGNED_ROUND((px) * (uint32_t)254, CIRQUE_PINNACLE_DIAMETER_MM * 10))

static cirque_pinnacles_state_t cirque_pinnacles_states[CIRQUE_PINNACLES_COUNT];
static gamepad_qt_device_t      gamepad_qt_devices[NUM_GAMEPAD_QTS];

void keyboard_post_init_user(void)
{
#if defined(CONSOLE_ENABLE)
    debug_enable = true;
    debug_matrix = false;
    debug_keyboard = false;
    debug_mouse = true;
#endif
    dprintf("keyboard initialized.");
}

void pointing_device_driver_init(void) {
    print("Initializing pointing devices...\n");
    memset(cirque_pinnacles_states, 0, sizeof(cirque_pinnacles_states));
    cirque_pinnacles_states[0].swap_xy = true;
    cirque_pinnacles_init(CIRQUE_PINNACLE_SPI_CS_PIN_LEFT);
    cirque_pinnacles_states[1].swap_xy = true;
    cirque_pinnacles_init(CIRQUE_PINNACLE_SPI_CS_PIN_RIGHT);

    memset(gamepad_qt_devices, 0, sizeof(gamepad_qt_devices));
    gamepad_qt_devices[0].seesaw.address = 0x50;
    gamepad_qt_devices[0].seesaw.flow = -1;
    gamepad_qts_init(gamepad_qt_devices, 1);
    printf("Done initializing pointing devices.\n");
}

static uint16_t _dpi = POINTING_DEVICE_DEFAULT_DPI;

uint16_t pointing_device_driver_get_cpi(void) {
    return CIRQUE_PINNACLE_PX_TO_INCH(_dpi);
}

void pointing_device_driver_set_cpi(uint16_t cpi) {
    _dpi = CIRQUE_PINNACLE_INCH_TO_PX(cpi);
}

static uint16_t pointing_device_tapped_at = 0;

report_mouse_t pointing_device_driver_get_report(report_mouse_t mouse_report) {
    gamepad_qts_update_states(gamepad_qt_devices, NUM_GAMEPAD_QTS);
    // if (gamepad_qt_devices[0].state.x != 0 || gamepad_qt_devices[0].state.y != 0) {
    //     dprintf("Gamepad QT %d %d (%d, %d)\n", gamepad_qt_devices[0].seesaw.address, gamepad_qt_devices[0].seesaw.hardware_type, gamepad_qt_devices[0].state.x, gamepad_qt_devices[0].state.y);
    // }

    if (cirque_pinnacles_read_data(CIRQUE_PINNACLE_SPI_CS_PIN_LEFT, &(cirque_pinnacles_states[0])) == DATA_UPDATED) {
        if (cirque_pinnacles_states[0].touching) {
            mouse_report.h = (cirque_pinnacles_states[0].x - cirque_pinnacles_states[0].prev_x) / 256;
            mouse_report.v = (cirque_pinnacles_states[0].y - cirque_pinnacles_states[0].prev_y) / 256;
            #if CIRQUE_PINNACLES_SCROLL_REVERSE
            mouse_report.v *= -1;
            #endif
        }
    }
    if (cirque_pinnacles_read_data(CIRQUE_PINNACLE_SPI_CS_PIN_RIGHT, &(cirque_pinnacles_states[1])) == DATA_UPDATED) {
        if (cirque_pinnacles_states[1].touching) {
            pointing_device_tapped_at = timer_read();
            mouse_report.x = (cirque_pinnacles_states[1].x - cirque_pinnacles_states[1].prev_x) / 256 * pointing_device_driver_get_cpi();
            mouse_report.y = - (cirque_pinnacles_states[1].y - cirque_pinnacles_states[1].prev_y) / 256 * pointing_device_driver_get_cpi();
        }

        if (cirque_pinnacles_states[1].tapped) {
            pointing_device_tapped_at = timer_read();
            if (cirque_pinnacles_states[1].tap_x < 1) {
                mouse_report.buttons = pointing_device_handle_buttons(mouse_report.buttons, true, POINTING_DEVICE_BUTTON3);
            } else if (cirque_pinnacles_states[1].tap_x > 1) {
                mouse_report.buttons = pointing_device_handle_buttons(mouse_report.buttons, true, POINTING_DEVICE_BUTTON2);
            } else {
                mouse_report.buttons = pointing_device_handle_buttons(mouse_report.buttons, true, POINTING_DEVICE_BUTTON1);
            }
        }
    }

    if (timer_elapsed(pointing_device_tapped_at) > CIRQUE_PINNACLES_TAP_TERM && !cirque_pinnacles_states[1].touching) {
        // clear mouse down, have held off long enough
        mouse_report.buttons = pointing_device_handle_buttons(mouse_report.buttons, false, POINTING_DEVICE_BUTTON1);
        mouse_report.buttons = pointing_device_handle_buttons(mouse_report.buttons, false, POINTING_DEVICE_BUTTON2);
        mouse_report.buttons = pointing_device_handle_buttons(mouse_report.buttons, false, POINTING_DEVICE_BUTTON3);
    }

    return mouse_report;
}

report_mouse_t pointing_device_task_user(report_mouse_t mouse_report)
{
    if (mouse_report.x != 0 || mouse_report.y != 0 || mouse_report.buttons != 0) {
        dprintf("Pointing Device: X: %d, Y: %d, Buttons: %d\n", mouse_report.x, mouse_report.y, mouse_report.buttons);
    }

    return mouse_report;
}
