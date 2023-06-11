/********************************** (C) COPYRIGHT *******************************
 * File Name          : MCU.c
 * Author             : ChnMasterOG, WCH
 * Version            : V1.2
 * Date               : 2022/1/26
 * Description        : 硬件任务处理函数及BLE和硬件初始化
 * Copyright (c) 2023 ChnMasterOG
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#include "APP.h"
#include "HAL.h"

/* HID data */
UINT8 HID_DATA[HID_DATA_LENGTH] = { 0x0, 0x0, 0x0, 0x0, 0x0,
                                    0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
                                    0x2, 0x0 }; // bit1~bit4: mouse data, bit6~bit13: key data, bit15: vol data
/* 鼠标数据 */
UINT8* HIDMouse = &HID_DATA[1];
/* 键盘数据 */
UINT8* HIDKeyboard = &HID_DATA[6];
/* 音量控制数据 */
UINT8* HIDVolume = &HID_DATA[15];

tmosTaskID halTaskID = INVALID_TASK_ID;

CapsLock_LEDOn_Status_t g_CapsLock_LEDOn_Status;  // 大小写信号
Ready_Status_t g_Ready_Status;  // 就绪信号
Enable_Status_t g_Enable_Status = { // 使能信号
   .ble = TRUE,
   .tp = TRUE,
};
uint8_t g_TP_speed_div = 1;
enum LP_Type g_lp_type;   // 记录下电前的低功耗模式
BOOL priority_USB = TRUE;   // USB和蓝牙/RF同时连接选择

static uint32_t EP_counter = 0;   // 彩蛋计数器
static uint32_t idle_cnt = 0;     // 无有效操作计数值，idle_cnt大于阈值则进入休眠
uint32_t sys_time = 0;            // 系统计时

/*******************************************************************************
 * Function Name  : TP78_Idle_Clr
 * Description    : 清除idle
 * Input          : 无
 * Return         : 无
 *******************************************************************************/
static void TP78_Idle_Clr(void)
{
  if (idle_cnt >= IDLE_MAX_PERIOD) {  // 退出idle
#if (defined HAL_OLED) && (HAL_OLED == TRUE)
    OLED_UI_idle(0);
#endif
  }
  idle_cnt = 0;
}

/*******************************************************************************
 * Function Name  : HID_KEYBOARD_Process
 * Description    : 主循环键盘处理函数
 * Input          : 无
 * Return         : 无
 *******************************************************************************/
__attribute__((weak)) void HID_KEYBOARD_Process(void)
{
  uint8_t res;
  KEYBOARD_Detection();
  if ( g_Ready_Status.rf == TRUE && priority_USB != TRUE ) {  // RF心跳包
    tmos_set_event( RFTaskId, SBP_RF_HEARTBEAT_REPORT_EVT );  // RF心跳包事件
  }
  if (g_Ready_Status.keyboard_key_data == TRUE) {    // 产生键盘数据
    g_Ready_Status.keyboard_key_data = FALSE;
    TP78_Idle_Clr();
    if ( EnterPasskey_flag == TRUE ) { // 处理输入配对密码
      res = KEYBOARD_EnterNumber(&BLE_Passkey, "Passkey=?", "Send!", 6);
      if ( res == 0 ) {
        OLED_UI_add_CANCELINFO_delay_task(3000);
        EnterPasskey_flag = FALSE;
        tmos_start_task( hidEmuTaskId, START_SEND_PASSKEY_EVT, 400 );
      }
    } else if ( g_keyboard_status.enter_cfg == TRUE ) { // 处理界面配置
      if ( KEYBOARD_Custom_Function() && g_oled_smooth_end_flag == TRUE ) { // 不带有Fn键且动画执行结束
        if (KeyboardDat->Key1 == KEY_A) {
          OLED_UI_draw_menu(OLED_UI_SWIPE_LEFT);
        } else if (KeyboardDat->Key1 == KEY_D) {
          OLED_UI_draw_menu(OLED_UI_SWIPE_RIGHT);
        } else if (KeyboardDat->Key1 == KEY_W) {
          OLED_UI_draw_menu(OLED_UI_SWIPE_UP);
        } else if (KeyboardDat->Key1 == KEY_S) {
          OLED_UI_draw_menu(OLED_UI_SWIPE_DOWN);
        } else if (KeyboardDat->Key1 != KEY_None) {
          OLED_UI_draw_menu(OLED_UI_MENU_REFRESH);
        }
      }
    } else {
      if ( KEYBOARD_Custom_Function() ) { // 带有Fn键处理信息则不产生键盘事件
        if ( g_Ready_Status.usb == TRUE && priority_USB == TRUE ) {
          tmos_set_event( usbTaskID, USB_KEYBOARD_EVENT );  // USB键盘事件
        } else if ( g_Ready_Status.ble == TRUE ) {
          tmos_set_event( hidEmuTaskId, START_KEYBOARD_REPORT_EVT );  // 蓝牙键盘事件
        } else if ( g_Ready_Status.rf == TRUE ) {
          tmos_set_event( RFTaskId, SBP_RF_KEYBOARD_REPORT_EVT );  // RF键盘事件
        }
      }
      if ( g_Ready_Status.keyboard_mouse_data == TRUE ) { // 发送键盘鼠标数据
        g_Ready_Status.keyboard_mouse_data = FALSE;
        tmos_memset(&HIDMouse[1], 0, 3);   // 只按左中右键没有其他操作
        if ( g_Ready_Status.usb == TRUE && priority_USB == TRUE ) {
          tmos_set_event( usbTaskID, USB_MOUSE_EVENT );  //USB鼠标事件
        } else if ( g_Ready_Status.ble == TRUE ) {
          tmos_set_event( hidEmuTaskId, START_MOUSE_REPORT_EVT );  //蓝牙鼠标事件
        } else if ( g_Ready_Status.rf == TRUE ) {
          tmos_set_event( RFTaskId, SBP_RF_MOUSE_REPORT_EVT );  // RF鼠标事件
        }
      }
    }
  }
}

/*******************************************************************************
 * Function Name  : HID_PS2TP_Process
 * Description    : PS2小红点处理函数
 * Input          : 无
 * Return         : 无
 *******************************************************************************/
