ANALOG_DRIVER_REQUIRED = no
BOOTLOADER = rp2040
I2C_DRIVER_REQUIRED = yes
JOYSTICK_DRIVER = digital
JOYSTICK_ENABLE = yes
OLED_DRIVER = ssd1306
OLED_TRANSPORT = i2c
POINTING_DEVICE_DRIVER = custom
SPI_DRIVER_REQUIRED = yes

SRC +=  \
		../cirque_pinnacles.c \
		../joysticks.c \
        ../oled.c \
		drivers/sensors/cirque_pinnacle.c \
		drivers/sensors/cirque_pinnacle_spi.c
