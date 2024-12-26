#include "tap_dance_quad.h"

td_quad_state_t current_dance_quad_state(tap_dance_state_t* state)
{
    switch (state->count) {
    case 1:
        if (state->interrupted || !state->pressed)
            return TD_SINGLE_TAP;
        return TD_SINGLE_HOLD;
    case 2:
        if (state->interrupted)
            return TD_DOUBLE_SINGLE_TAP;
        else if (state->pressed)
            return TD_DOUBLE_HOLD;
        return TD_DOUBLE_TAP;
    case 3:
        if (state->interrupted || !state->pressed)
            return TD_TRIPLE_TAP;
        return TD_TRIPLE_HOLD;
    default:
        return TD_UNKNOWN;
    }
}

void td_quad_finished(tap_dance_state_t* state, void* user_data)
{
    td_quad_keycodes_t* kcs = (td_quad_keycodes_t*)user_data;
    switch (current_dance_quad_state(state)) {
    case TD_SINGLE_TAP:
        if (kcs->kc_tap) {
            register_code(kcs->kc_tap);
        }
        break;
    case TD_SINGLE_HOLD:
        if (kcs->kc_hold) {
            register_code(kcs->kc_hold);
        }
        break;
    case TD_DOUBLE_TAP:
        if (kcs->kc_double_tap) {
            register_code(kcs->kc_double_tap);
        }
        break;
    case TD_DOUBLE_HOLD:
        if (kcs->kc_double_tap_hold) {
            register_code(kcs->kc_double_tap_hold);
        }
        break;
    case TD_DOUBLE_SINGLE_TAP:
        if (kcs->kc_tap) {
            tap_code(kcs->kc_tap);
            register_code(kcs->kc_tap);
        }
        break;
    default:
        break;
    }
}

void td_quad_reset(tap_dance_state_t* state, void* user_data)
{
    td_quad_keycodes_t* kcs = (td_quad_keycodes_t*)user_data;
    switch (current_dance_quad_state(state)) {
    case TD_SINGLE_TAP:
        if (kcs->kc_tap) {
            unregister_code(kcs->kc_tap);
        }
        break;
    case TD_SINGLE_HOLD:
        if (kcs->kc_hold) {
            unregister_code(kcs->kc_hold);
        }
        break;
    case TD_DOUBLE_TAP:
        if (kcs->kc_double_tap) {
            unregister_code(kcs->kc_double_tap);
        }
        break;
    case TD_DOUBLE_HOLD:
        if (kcs->kc_double_tap_hold) {
            unregister_code(kcs->kc_double_tap_hold);
        }
        break;
    case TD_DOUBLE_SINGLE_TAP:
        if (kcs->kc_tap) {
            unregister_code(kcs->kc_tap);
        }
        break;
    default:
        break;
    }
}
