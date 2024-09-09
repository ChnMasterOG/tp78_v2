/********************************** (C) COPYRIGHT *******************************
 * File Name          : OLED_UI_CFG.c
 * Author             : ChnMasterOG
 * Version            : V1.0
 * Date               : 2023/3/4
 * Description        : 存放OLED UI常量配置
 * Copyright (c) 2023 ChnMasterOG
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#include "HAL.h"

/* 菜单项 */
const oled_ui_menu_structure cfg_menu_1 = {
  .type = OLED_UI_TYPE_MENU,
  .text[0] = "BLEdevice",
  .text[1] = "LEDstyle",
  .text[2] = "3-Mode",
  .p[0] = (uint8_t*)&bledevice_en,
  .p[1] = (uint8_t*)&ledstyle_en,
  .p[2] = (uint8_t*)&workmode_en,
  .p[3] = (uint8_t*)&main_menu,
  .p[4] = (uint8_t*)&cfg_menu_5,
  .p[5] = (uint8_t*)&cfg_menu_2,
  .menu_size = 3,
};
const oled_ui_menu_structure cfg_menu_2 = {
  .type = OLED_UI_TYPE_MENU,
  .text[0] = "UDISKmode",
  .text[1] = "MPRspeed",
  .text[2] = "MPRparam6",
  .p[0] = (uint8_t*)&udiskmode_en,
  .p[1] = (uint8_t*)&mprparam3_en,
  .p[2] = (uint8_t*)&mprparam6_en,
  .p[3] = (uint8_t*)&main_menu,
  .p[4] = (uint8_t*)&cfg_menu_1,
  .p[5] = (uint8_t*)&cfg_menu_3,
  .menu_size = 3,
};
const oled_ui_menu_structure cfg_menu_3 = {
  .type = OLED_UI_TYPE_MENU,
  .text[0] = "MPRparam7",
  .text[1] = "tpSpd_div",
  .text[2] = "Brightness",
  .p[0] = (uint8_t*)&mprparam7_en,
  .p[1] = (uint8_t*)&tp_speed_div_en,
  .p[2] = (uint8_t*)&led_brightness_en,
  .p[3] = (uint8_t*)&main_menu,
  .p[4] = (uint8_t*)&cfg_menu_2,
  .p[5] = (uint8_t*)&cfg_menu_4,
  .menu_size = 3,
};
const oled_ui_menu_structure cfg_menu_4 = {
  .type = OLED_UI_TYPE_MENU,
  .text[0] = "RF_freq",
  .text[1] = "Tbtn_en",
  .text[2] = "idle_cnt",
  .p[0] = (uint8_t*)&rf_freq_level_en,
  .p[1] = (uint8_t*)&touchbar_button_ctl_ena_en,
  .p[2] = (uint8_t*)&idle_max_cnt_en,
  .p[3] = (uint8_t*)&main_menu,
  .p[4] = (uint8_t*)&cfg_menu_3,
  .p[5] = (uint8_t*)&cfg_menu_5,
  .menu_size = 3,
};
const oled_ui_menu_structure cfg_menu_5 = {
  .type = OLED_UI_TYPE_MENU,
  .text[0] = "lp_cnt",
  .text[1] = "RF_chk_ms",
  .text[2] = "motor_en",
  .p[0] = (uint8_t*)&lp_max_cnt_en,
  .p[1] = (uint8_t*)&rf_chk_ms_en,
  .p[2] = (uint8_t*)&enable_motor_en,
  .p[3] = (uint8_t*)&main_menu,
  .p[4] = (uint8_t*)&cfg_menu_4,
  .p[5] = (uint8_t*)&cfg_menu_1,
  .menu_size = 3,
};
const oled_ui_menu_structure key_status_menu_1 = {
  .type = OLED_UI_TYPE_MENU,
  .text[0] = "bat_adc",
  .text[1] = "capmouseU",
  .text[2] = "capmouseD",
  .p[0] = (uint8_t*)&adc_val_status,
  .p[1] = (uint8_t*)&key_status_mousecap_U,
  .p[2] = (uint8_t*)&key_status_mousecap_D,
  .p[3] = (uint8_t*)&main_menu,
  .p[4] = (uint8_t*)&key_status_menu_4,
  .p[5] = (uint8_t*)&key_status_menu_2,
  .menu_size = 3,
};
const oled_ui_menu_structure key_status_menu_2 = {
  .type = OLED_UI_TYPE_MENU,
  .text[0] = "capmouseL",
  .text[1] = "capmouseR",
  .text[2] = "touchbarL1",
  .p[0] = (uint8_t*)&key_status_mousecap_L,
  .p[1] = (uint8_t*)&key_status_mousecap_R,
  .p[2] = (uint8_t*)&key_status_touchbar_1,
  .p[3] = (uint8_t*)&main_menu,
  .p[4] = (uint8_t*)&key_status_menu_1,
  .p[5] = (uint8_t*)&key_status_menu_3,
  .menu_size = 3,
};
const oled_ui_menu_structure key_status_menu_3 = {
  .type = OLED_UI_TYPE_MENU,
  .text[0] = "touchbarL2",
  .text[1] = "touchbarL3",
  .text[2] = "touchbarM",
  .p[0] = (uint8_t*)&key_status_touchbar_2,
  .p[1] = (uint8_t*)&key_status_touchbar_3,
  .p[2] = (uint8_t*)&key_status_touchbar_4,
  .p[3] = (uint8_t*)&main_menu,
  .p[4] = (uint8_t*)&key_status_menu_2,
  .p[5] = (uint8_t*)&key_status_menu_4,
  .menu_size = 3,
};
const oled_ui_menu_structure key_status_menu_4 = {
  .type = OLED_UI_TYPE_MENU,
  .text[0] = "touchbarR1",
  .text[1] = "touchbarR2",
  .text[2] = "touchbarR3",
  .p[0] = (uint8_t*)&key_status_touchbar_5,
  .p[1] = (uint8_t*)&key_status_touchbar_6,
  .p[2] = (uint8_t*)&key_status_touchbar_7,
  .p[3] = (uint8_t*)&main_menu,
  .p[4] = (uint8_t*)&key_status_menu_3,
  .p[5] = (uint8_t*)&key_status_menu_1,
  .menu_size = 3,
};
const oled_ui_menu_structure debug_menu_1 = {
  .type = OLED_UI_TYPE_MENU,
  .text[0] = "scanTPM",
  .p[0] = (uint8_t*)&scan_tpm,
  .p[3] = (uint8_t*)&main_menu,
  .p[4] = (uint8_t*)&debug_menu_1,
  .p[5] = (uint8_t*)&debug_menu_1,
  .menu_size = 1,
};
const oled_ui_menu_structure main_menu = {
  .type = OLED_UI_TYPE_MENU,
  .text[0] = "KeyStatus",
  .text[1] = "KeyCfg",
  .text[2] = "Debug",
  .p[0] = (uint8_t*)&key_status_menu_1,
  .p[1] = (uint8_t*)&cfg_menu_1,
  .p[2] = (uint8_t*)&debug_menu_1,
  .menu_size = 3,
};

