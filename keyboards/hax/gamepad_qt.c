// #include "Adafruit_seesaw.h"
#include "gamepad_qt.h"
#include "i2c_master.h"

#define BUTTON_X         6
#define BUTTON_Y         2
#define BUTTON_A         5
#define BUTTON_B         1
#define BUTTON_SELECT    0
#define BUTTON_START    16
uint32_t button_mask = (1UL << BUTTON_X) | (1UL << BUTTON_Y) | (1UL << BUTTON_START) |
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
