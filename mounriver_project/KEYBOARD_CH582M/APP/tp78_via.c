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

#define SP_KEY_ENCODE_ST_PRESS_0      0
#define SP_KEY_ENCODE_ST_PRESS_1      1
#define SP_KEY_ENCODE_ST_PRESS_2      2
#define SP_KEY_ENCODE_ST_NORMAL       3
#define SP_KEY_ENCODE_ST_RELEASE_0    4
#define SP_KEY_ENCODE_ST_RELEASE_1    5
#define SP_KEY_ENCODE_ST_RELEASE_2    6
#define SP_KEY_ENCODE_ST_END          7

static uint8_t sp_key_index = 0;
static uint8_t sp_key_normal_key_index = 0;
static uint8_t sp_key_next_direct_keycode_flag = FALSE;
static uint8_t sp_key_encode_st = SP_KEY_ENCODE_ST_PRESS_0;

const uint8_t ascii_to_keycode[256] = {
                                       /* 0-9 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                       /* 10-19 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                       /* 20-29 */ KEY_SPACEBAR, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                       /* 30-39 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, KEY_Quotation,
                                       /* 40-49 */ 0, 0, 0, 0, KEY_Comma, KEY_Subtraction, KEY_FullStop, KEY_Slash, KEY_0, KEY_1,
                                       /* 50-59 */ KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, 0, KEY_Semicolon,
                                       /* 60-69 */ 0, KEY_Equal, 0, 0, 0, 0, 0, 0, 0, 0,
                                       /* 70-79 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                       /* 80-89 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                       /* 90-99 */ 0, KEY_LSbrackets, KEY_NonUS_WS, KEY_RSbrackets, 0, 0, KEY_GraveAccent, KEY_A, KEY_B, KEY_C,
                                       /* 100-109 */ KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J, KEY_K, KEY_L, KEY_M,
                                       /* 110-119 */ KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U, KEY_V, KEY_W,
                                       /* 120-129 */ KEY_X, KEY_Y, KEY_Z, 0, 0, 0, 0, 0, 0, 0,
                                       /* 120-129 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
const uint8_t keycode_to_ascii[256] = {
                                       /* 0-9 */ 0, 0, 0, 0, 'a', 'b', 'c', 'd', 'e', 'f',
                                       /* 10-19 */ 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
                                       /* 20-29 */ 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
                                       /* 30-39 */ '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
                                       /* 40-49 */ 0, 0, 0, 0, ' ', '-', '=', '[', ']', 0,
                                       /* 50-59 */ '\\', ';', '\'', '`', ',', '.', '/', 0, 0, 0,
                                       /* 60-69 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                       /* 70-79 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                       /* 80-89 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

/* for test mode */
static uint16_t test_value1 = 50, test_value2 = 30;

/*******************************************************************************
 * Function Name  : via_MACRO_buffer_encode
 * Description    : SP_KEY格式转换为via MACRO格式
 * Input          : *data - via MACRO格式数据
 * Return         : None
 *******************************************************************************/
static void via_MACRO_buffer_encode(uint8_t *data)
{
  uint8_t size = *data++;

  if (sp_key_index >= SP_KEY_NUMBER)
    return;

  while (size) {
    switch (sp_key_encode_st) {
      case SP_KEY_ENCODE_ST_PRESS_0:
      case SP_KEY_ENCODE_ST_RELEASE_0:
        if (sp_key_normal_key_index < 8) {
          if (SP_Key_Map[sp_key_index][0] & (1 << sp_key_normal_key_index)) {
            sp_key_encode_st = sp_key_encode_st == SP_KEY_ENCODE_ST_PRESS_0 ? SP_KEY_ENCODE_ST_PRESS_1 : SP_KEY_ENCODE_ST_RELEASE_1;
            *data = 0x1;  // SS_TAP_CODE
            data++;
            size--;
          } else {
            sp_key_normal_key_index++;
          }
        } else {
          if (keycode_to_ascii[SP_Key_Map[sp_key_index][sp_key_normal_key_index - 6]] == 0 && SP_Key_Map[sp_key_index][sp_key_normal_key_index - 6] != KEY_None) {
            sp_key_encode_st = sp_key_encode_st == SP_KEY_ENCODE_ST_PRESS_0 ? SP_KEY_ENCODE_ST_PRESS_1 : SP_KEY_ENCODE_ST_RELEASE_1;
            *data = 0x1;  // SS_TAP_CODE
            data++;
            size--;
          } else {
            sp_key_normal_key_index++;
          }
        }
        break;
      case SP_KEY_ENCODE_ST_PRESS_1:
        sp_key_encode_st = SP_KEY_ENCODE_ST_PRESS_2;
        *data = 0x2; // SS_DOWN_CODE
        data++;
        size--;
        break;
      case SP_KEY_ENCODE_ST_RELEASE_1:
        sp_key_encode_st = SP_KEY_ENCODE_ST_RELEASE_2;
        *data = 0x3; // SS_UP_CODE
        data++;
        size--;
        break;
      case SP_KEY_ENCODE_ST_PRESS_2:
      case SP_KEY_ENCODE_ST_RELEASE_2:
        sp_key_encode_st = sp_key_encode_st == SP_KEY_ENCODE_ST_PRESS_2 ? SP_KEY_ENCODE_ST_PRESS_0 : SP_KEY_ENCODE_ST_RELEASE_0;
        if (sp_key_normal_key_index < 8) {
          *data = KEY_LeftCTRL + sp_key_normal_key_index;
        } else {
          *data = SP_Key_Map[sp_key_index][sp_key_normal_key_index - 6];
        }
        sp_key_normal_key_index++;
        data++;
        size--;
        break;
      case SP_KEY_ENCODE_ST_NORMAL:
        if (sp_key_normal_key_index < 8) {
          sp_key_normal_key_index++;
        } else {
          if (keycode_to_ascii[SP_Key_Map[sp_key_index][sp_key_normal_key_index - 6]] != 0 && SP_Key_Map[sp_key_index][sp_key_normal_key_index - 6] != KEY_None) {
            *data = keycode_to_ascii[SP_Key_Map[sp_key_index][sp_key_normal_key_index - 6]];
            data++;
            size--;
            sp_key_normal_key_index++;
          } else {
            sp_key_normal_key_index++;
          }
        }
        break;
      case SP_KEY_ENCODE_ST_END:
        *data = 0x0;  // SS_END_CODE
        data++;
        size--;
        break;
      default:
        break;
    }
    if (sp_key_normal_key_index == 6 + 8) {
      switch (sp_key_encode_st) {
        case SP_KEY_ENCODE_ST_PRESS_0:
          sp_key_encode_st = SP_KEY_ENCODE_ST_NORMAL;
          sp_key_normal_key_index = 0;
          break;
        case SP_KEY_ENCODE_ST_NORMAL:
          sp_key_encode_st = SP_KEY_ENCODE_ST_RELEASE_0;
          sp_key_normal_key_index = 0;
          break;
        case SP_KEY_ENCODE_ST_RELEASE_0:
          sp_key_encode_st = SP_KEY_ENCODE_ST_END;
          break;
        case SP_KEY_ENCODE_ST_END:
          sp_key_index++;
          sp_key_encode_st = SP_KEY_ENCODE_ST_PRESS_0;
          sp_key_normal_key_index = 0;
          if (sp_key_index >= SP_KEY_NUMBER)
            return;
          break;
        default:
          break;
      }
    }
  }
}

/*******************************************************************************
 * Function Name  : via_MACRO_decode
 * Description    : via MACRO格式解析为SP_KEY报文
 * Input          : *data - via MACRO格式数据
 * Return         : None
 *******************************************************************************/
static void via_MACRO_buffer_decode(uint8_t *data)
{
  uint8_t size = *data++;

  while (size) {
    switch (*data) {
      case 0x0: // END_CODE
        sp_key_index++;
        sp_key_normal_key_index = 0;
        break;
      //case 0x1: // SS_TAP_CODE
      case 0x2: // SS_DOWN_CODE
      //case 0x3: // SS_UP_CODE
        sp_key_next_direct_keycode_flag = TRUE;
        break;
      case KEY_LeftCTRL ... KEY_RightGUI:
        SP_Key_Map[sp_key_index][0] |= 1 << (*data - KEY_LeftCTRL);
        sp_key_next_direct_keycode_flag = FALSE;
        break;
      default:
        if (sp_key_normal_key_index < 6) {
          if (sp_key_next_direct_keycode_flag == TRUE) {
            SP_Key_Map[sp_key_index][sp_key_normal_key_index + 2] = *data;
            sp_key_next_direct_keycode_flag = FALSE;
          } else {
            SP_Key_Map[sp_key_index][sp_key_normal_key_index + 2] = ascii_to_keycode[*data];
          }
          sp_key_normal_key_index++;
        }
    }
    data++;
    size--;
  }
}

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
        case 2: { // id_Work_mode
          uint16_t workmode;
          if (*command_id == VIA_ID_CUSTOM_SET_VALUE) {
            workmode = data[3];
            HAL_Fs_Write_keyboard_cfg(FS_LINE_WORK_MODE, 1, &workmode); // 模式写入
          } else {
            HAL_Fs_Read_keyboard_cfg(FS_LINE_WORK_MODE, 1, &workmode); // 模式读取
            data[3] = (uint8_t)workmode;
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
        if (command_data[0] == 0) { // layer 0
          if (CustomKey[command_data[1]][command_data[2]] >= KEY_SP_1 && CustomKey[command_data[1]][command_data[2]] <= KEY_SP_7) {
            command_data[3] = 0x77;
            command_data[4] = CustomKey[command_data[1]][command_data[2]] - KEY_SP_1;
          } else {
            command_data[4] = CustomKey[command_data[1]][command_data[2]];
          }
        } else {  // layer 1
          if (Extra_CustomKey[command_data[1]][command_data[2]] >= KEY_SP_1 && Extra_CustomKey[command_data[1]][command_data[2]] <= KEY_SP_7) {
            command_data[3] = 0x77;
            command_data[4] = Extra_CustomKey[command_data[1]][command_data[2]] - KEY_SP_1;
          } else {
            command_data[4] = Extra_CustomKey[command_data[1]][command_data[2]];
          }
        }
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
          if (command_data[3] == 0x77)  // SP_KEY
            CustomKey[command_data[1]][command_data[2]] = KEY_SP_1 + command_data[4];
          else
            CustomKey[command_data[1]][command_data[2]] = command_data[4];
          HAL_Fs_Write_keyboard_mat("0:keyboard_mat.txt", (const uint8_t*)CustomKey);
        } else {  // layer 1
          if (command_data[3] == 0x77)  // SP_KEY
            Extra_CustomKey[command_data[1]][command_data[2]] = KEY_SP_1 + command_data[4];
          else
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
        command_data[0] = SP_KEY_NUMBER;  // SP按键个数
        break;
    }
    case (uint8_t)VIA_ID_DYNAMIC_KEYMAP_MACRO_GET_BUFFER_SIZE: {
        command_data[0] = 0;
        command_data[1] = SP_KEY_NUMBER * (3 * 8 + 6 + 1);  // 3 * 8(ctrl/gui/alt/shift特殊键) + 6(普通按键) + 1(结束符)
        sp_key_index = 0;
        sp_key_normal_key_index = 0;
        sp_key_encode_st = SP_KEY_ENCODE_ST_PRESS_0;
        break;
    }
    case (uint8_t)VIA_ID_DYNAMIC_KEYMAP_MACRO_GET_BUFFER: {
        /************* format *************
          command_data[0] = offset HSB
          command_data[1] = offset LSB
          command_data[2] = size
        **********************************/
        via_MACRO_buffer_encode(&command_data[2]);
        break;
    }
    case (uint8_t)VIA_ID_DYNAMIC_KEYMAP_MACRO_SET_BUFFER: {
        /************* format *************
          command_data[0] = offset HSB
          command_data[1] = offset LSB
          command_data[2] = size
        **********************************/
        if (((command_data[0] << 8) | command_data[1]) == SP_KEY_NUMBER * (3 * 8 + 6 + 1) - 1 && command_data[2] == 0x1 && command_data[3] == 0xFF) { // START flag
          sp_key_index = 0;
          sp_key_normal_key_index = 0;
          sp_key_next_direct_keycode_flag = FALSE;
        }
        via_MACRO_buffer_decode(&command_data[2]);
        HAL_Fs_Write_keyboard_spkey((uint8_t*)SP_Key_Map); // save configurations
        break;
    }
    case (uint8_t)VIA_ID_DYNAMIC_KEYMAP_MACRO_RESET: {
        memset(SP_Key_Map, 0, SP_KEY_NUMBER * 8);
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
        if (offset < COL_SIZE * ROW_SIZE) { // layer 0
          keyarr_ptr = (uint8_t*)CustomKey;
        } else {  // layer 1
          keyarr_ptr = (uint8_t*)Extra_CustomKey;
          offset -= COL_SIZE * ROW_SIZE;
        }
        keyarr_ptr += offset;
        for (i = offset; i < offset + size; i++, j+=2) {
          if (i >= COL_SIZE * ROW_SIZE) {
            command_data[j] = 0;
            command_data[j + 1] = 0;
          } else {
            command_data[j] = 0;  // MSB set to zero(USB为大端传输)
            if (keyarr_ptr[k] >= KEY_SP_1 && keyarr_ptr[k] <= KEY_SP_7) {
              command_data[j] = 0x77;
              command_data[j + 1] = keyarr_ptr[k++] - KEY_SP_1;
            } else {
              command_data[j + 1] = keyarr_ptr[k++];
            }
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
        if (offset < COL_SIZE * ROW_SIZE) { // layer 0
          keyarr_ptr = (uint8_t*)CustomKey;
        } else {  // layer 1
          keyarr_ptr = (uint8_t*)Extra_CustomKey;
          offset -= COL_SIZE * ROW_SIZE;
        }
        keyarr_ptr += offset;
        for (i = offset; i < offset + size; i++, j+=2) {
          if (i >= COL_SIZE * ROW_SIZE) break;
          else {
            if (command_data[j] == 0x77)  // SP_KEY
              keyarr_ptr[k++] = KEY_SP_1 + command_data[j + 1];
            else
              keyarr_ptr[k++] = command_data[j + 1];
          }
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
    case (uint8_t)VIA_ID_DYNAMIC_KEYMAP_MAGNET_GET_RANGE: { // unsupport magnet
        if (g_Test_Mode) {  // visual magnet range
          /************* format *************
            command_data[0] = minRange offset HSB
            command_data[1] = minRange offset LSB
            command_data[2] = maxRange offset HSB
            command_data[3] = maxRange offset LSB
          **********************************/
          command_data[0] = 0;
          command_data[1] = 0;
          command_data[2] = 0;
          command_data[3] = 100;
        }
        break;
    }
    case (uint8_t)VIA_ID_DYNAMIC_KEYMAP_MAGNET_SET_RANGE: { // unsupport magnet
        break;
    }
    case (uint8_t)VIA_ID_DYNAMIC_KEYMAP_MAGNET_GET_PRESS_BUFFER:
    case (uint8_t)VIA_ID_DYNAMIC_KEYMAP_MAGNET_GET_RELEASE_BUFFER: { // unsupport magnet
        if (g_Test_Mode) {  // visual magnet buffer
          uint16_t offset = (command_data[0] << 8) | command_data[1];
          uint16_t size = command_data[2];
          uint16_t i, j = 3;
          for (i = offset; i < offset + size; i++, j+=2) {
            if (offset == 0 && i == 0) {
              command_data[j] = (test_value1 >> 8) & 0xFF;
              command_data[j + 1] = test_value1 & 0xFF;
            } else {
              command_data[j] = (test_value2 >> 8) & 0xFF;
              command_data[j + 1] = test_value2 & 0xFF;
            }
          }
        }
        break;
    }
    case (uint8_t)VIA_ID_DYNAMIC_KEYMAP_MAGNET_SET_PRESS_BUFFER:
    case (uint8_t)VIA_ID_DYNAMIC_KEYMAP_MAGNET_SET_RELEASE_BUFFER: { // unsupport magnet
        if (g_Test_Mode) {  // visual magnet buffer
          uint16_t offset = (command_data[0] << 8) | command_data[1];
          uint16_t size = command_data[2];
          uint16_t i, j = 3;
          for (i = offset; i < offset + size; i++, j+=2) {
            if (offset == 0 && i == 0) {
              test_value1 = ((uint16_t)command_data[j] << 8) | command_data[j + 1];
            } else {
              test_value2 = ((uint16_t)command_data[j] << 8) | command_data[j + 1];
            }
          }
        }
        break;
    }
    case (uint8_t)VIA_ID_DYNAMIC_KEYMAP_MAGNET_GET_PRESS_VALUE:
    case (uint8_t)VIA_ID_DYNAMIC_KEYMAP_MAGNET_GET_RELEASE_VALUE: { // unsupport magnet
        if (g_Test_Mode) {  // visual magnet value
          /************* format *************
            command_data[0] = row
            command_data[1] = col
            command_data[2] = offset HSB
            command_data[3] = offset LSB
          **********************************/
          if (command_data[0] == 0 && command_data[1] == 0) {
            command_data[2] = (test_value1 >> 8) & 0xFF;
            command_data[3] = test_value1 & 0xFF;
          } else {
            command_data[2] = (test_value2 >> 8) & 0xFF;
            command_data[3] = test_value2 & 0xFF;
          }
        }
        break;
    }
    case (uint8_t)VIA_ID_DYNAMIC_KEYMAP_MAGNET_SET_PRESS_VALUE:
    case (uint8_t)VIA_ID_DYNAMIC_KEYMAP_MAGNET_SET_RELEASE_VALUE: { // unsupport magnet
        if (g_Test_Mode) {  // visual magnet value
          /************* format *************
            command_data[0] = row
            command_data[1] = col
            command_data[2] = offset HSB
            command_data[3] = offset LSB
          **********************************/
          if (command_data[0] == 0 && command_data[1] == 0) {
            test_value1 = ((uint16_t)command_data[2] << 8) | command_data[3];
          } else {
            test_value2 = ((uint16_t)command_data[2] << 8) | command_data[3];
          }
        }
        break;
    }
    case (uint8_t)VIA_ID_DYNAMIC_KEYMAP_MAGNET_GET_DIR: // unsupport magnet
    case (uint8_t)VIA_ID_DYNAMIC_KEYMAP_MAGNET_SET_DIR: // unsupport magnet
        break;
    default: {
        *command_id = VIA_ID_UNHANDLED;
        break;
    }
  }
}

