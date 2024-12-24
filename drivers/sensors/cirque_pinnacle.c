// Copyright (c) 2018 Cirque Corp. Restrictions apply. See: www.cirque.com/sw-license
// based on https://github.com/cirque-corp/Cirque_Pinnacle_1CA027/tree/master/Circular_Trackpad
// with modifications and changes for QMK
// refer to documentation: Gen2 and Gen3 (Pinnacle ASIC) at https://www.cirque.com/documentation

#include "cirque_pinnacle.h"
#include "cirque_pinnacle_gestures.h"
#include "wait.h"
#include "timer.h"

#include <stdlib.h>

#define ZVALUE_MAP_ROWS_Y 6
#define ZVALUE_MAP_COLS_X 8
static const uint8_t CIRQUE_ZVALUE_MAP[ZVALUE_MAP_ROWS_Y][ZVALUE_MAP_COLS_X] = {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 2, 3, 5, 5, 3, 2, 0},
        {0, 3, 5, 15, 15, 5, 2, 0},
        {0, 3, 5, 15, 15, 5, 3, 0},
        {0, 2, 3, 5, 5, 3, 2, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
};

#ifndef CIRQUE_PINNACLE_ATTENUATION
#    ifdef CIRQUE_PINNACLE_CURVED_OVERLAY
#        define CIRQUE_PINNACLE_ATTENUATION EXTREG__TRACK_ADCCONFIG__ADC_ATTENUATE_2X
#    else
#        define CIRQUE_PINNACLE_ATTENUATION EXTREG__TRACK_ADCCONFIG__ADC_ATTENUATE_4X
#    endif
#endif

bool     touchpad_init;
uint16_t scale_data = CIRQUE_PINNACLE_DEFAULT_SCALE;

void cirque_pinnacle_clear_flags(uint8_t device_address);
void cirque_pinnacle_enable_feed(uint8_t device_address, bool feedEnable);
void RAP_ReadBytes(uint8_t device_address, uint8_t address, uint8_t* data, uint8_t count);
void RAP_Write(uint8_t device_address, uint8_t address, uint8_t data);

uint16_t cirque_pinnacle_get_scale(void) {
    return scale_data;
}
void cirque_pinnacle_set_scale(uint16_t scale) {
    scale_data = scale;
}

// scale to [0, resolution - 1]
uint16_t cirque_pinnacle_scale_absolute_x(uint16_t value, uint16_t resolution) {
    if (value < CIRQUE_PINNACLE_X_LOWER) {
        value = CIRQUE_PINNACLE_X_LOWER;
    } else if (value > CIRQUE_PINNACLE_X_UPPER) {
        value = CIRQUE_PINNACLE_X_UPPER;
    }
    value -= CIRQUE_PINNACLE_X_LOWER;
    return (uint16_t)(value * (resolution - 1) / CIRQUE_PINNACLE_X_RANGE);
}

uint16_t cirque_pinnacle_scale_absolute_y(uint16_t value, uint16_t resolution) {
    if (value < CIRQUE_PINNACLE_Y_LOWER) {
        value = CIRQUE_PINNACLE_Y_LOWER;
    } else if (value > CIRQUE_PINNACLE_Y_UPPER) {
        value = CIRQUE_PINNACLE_Y_UPPER;
    }
    value -= CIRQUE_PINNACLE_Y_LOWER;
    return (uint16_t)(value * (resolution - 1) / CIRQUE_PINNACLE_Y_RANGE);
}

// scale coordinates to [0, xResolution - 1] x [0, yResolution - 1] range
void cirque_pinnacle_scale_absolute_data(pinnacle_absolute_data_t* coordinates, uint16_t xResolution, uint16_t yResolution) {
    coordinates->xValue = cirque_pinnacle_scale_absolute_x(coordinates->xValue, xResolution);
    coordinates->yValue = cirque_pinnacle_scale_absolute_y(coordinates->yValue, yResolution);
}