/* ADC状态项 */
const oled_ui_val_status_structure adc_val_status = {
  .type = OLED_UI_TYPE_VAL_STATUS,
  .p = (uint8_t*)&key_status_menu_1,
  .p_val = (uint32_t*)&BAT_adcVal,
};

/* MPR121状态项 */
const oled_ui_mpr121_status_structure key_status_mousecap_U = {
  .type = OLED_UI_TYPE_MPR121_STATUS,
  .p = (uint8_t*)&key_status_menu_1,
  .reg = MPR121_PINMUX_TO_ELE(MPR121_PINMUX_CapMouseU),
  .is_half_word = TRUE,
};
const oled_ui_mpr121_status_structure key_status_mousecap_D = {
  .type = OLED_UI_TYPE_MPR121_STATUS,
  .p = (uint8_t*)&key_status_menu_1,
  .reg = MPR121_PINMUX_TO_ELE(MPR121_PINMUX_CapMouseD),
  .is_half_word = TRUE,
};
const oled_ui_mpr121_status_structure key_status_mousecap_L = {
  .type = OLED_UI_TYPE_MPR121_STATUS,
  .p = (uint8_t*)&key_status_menu_2,
  .reg = MPR121_PINMUX_TO_ELE(MPR121_PINMUX_CapMouseL),
  .is_half_word = TRUE,
};
const oled_ui_mpr121_status_structure key_status_mousecap_R = {
  .type = OLED_UI_TYPE_MPR121_STATUS,
  .p = (uint8_t*)&key_status_menu_2,
  .reg = MPR121_PINMUX_TO_ELE(MPR121_PINMUX_CapMouseR),
  .is_half_word = TRUE,
};
const oled_ui_mpr121_status_structure key_status_touchbar_1 = {
  .type = OLED_UI_TYPE_MPR121_STATUS,
  .p = (uint8_t*)&key_status_menu_2,
  .reg = MPR121_PINMUX_TO_ELE(MPR121_PINMUX_L1),
  .is_half_word = TRUE,
};
const oled_ui_mpr121_status_structure key_status_touchbar_2 = {
  .type = OLED_UI_TYPE_MPR121_STATUS,
  .p = (uint8_t*)&key_status_menu_3,
  .reg = MPR121_PINMUX_TO_ELE(MPR121_PINMUX_L2),
  .is_half_word = TRUE,
};
const oled_ui_mpr121_status_structure key_status_touchbar_3 = {
  .type = OLED_UI_TYPE_MPR121_STATUS,
  .p = (uint8_t*)&key_status_menu_3,
  .reg = MPR121_PINMUX_TO_ELE(MPR121_PINMUX_L3),
  .is_half_word = TRUE,
};
const oled_ui_mpr121_status_structure key_status_touchbar_4 = {
  .type = OLED_UI_TYPE_MPR121_STATUS,
  .p = (uint8_t*)&key_status_menu_3,
  .reg = MPR121_PINMUX_TO_ELE(MPR121_PINMUX_M),
  .is_half_word = TRUE,
};
const oled_ui_mpr121_status_structure key_status_touchbar_5 = {
  .type = OLED_UI_TYPE_MPR121_STATUS,
  .p = (uint8_t*)&key_status_menu_4,
  .reg = MPR121_PINMUX_TO_ELE(MPR121_PINMUX_R1),
  .is_half_word = TRUE,
};
const oled_ui_mpr121_status_structure key_status_touchbar_6 = {
  .type = OLED_UI_TYPE_MPR121_STATUS,
  .p = (uint8_t*)&key_status_menu_4,
  .reg = MPR121_PINMUX_TO_ELE(MPR121_PINMUX_R2),
  .is_half_word = TRUE,
};
const oled_ui_mpr121_status_structure key_status_touchbar_7 = {
  .type = OLED_UI_TYPE_MPR121_STATUS,
  .p = (uint8_t*)&key_status_menu_4,
  .reg = MPR121_PINMUX_TO_ELE(MPR121_PINMUX_R3),
  .is_half_word = TRUE,
};

