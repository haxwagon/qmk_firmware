ANALOG_DRIVER_REQUIRED = yes
BOOTLOADER = rp2040
COMBO_ENABLE = yes
CONSOLE_ENABLE = yes
DYNAMIC_MACRO_ENABLE = yes
ENCODER_ENABLE = no
JOYSTICK_DRIVER = analog
JOYSTICK_ENABLE = yes
KEY_OVERRIDE_ENABLE = yes
OLED_DRIVER = ssd1306
OLED_ENABLE = yes
OLED_TRANSPORT = i2c
POINTING_DEVICE_DRIVER = cirque_pinnacle_i2c
POINTING_DEVICE_ENABLE = yes
TAP_DANCE_ENABLE = yes
TRI_LAYER_ENABLE = yes

SRC +=  display.c \
		pointing.c \
		../../../cirque_pinnacles.c \
		../../../gamepad_qt.c \
		../../../joysticks.c \
		../../../pointing_dpi.c \
		../../../tap_dance_quad.c
