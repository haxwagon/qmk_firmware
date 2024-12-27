#include QMK_KEYBOARD_H
#include "gamepad_qts.h"
#include "seesaw.h"

#define BUTTON_X         6
#define BUTTON_Y         2
#define BUTTON_A         5
#define BUTTON_B         1
#define BUTTON_SELECT    0
#define BUTTON_START    16
uint32_t buttons_mask = (1UL << BUTTON_X) | (1UL << BUTTON_Y) | (1UL << BUTTON_START) |
                        (1UL << BUTTON_A) | (1UL << BUTTON_B) | (1UL << BUTTON_SELECT);

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



/** GPIO module function address registers
 */

// /*!
//  ***************************************************************************
//  *  @brief      set the mode of multiple GPIO pins at once.
//  *
//  *  @param      pins a bitmask of the pins to write. On the SAMD09 breakout,
//  *this corresponds to the number on the silkscreen. For example, passing 0b0110
//  *will set the mode of pins 2 and 3.
//  *	@param		mode the mode to set the pins to. One of INPUT, OUTPUT,
//  *or INPUT_PULLUP.
//  ************************************************************************/
// void Adafruit_seesaw::pinModeBulk(uint32_t pins, uint8_t mode) {
//   uint8_t cmd[] = {(uint8_t)(pins >> 24), (uint8_t)(pins >> 16),
//                    (uint8_t)(pins >> 8), (uint8_t)pins};
//   switch (mode) {
//   case OUTPUT:
//     this->write(SEESAW_GPIO_BASE, SEESAW_GPIO_DIRSET_BULK, cmd, 4);
//     break;
//   case INPUT:
//     this->write(SEESAW_GPIO_BASE, SEESAW_GPIO_DIRCLR_BULK, cmd, 4);
//     break;
//   case INPUT_PULLUP:
//     this->write(SEESAW_GPIO_BASE, SEESAW_GPIO_DIRCLR_BULK, cmd, 4);
//     this->write(SEESAW_GPIO_BASE, SEESAW_GPIO_PULLENSET, cmd, 4);
//     this->write(SEESAW_GPIO_BASE, SEESAW_GPIO_BULK_SET, cmd, 4);
//     break;
//   case INPUT_PULLDOWN:
//     this->write(SEESAW_GPIO_BASE, SEESAW_GPIO_DIRCLR_BULK, cmd, 4);
//     this->write(SEESAW_GPIO_BASE, SEESAW_GPIO_PULLENSET, cmd, 4);
//     this->write(SEESAW_GPIO_BASE, SEESAW_GPIO_BULK_CLR, cmd, 4);
//     break;
//   }
// }


// int last_x = 0, last_y = 0;

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

//     uint32_t buttons = ss.digitalReadBulk(button_mask);

//     //Serial.println(buttons, BIN);

//     if (! (buttons & (1UL << BUTTON_A))) {
//       Serial.println("Button A pressed");
//     }
//     if (! (buttons & (1UL << BUTTON_B))) {
//       Serial.println("Button B pressed");
//     }
//     if (! (buttons & (1UL << BUTTON_Y))) {
//       Serial.println("Button Y pressed");
//     }
//     if (! (buttons & (1UL << BUTTON_X))) {
//       Serial.println("Button X pressed");
//     }
//     if (! (buttons & (1UL << BUTTON_SELECT))) {
//       Serial.println("Button SELECT pressed");
//     }
//     if (! (buttons & (1UL << BUTTON_START))) {
//       Serial.println("Button START pressed");
//     }
// }


// uint16_t Adafruit_seesaw::analogRead(uint8_t pin) {
//   uint8_t buf[2];
//   uint8_t p = 0;

//   if (_hardwaretype == SEESAW_HW_ID_CODE_SAMD09) {
//     switch (pin) {
//     case ADC_INPUT_0_PIN:
//       p = 0;
//       break;
//     case ADC_INPUT_1_PIN:
//       p = 1;
//       break;
//     case ADC_INPUT_2_PIN:
//       p = 2;
//       break;
//     case ADC_INPUT_3_PIN:
//       p = 3;
//       break;
//     default:
//       return 0;
//     }
//   } else if ((_hardwaretype == SEESAW_HW_ID_CODE_TINY807) ||
//              (_hardwaretype == SEESAW_HW_ID_CODE_TINY817) ||
//              (_hardwaretype == SEESAW_HW_ID_CODE_TINY816) ||
//              (_hardwaretype == SEESAW_HW_ID_CODE_TINY806) ||
//              (_hardwaretype == SEESAW_HW_ID_CODE_TINY1616) ||
//              (_hardwaretype == SEESAW_HW_ID_CODE_TINY1617)) {
//     p = pin;
//   } else {
//     return 0;
//   }

