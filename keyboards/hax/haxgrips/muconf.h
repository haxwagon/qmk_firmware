#pragma once

#include_next <muconf.h>

#undef STM32_I2C_USE_I2C1
#define STM32_I2C_USE_I2C1 TRUE

#undef STM32_SPI_USE_SPI0
#define STM32_SPI_USE_SPI0 TRUE