/* 输入项 */
const oled_ui_enter_num_structure bledevice_en = {
  .type = OLED_UI_TYPE_ENTER_NUM,
  .p = (uint8_t*)&cfg_menu_1,
  .preStr = "val:",
  .postStr = "OK",
  .pStr_len = 4,
  .line = FS_LINE_BLE_DEVICE,
  .limit_len = 1,
};
const oled_ui_enter_num_structure ledstyle_en = {
  .type = OLED_UI_TYPE_ENTER_NUM,
  .p = (uint8_t*)&cfg_menu_1,
  .preStr = "val:",
  .postStr = "OK",
  .pStr_len = 4,
  .line = FS_LINE_LED_STYLE,
  .limit_len = 1,
};
const oled_ui_enter_num_structure workmode_en = {
  .type = OLED_UI_TYPE_ENTER_NUM,
  .p = (uint8_t*)&cfg_menu_1,
  .preStr = "val:",
  .postStr = "OK",
  .pStr_len = 4,
  .line = FS_LINE_WORK_MODE,
  .limit_len = 1,
};
const oled_ui_enter_num_structure udiskmode_en = {
  .type = OLED_UI_TYPE_ENTER_NUM,
  .p = (uint8_t*)&cfg_menu_2,
  .preStr = "val:",
  .postStr = "OK",
  .pStr_len = 4,
  .line = FS_LINE_UDISK_MODE,
  .limit_len = 1,
};
const oled_ui_enter_num_structure mprparam3_en = {
  .type = OLED_UI_TYPE_ENTER_NUM,
  .p = (uint8_t*)&cfg_menu_2,
  .preStr = "val:",
  .postStr = "OK",
  .pStr_len = 4,
  .line = FS_LINE_MPR_CAPMOUSE_MOV_SPD,
  .limit_len = 1,
};
const oled_ui_enter_num_structure mprparam6_en = {
  .type = OLED_UI_TYPE_ENTER_NUM,
  .p = (uint8_t*)&cfg_menu_2,
  .preStr = "val:",
  .postStr = "OK",
  .pStr_len = 4,
  .line = FS_LINE_MPR_DOUBLE_TOUCH_CNT,
  .limit_len = 0,
};
const oled_ui_enter_num_structure mprparam7_en = {
  .type = OLED_UI_TYPE_ENTER_NUM,
  .p = (uint8_t*)&cfg_menu_3,
  .preStr = "val:",
  .postStr = "OK",
  .pStr_len = 4,
  .line = FS_LINE_MPR_LONG_TOUCH_CNT,
  .limit_len = 0,
};
const oled_ui_enter_num_structure tp_speed_div_en = {
  .type = OLED_UI_TYPE_ENTER_NUM,
  .p = (uint8_t*)&cfg_menu_3,
  .preStr = "val:",
  .postStr = "OK",
  .pStr_len = 4,
  .line = FS_LINE_TP_SPEED_DIV,
  .limit_len = 1,
};
const oled_ui_enter_num_structure led_brightness_en = {
  .type = OLED_UI_TYPE_ENTER_NUM,
  .p = (uint8_t*)&cfg_menu_3,
  .preStr = "val:",
  .postStr = "OK",
  .pStr_len = 4,
  .line = FS_LINE_LED_BRIGHTNESS,
  .limit_len = 0,
};
const oled_ui_enter_num_structure rf_freq_level_en = {
  .type = OLED_UI_TYPE_ENTER_NUM,
  .p = (uint8_t*)&cfg_menu_4,
  .preStr = "val:",
  .postStr = "OK",
  .pStr_len = 4,
  .line = FS_LINE_RF_FREQ_LEVEL,
  .limit_len = 1,
};
const oled_ui_enter_num_structure touchbar_button_ctl_ena_en = {
  .type = OLED_UI_TYPE_ENTER_NUM,
  .p = (uint8_t*)&cfg_menu_4,
  .preStr = "val:",
  .postStr = "OK",
  .pStr_len = 4,
  .line = FS_LINE_TOUCHBAR_BUTTON_CTL_ENA,
  .limit_len = 1,
};
const oled_ui_enter_num_structure idle_max_cnt_en = {
  .type = OLED_UI_TYPE_ENTER_NUM,
  .p = (uint8_t*)&cfg_menu_4,
  .preStr = "val:",
  .postStr = "OK",
  .pStr_len = 4,
  .line = FS_LINE_IDLE_MAX_CNT,
  .limit_len = 0,
};
const oled_ui_enter_num_structure lp_max_cnt_en = {
  .type = OLED_UI_TYPE_ENTER_NUM,
  .p = (uint8_t*)&cfg_menu_5,
  .preStr = "val:",
  .postStr = "OK",
  .pStr_len = 4,
  .line = FS_LINE_LP_MAX_CNT,
  .limit_len = 0,
};
const oled_ui_enter_num_structure rf_chk_ms_en = {
  .type = OLED_UI_TYPE_ENTER_NUM,
  .p = (uint8_t*)&cfg_menu_5,
  .preStr = "val:",
  .postStr = "OK",
  .pStr_len = 4,
  .line = FS_LINE_RF_CHECK_ACK_MS,
  .limit_len = 3,
};
const oled_ui_enter_num_structure enable_motor_en = {
  .type = OLED_UI_TYPE_ENTER_NUM,
  .p = (uint8_t*)&cfg_menu_5,
  .preStr = "val:",
  .postStr = "OK",
  .pStr_len = 4,
  .line = FS_LINE_ENABLE_MOTOR,
  .limit_len = 1,
};

/* 执行项 */
const oled_ui_execute_structure scan_tpm = {
  .type = OLED_UI_TYPE_EXECUTE,
  .p = (uint8_t*)&cfg_menu_1,
  .preStr = "StartScan",
  .func = TPM_scan,
};
