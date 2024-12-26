#include QMK_KEYBOARD_H
// Mostly from https://github.com/adafruit/Adafruit_Seesaw/blob/master/Adafruit_seesaw.h
//             https://github.com/adafruit/Adafruit_Seesaw/blob/master/Adafruit_seesaw.cpp

#include "seesaw.h"
#include "i2c_master.h"

enum {
    SEESAW_STATUS_BASE  = 0x00,
    SEESAW_GPIO_BASE    = 0x01,
    SEESAW_SERCOM0_BASE = 0x02,

    SEESAW_TIMER_BASE     = 0x08,
    SEESAW_ADC_BASE       = 0x09,
    SEESAW_DAC_BASE       = 0x0A,
    SEESAW_INTERRUPT_BASE = 0x0B,
    SEESAW_DAP_BASE       = 0x0C,
    SEESAW_EEPROM_BASE    = 0x0D,
    SEESAW_NEOPIXEL_BASE  = 0x0E,
    SEESAW_TOUCH_BASE     = 0x0F,
    SEESAW_KEYPAD_BASE    = 0x10,
    SEESAW_ENCODER_BASE   = 0x11,
    SEESAW_SPECTRUM_BASE  = 0x12,
};

/** GPIO module function address registers
 */
enum {
    SEESAW_GPIO_DIRSET_BULK = 0x02,
    SEESAW_GPIO_DIRCLR_BULK = 0x03,
    SEESAW_GPIO_BULK        = 0x04,
    SEESAW_GPIO_BULK_SET    = 0x05,
    SEESAW_GPIO_BULK_CLR    = 0x06,
    SEESAW_GPIO_BULK_TOGGLE = 0x07,
    SEESAW_GPIO_INTENSET    = 0x08,
    SEESAW_GPIO_INTENCLR    = 0x09,
    SEESAW_GPIO_INTFLAG     = 0x0A,
    SEESAW_GPIO_PULLENSET   = 0x0B,
    SEESAW_GPIO_PULLENCLR   = 0x0C,
};

/** status module function address registers
 */
enum {
    SEESAW_STATUS_HW_ID   = 0x01,
    SEESAW_STATUS_VERSION = 0x02,
    SEESAW_STATUS_OPTIONS = 0x03,
    SEESAW_STATUS_TEMP    = 0x04,
    SEESAW_STATUS_SWRST   = 0x7F,
};

/** timer module function address registers
 */
enum {
    SEESAW_TIMER_STATUS = 0x00,
    SEESAW_TIMER_PWM    = 0x01,
    SEESAW_TIMER_FREQ   = 0x02,
};

/** ADC module function address registers
 */
enum {
    SEESAW_ADC_STATUS         = 0x00,
    SEESAW_ADC_INTEN          = 0x02,
    SEESAW_ADC_INTENCLR       = 0x03,
    SEESAW_ADC_WINMODE        = 0x04,
    SEESAW_ADC_WINTHRESH      = 0x05,
    SEESAW_ADC_CHANNEL_OFFSET = 0x07,
};

/** Sercom module function address registers
 */
enum {
    SEESAW_SERCOM_STATUS   = 0x00,
    SEESAW_SERCOM_INTEN    = 0x02,
    SEESAW_SERCOM_INTENCLR = 0x03,
    SEESAW_SERCOM_BAUD     = 0x04,
    SEESAW_SERCOM_DATA     = 0x05,
};

/** neopixel module function address registers
 */
enum {
    SEESAW_NEOPIXEL_STATUS     = 0x00,
    SEESAW_NEOPIXEL_PIN        = 0x01,
    SEESAW_NEOPIXEL_SPEED      = 0x02,
    SEESAW_NEOPIXEL_BUF_LENGTH = 0x03,
    SEESAW_NEOPIXEL_BUF        = 0x04,
    SEESAW_NEOPIXEL_SHOW       = 0x05,
};

/** touch module function address registers
 */
enum {
    SEESAW_TOUCH_CHANNEL_OFFSET = 0x10,
};

/** keypad module function address registers
 */