__attribute__((weak)) void HID_PS2TP_Process(void)
{
  uint8_t tmp;
  if (g_Ready_Status.ps2_data == TRUE && g_Enable_Status.tp == TRUE) {    // 发送小红点鼠标数据
    g_Ready_Status.ps2_data = FALSE;
    TP78_Idle_Clr();
    if ( PS2_byte_cnt == 3 ) {  // 接收完数据报
      PS2_byte_cnt = 0;
#if (defined TP_Reverse) && (TP_Reverse == TRUE)
      HIDMouse[1] = -HIDMouse[1]; // 反转X轴
      HIDMouse[2] = -HIDMouse[2]; // 反转Y轴
#endif
      /* 小红点减速 */
      tmp = (char)HIDMouse[1] / (char)g_TP_speed_div;
      if ( tmp == 0 && HIDMouse[1]!=0) HIDMouse[1] = ( HIDMouse[1] < 128) ? 1 : -1;
      else HIDMouse[1] = tmp;
      tmp = (char)HIDMouse[2] / (char)g_TP_speed_div;
      if ( tmp == 0 && HIDMouse[2]!=0) HIDMouse[2] = ( HIDMouse[2] < 128) ? 1 : -1;
      else HIDMouse[2] = tmp;
      
      /* 鼠标事件 */
      if ( g_Ready_Status.usb == TRUE && priority_USB == TRUE ) {
        tmos_set_event( usbTaskID, USB_MOUSE_EVENT );  //USB鼠标事件
      } else if ( g_Ready_Status.ble == TRUE ) {
        tmos_set_event( hidEmuTaskId, START_MOUSE_REPORT_EVT );  //蓝牙鼠标事件
      } else if ( g_Ready_Status.rf == TRUE ) {
        tmos_set_event( RFTaskId, SBP_RF_MOUSE_REPORT_EVT );  // RF鼠标事件
      }
    }
    PS2_En_Data_Report();
  }
}


/*******************************************************************************
 * Function Name  : HID_I2CTP_Process
 * Description    : I2C小红点处理函数
 * Input          : 无
 * Return         : 无
 *******************************************************************************/
__attribute__((weak)) void HID_I2CTP_Process(void)
{
  uint8_t tmp;
  if (g_Ready_Status.i2ctp_data == TRUE && g_Enable_Status.tp == TRUE) {    // 发送小红点鼠标数据
    g_Ready_Status.i2ctp_data = FALSE;
    TP78_Idle_Clr();
    if (I2C_TP_ReadPacket() == 0) { // 正常接受完数据包
#if (defined TP_Reverse) && (TP_Reverse == TRUE)
      HIDMouse[1] = -HIDMouse[1]; // 反转X轴
      HIDMouse[2] = -HIDMouse[2]; // 反转Y轴
#endif
      /* 小红点减速 */
      tmp = (char)HIDMouse[1] / (char)g_TP_speed_div;
      if ( tmp == 0 && HIDMouse[1]!=0) HIDMouse[1] = ( HIDMouse[1] < 128) ? 1 : -1;
      else HIDMouse[1] = tmp;
      tmp = (char)HIDMouse[2] / (char)g_TP_speed_div;
      if ( tmp == 0 && HIDMouse[2]!=0) HIDMouse[2] = ( HIDMouse[2] < 128) ? 1 : -1;
      else HIDMouse[2] = tmp;

      /* 鼠标事件 */
      if ( g_Ready_Status.usb == TRUE && priority_USB == TRUE ) {
        tmos_set_event( usbTaskID, USB_MOUSE_EVENT );  //USB鼠标事件
      } else if ( g_Ready_Status.ble == TRUE ) {
        tmos_set_event( hidEmuTaskId, START_MOUSE_REPORT_EVT );  //蓝牙鼠标事件
      } else if ( g_Ready_Status.rf == TRUE ) {
        tmos_set_event( RFTaskId, SBP_RF_MOUSE_REPORT_EVT );  // RF鼠标事件
      }
    } else {
//      OLED_UI_add_SHOWINFO_task("TPdat ER");
//      OLED_UI_add_CANCELINFO_delay_task(3000);
    }
  }
}

/*******************************************************************************
 * Function Name  : HID_CapMouse_Process
 * Description    : MPR121触摸板处理函数
 * Input          : 无
 * Return         : 无
 *******************************************************************************/
__attribute__((weak)) void HID_CapMouse_Process(void)
{
  mpr121_operation_data_t oper_dat;

  MPR121_get_result(&oper_dat);
  if (oper_dat.cap_mouse_data_change) {
    oper_dat.cap_mouse_data_change = FALSE;
    MPR121_set_result(&oper_dat);
    TP78_Idle_Clr();
    if ( g_Ready_Status.usb == TRUE && priority_USB == TRUE ) {
      tmos_set_event( usbTaskID, USB_MOUSE_EVENT );  // USB鼠标事件
    } else if ( g_Ready_Status.ble == TRUE ) {
      tmos_set_event( hidEmuTaskId, START_MOUSE_REPORT_EVT );  //蓝牙鼠标事件
    } else if ( g_Ready_Status.rf == TRUE ) {
      tmos_set_event( RFTaskId, SBP_RF_MOUSE_REPORT_EVT );  // RF键盘事件
    }
  }
}

/*******************************************************************************
 * Function Name  : HID_VOL_Process
 * Description    : 音量控制处理函数
 * Input          : 无
 * Return         : 无
 *******************************************************************************/
__attribute__((weak)) void HID_VOL_Process(void)
{
  TP78_Idle_Clr();
  if ( g_Ready_Status.usb == TRUE && priority_USB == TRUE ) {
    tmos_set_event( usbTaskID, USB_VOL_EVENT );  //USB音量事件
  } else if ( g_Ready_Status.ble == TRUE ) {
    tmos_set_event( hidEmuTaskId, START_VOL_REPORT_EVT );  //蓝牙音量事件
  } else if ( g_Ready_Status.rf == TRUE ) {
    tmos_set_event( RFTaskId, SBP_RF_VOL_REPORT_EVT );  // RF键盘事件
  }
}

