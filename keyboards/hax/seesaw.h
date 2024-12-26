#pragma once

#ifndef SEESAW_DEFAULT_WRITE_TIMEOUT
#    define SEESAW_DEFAULT_WRITE_TIMEOUT 30
#endif

typedef struct {
    uint8_t address;
    uint8_t hardware_type;
} seesaw_device_t;

uint16_t seesaw_analog_read(seesaw_device_t device, uint8_t pin);
bool seesaw_begin(seesaw_device_t* device, bool reset); // returns hardware type
bool seesaw_detected(seesaw_device_t device);
bool seesaw_digital_read(seesaw_device_t device, uint8_t pin);
uint32_t seesaw_digital_read_bulk(seesaw_device_t device, uint32_t pins);
uint32_t seesaw_digital_read_bulk_b(seesaw_device_t device, uint32_t pins);
bool seesaw_read(seesaw_device_t device, uint8_t regHigh, uint8_t regLow, uint8_t *buf, uint8_t num);
bool seesaw_reset(seesaw_device_t device);
bool seesaw_write(seesaw_device_t device, uint8_t regHigh, uint8_t regLow, uint8_t *buf, uint8_t num);
bool seesaw_write_byte(seesaw_device_t device, uint8_t regHigh, uint8_t regLow, uint8_t byte);
bool seesaw_write_timeout(seesaw_device_t device, uint8_t regHigh, uint8_t regLow, uint8_t *buf, uint8_t num, uint16_t timeout);