void cirque_pinnacle_scale_relative_data(pinnacle_relative_data_t* coordinates, uint16_t xResolution, uint16_t yResolution) {
    int32_t        xTemp = 0, yTemp = 0;
    ldiv_t         temp;
    static int32_t xRemainder, yRemainder;

    temp       = ldiv(((int32_t)coordinates->xDelta) * (int32_t)xResolution + xRemainder, (int32_t)256);
    xTemp      = temp.quot;
    xRemainder = temp.rem;

    temp       = ldiv(((int32_t)coordinates->yDelta) * (int32_t)yResolution + yRemainder, (int32_t)256);
    yTemp      = temp.quot;
    yRemainder = temp.rem;

    coordinates->xDelta = (int16_t)xTemp;
    coordinates->yDelta = (int16_t)yTemp;
}

// Clears Status1 register flags (SW_CC and SW_DR)
void cirque_pinnacle_clear_flags(uint8_t device_address) {
    RAP_Write(device_address, HOSTREG__STATUS1, HOSTREG__STATUS1_DEFVAL & ~(HOSTREG__STATUS1__COMMAND_COMPLETE | HOSTREG__STATUS1__DATA_READY));
    wait_us(50);
}

// Enables/Disables the feed
void cirque_pinnacle_enable_feed(uint8_t device_address, bool feedEnable) {
    uint8_t feedconfig1;
    RAP_ReadBytes(device_address, HOSTREG__FEEDCONFIG1, &feedconfig1, 1);

    if (feedEnable) {
        feedconfig1 |= HOSTREG__FEEDCONFIG1__FEED_ENABLE;
    } else {
        feedconfig1 &= ~HOSTREG__FEEDCONFIG1__FEED_ENABLE;
    }
    RAP_Write(device_address, HOSTREG__FEEDCONFIG1, feedconfig1);
}

/*  ERA (Extended Register Access) Functions  */
// Reads <count> bytes from an extended register at <address> (16-bit address),
// stores values in <*data>
void ERA_ReadBytes(uint8_t device_address, uint16_t address, uint8_t* data, uint16_t count) {
    uint8_t  ERAControlValue = 0xFF;
    uint16_t timeout_timer;

    cirque_pinnacle_enable_feed(device_address, false); // Disable feed

    RAP_Write(device_address, HOSTREG__EXT_REG_AXS_ADDR_HIGH, (uint8_t)(address >> 8));    // Send upper byte of ERA address
    RAP_Write(device_address, HOSTREG__EXT_REG_AXS_ADDR_LOW, (uint8_t)(address & 0x00FF)); // Send lower byte of ERA address

    for (uint16_t i = 0; i < count; i++) {
        RAP_Write(device_address, HOSTREG__EXT_REG_AXS_CTRL, HOSTREG__EREG_AXS__INC_ADDR_READ | HOSTREG__EREG_AXS__READ); // Signal ERA-read (auto-increment) to Pinnacle

        // Wait for status register 0x1E to clear
        timeout_timer = timer_read();
        do {
            RAP_ReadBytes(device_address, HOSTREG__EXT_REG_AXS_CTRL, &ERAControlValue, 1);
        } while ((ERAControlValue != 0x00) && (timer_elapsed(timeout_timer) <= CIRQUE_PINNACLE_TIMEOUT));

        RAP_ReadBytes(device_address, HOSTREG__EXT_REG_AXS_VALUE, data + i, 1);

        cirque_pinnacle_clear_flags(device_address);
    }
}

// Writes a byte, <data>, to an extended register at <address> (16-bit address)
void ERA_WriteByte(uint8_t device_address, uint16_t address, uint8_t data) {
    uint8_t  ERAControlValue = 0xFF;
    uint16_t timeout_timer;

    cirque_pinnacle_enable_feed(device_address, false); // Disable feed

    RAP_Write(device_address, HOSTREG__EXT_REG_AXS_VALUE, data); // Send data byte to be written

    RAP_Write(device_address, HOSTREG__EXT_REG_AXS_ADDR_HIGH, (uint8_t)(address >> 8));    // Upper byte of ERA address
    RAP_Write(device_address, HOSTREG__EXT_REG_AXS_ADDR_LOW, (uint8_t)(address & 0x00FF)); // Lower byte of ERA address

    RAP_Write(device_address, HOSTREG__EXT_REG_AXS_CTRL, HOSTREG__EREG_AXS__WRITE); // Signal an ERA-write to Pinnacle

    // Wait for status register 0x1E to clear
    timeout_timer = timer_read();
    do {
        RAP_ReadBytes(device_address, HOSTREG__EXT_REG_AXS_CTRL, &ERAControlValue, 1);
    } while ((ERAControlValue != 0x00) && (timer_elapsed(timeout_timer) <= CIRQUE_PINNACLE_TIMEOUT));

    cirque_pinnacle_clear_flags(device_address);
}