//   this->read(SEESAW_ADC_BASE, SEESAW_ADC_CHANNEL_OFFSET + p, buf, 2, 500);
//   uint16_t ret = ((uint16_t)buf[0] << 8) | buf[1];
//   delay(1);
//   return ret;
// }

/*!
 ****************************************************************************
 *  @brief      read the status of multiple pins on port A.
 *
 *  @param      pins a bitmask of the pins to write. On the SAMD09 breakout,
 *this corresponds to the number on the silkscreen. For example, passing 0b0110
 *will return the values of pins 2 and 3.
 *
 *  @return     the status of the passed pins. If 0b0110 was passed and pin 2 is
 *high and pin 3 is low, 0b0010 (decimal number 2) will be returned.
 *******************************************************************/
// uint32_t Adafruit_seesaw::digitalReadBulk(uint32_t pins) {
//   uint8_t buf[4];
//   this->read(SEESAW_GPIO_BASE, SEESAW_GPIO_BULK, buf, 4);
//   uint32_t ret = ((uint32_t)buf[0] << 24) | ((uint32_t)buf[1] << 16) |
//                  ((uint32_t)buf[2] << 8) | (uint32_t)buf[3];
//   return ret & pins;
// }


/*
// Masks for Cirque Register Access Protocol (RAP)
#define WRITE_MASK 0x80
#define READ_MASK 0xA0

extern bool touchpad_init;

//  RAP Functions
// Reads <count> Pinnacle registers starting at <address>
void RAP_ReadBytes(uint8_t device_address, uint8_t address, uint8_t* data, uint8_t count) {
    uint8_t cmdByte = READ_MASK | address; // Form the READ command byte
    if (touchpad_init) {
        i2c_write_register(device_address << 1, cmdByte, NULL, 0, CIRQUE_PINNACLE_TIMEOUT);
        if (i2c_read_register(device_address << 1, cmdByte, data, count, CIRQUE_PINNACLE_TIMEOUT) != I2C_STATUS_SUCCESS) {
            pd_dprintf("error cirque_pinnacle i2c_read_register\n");
            touchpad_init = false;
        }
    }
}

// Writes single-byte <data> to <address>
void RAP_Write(uint8_t device_address, uint8_t address, uint8_t data) {
    uint8_t cmdByte = WRITE_MASK | address; // Form the WRITE command byte

    if (touchpad_init) {
        if (i2c_write_register(device_address << 1, cmdByte, &data, sizeof(data), CIRQUE_PINNACLE_TIMEOUT) != I2C_STATUS_SUCCESS) {
            pd_dprintf("error cirque_pinnacle i2c_write_register\n");
            touchpad_init = false;
        }
    }
}
*/


void gamepad_qts_init(gamepad_qt_device_t* devices, uint8_t count) {
// void setup() {
//   Serial.begin(115200);

//   while(!Serial) {
//     delay(10);
//   }

//   Serial.println("Gamepad QT example!");

//   if(!ss.begin(0x50)){
//     Serial.println("ERROR! seesaw not found");
//     while(1) delay(1);
//   }
//   Serial.println("seesaw started");
//   uint32_t version = ((ss.getVersion() >> 16) & 0xFFFF);
//   if (version != 5743) {
//     Serial.print("Wrong firmware loaded? ");
//     Serial.println(version);
//     while(1) delay(10);
//   }
//   Serial.println("Found Product 5743");

//   ss.pinModeBulk(button_mask, INPUT_PULLUP);
//   ss.setGPIOInterrupts(button_mask, 1);

// #if defined(IRQ_PIN)
//   pinMode(IRQ_PIN, INPUT);
// #endif
// }
    for (uint8_t i = 0; i < count; i++) {
        printf("initializing seesaw %d\n", i);
        seesaw_begin(&(devices[i].seesaw), true);
    }
}

bool gamepad_qts_update_state(gamepad_qt_device_t* device) {
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

bool gamepad_qts_update_states(gamepad_qt_device_t* devices, uint8_t count) {
    bool updated = false;
    for (uint8_t i = 0; i < count; i++) {
        updated = gamepad_qts_update_state(&(devices[i])) || updated;
    }
    return updated;
}