/*******************************************************************************
 * Function Name  : SW_PaintedEgg_Process
 * Description    : 彩蛋软件处理函数
 * Input          : 无
 * Return         : 无
 *******************************************************************************/
__attribute__((weak)) void SW_PaintedEgg_Process(void)
{
  KEYBOARD_Detection();
  if (g_Ready_Status.keyboard_key_data == TRUE) { // 产生键盘事件
    TP78_Idle_Clr();
    g_Ready_Status.keyboard_key_data = FALSE;
    if (KEYBOARD_Custom_Function() != 0) {
      switch (KeyboardDat->Key1) {
        case KEY_W: if (BodyDir[0] != DirDown) BodyDir[0] = DirUp; break;
        case KEY_S: if (BodyDir[0] != DirUp) BodyDir[0] = DirDown; break;
        case KEY_A: if (BodyDir[0] != DirRight) BodyDir[0] = DirLeft; break;
        case KEY_D: if (BodyDir[0] != DirLeft) BodyDir[0] = DirRight; break;
      }
    }
  }
  if (++EP_counter > 50) {  // 50次事件更新一次(控制snake速度)
    EP_counter = 0;
    MoveSnake();
  }
}

/*******************************************************************************
 * Function Name  : SW_OLED_Capslock_Process
 * Description    : 大小写状态OLED处理函数(检测LED状态：更新CapsLock指示)
 * Input          : 无
 * Return         : 无
 *******************************************************************************/
__attribute__((weak)) void SW_OLED_Capslock_Process(void)
{
  if ( g_Ready_Status.usb == TRUE && priority_USB && g_CapsLock_LEDOn_Status.usb != g_CapsLock_LEDOn_Status.ui ) {
    g_CapsLock_LEDOn_Status.ui = g_CapsLock_LEDOn_Status.usb;
#ifdef OLED_0_91
    OLED_UI_ShowCapslock(56, 1, g_CapsLock_LEDOn_Status.ui);
#endif
  } else if ( g_Ready_Status.ble == TRUE && g_CapsLock_LEDOn_Status.ble != g_CapsLock_LEDOn_Status.ui ) {
    g_CapsLock_LEDOn_Status.ui = g_CapsLock_LEDOn_Status.ble;
#ifdef OLED_0_91
    OLED_UI_ShowCapslock(56, 1, g_CapsLock_LEDOn_Status.ui);
#endif
  } else if ( g_Ready_Status.rf == TRUE && g_CapsLock_LEDOn_Status.rf != g_CapsLock_LEDOn_Status.ui ) {
    g_CapsLock_LEDOn_Status.ui = g_CapsLock_LEDOn_Status.rf;
#ifdef OLED_0_91
    OLED_UI_ShowCapslock(56, 1, g_CapsLock_LEDOn_Status.ui);
#endif
  } else {
    return;
  }
#ifdef OLED_0_66
  if (g_CapsLock_LEDOn_Status.ui == TRUE) {
    OLED_UI_slot_add((uint8_t*)UI_Slot_Icon[OLED_UI_ICON_CAPSLOCK_IDX]);
  } else {
    OLED_UI_slot_delete((uint8_t*)UI_Slot_Icon[OLED_UI_ICON_CAPSLOCK_IDX]);
  }
  OLED_UI_add_default_task(OLED_UI_FLAG_DRAW_SLOT);
#endif
}

/*******************************************************************************
 * Function Name  : SW_OLED_UBStatus_Process
 * Description    : USB或BLE/RF状态OLED处理函数(检测USB/蓝牙/RF连接状态：连接状态改变OLED显示)
 * Input          : 无
 * Return         : 无
 *******************************************************************************/
