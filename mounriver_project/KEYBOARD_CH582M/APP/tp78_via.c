/********************************** (C) COPYRIGHT *******************************
 * File Name          : tp78_via.c
 * Author             : ChnMasterOG
 * Version            : V1.0
 * Date               : 2023/4/28
 * Description        : TP78的VIA支持库
 * Copyright (c) 2023 ChnMasterOG
 * SPDX-License-Identifier: GNU GENERAL PUBLIC LICENSE v2.0 or later
 *******************************************************************************/

#include "HAL.h"
#include "tp78_via.h"

/*******************************************************************************
 * Function Name  : via_custom_value_command
 * Description    : via客制化命令
 * Input          : *data - raw data, len - 数据长度
 * Return         : None
 *******************************************************************************/
static void via_custom_value_command(uint8_t *data, uint8_t len) {
  uint8_t *command_id = &data[0];
  uint8_t *command_channel = &data[1];
  uint8_t *command_value = &data[2];

  switch (*command_channel) {
    case 0: { // wireless
      switch (*command_value) {
        case 1: { // id_BLE_device
          uint16_t DeviceID;
          if (*command_id == VIA_ID_CUSTOM_SET_VALUE) {
            DATAFLASH_Write_DeviceID(data[3]);
          } else {
            HAL_Fs_Read_keyboard_cfg(FS_LINE_BLE_DEVICE, 1, &DeviceID);
            data[3] = (uint8_t)DeviceID;
          }
          break;
        }
        case 2: { // id_RF_mode
          uint16_t rf_ready;
          if (*command_id == VIA_ID_CUSTOM_SET_VALUE) {
            DATAFLASH_Write_RForBLE(data[3]);
          } else {
            HAL_Fs_Read_keyboard_cfg(FS_LINE_RF_READY, 1, &rf_ready);
            data[3] = (uint8_t)rf_ready;
          }
          break;
        }
        case 3: { // id_RF_freq
          uint16_t freq_level;
          if (*command_id == VIA_ID_CUSTOM_SET_VALUE) {
            DATAFLASH_Write_RFfreqlevel(data[3]);
          } else {
            HAL_Fs_Read_keyboard_cfg(FS_LINE_RF_FREQ_LEVEL, 1, &freq_level);
            data[3] = (uint8_t)freq_level;
          }
          break;
        }
        default: {
          break;
        }
      }
      break;
    }
    case 1: { // RGBLight
      switch (*command_value) {
        case 1: { // id_BLE_device
          uint16_t ledstyle;
          if (*command_id == VIA_ID_CUSTOM_SET_VALUE) {
            ledstyle = data[3];
            HAL_Fs_Write_keyboard_cfg(FS_LINE_LED_STYLE, 1, &ledstyle);
          } else {
            HAL_Fs_Read_keyboard_cfg(FS_LINE_LED_STYLE, 1, &ledstyle);
            data[3] = (uint8_t)ledstyle;
          }
          break;
        }
        case 2: { // id_LED_Brightness
          uint16_t brightness;
          if (*command_id == VIA_ID_CUSTOM_SET_VALUE) {
            brightness = data[3];
            HAL_Fs_Write_keyboard_cfg(FS_LINE_LED_BRIGHTNESS, 1, &brightness);
          } else {
            HAL_Fs_Read_keyboard_cfg(FS_LINE_LED_BRIGHTNESS, 1, &brightness);
            data[3] = (uint8_t)brightness;
          }
          break;
        }
        default: {
          break;
        }
      }
      break;
    }
    default: {
      break;
    }
  }
}

/*******************************************************************************
 * Function Name  : via_data_processing
 * Description    : 处理USB接收的via raw data
 * Input          : *data - raw data, len - 数据长度
 * Return         : None
 *******************************************************************************/