enum {
    SEESAW_KEYPAD_STATUS   = 0x00,
    SEESAW_KEYPAD_EVENT    = 0x01,
    SEESAW_KEYPAD_INTENSET = 0x02,
    SEESAW_KEYPAD_INTENCLR = 0x03,
    SEESAW_KEYPAD_COUNT    = 0x04,
    SEESAW_KEYPAD_FIFO     = 0x10,
};

/** keypad module edge definitions
 */
enum {
    SEESAW_KEYPAD_EDGE_HIGH = 0,
    SEESAW_KEYPAD_EDGE_LOW,
    SEESAW_KEYPAD_EDGE_FALLING,
    SEESAW_KEYPAD_EDGE_RISING,
};

/** encoder module edge definitions
 */
enum {
    SEESAW_ENCODER_STATUS   = 0x00,
    SEESAW_ENCODER_INTENSET = 0x10,
    SEESAW_ENCODER_INTENCLR = 0x20,
    SEESAW_ENCODER_POSITION = 0x30,
    SEESAW_ENCODER_DELTA    = 0x40,
};

/** Audio spectrum module function address registers
 */
enum {
    SEESAW_SPECTRUM_RESULTS_LOWER = 0x00, // Audio spectrum bins 0-31
    SEESAW_SPECTRUM_RESULTS_UPPER = 0x01, // Audio spectrum bins 32-63
    // If some future device supports a larger spectrum, can add additional
    // "bins" working upward from here. Configurable setting registers then
    // work downward from the top to avoid collision between spectrum bins
    // and configurables.
    SEESAW_SPECTRUM_CHANNEL = 0xFD,
    SEESAW_SPECTRUM_RATE    = 0xFE,
    SEESAW_SPECTRUM_STATUS  = 0xFF,
};

#define ADC_INPUT_0_PIN 2 ///< default ADC input pin
#define ADC_INPUT_1_PIN 3 ///< default ADC input pin
#define ADC_INPUT_2_PIN 4 ///< default ADC input pin
#define ADC_INPUT_3_PIN 5 ///< default ADC input pin

#define PWM_0_PIN 4 ///< default PWM output pin
#define PWM_1_PIN 5 ///< default PWM output pin
#define PWM_2_PIN 6 ///< default PWM output pin
#define PWM_3_PIN 7 ///< default PWM output pin

#ifndef INPUT_PULLDOWN
#    define INPUT_PULLDOWN \
        0x03 ///< for compatibility with platforms that do not already define
             ///< INPUT_PULLDOWN
#endif

/*=========================================================================*/
// clang-format off
#define SEESAW_HW_ID_CODE_SAMD09 0x55 ///< seesaw HW ID code for SAMD09
#define SEESAW_HW_ID_CODE_TINY806 0x84 ///< seesaw HW ID code for ATtiny806
#define SEESAW_HW_ID_CODE_TINY807 0x85 ///< seesaw HW ID code for ATtiny807
#define SEESAW_HW_ID_CODE_TINY816 0x86 ///< seesaw HW ID code for ATtiny816
#define SEESAW_HW_ID_CODE_TINY817 0x87 ///< seesaw HW ID code for ATtiny817
#define SEESAW_HW_ID_CODE_TINY1616 0x88 ///< seesaw HW ID code for ATtiny1616
#define SEESAW_HW_ID_CODE_TINY1617 0x89 ///< seesaw HW ID code for ATtiny1617
// clang-format on

