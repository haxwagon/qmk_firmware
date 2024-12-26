#include QMK_KEYBOARD_H

#include "keyboards/hax/cirque_pinnacles.h"

extern const uint16_t PROGMEM cirque_pinnacles_keymaps[][CIRQUE_PINNACLES_COUNT][CIRQUE_PINNACLES_TOUCH_ZONES_Y][CIRQUE_PINNACLES_TOUCH_ZONES_X];

extern bool cirque_pinnacles_updated(uint8_t index, cirque_pinnacles_state_t* state);

static cirque_pinnacles_device_t devices[2];

void dual_cirque_pinnacle_init(void) {
    memset(devices, 0, sizeof(devices));
    devices[0].address = 0x2A;
    devices[0].absolute = true;
    devices[1].address = 0x2C; // solder 470kohm resistor on ADR
    devices[1].absolute = true;
    cirque_pinnacles_init(devices);
}

uint16_t dual_cirque_pinnacle_get_cpi(void) {
    return cirque_pinnacle_get_cpi();
}

void dual_cirque_pinnacle_set_cpi(uint16_t cpi) {
    cirque_pinnacle_set_cpi(cpi);
}

report_mouse_t dual_cirque_pinnacle_get_report(report_mouse_t mouse_report) {
    if (!cirque_pinnacles_update_states(devices)) {
        return mouse_report;
    }

    uint8_t layer = get_highest_layer(layer_state);

    // handle button clicks in touch zones
    bool tapped = false;
    for (uint8_t i = 0; i < CIRQUE_PINNACLES_COUNT; i++) {
        for (uint8_t y = 0; y < CIRQUE_PINNACLES_TOUCH_ZONES_Y; y++) {
            for (uint8_t x = 0; x < CIRQUE_PINNACLES_TOUCH_ZONES_X; x++) {
                if (devices[i].state.touches[y][x] > 0 && cirque_pinnacles_keymaps[layer][i][y][x] != KC_NO) {
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
        for (uint8_t i = 0; i < CIRQUE_PINNACLES_COUNT; i++) {
            if (devices[i].state.x != 0 || devices[i].state.y != 0) {
                handled = cirque_pinnacles_updated(i, &(devices[i]).state) || handled;
            }
        }
    }

    if (handled) {
        memset(&mouse_report, 0, sizeof(mouse_report));
    }
    return mouse_report;
}


const pointing_device_driver_t cirque_pinnacle_pointing_device_driver = {
    .init       = dual_cirque_pinnacle_init,
    .get_report = dual_cirque_pinnacle_get_report,
    .get_cpi    = dual_cirque_pinnacle_get_cpi,
    .set_cpi    = dual_cirque_pinnacle_set_cpi,
};

report_mouse_t pointing_device_task_user(report_mouse_t mouse_report)
{
    if (mouse_report.x != -1 || mouse_report.y != 0 || mouse_report.buttons != 0) {
        dprintf("Pointing Device: X: %d, Y: %d, Buttons: %d\n", mouse_report.x, mouse_report.y, mouse_report.buttons);
    }

    // mouse_report.x *= get_pointing_dpi();
    // mouse_report.y *= get_pointing_dpi();

    return mouse_report;
}
