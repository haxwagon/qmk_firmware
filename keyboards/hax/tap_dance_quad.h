#pragma once

#include QMK_KEYBOARD_H

typedef enum {
    TD_NONE,
    TD_UNKNOWN,
    TD_SINGLE_TAP,
    TD_SINGLE_HOLD,
    TD_DOUBLE_TAP,
    TD_DOUBLE_HOLD,
    TD_DOUBLE_SINGLE_TAP, // Send two single taps
    TD_TRIPLE_TAP,
    TD_TRIPLE_HOLD
} td_quad_state_t;

typedef struct
{
    uint16_t kc_tap;
    uint16_t kc_hold;
    uint16_t kc_double_tap;
    uint16_t kc_double_tap_hold;
} td_quad_keycodes_t;

void td_quad_finished(tap_dance_state_t* state, void* user_data);
void td_quad_reset(tap_dance_state_t* state, void* user_data);

#define ACTION_TAP_DANCE_QUAD(kc_tap, kc_hold, kc_double_tap, kc_double_tap_hold)                           \
    {                                                                                                       \
        .fn = { NULL, td_quad_finished, td_quad_reset, NULL },                                              \
        .user_data = (void*)&((td_quad_keycodes_t) { kc_tap, kc_hold, kc_double_tap, kc_double_tap_hold }), \
    }