bool seesaw_begin(uint8_t device_address, bool reset) {
    i2c_init();

    bool found = seesaw_detected(device_address);
    dprintf("Device 0x%04x %sfound\n", device_address, found ? "" : "not ");
    return found;

    // for (int retries = 0; retries < 10; retries++) {
    //     if (_i2c_dev->begin()) {
    //         found = true;
    //         break;
    //     }
    //     delay(10);
    // }

    // if (!found) {
    //     return false;
    // }

    // if (reset) {
    //     found = false;
    //     SWReset();
    //     for (int retries = 0; retries < 10; retries++) {
    //         if (_i2c_dev->detected()) {
    //             found = true;
    //             break;
    //         }
    //         delay(10);
    //     }
    // }

    // if (!found) {
    //     return false;
    // }

    // found = false;
    // for (int retries = 0; !found && retries < 10; retries++) {
    //     uint8_t c = 0;

    //     seesaw_read(SEESAW_STATUS_BASE, SEESAW_STATUS_HW_ID, &c, 1);
    //     if ((c == SEESAW_HW_ID_CODE_SAMD09) || (c == SEESAW_HW_ID_CODE_TINY817) || (c == SEESAW_HW_ID_CODE_TINY807) || (c == SEESAW_HW_ID_CODE_TINY816) || (c == SEESAW_HW_ID_CODE_TINY806) || (c == SEESAW_HW_ID_CODE_TINY1616) || (c == SEESAW_HW_ID_CODE_TINY1617)) {
    //         found         = true;
    //         _hardwaretype = c;
    //     }

    //     delay(10);
    // }

}

bool seesaw_detected(uint8_t device_address) {
    return seesaw_write(device_address, 0, NULL, 0);

}

//  ****************************************************************************
//  *  @brief      Read the current status of a GPIO pin
//  *
//  *  @param      pin the pin number. On the SAMD09 breakout, this corresponds to
//  *the number on the silkscreen.
//  *
//  *  @return     the status of the pin. HIGH or LOW (1 or 0).
//  ****************************************************************************
bool seesaw_digital_read(uint8_t pin) {
  if (pin >= 32) {
    return seesaw_digital_read_bulk_b((1ul << (pin - 32))) != 0;
  } else {
    return seesaw_digital_read_bulk((1ul << pin)) != 0;
  }
}

//  ****************************************************************************
//  *  @brief      read the status of multiple pins on port A.
//  *
//  *  @param      pins a bitmask of the pins to write. On the SAMD09 breakout,
//  *this corresponds to the number on the silkscreen. For example, passing 0b0110
//  *will return the values of pins 2 and 3.
//  *
//  *  @return     the status of the passed pins. If 0b0110 was passed and pin 2 is
//  *high and pin 3 is low, 0b0010 (decimal number 2) will be returned.
//  ****************************************************************************
uint32_t seesaw_digital_read_bulk(uint32_t pins) {
  uint8_t buf[4];
  seesaw_read(SEESAW_GPIO_BASE, SEESAW_GPIO_BULK, buf, 4);
  uint32_t ret = ((uint32_t)buf[0] << 24) | ((uint32_t)buf[1] << 16) |
                 ((uint32_t)buf[2] << 8) | (uint32_t)buf[3];
  return ret & pins;
}

//  **************************************************************************
//  *  @brief      read the status of multiple pins on port B.
//  *
//  *  @param      pins a bitmask of the pins to write.
//  *
//  *  @return     the status of the passed pins. If 0b0110 was passed and pin 2 is
//  *high and pin 3 is low, 0b0010 (decimal number 2) will be returned.
//  ***************************************************************************
uint32_t seesaw_digital_read_bulk_b(uint32_t pins) {
  uint8_t buf[8];
  seesaw_read(SEESAW_GPIO_BASE, SEESAW_GPIO_BULK, buf, 8);
  uint32_t ret = ((uint32_t)buf[4] << 24) | ((uint32_t)buf[5] << 16) |
                 ((uint32_t)buf[6] << 8) | (uint32_t)buf[7];
  return ret & pins;
}