bool cirque_pinnacle_set_adc_attenuation(uint8_t device_address, uint8_t adcGain) {
    uint8_t adcconfig = 0x00;

    ERA_ReadBytes(device_address, EXTREG__TRACK_ADCCONFIG, &adcconfig, 1);
    adcGain &= EXTREG__TRACK_ADCCONFIG__ADC_ATTENUATE_MASK;
    if (adcGain == (adcconfig & EXTREG__TRACK_ADCCONFIG__ADC_ATTENUATE_MASK)) {
        return false;
    }
    adcconfig &= ~EXTREG__TRACK_ADCCONFIG__ADC_ATTENUATE_MASK;
    adcconfig |= adcGain;
    ERA_WriteByte(device_address, EXTREG__TRACK_ADCCONFIG, adcconfig);
    ERA_ReadBytes(device_address, EXTREG__TRACK_ADCCONFIG, &adcconfig, 1);

    return true;
}

// Changes thresholds to improve detection of fingers
// Not needed for flat overlay?
void cirque_pinnacle_tune_edge_sensitivity(uint8_t device_address) {
    uint8_t widezmin = 0x00;

    ERA_ReadBytes(device_address, EXTREG__XAXIS_WIDEZMIN, &widezmin, 1);
    ERA_WriteByte(device_address, EXTREG__XAXIS_WIDEZMIN, 0x04); // magic number from Cirque sample code
    ERA_ReadBytes(device_address, EXTREG__XAXIS_WIDEZMIN, &widezmin, 1);

    ERA_ReadBytes(device_address, EXTREG__YAXIS_WIDEZMIN, &widezmin, 1);
    ERA_WriteByte(device_address, EXTREG__YAXIS_WIDEZMIN, 0x03); // magic number from Cirque sample code
    ERA_ReadBytes(device_address, EXTREG__YAXIS_WIDEZMIN, &widezmin, 1);
}

// Perform calibration
void cirque_pinnacle_calibrate_device(uint8_t device_address) {
    uint8_t  calconfig;
    uint16_t timeout_timer;

    RAP_ReadBytes(device_address, HOSTREG__CALCONFIG1, &calconfig, 1);
    calconfig |= HOSTREG__CALCONFIG1__CALIBRATE;
    RAP_Write(device_address, HOSTREG__CALCONFIG1, calconfig);

    // Calibration takes ~100ms according to GT-AN-090624, doubling the timeout just to be safe
    timeout_timer = timer_read();
    do {
        RAP_ReadBytes(device_address, HOSTREG__CALCONFIG1, &calconfig, 1);
    } while ((calconfig & HOSTREG__CALCONFIG1__CALIBRATE) && (timer_elapsed(timeout_timer) <= 200));

    cirque_pinnacle_clear_flags(device_address);
}

// Enable/disable cursor smoothing, smoothing is enabled by default
void cirque_pinnacle_cursor_smoothing_device(uint8_t device_address, bool enable) {
    uint8_t feedconfig3;

    RAP_ReadBytes(device_address, HOSTREG__FEEDCONFIG3, &feedconfig3, 1);
    if (enable) {
        feedconfig3 &= ~HOSTREG__FEEDCONFIG3__DISABLE_CROSS_RATE_SMOOTHING;
    } else {
        feedconfig3 |= HOSTREG__FEEDCONFIG3__DISABLE_CROSS_RATE_SMOOTHING;
    }
    RAP_Write(device_address, HOSTREG__FEEDCONFIG3, feedconfig3);
}