__attribute__((weak)) void SW_OLED_UBStatus_Process(void)
{
  if (g_Ready_Status.usb_l != g_Ready_Status.usb) {
    TP78_Idle_Clr();
    g_Ready_Status.usb_l = g_Ready_Status.usb;
    if ( g_Ready_Status.usb == TRUE ) {
#ifdef OLED_0_91
      OLED_UI_add_SHOWSTRING_task(8, 0, "USB");
#endif
#ifdef OLED_0_66
      OLED_UI_slot_add((uint8_t*)UI_Slot_Icon[OLED_UI_ICON_USB_IDX]);
#endif
    } else {
#ifdef OLED_0_91
      OLED_UI_add_SHOWSTRING_task(8, 0, "   ");
#endif
#ifdef OLED_0_66
      OLED_UI_slot_delete((uint8_t*)UI_Slot_Icon[OLED_UI_ICON_USB_IDX]);
#endif
    }
    if ( g_Ready_Status.usb ^ (g_Ready_Status.ble || g_Ready_Status.rf) ) priority_USB = TRUE;  // USB和BLE/RF只出现一个
    else if ( g_Ready_Status.usb == TRUE && (g_Ready_Status.ble == TRUE || g_Ready_Status.rf == TRUE) ) {    // 同时出现USB和BLE/RF时进行显示
#ifdef OLED_0_91
      OLED_UI_ShowOK(26 + !priority_USB * 30, 0, TRUE);
#endif
#ifdef OLED_0_66
      if ( priority_USB ) {
        OLED_UI_slot_active((uint8_t*)UI_Slot_Icon[OLED_UI_ICON_USB_IDX],
                            (uint8_t*)UI_Slot_Icon[OLED_UI_ICON_USB_IDX]);
      } else if ( g_Ready_Status.ble == TRUE ) {
        OLED_UI_slot_active((uint8_t*)UI_Slot_Icon[OLED_UI_ICON_BLE_UNCONNECT_IDX + DeviceAddress[5]],
                            (uint8_t*)UI_Slot_Icon[OLED_UI_ICON_BLE_UNCONNECT_IDX + DeviceAddress[5]]);
      } else {  // RF_Ready
        OLED_UI_slot_active((uint8_t*)UI_Slot_Icon[OLED_UI_ICON_RF_IDX],
                            (uint8_t*)UI_Slot_Icon[OLED_UI_ICON_RF_IDX]);
      }
#endif
    } else {
#ifdef OLED_0_91
      OLED_UI_ShowOK(26, 0, FALSE);
      OLED_UI_ShowOK(56, 0, FALSE);
#endif
    }
#ifdef OLED_0_66
    OLED_UI_add_default_task(OLED_UI_FLAG_DRAW_SLOT);
#endif
  } else if (g_Ready_Status.ble_l != g_Ready_Status.ble) {
    TP78_Idle_Clr();
    g_Ready_Status.ble_l = g_Ready_Status.ble;
//    HalLedSet(HAL_LED_1, BLE_Ready);
    if ( g_Ready_Status.ble == TRUE ) {
#ifdef OLED_0_91
      OLED_UI_add_SHOWSTRING_task(38, 0, "BLE");
#endif
#ifdef OLED_0_66
      OLED_UI_slot_add((uint8_t*)UI_Slot_Icon[OLED_UI_ICON_BLE_UNCONNECT_IDX + DeviceAddress[5]]);
#endif
    } else {
#ifdef OLED_0_91
      OLED_UI_add_SHOWSTRING_task(38, 0, "   ");
#endif
#ifdef OLED_0_66
      OLED_UI_slot_delete((uint8_t*)UI_Slot_Icon[OLED_UI_ICON_BLE_UNCONNECT_IDX + DeviceAddress[5]]);
#endif
      if ( EnterPasskey_flag == TRUE ) {  // 打断输入配对状态
        EnterPasskey_flag = FALSE;
        BLE_Passkey = 0;
//        OLED_Set_Scroll_ENA(1);
        OLED_UI_add_SHOWINFO_task("Close!");
        OLED_UI_add_CANCELINFO_delay_task(2000);
      }
    }
    if ( g_Ready_Status.usb ^ g_Ready_Status.ble ) priority_USB = TRUE;  // USB和BLE/RF只出现一个
    else if ( g_Ready_Status.usb == TRUE && g_Ready_Status.ble == TRUE ) {  // 同时出现USB和BLE时进行显示
#ifdef OLED_0_91
      OLED_UI_ShowOK(26 + !priority_USB * 30, 0, TRUE);
#endif
#ifdef OLED_0_66
      if ( priority_USB ) {
        OLED_UI_slot_active((uint8_t*)UI_Slot_Icon[OLED_UI_ICON_USB_IDX],
                            (uint8_t*)UI_Slot_Icon[OLED_UI_ICON_USB_IDX]);
      } else {
        OLED_UI_slot_active((uint8_t*)UI_Slot_Icon[OLED_UI_ICON_BLE_UNCONNECT_IDX + DeviceAddress[5]],
                            (uint8_t*)UI_Slot_Icon[OLED_UI_ICON_BLE_UNCONNECT_IDX + DeviceAddress[5]]);
      }
#endif
    } else {
#ifdef OLED_0_91
      OLED_UI_ShowOK(26, 0, FALSE);
      OLED_UI_ShowOK(56, 0, FALSE);
#endif
    }
#ifdef OLED_0_66
    OLED_UI_add_default_task(OLED_UI_FLAG_DRAW_SLOT);
#endif
  } else if (g_Ready_Status.rf_l != g_Ready_Status.rf) {
    TP78_Idle_Clr();
    g_Ready_Status.rf_l = g_Ready_Status.rf;
    if ( g_Ready_Status.rf == TRUE ) {
#ifdef OLED_0_91
      OLED_UI_add_SHOWSTRING_task(41, 0, "RF");
#endif
#ifdef OLED_0_66
      OLED_UI_slot_add((uint8_t*)UI_Slot_Icon[OLED_UI_ICON_RF_IDX]);
#endif
    } else {
#ifdef OLED_0_91
      OLED_UI_add_SHOWSTRING_task(41, 0, "  ");
#endif
#ifdef OLED_0_66
      OLED_UI_slot_delete((uint8_t*)UI_Slot_Icon[OLED_UI_ICON_RF_IDX]);
#endif
    }
    if ( g_Ready_Status.usb ^ g_Ready_Status.rf ) priority_USB = TRUE;  // USB和BLE/RF只出现一个
    else if ( g_Ready_Status.usb == TRUE && g_Ready_Status.rf == TRUE ) { // 同时出现USB和RF时进行显示
#ifdef OLED_0_91
      OLED_UI_ShowOK(26 + !priority_USB * 30, 0, TRUE);
#endif
#ifdef OLED_0_66
      if ( priority_USB ) {
        OLED_UI_slot_active((uint8_t*)UI_Slot_Icon[OLED_UI_ICON_USB_IDX],
                            (uint8_t*)UI_Slot_Icon[OLED_UI_ICON_USB_IDX]);
      } else {
        OLED_UI_slot_active((uint8_t*)UI_Slot_Icon[OLED_UI_ICON_RF_IDX],
                            (uint8_t*)UI_Slot_Icon[OLED_UI_ICON_RF_IDX]);
      }
#endif
    } else {
#ifdef OLED_0_91
      OLED_UI_ShowOK(26, 0, FALSE);
      OLED_UI_ShowOK(56, 0, FALSE);
#endif
    }
#ifdef OLED_0_66
    OLED_UI_add_default_task(OLED_UI_FLAG_DRAW_SLOT);
#endif
  }
}

/*******************************************************************************
 * Function Name  : HW_Battery_Process
 * Description    : 电池电量处理函数
 * Input          : 无
 * Return         : 无
 *******************************************************************************/
__attribute__((weak)) void HW_Battery_Process(void)
{
  BAT_ADC_DIS();  // MOS关闭
  BATTERY_ADC_Calculation( );
#if (defined HAL_OLED)  && (HAL_OLED == TRUE)
#ifdef OLED_0_91
  if ( EnterPasskey_flag == FALSE ) {
#endif
    BATTERY_DrawBMP( ); // 绘制电池
#ifdef OLED_0_91
  }
#endif
#endif
  BAT_ADC_ENA();  // MOS打开
  BATTERY_DMA_ENABLE( );  // DMA使能
}

/*******************************************************************************
 * Function Name  : HW_WS2812_Process
 * Description    : 按键背光处理函数
 * Input          : 无
 * Return         : 无
 *******************************************************************************/
__attribute__((weak)) void HW_WS2812_Process(void)
{
  WS2812_Send( );
}

