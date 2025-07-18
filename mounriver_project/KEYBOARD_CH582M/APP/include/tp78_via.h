/********************************** (C) COPYRIGHT *******************************
 * File Name          : tp78_via.h
 * Author             : ChnMasterOG
 * Version            : V1.0
 * Date               : 2023/4/28
 * Description        : TP78 VIA support
 * Copyright (c) 2023 ChnMasterOG
 * SPDX-License-Identifier: GNU GENERAL PUBLIC LICENSE v2.0 or later
 *******************************************************************************/

#ifndef TP78_VIA_H
#define TP78_VIA_H

#include "CH58x_common.h"

#define VIA_FIRMWARE_VERSION      0x00000000  // 4 bytes
#define VIA_PROTOCOL_VERSION      0x000C  // 2 bytes

typedef enum {
  VIA_ID_GET_PROTOCOL_VERSION = 0x01, // always 0x01
  VIA_ID_GET_KEYBOARD_VALUE = 0x02,
  VIA_ID_SET_KEYBOARD_VALUE = 0x03,
  VIA_ID_DYNAMIC_KEYMAP_GET_KEYCODE = 0x04,
  VIA_ID_DYNAMIC_KEYMAP_SET_KEYCODE = 0x05,
  VIA_ID_DYNAMIC_KEYMAP_RESET = 0x06,
  VIA_ID_CUSTOM_SET_VALUE = 0x07,
  VIA_ID_CUSTOM_GET_VALUE = 0x08,
  VIA_ID_CUSTOM_SAVE = 0x09,
  VIA_ID_EEPROM_RESET = 0x0A,
  VIA_ID_BOOTLOADER_JUMP = 0x0B,
  VIA_ID_DYNAMIC_KEYMAP_MACRO_GET_COUNT = 0x0C,
  VIA_ID_DYNAMIC_KEYMAP_MACRO_GET_BUFFER_SIZE = 0x0D,
  VIA_ID_DYNAMIC_KEYMAP_MACRO_GET_BUFFER = 0x0E,
  VIA_ID_DYNAMIC_KEYMAP_MACRO_SET_BUFFER = 0x0F,
  VIA_ID_DYNAMIC_KEYMAP_MACRO_RESET = 0x10,
  VIA_ID_DYNAMIC_KEYMAP_GET_LAYER_COUNT = 0x11,
  VIA_ID_DYNAMIC_KEYMAP_GET_BUFFER = 0x12,
  VIA_ID_DYNAMIC_KEYMAP_SET_BUFFER = 0x13,
  VIA_ID_DYNAMIC_KEYMAP_GET_ENCODER = 0x14,
  VIA_ID_DYNAMIC_KEYMAP_SET_ENCODER = 0x15,
  VIA_ID_DYNAMIC_KEYMAP_MAGNET_GET_RANGE = 0x20,
  VIA_ID_DYNAMIC_KEYMAP_MAGNET_SET_RANGE = 0x21,
  VIA_ID_DYNAMIC_KEYMAP_MAGNET_GET_PRESS_BUFFER = 0x22,
  VIA_ID_DYNAMIC_KEYMAP_MAGNET_SET_PRESS_BUFFER = 0x23,
  VIA_ID_DYNAMIC_KEYMAP_MAGNET_GET_RELEASE_BUFFER = 0x24,
  VIA_ID_DYNAMIC_KEYMAP_MAGNET_SET_RELEASE_BUFFER = 0x25,
  VIA_ID_DYNAMIC_KEYMAP_MAGNET_GET_PRESS_VALUE = 0x26,
  VIA_ID_DYNAMIC_KEYMAP_MAGNET_SET_PRESS_VALUE = 0x27,
  VIA_ID_DYNAMIC_KEYMAP_MAGNET_GET_RELEASE_VALUE = 0x28,
  VIA_ID_DYNAMIC_KEYMAP_MAGNET_SET_RELEASE_VALUE = 0x29,
  VIA_ID_DYNAMIC_KEYMAP_MAGNET_GET_DIR = 0x2A,
  VIA_ID_DYNAMIC_KEYMAP_MAGNET_SET_DIR = 0x2B,
  VIA_ID_UNHANDLED = 0xFF,
}via_command_id;

typedef enum {
  VIA_ID_UPTIME = 0x01,
  VIA_ID_LAYOUT_OPTIONS = 0x02,
  VIA_ID_SWITCH_MATRIX_STATE = 0x03,
  VIA_ID_FIRMWARE_VERSION = 0x04,
  VIA_ID_DEVICE_INDICATION = 0x05,
}via_keyboard_value_id;

typedef enum {
  VIA_ID_CUSTOM_CHANNEL = 0x00,
  VIA_ID_BACKLIGHT_CHANNEL = 0x01,
  VIA_ID_RGBLIGHT_CHANNEL = 0x02,
  VIA_ID_RGB_MATRIX_CHANNEL = 0x03,
  VIA_ID_AUDIO_CHANNEL = 0x04,
}via_channel_id;

typedef enum {
  VIA_ID_BACKLIGHT_BRIGHTNESS = 0x01,
  VIA_ID_BACKLIGHT_EFFECT = 0x02,
}via_backlight_value;

typedef enum {
  VIA_ID_RGBLIGHT_BRIGHTNESS = 0x01,
  VIA_ID_RGBLIGHT_EFFECT = 0x02,
  VIA_ID_RGBLIGHT_EFFECT_SPEED = 0x03,
  VIA_ID_RGBLIGHT_COLOR = 0x04,
}via_rgblight_value;

typedef enum {
  VIA_ID_RGB_MATRIX_BRIGHTNESS = 0x01,
  VIA_ID_RGB_MATRIX_EFFECT = 0x02,
  VIA_ID_RGB_MATRIX_EFFECT_SPEED = 0x03,
  VIA_ID_RGB_MATRIX_COLOR = 0x04,
}via_rgb_matrix_value;

typedef enum {
  VIA_ID_AUDIO_ENABLE = 0x01,
  VIA_ID_AUDIO_CLICKY_ENABLE = 0x02,
}via_audio_value;

void via_data_processing(uint8_t *data, uint8_t len);

#endif