// Check sensor is connected
bool cirque_pinnacle_connected(uint8_t device_address) {
    uint8_t current_zidle = 0;
    uint8_t temp_zidle    = 0;
    RAP_ReadBytes(device_address, HOSTREG__ZIDLE, &current_zidle, 1);
    RAP_Write(device_address, HOSTREG__ZIDLE, HOSTREG__ZIDLE_DEFVAL);
    RAP_ReadBytes(device_address, HOSTREG__ZIDLE, &temp_zidle, 1);
    if (temp_zidle == HOSTREG__ZIDLE_DEFVAL) {
        RAP_Write(device_address, HOSTREG__ZIDLE, current_zidle);
        return true;
    }
    return false;
}

/*  Pinnacle-based TM040040/TM035035/TM023023 Functions  */
void cirque_pinnacle_init_device(uint8_t device_address, bool is_absolute) {
#if defined(POINTING_DEVICE_DRIVER_cirque_pinnacle_spi)
    spi_init();
#elif defined(POINTING_DEVICE_DRIVER_cirque_pinnacle_i2c)
    i2c_init();
#endif

    touchpad_init = true;

    // send a RESET command now, in case QMK had a soft-reset without a power cycle
    RAP_Write(device_address, HOSTREG__SYSCONFIG1, HOSTREG__SYSCONFIG1__RESET);
    wait_ms(30); // Pinnacle needs 10-15ms to boot, so wait long enough before configuring
    RAP_Write(device_address, HOSTREG__SYSCONFIG1, HOSTREG__SYSCONFIG1_DEFVAL);
    wait_us(50);

    // Host clears SW_CC flag
    cirque_pinnacle_clear_flags(device_address);

    if (is_absolute) {
        RAP_Write(device_address, HOSTREG__FEEDCONFIG2, HOSTREG__FEEDCONFIG2_DEFVAL);
    } else {
        // FeedConfig2 (Feature flags for Relative Mode Only)
        uint8_t feedconfig2 = HOSTREG__FEEDCONFIG2__GLIDE_EXTEND_DISABLE | HOSTREG__FEEDCONFIG2__INTELLIMOUSE_MODE;
    #    if !defined(CIRQUE_PINNACLE_TAP_ENABLE)
        feedconfig2 |= HOSTREG__FEEDCONFIG2__ALL_TAP_DISABLE;
    #    endif
    #    if !defined(CIRQUE_PINNACLE_SECONDARY_TAP_ENABLE)
        feedconfig2 |= HOSTREG__FEEDCONFIG2__SECONDARY_TAP_DISABLE;
    #    elif !defined(CIRQUE_PINNACLE_TAP_ENABLE)
    #        error CIRQUE_PINNACLE_TAP_ENABLE must be defined for CIRQUE_PINNACLE_SECONDARY_TAP_ENABLE to work
    #    endif
    #    if !defined(CIRQUE_PINNACLE_SIDE_SCROLL_ENABLE)
        feedconfig2 |= HOSTREG__FEEDCONFIG2__SCROLL_DISABLE;
    #    endif
        RAP_Write(device_address, HOSTREG__FEEDCONFIG2, feedconfig2);
    }

    // FeedConfig1 (Data Output Flags)
    RAP_Write(device_address, HOSTREG__FEEDCONFIG1, CIRQUE_PINNACLE_POSITION_MODE ? HOSTREG__FEEDCONFIG1__DATA_TYPE__REL0_ABS1 : HOSTREG__FEEDCONFIG1_DEFVAL);

#if CIRQUE_PINNACLE_POSITION_MODE
    // Host sets z-idle packet count to 5 (default is 0x1E/30)
    RAP_Write(device_address, HOSTREG__ZIDLE, 5);
#endif

    bool calibrate = cirque_pinnacle_set_adc_attenuation(device_address, CIRQUE_PINNACLE_ATTENUATION);

#ifdef CIRQUE_PINNACLE_CURVED_OVERLAY
    cirque_pinnacle_tune_edge_sensitivity();
    calibrate = true;
#endif
    if (calibrate) {
        // Force a calibration after setting ADC attenuation
        cirque_pinnacle_calibrate_device(device_address);
    }

    cirque_pinnacle_enable_feed(device_address, true);

#ifndef CIRQUE_PINNACLE_SKIP_SENSOR_CHECK
    touchpad_init = cirque_pinnacle_connected(device_address);
#endif
}