/*******************************************************************************
 * Function Name  : HW_MODULE_Process
 * Description    : I2C模块通信
 * Input          : 无
 * Return         : 无
 *******************************************************************************/
__attribute__((weak)) void HW_MODULE_Process(void)
{
  // TODO
}

/*******************************************************************************
 * Function Name  : HW_TouchBar_Process
 * Description    : 触摸条处理
 * Input          : 无
 * Return         : 无
 *******************************************************************************/
__attribute__((weak)) void HW_TouchBar_Process(void)
{
  mpr121_operation_data_t oper_dat;

  MPR121_get_result(&oper_dat);
  if (oper_dat.touchbar_data_change) {
    oper_dat.touchbar_data_change = FALSE;
    MPR121_set_result(&oper_dat);
    if ( g_Ready_Status.usb == TRUE && priority_USB == TRUE ) {
      tmos_set_event( usbTaskID, USB_MOUSE_EVENT );  //USB鼠标事件
    } else if ( g_Ready_Status.ble == TRUE ) {
      tmos_set_event( hidEmuTaskId, START_MOUSE_REPORT_EVT );  //蓝牙鼠标事件
    } else if ( g_Ready_Status.rf == TRUE ) {
      tmos_set_event( RFTaskId, SBP_RF_MOUSE_REPORT_EVT );  // RF键盘事件
    }
  }
}

/*******************************************************************************
 * @fn          Lib_Calibration_LSI
 *
 * @brief       内部32k校准
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void Lib_Calibration_LSI( void )
{
  Calibration_LSI( Level_128 ); // Level_64
}

#if (defined (BLE_SNV)) && (BLE_SNV == TRUE)
/*******************************************************************************
 * @fn          Lib_Read_Flash
 *
 * @brief       Lib 操作Flash回调
 *
 * input parameters
 *
 * @param       addr.
 * @param       num.
 * @param       pBuf.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
u32 Lib_Read_Flash( u32 addr, u32 num, u32 *pBuf )
{
//  EEPROM_READ( addr, pBuf, num * 4 );
  FLASH_ROM_READ( addr, pBuf, num * 4 );
  return 0;
}

/*******************************************************************************
 * @fn          Lib_Write_Flash
 *
 * @brief       Lib 操作Flash回调
 *
 * input parameters
 *
 * @param       addr.
 * @param       num.
 * @param       pBuf.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
u32 Lib_Write_Flash( u32 addr, u32 num, u32 *pBuf )
{
//#if(defined(BLE_SNV_NUM))
//  EEPROM_ERASE( addr, EEPROM_PAGE_SIZE*2*BLE_SNV_NUM );
//#else
//  EEPROM_ERASE( addr, EEPROM_PAGE_SIZE*2 );
//#endif
//  EEPROM_WRITE( addr, pBuf, num * 4 );
  if ( addr <= 0x6F000 ) FLASH_ROM_ERASE( addr, 0x1000 );  // 4096
  FLASH_ROM_WRITE( addr, pBuf, num * 4 );
  return 0;
}
#endif

/*******************************************************************************
 * @fn          CH58X_BLEInit
 *
 * @brief       BLE 库初始化
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void CH58X_BLEInit(void)
{
    uint8_t     i;
    bleConfig_t cfg;
    if(tmos_memcmp(VER_LIB, VER_FILE, strlen(VER_FILE)) == FALSE)
    {
        PRINT("head file error...\n");
        while(1);
    }
    SysTick_Config(SysTick_LOAD_RELOAD_Msk);
    PFIC_DisableIRQ(SysTick_IRQn);

    tmos_memset(&cfg, 0, sizeof(bleConfig_t));
    cfg.MEMAddr = (uint32_t)MEM_BUF; 
    cfg.MEMLen = (uint32_t)BLE_MEMHEAP_SIZE; //Ram
    cfg.BufMaxLen = (uint32_t)BLE_BUFF_MAX_LEN; // 蓝牙通信的一个包的最大长度（字节数），其中有7字节为固定的协议开销
    cfg.BufNumber = (uint32_t)BLE_BUFF_NUM; //控制器缓存区能接受的包的个数
    cfg.TxNumEvent = (uint32_t)BLE_TX_NUM_EVENT; //临界事件中发送的包的个数，≤BLE_BUFF_NUM
    cfg.TxPower = (uint32_t)BLE_TX_POWER; //蓝牙发射功率
#if(defined(BLE_SNV)) && (BLE_SNV == TRUE)
    cfg.SNVAddr = (uint32_t)BLE_SNV_ADDR; //蓝牙绑定信息flash起始地址 每个绑定设备需要256字节空间
#if(defined(BLE_SNV_NUM))
    cfg.SNVNum = (uint32_t)BLE_SNV_NUM;
#endif
    cfg.readFlashCB = Lib_Read_Flash; 
    cfg.writeFlashCB = Lib_Write_Flash;
#endif
#if(CLK_OSC32K)
    cfg.SelRTCClock = (uint32_t)CLK_OSC32K;
#endif
    cfg.ConnectNumber = (PERIPHERAL_MAX_CONNECTION & 3) | (CENTRAL_MAX_CONNECTION << 2); // PERIPHERAL_MAX_CONNECTION可以同时和多少个主机连接，
                                                                                         // CENTRAL_MAX_CONNECTION 可以同时连接多少个从机，
  
    cfg.srandCB = SYS_GetSysTickCnt;
#if(defined TEM_SAMPLE) && (TEM_SAMPLE == TRUE)
    cfg.tsCB = HAL_GetInterTempValue; // 根据温度变化校准RF和内部RC( 大于7摄氏度 )
  #if(CLK_OSC32K)
    cfg.rcCB = Lib_Calibration_LSI; // 内部32K时钟校准函数
  #endif
#endif
#if(defined(HAL_SLEEP)) && (HAL_SLEEP == TRUE)
    cfg.WakeUpTime = WAKE_UP_RTC_MAX_TIME;
    cfg.sleepCB = CH58X_LowPower; // 启用睡眠
#endif
#if(defined(BLE_MAC)) && (BLE_MAC == TRUE)
    for(i = 0; i < 6; i++)
    {
        cfg.MacAddr[i] = MacAddr[5 - i];
    }
#else
    {
        uint8_t MacAddr[6];
        GetMACAddress(MacAddr);
        for(i = 0; i < 6; i++)
        {
            cfg.MacAddr[i] = MacAddr[i]; // 使用芯片mac地址
        }
    }
#endif
    if(!cfg.MEMAddr || cfg.MEMLen < 4 * 1024)
    {
        while(1);
    }
    i = BLE_LibInit(&cfg); //初始化蓝牙库
    if(i)
    {
        PRINT("LIB init error code: %x ...\n", i);
        while(1);
    }
}

/*******************************************************************************
 * @fn          HAL_ProcessEvent
 *
 * @brief       硬件层事务处理
 *
 * input parameters
 *
 * @param       task_id.
 * @param       events.
 *
 * output parameters
 *
 * @param       events.
 *
 * @return      None.
 */
