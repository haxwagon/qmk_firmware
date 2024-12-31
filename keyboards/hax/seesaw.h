#pragma once

#ifndef SEESAW_DEFAULT_I2C_TIMEOUT_MS
#define SEESAW_DEFAULT_I2C_TIMEOUT_MS 30
#endif

typedef struct {
    uint8_t address;
    int8_t flow;
    uint8_t hardware_type;
} seesaw_device_t;

uint16_t seesaw_analog_read(seesaw_device_t device, uint8_t pin);
bool seesaw_begin(seesaw_device_t* device, bool reset); // returns hardware type
bool seesaw_detected(seesaw_device_t device);
bool seesaw_digital_read(seesaw_device_t device, uint8_t pin);
uint32_t seesaw_digital_read_bulk(seesaw_device_t device, uint32_t pins);
uint32_t seesaw_digital_read_bulk_b(seesaw_device_t device, uint32_t pins);
uint32_t seesaw_get_options(seesaw_device_t device);
uint32_t seesaw_get_version(seesaw_device_t device);
void seesaw_pin_mode(seesaw_device_t device, uint8_t pin, uint8_t mode);
void seesaw_pin_mode_bulk(seesaw_device_t device, uint32_t pins, uint8_t mode);
void seesaw_pin_mode_bulk_double(seesaw_device_t device, uint32_t pinsa, uint32_t pinsb, uint8_t mode);
bool seesaw_read(seesaw_device_t device, uint8_t regHigh, uint8_t regLow, uint8_t* buf, uint8_t num);
bool seesaw_read_delay(seesaw_device_t device, uint8_t regHigh, uint8_t regLow, uint8_t* buf, uint8_t num, uint16_t delay_ms);
bool seesaw_read_delay_timeout(seesaw_device_t device, uint8_t regHigh, uint8_t regLow, uint8_t* buf, uint8_t num, uint16_t delay_ms, uint16_t timeout_ms);
bool seesaw_reset(seesaw_device_t device);
bool seesaw_write(seesaw_device_t device, uint8_t regHigh, uint8_t regLow, uint8_t* buf, uint8_t num);
bool seesaw_write_byte(seesaw_device_t device, uint8_t regHigh, uint8_t regLow, uint8_t byte);
bool seesaw_write_timeout(seesaw_device_t device, uint8_t regHigh, uint8_t regLow, uint8_t* buf, uint8_t num, uint16_t timeout_ms);