uint16_t cirque_pinnacle_get_cpi(void) {
    return CIRQUE_PINNACLE_PX_TO_INCH(cirque_pinnacle_get_scale());
}
void cirque_pinnacle_set_cpi(uint16_t cpi) {
    cirque_pinnacle_set_scale(CIRQUE_PINNACLE_INCH_TO_PX(cpi));
}

bool is_touch_hovering(pinnacle_absolute_data_t* touchData) {
    pinnacle_absolute_data_t tempTouchData;
    memcpy(&tempTouchData, touchData, sizeof(pinnacle_absolute_data_t));
    cirque_pinnacle_scale_absolute_data(&tempTouchData, ZVALUE_MAP_COLS_X, ZVALUE_MAP_ROWS_Y);
    return touchData->zValue < CIRQUE_ZVALUE_MAP[tempTouchData.yValue][tempTouchData.xValue];
}

pinnacle_absolute_data_t cirque_pinnacle_read_absolute_device_data(uint8_t device_address) {
    uint8_t                  data_ready = 0;
    uint8_t                  data[6]    = {0};
    pinnacle_absolute_data_t result     = {0};

    // Check if there is valid data available
    RAP_ReadBytes(device_address, HOSTREG__STATUS1, &data_ready, 1);
    if ((data_ready & HOSTREG__STATUS1__DATA_READY) == 0) {
        // no data available yet
        result.valid = false; // be explicit
        return result;
    }

    // Read all data bytes
    RAP_ReadBytes(device_address, HOSTREG__PACKETBYTE_0, data, 6);

    // Get ready for the next data sample
    cirque_pinnacle_clear_flags(device_address);

    // Decode data for absolute mode
    // Register 0x13 is unused in this mode (palm detection area)
    result.buttonFlags = data[0] & 0x3F;                             // bit0 to bit5 are switch 0-5, only hardware button presses (from input pin on the Pinnacle chip)
    result.xValue      = data[2] | ((data[4] & 0x0F) << 8);          // merge high and low bits for X
    result.yValue      = data[3] | ((data[4] & 0xF0) << 4);          // merge high and low bits for Y
    result.zValue      = data[5] & 0x3F;                             // Z is only lower 6 bits, upper 2 bits are reserved/unused
    result.touchDown   = (result.xValue != 0 || result.yValue != 0); // (0,0) is a "magic coordinate" to indicate "finger touched down"
    result.hovering    = is_touch_hovering(&result);

#ifdef CIRQUE_PINNACLE_REACHABLE_CALIBRATION
    static uint16_t xMin = UINT16_MAX, yMin = UINT16_MAX, yMax = 0, xMax = 0;
    if (result.xValue < xMin) xMin = result.xValue;
    if (result.xValue > xMax) xMax = result.xValue;
    if (result.yValue < yMin) yMin = result.yValue;
    if (result.yValue > yMax) yMax = result.yValue;
    pd_dprintf("%s: xLo=%3d xHi=%3d yLo=%3d yHi=%3d\n", __FUNCTION__, xMin, xMax, yMin, yMax);
#endif

    result.valid = true;
    return result;
}

pinnacle_relative_data_t cirque_pinnacle_read_relative_device_data(uint8_t device_address) {
    uint8_t                  data_ready = 0;
    uint8_t                  data[6]    = {0};
    pinnacle_relative_data_t result     = {0};

    // Check if there is valid data available
    RAP_ReadBytes(device_address, HOSTREG__STATUS1, &data_ready, 1);
    if ((data_ready & HOSTREG__STATUS1__DATA_READY) == 0) {
        // no data available yet
        result.valid = false; // be explicit
        return result;
    }

    // Read all data bytes
    RAP_ReadBytes(device_address, HOSTREG__PACKETBYTE_0, data, 6);

    // Get ready for the next data sample
    cirque_pinnacle_clear_flags(device_address);

    // Decode data for relative mode
    // Registers 0x16 and 0x17 are unused in this mode
    result.buttonFlags = data[0] & 0x07; // Only three buttons are supported
    if ((data[0] & 0x10) && data[1] != 0) {
        result.xDelta = -((int16_t)256 - (int16_t)(data[1]));
    } else {
        result.xDelta = data[1];
    }
    if ((data[0] & 0x20) && data[2] != 0) {
        result.yDelta = ((int16_t)256 - (int16_t)(data[2]));
    } else {
        result.yDelta = -((int16_t)data[2]);
    }
    result.scrollWheelCount = ((int8_t*)data)[3];

#ifdef CIRQUE_PINNACLE_REACHABLE_CALIBRATION
    static uint16_t xMin = UINT16_MAX, yMin = UINT16_MAX, yMax = 0, xMax = 0;
    if (result.xValue < xMin) xMin = result.xValue;
    if (result.xValue > xMax) xMax = result.xValue;
    if (result.yValue < yMin) yMin = result.yValue;
    if (result.yValue > yMax) yMax = result.yValue;
    pd_dprintf("%s: xLo=%3d xHi=%3d yLo=%3d yHi=%3d\n", __FUNCTION__, xMin, xMax, yMin, yMax);
#endif

    result.valid = true;
    return result;
}

