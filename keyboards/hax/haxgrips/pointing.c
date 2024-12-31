#include QMK_KEYBOARD_H

#define NUM_CIRQUE_PINNACLES 2
#define NUM_GAMEPAD_QTS 1

#include "keyboards/hax/cirque_pinnacles.h"
#include "keyboards/hax/gamepad_qts.h"

extern const uint16_t PROGMEM cirque_pinnacles_keymaps[][NUM_CIRQUE_PINNACLES][CIRQUE_PINNACLES_TOUCH_ZONES_Y][CIRQUE_PINNACLES_TOUCH_ZONES_X];

extern bool cirque_pinnacles_updated(uint8_t index, cirque_pinnacles_state_t* state);

#ifdef POINTING_DEVICE_custom

static cirque_pinnacles_device_t cirque_pinnacle_devices[NUM_CIRQUE_PINNACLES];
static gamepad_qt_device_t gamepad_qt_devices[NUM_GAMEPAD_QTS];
static uint16_t _cpi;

void pointing_device_driver_init(void)
{
    print("Initializing pointing devices...\n");
    memset(cirque_pinnacle_devices, 0, sizeof(cirque_pinnacle_devices));
    cirque_pinnacle_devices[0].address = 0x2A;
    cirque_pinnacle_devices[0].absolute = true;
    cirque_pinnacle_devices[1].address = 0x2C; // solder 470kohm resistor on ADR
    cirque_pinnacle_devices[1].absolute = true;
    cirque_pinnacles_init(cirque_pinnacle_devices, 2);

    memset(gamepad_qt_devices, 0, sizeof(gamepad_qt_devices));
    gamepad_qt_devices[0].seesaw.address = 0x50;
    gamepad_qt_devices[0].seesaw.flow = -1;
    gamepad_qts_init(gamepad_qt_devices, 1);
    printf("Done initializing pointing devices.\n");
}

uint16_t pointing_device_driver_get_cpi(void)
{
    return _cpi;
}

void pointing_device_driver_set_cpi(uint16_t cpi)
{
    _cpi = cpi;
}

report_mouse_t pointing_device_driver_get_report(report_mouse_t mouse_report)
{
    bool updated = false;

    updated = cirque_pinnacles_update_states(cirque_pinnacle_devices, NUM_CIRQUE_PINNACLES) || updated;
    updated = gamepad_qts_update_states(gamepad_qt_devices, NUM_GAMEPAD_QTS) || updated;

    if (gamepad_qt_devices[0].state.x != 0 || gamepad_qt_devices[0].state.y != 0) {
        // dprintf("Gamepad QT %d %d (%d, %d)\n", gamepad_qt_devices[0].seesaw.address, gamepad_qt_devices[0].seesaw.hardware_type, gamepad_qt_devices[0].state.x, gamepad_qt_devices[0].state.y);
    }

    if (!updated) {
        return mouse_report;
    }

    uint8_t layer = get_highest_layer(layer_state);

    // handle button clicks in touch zones
    bool tapped = false;
    for (uint8_t i = 0; i < NUM_CIRQUE_PINNACLES; i++) {
        for (uint8_t y = 0; y < CIRQUE_PINNACLES_TOUCH_ZONES_Y; y++) {
            for (uint8_t x = 0; x < CIRQUE_PINNACLES_TOUCH_ZONES_X; x++) {
                if (cirque_pinnacle_devices[i].state.touches[y][x] > 0 && cirque_pinnacles_keymaps[layer][i][y][x] != KC_NO) {
                    tap_code(cirque_pinnacles_keymaps[layer][i][y][x]);
                    dprintf("Pressed keycode: %d on cirque %d (%d, %d) on layer %d\n", cirque_pinnacles_keymaps[i][layer][y][x], i, x, y, layer);
                    tapped = true;
                }
            }
        }
    }

    bool handled = false;
    if (tapped) {
        handled = true;
    } else {
        // handle movement since there wasn't a tap
        for (uint8_t i = 0; i < NUM_CIRQUE_PINNACLES; i++) {
            if (cirque_pinnacle_devices[i].state.x != 0 || cirque_pinnacle_devices[i].state.y != 0) {
                handled = cirque_pinnacles_updated(i, &(cirque_pinnacle_devices[i]).state) || handled;
            }
        }
    }

    if (handled) {
        memset(&mouse_report, 0, sizeof(mouse_report));
    }
    return mouse_report;
}
#endif