tmosEvents HAL_ProcessEvent( tmosTaskID task_id, tmosEvents events )
{
  uint8 * msgPtr;
#if ((defined HAL_MPR121_CAPMOUSE) && (HAL_MPR121_CAPMOUSE == TRUE)) || ((defined HAL_MPR121_TOUCHBAR) && (HAL_MPR121_TOUCHBAR == TRUE))
  uint16_t dat16;
  static uint8_t collect_cnt = 0;
#endif
#if (defined HAL_MPR121_CAPMOUSE) && (HAL_MPR121_CAPMOUSE == TRUE)
  uint8_t i;
#endif
#if (defined HAL_MODULE) && (HAL_MODULE == TRUE)
  uint8_t dat, err;
#endif

  if ( events & SYS_EVENT_MSG )
  {    // 处理HAL层消息，调用tmos_msg_receive读取消息，处理完成后删除消息。
    msgPtr = tmos_msg_receive( task_id );
    if ( msgPtr )
    {
      /* De-allocate */
      tmos_msg_deallocate( msgPtr );
    }
    return events ^ SYS_EVENT_MSG;
  }

#if (defined HAL_LED) && (HAL_LED == TRUE)
  // 板载LED闪烁事件
  if ( events & LED_BLINK_EVENT )
  {
    HalLedUpdate( );
    return events ^ LED_BLINK_EVENT;
  }
#endif // HAL_LED

#if (defined HAL_KEY) && (HAL_KEY == TRUE)
  // 板载按键事件
  if ( events & KEY_EVENT )
  {
    HAL_KeyPoll(); /* Check for keys */
    tmos_start_task( halTaskID, KEY_EVENT, MS1_TO_SYSTEM_TIME(100) );
    return events ^ KEY_EVENT;
  }
#endif

  // 电池ADC检测事件
  if ( events & BATTERY_EVENT )
  {
#if (defined HAL_BATTADC) && (HAL_BATTADC == TRUE)
    HW_Battery_Process();
#endif
    tmos_start_task( halTaskID, BATTERY_EVENT, MS1_TO_SYSTEM_TIME(3000) );  // 3s更新采样值
    return events ^ BATTERY_EVENT;
  }

  // 扩展模块初始化事件
  if ( events & MODULE_INITIAL_EVENT )
  {
#if (defined HAL_MODULE) && (HAL_MODULE == TRUE)


#endif
    return events ^ MODULE_INITIAL_EVENT;
  }

  // 鼠标处理事件
  if ( events & HAL_MOUSE_EVENT )
  {
#if ((defined HAL_PS2) && (HAL_PS2 == TRUE)) || ((defined HAL_I2C_TP) && (HAL_I2C_TP == TRUE)) || ((defined HAL_MPR121_CAPMOUSE) && (HAL_MPR121_CAPMOUSE == TRUE))
#if (defined HAL_PS2) && (HAL_PS2 == TRUE)
    HID_PS2TP_Process();
#elif (defined HAL_I2C_TP) && (HAL_I2C_TP == TRUE)
    HID_I2CTP_Process();
#else  // default MPR121 cap mouse
    HID_CapMouse_Process();
#endif
#endif
    tmos_start_task( halTaskID, HAL_MOUSE_EVENT, MS1_TO_SYSTEM_TIME(25) ); // 处理鼠标(至少20ms保证蓝牙线程)
    return events ^ HAL_MOUSE_EVENT;
  }

  // 键盘处理事件
  if ( events & HAL_KEYBOARD_EVENT )
  {
#if (defined HAL_KEYBOARD) && (HAL_KEYBOARD == TRUE)
    HID_KEYBOARD_Process();
#endif
    if ( g_Enable_Status.paintedegg == FALSE ) {
#if (ROW_SCAN_MODE)
      tmos_start_task( halTaskID, HAL_KEYBOARD_EVENT, MS1_TO_SYSTEM_TIME(5) ); // 处理键盘
#else
      tmos_start_task( halTaskID, HAL_KEYBOARD_EVENT, MS1_TO_SYSTEM_TIME(5) ); // 处理键盘
#endif
    }
    return events ^ HAL_KEYBOARD_EVENT;
  }

  // 定时主循环事件
  if ( events & MAIN_CIRCULATION_EVENT)
  {
    // 彩蛋模式
#if (defined SW_PAINTEDEGG) && (SW_PAINTEDEGG == TRUE)
    if ( g_Enable_Status.paintedegg == TRUE ) {
      SW_PaintedEgg_Process();
      goto main_circulation_end;
    }
#endif
    // 触摸条处理
#if (defined HAL_MPR121_TOUCHBAR) && (HAL_MPR121_TOUCHBAR == TRUE)
    HW_TouchBar_Process();
#endif
    // CP通信处理
#if (defined MSG_CP) && (MSG_CP == TRUE)
    if (g_Ready_Status.cp == TRUE) {
      HW_MSG_CP_Process();
    }
#endif
    // OLED信息更新处理
#if (defined HAL_OLED) && (HAL_OLED == TRUE)
    SW_OLED_UBStatus_Process();
    SW_OLED_Capslock_Process();
#endif
    main_circulation_end:
    tmos_start_task( halTaskID, MAIN_CIRCULATION_EVENT, MS1_TO_SYSTEM_TIME(5) ); // 5ms周期
    return events ^ MAIN_CIRCULATION_EVENT;
  }

  // 喂狗线程
  if ( events & FEEDDOG_EVENT )
  {
    ++idle_cnt;
    ++sys_time;
    if (idle_cnt == IDLE_MAX_PERIOD) {  // 进入idle
#if (defined HAL_OLED) && (HAL_OLED == TRUE)
      OLED_UI_idle(1);
#endif
    }
    if (idle_cnt >= LP_MAX_PERIOD) {  // 进入低功耗模式
      idle_cnt = 0;
#if (defined HAL_OLED) && (HAL_OLED == TRUE)
      OLED_UI_idle(0);
      OLED_Clr(0, 2, 64, 5);  // 立即执行 - 后续进入低功耗
#endif
      if (g_Ready_Status.usb == TRUE) g_lp_type = lp_idle_mode;
      else g_lp_type = lp_shutdown_mode;
      GotoLowpower(g_lp_type);
    }
#if (defined HAL_WDG) && (HAL_WDG == TRUE)
    WWDG_SetCounter(0);
#endif
    tmos_start_task( halTaskID, FEEDDOG_EVENT, MS1_TO_SYSTEM_TIME(25) ); // 25ms周期
    return events ^ FEEDDOG_EVENT;
  }

  // WS2812控制事件
  if ( events & WS2812_EVENT )
  {
#if (defined HAL_WS2812_PWM) && (HAL_WS2812_PWM == TRUE)
    HW_WS2812_Process();
#endif
    tmos_start_task( halTaskID, WS2812_EVENT, MS1_TO_SYSTEM_TIME(80) ); // 80ms周期控制背光
    return events ^ WS2812_EVENT;
  }

  // MPR121事件
  if ( events & MPR121_EVENT )
  {
#if ((defined HAL_MPR121_CAPMOUSE) && (HAL_MPR121_CAPMOUSE == TRUE)) || ((defined HAL_MPR121_TOUCHBAR) && (HAL_MPR121_TOUCHBAR == TRUE))
    if (++collect_cnt >= ALG_COLLECT_CNT) {
      collect_cnt = 0;
    }
#endif
#if (defined HAL_MPR121_CAPMOUSE) && (HAL_MPR121_CAPMOUSE == TRUE)
    if (collect_cnt == 0) {
      if ( g_keyboard_status.enter_cfg == FALSE ) MPR121_alg_judge_cap_mouse();  // 配置参数模式不进行cap_mouse判断
    } else {
      MPR121_update_data();
    }
#endif
#if (defined HAL_MPR121_TOUCHBAR) && (HAL_MPR121_TOUCHBAR == TRUE)
    if (collect_cnt == 0) {
      if (g_keyboard_status.enter_cfg == FALSE) {  // 配置参数模式不进行touchbar判断
        if (g_Enable_Status.tp) MPR121_alg_judge_touchbar(ALG_ENABLE_TOUCHBAR_SLIDE | ALG_ENABLE_TOUCHBAR_TOUCH);
        else MPR121_alg_judge_touchbar(ALG_ENABLE_TOUCHBAR_SLIDE);  // 小红点失能时同时失能触摸条触摸判断
        MPR121_Post_Operation();
      }
    } else {
#if !(defined HAL_MPR121_CAPMOUSE) || (HAL_MPR121_CAPMOUSE == FALSE)
      MPR121_update_data();   // update mpr121 status
#endif
    }
#endif
    tmos_start_task( halTaskID, MPR121_EVENT, MS1_TO_SYSTEM_TIME(MPR121_TASK_PERIOD) ); // (MPR121_TASK_PERIOD)ms控制周期
    return events ^ MPR121_EVENT;
  }

  // OLED UI事件
  if ( events & OLED_UI_EVENT )
  {
#if (defined HAL_OLED) && (HAL_OLED == TRUE)
    OLED_UI_draw_thread_callback();
#endif
    tmos_start_task( halTaskID, OLED_UI_EVENT, MS1_TO_SYSTEM_TIME(1000/g_oled_fresh_rate) ); // 根据OLED刷新率控制
    return events ^ OLED_UI_EVENT;
  }

  // USB Ready事件
  if ( events & USB_READY_EVENT )
  {
    g_Ready_Status.usb = TRUE;
    return events ^ USB_READY_EVENT;
  }

  // 马达停止事件
  if ( events & MOTOR_STOP_EVENT )
  {
    MOTOR_STOP();
    return events ^ MOTOR_STOP_EVENT;
  }

  // 硬件初始化事件
  if( events & HAL_REG_INIT_EVENT )
  {
#if(defined BLE_CALIBRATION_ENABLE) && (BLE_CALIBRATION_ENABLE == TRUE) // 校准任务，单次校准耗时小于10ms
    BLE_RegInit();  // 校准RF
#if(defined CLK_OSC32K && CLK_OSC32K != 0x80)
    Lib_Calibration_LSI(); // 校准内部RC
#endif
    tmos_start_task(halTaskID, HAL_REG_INIT_EVENT, MS1_TO_SYSTEM_TIME(BLE_CALIBRATION_PERIOD));
    return events ^ HAL_REG_INIT_EVENT;
#endif
  }

  // 测试事件
//  static BOOL motor_status = FALSE;
  if ( events & HAL_TEST_EVENT )
  {
//    if (motor_status == FALSE) {
//      motor_status = TRUE;
//      MOTOR_RUN();
//    } else {
//      motor_status = FALSE;
//      MOTOR_STOP();
//    }
    tmos_start_task( halTaskID, HAL_TEST_EVENT, MS1_TO_SYSTEM_TIME( 1000 ) );
    return events ^ HAL_TEST_EVENT;
  }

  return 0;
}