#ifdef POINTING_DEVICE_GESTURES_CURSOR_GLIDE_ENABLE
static bool cursor_glide_enable = true;

static cursor_glide_context_t glide = {.config = {
                                           .coef       = 102, /* Good default friction coef */
                                           .interval   = 10,  /* 100sps */
                                           .trigger_px = 10,  /* Default threshold in case of hover, set to 0 if you'd like */
                                       }};

void cirque_pinnacle_enable_cursor_glide(bool enable) {
    cursor_glide_enable = enable;
}

void cirque_pinnacle_configure_cursor_glide(float trigger_px) {
    glide.config.trigger_px = trigger_px;
}
#endif

report_mouse_t cirque_pinnacle_relative_data_to_mouse_report(pinnacle_relative_data_t data, report_mouse_t mouse_report) {
    // Scale coordinates to arbitrary X, Y resolution
    cirque_pinnacle_scale_relative_data(&data, cirque_pinnacle_get_scale(), cirque_pinnacle_get_scale());

    if (data.valid) {
        mouse_report.buttons = data.buttonFlags;
        mouse_report.x = CONSTRAIN_HID_XY(data.xDelta);
        mouse_report.y = CONSTRAIN_HID_XY(data.yDelta);
        mouse_report.v = data.scrollWheelCount;
    }
    return mouse_report;
}


#    ifdef POINTING_DEVICE_AUTO_MOUSE_ENABLE
static bool is_touch_down;

bool auto_mouse_activation(report_mouse_t mouse_report) {
    return is_touch_down || mouse_report.x != 0 || mouse_report.y != 0 || mouse_report.h != 0 || mouse_report.v != 0 || mouse_report.buttons;
}
#    endif

report_mouse_t cirque_pinnacle_absolute_data_to_mouse_report(pinnacle_absolute_data_t data, report_mouse_t mouse_report) {
    uint16_t                 scale        = cirque_pinnacle_get_scale();
    mouse_xy_report_t        report_x     = 0, report_y = 0;
    static                   uint16_t   x = 0, y        = 0, last_scale = 0;

#    if defined(CIRQUE_PINNACLE_TAP_ENABLE)
    mouse_report.buttons = pointing_device_handle_buttons(mouse_report.buttons, false, POINTING_DEVICE_BUTTON1);
#    endif
#    ifdef POINTING_DEVICE_GESTURES_CURSOR_GLIDE_ENABLE
    cursor_glide_t glide_report = {0};

    if (cursor_glide_enable) {
        glide_report = cursor_glide_check(&glide);
    }
#    endif

    if (!data.valid) {
#    ifdef POINTING_DEVICE_GESTURES_CURSOR_GLIDE_ENABLE
        if (cursor_glide_enable && glide_report.valid) {
            report_x = glide_report.dx;
            report_y = glide_report.dy;
            goto mouse_report_update;
        }
#    endif
        return mouse_report;
    }

    if (data.touchDown) {
        pd_dprintf("cirque_pinnacle touch data x=%4d y=%4d z=%2d\n", data.xValue, data.yValue, data.zValue);
    }

#    ifdef POINTING_DEVICE_AUTO_MOUSE_ENABLE
    is_touch_down = data.touchDown;
#    endif

    // Scale coordinates to arbitrary X, Y resolution
    cirque_pinnacle_scale_absolute_data(&data, scale, scale);

    if (!cirque_pinnacle_gestures(&mouse_report, data)) {
        if (last_scale && scale == last_scale && x && y && data.xValue && data.yValue) {
            report_x = CONSTRAIN_HID_XY((int16_t)(data.xValue - x));
            report_y = CONSTRAIN_HID_XY((int16_t)(data.yValue - y));
        }
        x          = data.xValue;
        y          = data.yValue;
        last_scale = scale;

#    ifdef POINTING_DEVICE_GESTURES_CURSOR_GLIDE_ENABLE
        if (cursor_glide_enable) {
            if (data.touchDown) {
                cursor_glide_update(&glide, report_x, report_y, data.zValue);
            } else if (!glide_report.valid) {
                glide_report = cursor_glide_start(&glide);
                if (glide_report.valid) {
                    report_x = glide_report.dx;
                    report_y = glide_report.dy;
                }
            }
        }
#    endif
    }

#    ifdef POINTING_DEVICE_GESTURES_CURSOR_GLIDE_ENABLE
mouse_report_update:
#    endif
    mouse_report.x = report_x;
    mouse_report.y = report_y;

    return mouse_report;
}

