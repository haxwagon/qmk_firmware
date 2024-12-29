ANALOG_DRIVER_REQUIRED = no
BOOTLOADER = rp2040
I2C_DRIVER_REQUIRED = yes
JOYSTICK_DRIVER = digital
OLED_DRIVER = ssd1306
OLED_TRANSPORT = i2c
POINTING_DEVICE_DRIVER = cirque_pinnacle_spi
SPI_DRIVER_REQUIRED = yes

SRC +=  \
		../cirque_pinnacles.c \
		../gamepad_qts.c \
		../joysticks.c \
        ../oled.c \
		../seesaw.c