/*******************************************************************************
 * @fn          HAL_Init
 *
 * @brief       硬件初始化
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void HAL_Init()
{
  uint16_t tmp;
  char debug_info[128] = "OK";

  halTaskID = TMOS_ProcessEventRegister( HAL_ProcessEvent );
  HAL_TimeInit();
  HAL_Fs_Init(debug_info);  // Fs先初始化
  DATAFLASH_Read_DeviceID();
  DATAFLASH_Read_RFfreqlevel();
#ifdef FIRST_USED
  if (g_Ready_Status.fatfs == TRUE) {
    KEYBOARD_Reset();
  }
#endif
#if (defined HAL_SLEEP) && (HAL_SLEEP == TRUE)
  HAL_SleepInit();
#endif
#if (defined HAL_KEY) && (HAL_KEY == TRUE)
  HAL_KeyInit( );
#endif
#if (defined HAL_WS2812_PWM) && (HAL_WS2812_PWM == TRUE)
  DATAFLASH_Read_LEDStyle( );
  HAL_Fs_Read_keyboard_cfg(FS_LINE_LED_BRIGHTNESS, 1, &tmp);
  g_LED_brightness = tmp;
  WS2812_PWM_Init( );
#endif
#if (defined HAL_HW_I2C) && (HAL_HW_I2C == TRUE)
  HW_I2C_Init( );
#endif
#if (defined HAL_OLED) && (HAL_OLED == TRUE)
  HAL_OLED_Init( );
#endif
#if (defined HAL_BATTADC) && (HAL_BATTADC == TRUE)
  BATTERY_Init( );
#endif
#if (defined HAL_USB) && (HAL_USB == TRUE)
  HAL_USBInit( );
#endif
#if (defined HAL_PS2) && (HAL_PS2 == TRUE)
  PS2_Init(debug_info, TRUE); // PS/2中断实现
  HAL_Fs_Read_keyboard_cfg(FS_LINE_TP_SPEED_DIV, 1, &tmp);
  g_TP_speed_div = (tmp == 0 ? 1 : tmp);
#endif
#if (defined HAL_KEYBOARD) && (HAL_KEYBOARD == TRUE)
  KEYBOARD_Init( );
#endif
#if (defined HAL_MOTOR) && (HAL_MOTOR == TRUE)
  MOTOR_Init( );
#endif
#if (defined MSG_CP) && (MSG_CP == TRUE)
  MSG_CP_Init(debug_info);
#endif
#if (defined HAL_RF) && (HAL_RF == TRUE) && !(defined TEST)
  DATAFLASH_Read_RForBLE( );
#endif
#if (defined HAL_HW_I2C) && (HAL_HW_I2C == TRUE)  // 最后初始化保证上电延迟
#if (defined HAL_I2C_TP) && (HAL_I2C_TP == TRUE)  // I2C_TP is used by HW I2C
  I2C_TP_Init(debug_info);
  HAL_Fs_Read_keyboard_cfg(FS_LINE_TP_SPEED_DIV, 1, &tmp);
  g_TP_speed_div = (tmp == 0 ? 1 : tmp);
#endif
#if ((defined HAL_MPR121_CAPMOUSE) && (HAL_MPR121_CAPMOUSE == TRUE)) || ((defined HAL_MPR121_TOUCHBAR) && (HAL_MPR121_TOUCHBAR == TRUE))  // MPR121需要上电延迟
  MPR121_Init(debug_info);
#endif
#endif
#if (defined HAL_LED) && (HAL_LED == TRUE)
  debug_info[7] = '\0';
  if ( strcmp(debug_info, "OK") == 0 ) {
    HAL_LedInit(0);
  } else {
    HAL_LedInit(1);
  }
#endif
#if ( defined BLE_CALIBRATION_ENABLE ) && ( BLE_CALIBRATION_ENABLE == TRUE )
  tmos_start_task( halTaskID, HAL_REG_INIT_EVENT, MS1_TO_SYSTEM_TIME( BLE_CALIBRATION_PERIOD ) );    // 添加校准任务，单次校准耗时小于10ms
#endif
  PRINT("%s\n", debug_info);
  /******* 初始化OLED UI *******/