#if CIRQUE_PINNACLE_POSITION_MODE

report_mouse_t cirque_pinnacle_get_device_report(uint8_t device_address, report_mouse_t mouse_report) {
    pinnacle_absolute_data_t touchData = cirque_pinnacle_read_absolute_device_data(device_address);

    return cirque_pinnacle_absolute_data_to_mouse_report(touchData, mouse_report);
}

#else

report_mouse_t cirque_pinnacle_get_device_report(uint8_t device_address, report_mouse_t mouse_report) {
    pinnacle_relative_data_t touchData = cirque_pinnacle_read_relative_device_data(device_address);

    return cirque_pinnacle_relative_data_to_mouse_report(touchData, mouse_report);
}

#endif



#ifndef CIRQUE_PINNACLE_CUSTOM

void cirque_pinnacle_init(void) {

    cirque_pinnacle_init_device(CIRQUE_PINNACLE_ADDR,
#if CIRQUE_PINNACLE_POSITION_MODE
        true
#else
        false
#endif
    );
}

void cirque_pinnacle_calibrate(void) {
    cirque_pinnacle_calibrate_device(CIRQUE_PINNACLE_ADDR);
}

void cirque_pinnacle_cursor_smoothing(bool enable) {
    cirque_pinnacle_cursor_smoothing_device(CIRQUE_PINNACLE_ADDR, enable);
}

report_mouse_t cirque_pinnacle_get_report(report_mouse_t mouse_report) {
    return cirque_pinnacle_get_device_report(CIRQUE_PINNACLE_ADDR, mouse_report);
}

pinnacle_data_t cirque_pinnacle_read_data(void) {
    pinnacle_data_t data;
#if CIRQUE_PINNACLE_POSITION_MODE
    pinnacle_absolute_data_t absolute_data = cirque_pinnacle_read_absolute_device_data(CIRQUE_PINNACLE_ADDR);
    data.valid       = absolute_data.valid;
    data.xValue      = absolute_data.xValue;
    data.yValue      = absolute_data.yValue;
    data.zValue      = absolute_data.zValue;
    data.buttonFlags = absolute_data.buttonFlags;
    data.touchDown   = absolute_data.touchDown;
#else
    pinnacle_relative_data_t relative_data = cirque_pinnacle_read_relative_device_data(CIRQUE_PINNACLE_ADDR);
    data.valid = relative_data.valid;
    data.xDelta = relative_data.xDelta;
    data.yDelta = relative_data.yDelta;
    data.wheelCount = relative_data.scrollWheelCount;
    data.buttons = relative_data.buttonFlags;
#endif
    return data;
}

// clang-format off
const pointing_device_driver_t cirque_pinnacle_pointing_device_driver = {
    .init       = cirque_pinnacle_init,
    .get_report = cirque_pinnacle_get_report,
    .set_cpi    = cirque_pinnacle_set_cpi,
    .get_cpi    = cirque_pinnacle_get_cpi
};
// clang-format on
#endif
