#include QMK_KEYBOARD_H
#include "cirque_pinnacles.h"

#include "drivers/sensors/cirque_pinnacle.h"

void cirque_pinnacles_init(cirque_pinnacles_device_t devices[CIRQUE_PINNACLES_COUNT]) {
    for (uint8_t i = 0; i < CIRQUE_PINNACLES_COUNT; i++) {
        cirque_pinnacle_init_device(devices[i].address, devices[i].absolute);
    }
}

bool cirque_pinnacles_update_state(cirque_pinnacles_device_t* device) {
    if (device->absolute) {
        pinnacle_absolute_data_t data = cirque_pinnacle_read_absolute_device_data(device->address);

        if (!data.valid) {
            return false;
        }

        memset(&(device->state), 0, sizeof(cirque_pinnacles_state_t));

        device->state.x = ((int16_t)cirque_pinnacle_scale_absolute_x(data.xValue, UINT16_MAX / 2) - (INT16_MAX / 4)) * 2;
        device->state.y = ((int16_t)cirque_pinnacle_scale_absolute_y(data.yValue, UINT16_MAX / 2) - (INT16_MAX / 4)) * 2;
        if (device->swap_xy) {
            int16_t temp = device->state.x;
            device->state.x = device->state.y;
            device->state.y = temp;
        }
        if (device->flip_x) {
            device->state.x *= -1;
        }
        if (device->flip_y) {
            device->state.y *= -1;
        }
        if (device->state.x > -CIRQUE_PINNACLES_DEADZONE && device->state.x < CIRQUE_PINNACLES_DEADZONE) {
            device->state.x = 0;
        }
        if (device->state.y > -CIRQUE_PINNACLES_DEADZONE && device->state.y < CIRQUE_PINNACLES_DEADZONE) {
            device->state.y = 0;
        }

        device->state.dx = device->state.x - device->last_x;
        device->state.dy = device->state.y - device->last_y;

        device->last_x = device->state.x;
        device->last_y = device->state.y;

        if (data.touchDown && !data.hovering) {
            uint16_t zone_x = cirque_pinnacle_scale_absolute_x(data.xValue, CIRQUE_PINNACLES_TOUCH_ZONES_X);
            uint16_t zone_y = cirque_pinnacle_scale_absolute_y(data.yValue, CIRQUE_PINNACLES_TOUCH_ZONES_Y);
            device->state.touches[zone_y][zone_x] = 1;
            // TODO: don't use 1, use the amount of time held down or something interesting
        }

        return true;
    } else {
        pinnacle_relative_data_t data = cirque_pinnacle_read_relative_device_data(device->address);

        if (!data.valid) {
            return false;
        }

        memset(&(device->state), 0, sizeof(cirque_pinnacles_state_t));
        device->state.dx = data.xDelta;
        device->state.dy = data.yDelta;
        if (data.buttonFlags > 0) {
            device->state.touches[CIRQUE_PINNACLES_TOUCH_ZONES_Y / 2][CIRQUE_PINNACLES_TOUCH_ZONES_X / 2] = 1;
        }
        return true;
    }

    return false;
}

bool cirque_pinnacles_update_states(cirque_pinnacles_device_t devices[CIRQUE_PINNACLES_COUNT]) {
    bool updated = false;
    for (uint8_t i = 0; i < CIRQUE_PINNACLES_COUNT; i++) {
        updated = cirque_pinnacles_update_state(&(devices[i])) || updated;
    }
    return updated;
}