// //  ***************************************************************************
// //  *  @brief      set the mode of multiple GPIO pins at once.
// //  *
// //  *  @param      pins a bitmask of the pins to write. On the SAMD09 breakout,
// //  *this corresponds to the number on the silkscreen. For example, passing 0b0110
// //  *will set the mode of pins 2 and 3.
// //  *	@param		mode the mode to set the pins to. One of INPUT, OUTPUT,
// //  *or INPUT_PULLUP.
// //  ****************************************************************************
// void pin_mode_bulk(uint32_t pins, uint8_t mode) {
//   uint8_t cmd[] = {(uint8_t)(pins >> 24), (uint8_t)(pins >> 16),
//                    (uint8_t)(pins >> 8), (uint8_t)pins};
//   switch (mode) {
//   case OUTPUT:
//     seesaw_write(SEESAW_GPIO_BASE, SEESAW_GPIO_DIRSET_BULK, cmd, 4);
//     break;
//   case INPUT:
//     seesaw_write(SEESAW_GPIO_BASE, SEESAW_GPIO_DIRCLR_BULK, cmd, 4);
//     break;
//   case INPUT_PULLUP:
//     seesaw_write(SEESAW_GPIO_BASE, SEESAW_GPIO_DIRCLR_BULK, cmd, 4);
//     seesaw_write(SEESAW_GPIO_BASE, SEESAW_GPIO_PULLENSET, cmd, 4);
//     seesaw_write(SEESAW_GPIO_BASE, SEESAW_GPIO_BULK_SET, cmd, 4);
//     break;
//   case INPUT_PULLDOWN:
//     seesaw_write(SEESAW_GPIO_BASE, SEESAW_GPIO_DIRCLR_BULK, cmd, 4);
//     seesaw_write(SEESAW_GPIO_BASE, SEESAW_GPIO_PULLENSET, cmd, 4);
//     seesaw_write(SEESAW_GPIO_BASE, SEESAW_GPIO_BULK_CLR, cmd, 4);
//     break;
//   }
// }

//  *****************************************************************************************
//  *  @brief      Read a specified number of bytes into a buffer from the seesaw.
//  *
//  *  @param      regHigh the module address register (ex. SEESAW_STATUS_BASE)
//  *	@param		regLow the function address register (ex.
//  *SEESAW_STATUS_VERSION)
//  *	@param		buf the buffer to read the bytes into
//  *	@param		num the number of bytes to read.
//  *	@param		delay an optional delay in between setting the read
//  *register and reading out the data. This is required for some seesaw functions
//  *(ex. reading ADC data)
//  *  @returns    True on I2C read success
//  *****************************************************************************************
bool seesaw_read(uint8_t device_address, uint8_t address, uint8_t *buf, uint8_t num) {
    uint8_t pos = 0;

    // on arduino we need to read in 32 byte chunks
    while (pos < num) {
        uint8_t read_now = MIN(32, num - pos);

        if (!seesaw_write(device_address, address, NULL, 0)) {
            return false;
        }

        if (!seesaw_read(device_address, address, buf + pos, read_now)) {
            return false;
        }
        pos += read_now;
    }
    return true;
}

//  *****************************************************************************************
//  *  @brief   Write a specified number of bytes to the seesaw from the passed *buffer.
//  *
//  *  @param   regHigh the module address register (ex. SEESAW_GPIO_BASE)
//  *  @param	regLow the function address register (ex. SEESAW_GPIO_BULK_SET)
//  *  @param	buf the buffer the the bytes from
//  *  @param	num the number of bytes to write.
//  *  @returns    True on I2C write success
//  *****************************************************************************************
bool seesaw_write(uint8_t device_address, uint8_t address, uint8_t *buf, uint8_t num) {
    return seesaw_write_timeout(device_address, address, buf, num, SEESAW_DEFAULT_WRITE_TIMEOUT);
}

bool seesaw_write_timeout(uint8_t device_address, uint8_t address, uint8_t *buf, uint8_t num, uint16_t timeout) {

// void RAP_Write(uint8_t device_address, uint8_t address, uint8_t data) {
//     uint8_t cmdByte = WRITE_MASK | address; // Form the WRITE command byte

//     if (touchpad_init) {
//         if (i2c_write_register(device_address << 1, cmdByte, &data, sizeof(data), CIRQUE_PINNACLE_TIMEOUT) != I2C_STATUS_SUCCESS) {
//             pd_dprintf("error cirque_pinnacle i2c_write_register\n");
//             touchpad_init = false;
//         }
//     }
// }
    return i2c_write_register(device_address << 1, address, buf, num, timeout) == I2C_STATUS_SUCCESS;
}
