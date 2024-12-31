#include QMK_KEYBOARD_H
#include "gamepad_qts.h"

#define BUTTON_X 6
#define BUTTON_Y 2
#define BUTTON_A 5
#define BUTTON_B 1
#define BUTTON_SELECT 0
#define BUTTON_START 16
uint32_t buttons_mask = (1UL << BUTTON_X) | (1UL << BUTTON_Y) | (1UL << BUTTON_START) | (1UL << BUTTON_A) | (1UL << BUTTON_B) | (1UL << BUTTON_SELECT);

enum {
    SEESAW_GPIO_DIRSET_BULK = 0x02,
    SEESAW_GPIO_DIRCLR_BULK = 0x03,
    SEESAW_GPIO_BULK = 0x04,
    SEESAW_GPIO_BULK_SET = 0x05,
    SEESAW_GPIO_BULK_CLR = 0x06,
    SEESAW_GPIO_BULK_TOGGLE = 0x07,
    SEESAW_GPIO_INTENSET = 0x08,
    SEESAW_GPIO_INTENCLR = 0x09,
    SEESAW_GPIO_INTFLAG = 0x0A,
    SEESAW_GPIO_PULLENSET = 0x0B,
    SEESAW_GPIO_PULLENCLR = 0x0C,
};

// //#define IRQ_PIN   5

// void loop() {
//   delay(10); // delay in loop to slow serial output

//   // Reverse x/y values to match joystick orientation
//   int x = 1023 - ss.analogRead(14);
//   int y = 1023 - ss.analogRead(15);

//   if ( (abs(x - last_x) > 3)  ||  (abs(y - last_y) > 3)) {
//     Serial.print("x: "); Serial.print(x); Serial.print(", "); Serial.print("y: "); Serial.println(y);
//     last_x = x;
//     last_y = y;
//   }

// #if defined(IRQ_PIN)
//   if(!digitalRead(IRQ_PIN)) {
//     return;
//   }
// #endif

void gamepad_qts_init(gamepad_qt_device_t* devices, uint8_t count)
{
    for (uint8_t i = 0; i < count; i++) {
        printf("initializing seesaw %d\n", i);
        seesaw_begin(&(devices[i].seesaw), true);
    }
}

bool gamepad_qts_update_state(gamepad_qt_device_t* device)
{
    device->last_x = device->state.x;
    device->last_y = device->state.y;

    memset(&(device->state), 0, sizeof(device->state));

    device->state.x = 1023 - seesaw_analog_read(device->seesaw, 14);
    device->state.y = 1023 - seesaw_analog_read(device->seesaw, 15);

    if (device->flip_x) {
        device->state.x = 1023 - device->state.x;
    }

    if (device->flip_y) {
        device->state.y = 1023 - device->state.y;
    }

    if (device->swap_xy) {
        uint16_t temp = device->state.x;
        device->state.x = device->state.y;
        device->state.y = temp;
    }

    if (device->state.x > -GAMEPAD_QTS_DEADZONE && device->state.x < GAMEPAD_QTS_DEADZONE) {
        device->state.x = 0;
    }
    if (device->state.y > -GAMEPAD_QTS_DEADZONE && device->state.y < GAMEPAD_QTS_DEADZONE) {
        device->state.y = 0;
    }

    uint32_t buttons = seesaw_digital_read_bulk(device->seesaw, buttons_mask);
    device->state.buttons.a = !(buttons & (1UL << BUTTON_A));
    device->state.buttons.b = !(buttons & (1UL << BUTTON_B));
    device->state.buttons.x = !(buttons & (1UL << BUTTON_X));
    device->state.buttons.y = !(buttons & (1UL << BUTTON_Y));
    device->state.buttons.select = !(buttons & (1UL << BUTTON_SELECT));
    device->state.buttons.start = !(buttons & (1UL << BUTTON_START));

    return true;
}

bool gamepad_qts_update_states(gamepad_qt_device_t* devices, uint8_t count)
{
    bool updated = false;
    for (uint8_t i = 0; i < count; i++) {
        updated = gamepad_qts_update_state(&(devices[i])) || updated;
    }
    return updated;
}
