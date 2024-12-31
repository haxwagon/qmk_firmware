#pragma once

#include "seesaw.h"

#ifndef GAMEPAD_QTS_COUNT
#define GAMEPAD_QTS_COUNT 1
#endif

#ifndef GAMEPAD_QTS_DEADZONE
#define GAMEPAD_QTS_DEADZONE 4
#endif

// The default I2C address is 0x50. The other address options can be calculated by "adding" the A0/A1 to the base of 0x50.
// A0 sets the lowest bit with a value of 1 and A1 sets the next bit with a value of 2. The final address is 0x50 + A0 + A1 which would be 0x53.
// * If only A0 is cut, the address is 0x50 + 1 = 0x51.
// * If only A1 is cut, the address is 0x50 + 2 = 0x52.
// * If both A0 and A1 are cut, the address is 0x50 + 1 + 2 = 0x53.

typedef struct {
    bool a;
    bool b;
    bool x;
    bool y;
    bool select;
    bool start;
} gamepad_qt_buttons_t;

typedef struct {
    uint16_t x;
    uint16_t y;
    gamepad_qt_buttons_t buttons;
} gamepad_qt_state_t;

typedef struct {
    seesaw_device_t seesaw;
    bool flip_x;
    bool flip_y;
    bool swap_xy;
    uint16_t last_x;
    uint16_t last_y;
    gamepad_qt_state_t state;
} gamepad_qt_device_t;

void gamepad_qts_init(gamepad_qt_device_t* devices, uint8_t count);
bool gamepad_qts_update_state(gamepad_qt_device_t* device);
bool gamepad_qts_update_states(gamepad_qt_device_t* devices, uint8_t count);
