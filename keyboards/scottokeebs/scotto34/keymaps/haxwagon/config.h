/*
Copyright 2023 Joe Scotto

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

// Define options
#define ENCODER_A_PINS { GP6 }
#define ENCODER_B_PINS { GP7 }
#define JOYSTICK_AXIS_COUNT 6
#define JOYSTICK_AXIS_RESOLUTION 10
#define JOYSTICK_BUTTON_COUNT 12
#define JOYSTICK_HAS_HAT
#define OLED_BRIGHTNESS 128
#define OLED_DISPLAY_128X64
#define OLED_DISPLAY_ADDRESS 0x3C
#define PERMISSIVE_HOLD
#define TAPPING_TERM 100
#define TAPPING_TERM_PER_KEY
#define TAPPING_TOGGLE 2
#define TRI_LAYER_LOWER_LAYER 1
#define TRI_LAYER_UPPER_LAYER 2
#define TRI_LAYER_ADJUST_LAYER 3
