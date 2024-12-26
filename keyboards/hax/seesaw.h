#ifndef SEESAW_DEFAULT_WRITE_TIMEOUT
#    define SEESAW_DEFAULT_WRITE_TIMEOUT 30
#endif

bool seesaw_begin(uint8_t device_address, bool reset);
bool seesaw_detected(uint8_t device_address);
bool seesaw_digital_read(uint8_t pin);
uint32_t seesaw_digital_read_bulk(uint32_t pins);
uint32_t seesaw_digital_read_bulk_b(uint32_t pins);
bool seesaw_read(uint8_t regHigh, uint8_t regLow, uint8_t *buf, uint8_t num);
bool seesaw_write(uint8_t regHigh, uint8_t regLow, uint8_t *buf, uint8_t num);
bool seesaw_write_timeout(uint8_t regHigh, uint8_t regLow, uint8_t *buf, uint8_t num, uint16_t timeout);