#ifdef OLED_0_96
  OLED_ShowString(2, 1, "L1");
  OLED_ShowChar(20, 1, 'S');
  OLED_ShowNum(26, 1, DATAFLASH_Read_LEDStyle( ), 1);
  OLED_ShowChar(38, 1, 'D');
  OLED_ShowNum(44, 1, DeviceAddress[5], 1);
  OLED_UI_ShowCapslock(56, 1, FALSE);
#endif
  OLED_UI_draw_empty_battery();  // 绘制空电池
  if ( strcmp(debug_info, "OK") != 0 ) {  // 有错误则显示
    OLED_UI_add_SHOWINFO_task("%s", debug_info);
    OLED_UI_add_CANCELINFO_delay_task(3000);
  }
//  OLED_Scroll(7, 7, 24, 16, 2, 1, 0);    // 128 FRAMES
//  OLED_UI_draw_menu(OLED_UI_MENU_REFRESH);
//  OLED_UI_draw_menu(OLED_UI_SWIPE_DOWN);
//  tmos_start_task( halTaskID, HAL_TEST_EVENT, 1600 );    // 添加测试任务
#ifndef FIRST_USED
#if (defined HAL_WDG) && (HAL_WDG == TRUE)
  WWDG_ResetCfg(ENABLE);  // 看门狗使能(溢出复位)
#endif
#endif
}

/*******************************************************************************
 * @fn      HAL_GetInterTempValue
 *
 * @brief   获取内部温感采样值，如果使用了ADC中断采样，需在此函数中暂时屏蔽中断.
 *
 * @return  内部温感采样值.
 */
uint16_t HAL_GetInterTempValue(void)
{
    uint8_t  sensor, channel, config, tkey_cfg;
    uint16_t adc_data;

    tkey_cfg = R8_TKEY_CFG;
    sensor = R8_TEM_SENSOR;
    channel = R8_ADC_CHANNEL;
    config = R8_ADC_CFG;
    ADC_InterTSSampInit();
    R8_ADC_CONVERT |= RB_ADC_START;
    while(R8_ADC_CONVERT & RB_ADC_START);
    adc_data = R16_ADC_DATA;
    R8_TEM_SENSOR = sensor;
    R8_ADC_CHANNEL = channel;
    R8_ADC_CFG = config;
    R8_TKEY_CFG = tkey_cfg;
    return (adc_data);
}

/******************************** endfile @ mcu ******************************/