void via_data_processing(uint8_t *data, uint8_t len)
{
  uint8_t *command_id = &data[0];
  uint8_t *command_data = &data[1];

  /* attention: data is transfered with big end format */
  switch (*command_id) {
    case (uint8_t)VIA_ID_GET_PROTOCOL_VERSION:
        command_data[0] = VIA_PROTOCOL_VERSION >> 8;
        command_data[1] = VIA_PROTOCOL_VERSION & 0xFF;
        break;
    case (uint8_t)VIA_ID_GET_KEYBOARD_VALUE:
        switch (command_data[0]) {
          case (uint8_t)VIA_ID_UPTIME: {
              uint32_t sec2 = R16_RTC_CNT_2S;
              uint16_t t32k = R16_RTC_CNT_32K;
              *(uint32_t*)command_data = (sec2 << 1) + ((t32k < 0x8000) ? 0 : 1);
              break;
          }
          case (uint8_t)VIA_ID_LAYOUT_OPTIONS: {
              /* TP78 unsupport this command */
              break;
          }
          case (uint8_t)VIA_ID_SWITCH_MATRIX_STATE: {
              /* report KeyMatrix */
              uint8_t i, j, temp, k = 2;
              for (i = 0; i < ROW_SIZE; i++) {
                command_data[k] = 0;
                for (j = COL_SIZE - 1; j != 0xFF; j--) {
                  temp = j % 8;
                  if (KeyMatrix[j][i]) command_data[k] |= 1 << temp;  // is pressed
                  if (temp == 0) k++;
                }
              }
              break;
          }
          case (uint8_t)VIA_ID_FIRMWARE_VERSION: {
              uint32_t value = VIA_FIRMWARE_VERSION;
              command_data[1] = (value >> 24) & 0xFF;
              command_data[2] = (value >> 16) & 0xFF;
              command_data[3] = (value >> 8) & 0xFF;
              command_data[4] = value & 0xFF;
              break;
          }
          default: {
              *command_id = (uint32_t)VIA_ID_UNHANDLED;
              break;
          }
        }
        break;
    case (uint8_t)VIA_ID_SET_KEYBOARD_VALUE:
        switch (command_data[0]) {
          case (uint8_t)VIA_ID_LAYOUT_OPTIONS: {
              // write keyboard layout
              break;
          }
          case (uint8_t)VIA_ID_DEVICE_INDICATION: {
              /* OLED indication */
              OLED_UI_add_SHOWINFO_task("VIAconnect");
              OLED_UI_add_CANCELINFO_delay_task(3000);
              break;
          }
          default: {
              *command_id = VIA_ID_UNHANDLED;
              break;
          }
        }
        break;
    case (uint8_t)VIA_ID_DYNAMIC_KEYMAP_GET_KEYCODE: {
        /********* format *********
          command_data[0] = layer
          command_data[1] = row
          command_data[2] = col
        **************************/
        command_data[3] = 0;
        if (command_data[1] > ROW_SIZE || command_data[2] > COL_SIZE) {
          command_data[4] = 0;
          break;
        }
        if (command_data[0] == 0) // layer 0
          command_data[4] = CustomKey[command_data[1]][command_data[2]];
        else  // layer 1
          command_data[4] = Extra_CustomKey[command_data[1]][command_data[2]];
        break;
    }
    case (uint8_t)VIA_ID_DYNAMIC_KEYMAP_SET_KEYCODE: {
        /********* format *********
          command_data[0] = layer
          command_data[1] = row
          command_data[2] = col
        **************************/
        if (command_data[1] > ROW_SIZE || command_data[2] > COL_SIZE) break;
        if (command_data[0] == 0) { // layer 0
          CustomKey[command_data[1]][command_data[2]] = command_data[4];
          HAL_Fs_Write_keyboard_mat("0:keyboard_mat.txt", (const uint8_t*)CustomKey);
        } else {  // layer 1
          Extra_CustomKey[command_data[1]][command_data[2]] = command_data[4];
          HAL_Fs_Write_keyboard_mat("0:keyboard_ext_mat.txt", (const uint8_t*)Extra_CustomKey);
        }
        break;
    }
    case (uint8_t)VIA_ID_DYNAMIC_KEYMAP_RESET: {
        memcpy(CustomKey, KeyArrary, COL_SIZE*ROW_SIZE);
        memcpy(Extra_CustomKey, Extra_KeyArrary, COL_SIZE*ROW_SIZE);
        DATAFLASH_Write_KeyArray();
        break;
    }
    case (uint8_t)VIA_ID_CUSTOM_SET_VALUE:
    case (uint8_t)VIA_ID_CUSTOM_GET_VALUE:
    case (uint8_t)VIA_ID_CUSTOM_SAVE: {
        via_custom_value_command(data, len);
        break;
    }
    case (uint8_t)VIA_ID_DYNAMIC_KEYMAP_MACRO_GET_COUNT: {
        command_data[0] = 0;
        break;
    }
    case (uint8_t)VIA_ID_DYNAMIC_KEYMAP_MACRO_GET_BUFFER_SIZE: {
        command_data[0] = command_data[1] = 0;
        break;
    }
    case (uint8_t)VIA_ID_DYNAMIC_KEYMAP_MACRO_GET_BUFFER: {
        /************* format *************
          command_data[0] = offset HSB
          command_data[1] = offset LSB
          command_data[2] = size
        **********************************/
        /* TP78 unsupport this command */
        command_data[0] = command_data[1] = command_data[2] = 0;
        break;
    }
    case (uint8_t)VIA_ID_DYNAMIC_KEYMAP_MACRO_SET_BUFFER: {
        /************* format *************
          command_data[0] = offset HSB
          command_data[1] = offset LSB
          command_data[2] = size
        **********************************/
        /* TP78 unsupport this command */
        break;
    }
    case (uint8_t)VIA_ID_DYNAMIC_KEYMAP_MACRO_RESET: {
        /* TP78 unsupport this command */
        break;
    }
    case (uint8_t)VIA_ID_DYNAMIC_KEYMAP_GET_LAYER_COUNT: {
        command_data[0] = 2;  // layer 0 - main, layer 1 - extra
        break;
    }
    case (uint8_t)VIA_ID_DYNAMIC_KEYMAP_GET_BUFFER: {
        /* via is used uint16 to record keymap */
        uint8_t *keyarr_ptr;
        uint16_t offset = (command_data[0] << 8) | command_data[1];
        uint16_t size = command_data[2];
        uint16_t i, j = 3, k = 0;
        offset >>= 1; // uint16 to uint8
        size >>= 1; // uint16 to uint8
        if (offset < (COL_SIZE + 1) * ROW_SIZE) { // layer 0
          keyarr_ptr = (uint8_t*)CustomKey;
        } else {  // layer 1
          keyarr_ptr = (uint8_t*)Extra_CustomKey;
          offset -= (COL_SIZE + 1) * ROW_SIZE;
        }
        keyarr_ptr += offset - offset / (COL_SIZE + 1); // 一行结束减去末尾空白键
        for (i = offset; i < offset + size; i++, j+=2) {
          if (i >= (COL_SIZE + 1) * ROW_SIZE) {
            command_data[j] = 0;
            command_data[j + 1] = 0;
          } else {
            command_data[j] = 0;  // MSB set to zero(USB为大端传输)
            if (i % (COL_SIZE + 1) == COL_SIZE) command_data[j + 1] = 0;  // 一行末尾保留空白
            else command_data[j + 1] = keyarr_ptr[k++];
          }
        }
        break;
    }
    case (uint8_t)VIA_ID_DYNAMIC_KEYMAP_SET_BUFFER: {
        /* via is used uint16 to record keymap */
        uint8_t *keyarr_ptr;
        uint16_t offset = (command_data[0] << 8) | command_data[1];
        uint16_t size = command_data[2];
        uint16_t i, j = 3, k = 0;
        offset >>= 1; // uint16 to uint8
        size >>= 1; // uint16 to uint8
        if (offset < (COL_SIZE + 1) * ROW_SIZE) { // layer 0
          keyarr_ptr = (uint8_t*)CustomKey;
        } else {  // layer 1
          keyarr_ptr = (uint8_t*)Extra_CustomKey;
          offset -= (COL_SIZE + 1) * ROW_SIZE;
        }
        keyarr_ptr += offset - offset / (COL_SIZE + 1); // 一行结束减去末尾空白键
        for (i = offset; i < offset + size; i++, j+=2) {
          if (i >= (COL_SIZE + 1) * ROW_SIZE) break;
          else if (i % (COL_SIZE + 1) != COL_SIZE) keyarr_ptr[k++] = command_data[j + 1];
        }
        DATAFLASH_Write_KeyArray(); // save configurations
        break;
    }
    case (uint8_t)VIA_ID_DYNAMIC_KEYMAP_GET_ENCODER: {
        break;
    }
    case (uint8_t)VIA_ID_DYNAMIC_KEYMAP_SET_ENCODER: {
        break;
    }
    default: {
        *command_id = VIA_ID_UNHANDLED;
        break;
    }
  }
}

