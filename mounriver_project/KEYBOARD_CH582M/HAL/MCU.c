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
NumLock_LEDOn_Status_t g_NumLock_LEDOn_Status;  // 大小写信号
Ready_Status_t g_Ready_Status;  // 就绪信号
Enable_Status_t g_Enable_Status = { // 使能信号
   .tp = TRUE,
};
uint8_t g_TP_speed_div = 1;
enum LP_Type g_lp_type;   // 记录下电前的低功耗模式

static uint32_t EP_counter = 0;   // 彩蛋计数器
static uint32_t idle_cnt = 0;     // 无有效操作计数值，idle_cnt大于阈值则进入休眠
uint32_t sys_time = 0;            // 系统计时

/*******************************************************************************
 * Function Name  : TP78_Idle_Clr
 * Description    : 清除idle
 * Input          : 无
 * Return         : 无
 *******************************************************************************/
static inline void TP78_Idle_Clr(void)
{
  if (idle_cnt >= IDLE_MAX_PERIOD) {  // 退出idle
#if (defined HAL_OLED) && (HAL_OLED == TRUE)
    OLED_UI_idle(0);
#endif
  }
  idle_cnt = 0;
}

/*******************************************************************************
 * Function Name  : OnBoard_SendMsg
 * Description    : 不同任务之间发送Msg
 * Input          : registeredTaskID: 任务ID
 *                  event: 事件类型
 *                  state: 事件状态
 *                  data: 事件指针
 * Return         : 无
 *******************************************************************************/
uint8_t OnBoard_SendMsg(uint8_t registeredTaskID, uint8 event, uint8 state, void *data)
{
  SendMSG_t *msgPtr;

  if ( registeredTaskID != TASK_NO_TASK )
  {
    // Send the address to the task
    msgPtr = ( SendMSG_t * ) tmos_msg_allocate( sizeof(SendMSG_t));
    if ( msgPtr )
    {
      msgPtr->hdr.event = event;
      msgPtr->hdr.status = state;
      msgPtr->pData = data;
      tmos_msg_send( registeredTaskID, ( uint8 * ) msgPtr );
    }
    return ( SUCCESS );
  }
  else
  {
    return ( FAILURE );
  }
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
  if ( g_Enable_Status.rf == TRUE ) {  // RF心跳包
    OnBoard_SendMsg(RFTaskId, HEARTBEAT_MESSAGE, 1, NULL);  // RF心跳包事件
  }
  if (g_Ready_Status.keyboard_key_data == TRUE) {    // 产生键盘数据
    g_Ready_Status.keyboard_key_data = FALSE;
    TP78_Idle_Clr();
    if ( EnterPasskey_flag == TRUE ) { // 处理输入配对密码
      res = KEYBOARD_EnterNumber(&BLE_Passkey, "Passkey=?", "Send!", 6);
      if ( res == 0 ) {
        OLED_UI_add_CANCELINFO_delay_task(3000);
        EnterPasskey_flag = FALSE;
        OnBoard_SendMsg(hidEmuTaskId, PASSKEY_MESSAGE, 1, NULL);
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
#if (defined HAL_TPM) && (HAL_TPM == TRUE) && (defined HAL_HW_I2C) && (HAL_HW_I2C == TRUE)
      if (g_keyboard_status.enter_cfg == FALSE) {
        TPM_notify_key_data();
      }
#endif
      if ( KEYBOARD_Custom_Function() ) { // 普通按键,带有Fn键处理信息则不产生键盘事件
        if ( g_Ready_Status.usb == TRUE ) {
          OnBoard_SendMsg(usbTaskID, KEY_MESSAGE, 1, NULL);  // USB键盘事件
        } else if ( g_Ready_Status.ble == TRUE ) {
          OnBoard_SendMsg(hidEmuTaskId, KEY_MESSAGE, 1, NULL);  // 蓝牙键盘事件
        } else if ( g_Enable_Status.rf == TRUE ) {
          OnBoard_SendMsg(RFTaskId, KEY_MESSAGE, 1, NULL);  // RF键盘事件
        }
      }
      if ( g_Ready_Status.keyboard_mouse_data == TRUE ) { // 发送键盘鼠标数据
        g_Ready_Status.keyboard_mouse_data = FALSE;
        tmos_memset(&HIDMouse[1], 0, 3);   // 只按左中右键没有其他操作
        if ( g_Ready_Status.usb == TRUE ) {
          OnBoard_SendMsg(usbTaskID, MOUSE_MESSAGE, 1, NULL);  // USB鼠标事件
        } else if ( g_Ready_Status.ble == TRUE ) {
          OnBoard_SendMsg(hidEmuTaskId, MOUSE_MESSAGE, 1, NULL);  //蓝牙鼠标事件
        } else if ( g_Enable_Status.rf == TRUE ) {
          OnBoard_SendMsg(RFTaskId, MOUSE_MESSAGE, 1, NULL);  // RF鼠标事件
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
      if ( g_Ready_Status.usb == TRUE ) {
        OnBoard_SendMsg(usbTaskID, MOUSE_MESSAGE, 1, NULL);  // 蓝牙鼠标事件
      } else if ( g_Ready_Status.ble == TRUE ) {
        OnBoard_SendMsg(hidEmuTaskId, MOUSE_MESSAGE, 1, NULL);  // 蓝牙鼠标事件
      } else if ( g_Enable_Status.rf == TRUE ) {
        OnBoard_SendMsg(RFTaskId, MOUSE_MESSAGE, 1, NULL);  // RF鼠标事件
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
  if (TPINT_GPIO_(ReadPortPin)( TPINT_Pin ) == 0 && g_Enable_Status.tp == TRUE) {    // 发送小红点鼠标数据
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
      if ( g_Ready_Status.usb == TRUE ) {
        OnBoard_SendMsg(usbTaskID, MOUSE_MESSAGE, 1, NULL);  // USB鼠标事件
      } else if ( g_Ready_Status.ble == TRUE ) {
        OnBoard_SendMsg(hidEmuTaskId, MOUSE_MESSAGE, 1, NULL);  //蓝牙鼠标事件
      } else if ( g_Enable_Status.rf == TRUE ) {
        OnBoard_SendMsg(RFTaskId, MOUSE_MESSAGE, 1, NULL);  // RF鼠标事件
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
    if ( g_Ready_Status.usb == TRUE ) {
      OnBoard_SendMsg(usbTaskID, MOUSE_MESSAGE, 1, NULL);  // USB鼠标事件
    } else if ( g_Ready_Status.ble == TRUE ) {
      OnBoard_SendMsg(hidEmuTaskId, MOUSE_MESSAGE, 1, NULL);  // 蓝牙鼠标事件
    } else if ( g_Enable_Status.rf == TRUE ) {
      OnBoard_SendMsg(RFTaskId, MOUSE_MESSAGE, 1, NULL);  // RF鼠标事件
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
  if ( g_Ready_Status.usb == TRUE ) {
    OnBoard_SendMsg(usbTaskID, VOL_MESSAGE, 1, NULL);  // USB音量事件
  } else if ( g_Ready_Status.ble == TRUE ) {
    OnBoard_SendMsg(hidEmuTaskId, VOL_MESSAGE, 1, NULL);  //蓝牙音量事件
  } else if ( g_Enable_Status.rf == TRUE ) {
    OnBoard_SendMsg(RFTaskId, VOL_MESSAGE, 1, NULL);  // RF音量事件
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
  if (++EP_counter > (400 / 5)) {  // 400ms更新一次(控制snake速度)
    EP_counter = 0;
    MoveSnake();
  }
}

/*******************************************************************************
 * Function Name  : SW_OLED_LEDStatus_Process
 * Description    : NumLock/CapsLock状态检测OLED处理函数
 * Input          : 无
 * Return         : 无
 *******************************************************************************/
__attribute__((weak)) void SW_OLED_LEDStatus_Process(void)
{
  uint8_t change_flag = 0;

  if ( g_Ready_Status.usb == TRUE ) {
    if ( g_CapsLock_LEDOn_Status.usb != g_CapsLock_LEDOn_Status.ui ) {
      g_CapsLock_LEDOn_Status.ui = g_CapsLock_LEDOn_Status.usb;
      change_flag |= 0x2;
#ifdef OLED_0_91
      OLED_UI_ShowCapslock(56, 1, g_CapsLock_LEDOn_Status.ui);
#endif
    }
    if ( g_NumLock_LEDOn_Status.usb != g_NumLock_LEDOn_Status.ui ) {
      g_NumLock_LEDOn_Status.ui = g_NumLock_LEDOn_Status.usb;
      change_flag |= 0x1;
    }
  } else if ( g_Ready_Status.ble == TRUE ) {
    if ( g_CapsLock_LEDOn_Status.ble != g_CapsLock_LEDOn_Status.ui ) {
      g_CapsLock_LEDOn_Status.ui = g_CapsLock_LEDOn_Status.ble;
      change_flag |= 0x2;
#ifdef OLED_0_91
      OLED_UI_ShowCapslock(56, 1, g_CapsLock_LEDOn_Status.ui);
#endif
    }
    if ( g_NumLock_LEDOn_Status.ble != g_NumLock_LEDOn_Status.ui ) {
      g_NumLock_LEDOn_Status.ui = g_NumLock_LEDOn_Status.ble;
      change_flag |= 0x1;
    }
  } else if ( g_Enable_Status.rf == TRUE ) {
    if ( g_CapsLock_LEDOn_Status.rf != g_CapsLock_LEDOn_Status.ui ) {
      g_CapsLock_LEDOn_Status.ui = g_CapsLock_LEDOn_Status.rf;
      change_flag |= 0x2;
#ifdef OLED_0_91
      OLED_UI_ShowCapslock(56, 1, g_CapsLock_LEDOn_Status.ui);
#endif
    }
    if ( g_NumLock_LEDOn_Status.rf != g_NumLock_LEDOn_Status.ui ) {
      g_NumLock_LEDOn_Status.ui = g_NumLock_LEDOn_Status.rf;
      change_flag |= 0x1;
    }
  } else {
    return;
  }
#ifdef OLED_0_66
  if (change_flag & 0x1) {
    if (g_NumLock_LEDOn_Status.ui == TRUE) {
      OLED_UI_slot_add((uint8_t*)UI_Slot_Icon[OLED_UI_ICON_NUMLOCK_IDX]);
    } else {
      OLED_UI_slot_delete((uint8_t*)UI_Slot_Icon[OLED_UI_ICON_NUMLOCK_IDX]);
    }
  }
  if (change_flag & 0x2) {
    if (g_CapsLock_LEDOn_Status.ui == TRUE) {
      OLED_UI_slot_add((uint8_t*)UI_Slot_Icon[OLED_UI_ICON_CAPSLOCK_IDX]);
    } else {
      OLED_UI_slot_delete((uint8_t*)UI_Slot_Icon[OLED_UI_ICON_CAPSLOCK_IDX]);
    }
  }
  OLED_UI_add_default_task(OLED_UI_FLAG_DRAW_SLOT);
#endif
}

/*******************************************************************************
 * Function Name  : SW_OLED_ConnectionStatus_Process
 * Description    : 检测USB/蓝牙/RF连接状态：连接状态改变OLED显示
 * Input          : 无
 * Return         : 无
 *******************************************************************************/
__attribute__((weak)) void SW_OLED_ConnectionStatus_Process(void)
{
  if (g_Ready_Status.usb_l != g_Ready_Status.usb) {
    TP78_Idle_Clr();
    g_Ready_Status.usb_l = g_Ready_Status.usb;
    if ( g_Ready_Status.usb == TRUE ) {
#ifdef OLED_0_91
      OLED_UI_add_SHOWSTRING_task(8, 0, "USB");
#endif
#ifdef OLED_0_66
      OLED_UI_slot_active((uint8_t*)UI_Slot_Icon[OLED_UI_ICON_USB_UNCONNECT_IDX],
                          (uint8_t*)UI_Slot_Icon[OLED_UI_ICON_USB_IDX]);
#endif
    } else {
#ifdef OLED_0_91
      OLED_UI_add_SHOWSTRING_task(8, 0, "   ");
#endif
#ifdef OLED_0_66
      OLED_UI_slot_active((uint8_t*)UI_Slot_Icon[OLED_UI_ICON_USB_IDX],
                          (uint8_t*)UI_Slot_Icon[OLED_UI_ICON_USB_UNCONNECT_IDX]);
#endif
    }
#ifdef OLED_0_66
    OLED_UI_add_default_task(OLED_UI_FLAG_DRAW_SLOT);
#endif
  } else if (g_Ready_Status.ble_l != g_Ready_Status.ble) {
    TP78_Idle_Clr();
    g_Ready_Status.ble_l = g_Ready_Status.ble;
    if ( g_Ready_Status.ble == TRUE ) {
#ifdef OLED_0_91
      OLED_UI_add_SHOWSTRING_task(38, 0, "BLE");
#endif
#ifdef OLED_0_66
      OLED_UI_slot_active((uint8_t*)UI_Slot_Icon[OLED_UI_ICON_BLE_UNCONNECT1_IDX + DeviceAddress[5] - 1],
                          (uint8_t*)UI_Slot_Icon[OLED_UI_ICON_BLE1_IDX + DeviceAddress[5] - 1]);
#endif
    } else {
#ifdef OLED_0_91
      OLED_UI_add_SHOWSTRING_task(38, 0, "   ");
#endif
#ifdef OLED_0_66
      OLED_UI_slot_active((uint8_t*)UI_Slot_Icon[OLED_UI_ICON_BLE1_IDX + DeviceAddress[5] - 1],
                          (uint8_t*)UI_Slot_Icon[OLED_UI_ICON_BLE_UNCONNECT1_IDX + DeviceAddress[5] - 1]);
#endif
      if ( EnterPasskey_flag == TRUE ) {  // 打断输入配对状态
        EnterPasskey_flag = FALSE;
        BLE_Passkey = 0;
        OLED_UI_add_SHOWINFO_task("Close!");
        OLED_UI_add_CANCELINFO_delay_task(2000);
      }
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
      OLED_UI_slot_active((uint8_t*)UI_Slot_Icon[OLED_UI_ICON_RF_UNCONNECT_IDX],
                          (uint8_t*)UI_Slot_Icon[OLED_UI_ICON_RF_IDX]);
#endif
    } else {
#ifdef OLED_0_91
      OLED_UI_add_SHOWSTRING_task(41, 0, "  ");
#endif
#ifdef OLED_0_66
      OLED_UI_slot_active((uint8_t*)UI_Slot_Icon[OLED_UI_ICON_RF_IDX],
                          (uint8_t*)UI_Slot_Icon[OLED_UI_ICON_RF_UNCONNECT_IDX]);
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
    if ( g_Ready_Status.usb == TRUE ) {
      OnBoard_SendMsg(usbTaskID, MOUSE_MESSAGE, 1, NULL);  // USB鼠标事件
    } else if ( g_Ready_Status.ble == TRUE ) {
      OnBoard_SendMsg(hidEmuTaskId, MOUSE_MESSAGE, 1, NULL);  //蓝牙鼠标事件
    } else if ( g_Enable_Status.rf == TRUE ) {
      OnBoard_SendMsg(RFTaskId, MOUSE_MESSAGE, 1, NULL);  // RF鼠标事件
    }
  }
}

/*******************************************************************************
 * Function Name  : ble_status_callback
 * Description    : 蓝牙状态回调函数
 * Input          : None
 * Return         : None
 *******************************************************************************/
void ble_status_callback( uint8_t code, uint32_t status ){
//  OLED_PRINT("%d %d", code, status);
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
  uint32_t irq_status;
  SYS_DisableAllIrq( &irq_status );
  Calibration_LSI( Level_64 ); // Level_64
  SYS_RecoverIrq( irq_status );
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
uint32_t Lib_Read_Flash(uint32_t addr, uint32_t num, uint32_t *pBuf)
{
  EEPROM_READ( addr, pBuf, num * 4 );
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
uint32_t Lib_Write_Flash(uint32_t addr, uint32_t num, uint32_t *pBuf)
{
  EEPROM_ERASE( addr, num * 4);
  EEPROM_WRITE( addr, pBuf, num * 4 );
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

//    FLASH_EEPROM_CMD( CMD_FLASH_ROM_LOCK, 0, NULL, 0 ); // 解锁Flash
    tmos_memset(&cfg, 0, sizeof(bleConfig_t));
    cfg.MEMAddr = (uint32_t)MEM_BUF; 
    cfg.MEMLen = (uint32_t)BLE_MEMHEAP_SIZE; //Ram
    cfg.BufMaxLen = (uint32_t)BLE_BUFF_MAX_LEN; // 蓝牙通信的一个包的最大长度（字节数），其中有7字节为固定的协议开销
    cfg.BufNumber = (uint32_t)BLE_BUFF_NUM; //控制器缓存区能接受的包的个数
    cfg.TxNumEvent = (uint32_t)BLE_TX_NUM_EVENT; //临界事件中发送的包的个数，≤BLE_BUFF_NUM
    cfg.TxPower = (uint32_t)BLE_TX_POWER; //蓝牙发射功率
#if(defined(BLE_SNV)) && (BLE_SNV == TRUE)
    switch (DeviceAddress[5])
    {
      case 1: {
        cfg.SNVAddr = (uint32_t)BLE_SNV_ADDR_D1;
        break;
      }
      case 2: {
        cfg.SNVAddr = (uint32_t)BLE_SNV_ADDR_D2;
        break;
      }
      case 3: {
        cfg.SNVAddr = (uint32_t)BLE_SNV_ADDR_D3;
        break;
      }
      case 4: {
        cfg.SNVAddr = (uint32_t)BLE_SNV_ADDR_D4;
        break;
      }
      default: {
        cfg.SNVAddr = (uint32_t)BLE_SNV_ADDR; //蓝牙绑定信息flash起始地址 每个绑定设备需要256字节空间
        break;
      }
    }
    cfg.readFlashCB = Lib_Read_Flash;
    cfg.writeFlashCB = Lib_Write_Flash;
#endif
//    cfg.staCB = ble_status_callback;
#if(CLK_OSC32K)
    cfg.SelRTCClock = (uint32_t)CLK_OSC32K;
#endif
    cfg.ConnectNumber = (PERIPHERAL_MAX_CONNECTION & 3) | (CENTRAL_MAX_CONNECTION << 2); // PERIPHERAL_MAX_CONNECTION可以同时和多少个主机连接，
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
//            cfg.MacAddr[i] = MacAddr[i]; // 使用芯片mac地址
            cfg.MacAddr[i] = DeviceAddress[i];
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
#if (defined HAL_TPM) && (HAL_TPM == TRUE) && (defined HAL_HW_I2C) && (HAL_HW_I2C == TRUE)
  static uint8_t tpm_cnt = 0;
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

  // 键盘处理事件
  if ( events & HAL_KEYBOARD_EVENT )
  {
    if ( g_Enable_Status.paintedegg == FALSE ) {
#if (defined HAL_KEYBOARD) && (HAL_KEYBOARD == TRUE)
      HID_KEYBOARD_Process();
#endif
    } else {    // 彩蛋模式
#if (defined SW_PAINTEDEGG) && (SW_PAINTEDEGG == TRUE)
      SW_PaintedEgg_Process();
#endif
    }
#if (ROW_SCAN_MODE)
    tmos_start_task( halTaskID, HAL_KEYBOARD_EVENT, MS1_TO_SYSTEM_TIME(5) ); // 处理键盘
#else
    tmos_start_task( halTaskID, HAL_KEYBOARD_EVENT, MS1_TO_SYSTEM_TIME(5) ); // 处理键盘
#endif
    return events ^ HAL_KEYBOARD_EVENT;
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
        if (g_Enable_Status.tp && g_Enable_Status.touchbar_button) {  // 通过小红点使能和触摸条配置联合控制
          MPR121_alg_judge_touchbar(ALG_ENABLE_TOUCHBAR_SLIDE | ALG_ENABLE_TOUCHBAR_TOUCH);
        } else {
          MPR121_alg_judge_touchbar(ALG_ENABLE_TOUCHBAR_SLIDE);
        }
        MPR121_Post_Operation();
      }
    } else {
#if !(defined HAL_MPR121_CAPMOUSE) || (HAL_MPR121_CAPMOUSE == FALSE)
      MPR121_update_data();   // update mpr121 status
#endif
    }
#endif
#if ((defined HAL_MPR121_CAPMOUSE) && (HAL_MPR121_CAPMOUSE == TRUE)) || ((defined HAL_MPR121_TOUCHBAR) && (HAL_MPR121_TOUCHBAR == TRUE))
    HW_TouchBar_Process();
#endif
    tmos_start_task( halTaskID, MPR121_EVENT, MS1_TO_SYSTEM_TIME(MPR121_TASK_PERIOD) ); // (MPR121_TASK_PERIOD)ms控制周期
    return events ^ MPR121_EVENT;
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

  // OLED UI事件
  if ( events & OLED_UI_EVENT )
  {
#if (defined HAL_OLED) && (HAL_OLED == TRUE)
    OLED_UI_draw_thread_callback();
#endif
    tmos_start_task( halTaskID, OLED_UI_EVENT, MS1_TO_SYSTEM_TIME(1000/g_oled_fresh_rate) ); // 根据OLED刷新率控制
    return events ^ OLED_UI_EVENT;
  }

  // 定时主循环事件
  if ( events & MAIN_CIRCULATION_EVENT)
  {
#if (defined HAL_WDG) && (HAL_WDG == TRUE)
    WWDG_SetCounter(0); // 喂狗
#endif
    ++idle_cnt;
    ++sys_time;
    if (idle_cnt == IDLE_MAX_PERIOD) {  // 进入idle
#if (defined HAL_OLED) && (HAL_OLED == TRUE)
      OLED_UI_idle(1);
#endif
    } else if (idle_cnt >= LP_MAX_PERIOD) {  // 进入低功耗模式
#if (defined HAL_OLED) && (HAL_OLED == TRUE)
      OLED_UI_idle(0);
      OLED_Clr(0, 2, 64, 5);  // 立即执行 - 后续进入低功耗
#endif
      idle_cnt = 0;
      if (g_Ready_Status.usb == TRUE) g_lp_type = lp_idle_mode;
      else g_lp_type = lp_shutdown_mode;
      GotoLowpower(g_lp_type);
    }
    // OLED信息更新处理
#if (defined HAL_OLED) && (HAL_OLED == TRUE)
    SW_OLED_ConnectionStatus_Process();
    SW_OLED_LEDStatus_Process();
#endif
    tmos_start_task( halTaskID, MAIN_CIRCULATION_EVENT, MS1_TO_SYSTEM_TIME(SYS_PERIOD) ); // SYS_PERIOD周期
    return events ^ MAIN_CIRCULATION_EVENT;
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

  // 电池ADC检测事件
  if ( events & BATTERY_EVENT )
  {
#if (defined HAL_BATTADC) && (HAL_BATTADC == TRUE)
    HW_Battery_Process();
#endif
    tmos_start_task( halTaskID, BATTERY_EVENT, MS1_TO_SYSTEM_TIME(3000) );  // 3s更新采样值
    return events ^ BATTERY_EVENT;
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

  // 马达停止事件
  if ( events & MOTOR_STOP_EVENT )
  {
    MOTOR_STOP();
    return events ^ MOTOR_STOP_EVENT;
  }

  // 扩展模块事件
  if ( events & TPM_EVENT )
  {
#if (defined HAL_TPM) && (HAL_TPM == TRUE) && (defined HAL_HW_I2C) && (HAL_HW_I2C == TRUE)
    if (g_keyboard_status.enter_cfg == FALSE) {
      if (tpm_cnt == 1) { // x2 period
        TPM_scan();
        tpm_cnt = 0;
      }
      TPM_process_data();
      tpm_cnt++;
    }
#endif
    tmos_start_task(halTaskID, TPM_EVENT, MS1_TO_SYSTEM_TIME(10));  // 10ms
    return events ^ TPM_EVENT;
  }

#ifdef DEBUG
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
#endif

  return 0;
}

/*******************************************************************************
 * Function Name  : FLASH_Init
 * Description    : dataflash文件系统初始化
 * Input          : None
 * Return         : None
 *******************************************************************************/
void FLASH_Init(void)
{
  uint16_t tmp;

  HAL_Fs_Init();  // Fs先初始化

  HAL_Fs_Read_keyboard_cfg(FS_LINE_WORK_MODE, 1, &tmp); // 模式读取
  DATAFLASH_Read_DeviceID();
#if 1
  switch (tmp) {
    case BLE_WORK_MODE:
      g_Enable_Status.ble = TRUE;
      g_Ready_Status.ble_l = TRUE;  // 初始化显示更新
      OLED_UI_add_SHOWINFO_task("BLE Mode");
      OLED_UI_add_CANCELINFO_delay_task(2000);
      break;
    case RF_WORK_MODE:
      g_Enable_Status.rf = TRUE;
      g_Ready_Status.rf_l = TRUE; // 初始化显示更新
      OLED_UI_add_SHOWINFO_task("RF Mode");
      OLED_UI_add_CANCELINFO_delay_task(2000);
      break;
    default:
      g_Enable_Status.usb = TRUE;
      g_Ready_Status.usb_l = TRUE;  // 初始化显示更新
      OLED_UI_add_SHOWINFO_task("USB Mode");
      OLED_UI_add_CANCELINFO_delay_task(2000);
  }
#endif
#if (defined HAL_WS2812_PWM) && (HAL_WS2812_PWM == TRUE)
  DATAFLASH_Read_LEDStyle( );
#endif
#if (defined HAL_RF) && (HAL_RF == TRUE) && !(defined TEST)
  DATAFLASH_Read_RFfreqlevel();
#endif
#if (defined HAL_WS2812_PWM) && (HAL_WS2812_PWM == TRUE)
  HAL_Fs_Read_keyboard_cfg(FS_LINE_LED_BRIGHTNESS, 1, &tmp);
  g_LED_brightness = tmp;
#endif
#if ((defined HAL_PS2) && (HAL_PS2 == TRUE)) || ((defined HAL_I2C_TP) && (HAL_I2C_TP == TRUE))
  HAL_Fs_Read_keyboard_cfg(FS_LINE_TP_SPEED_DIV, 1, &tmp);
  g_TP_speed_div = (tmp == 0 ? 1 : tmp);
#endif
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
void HAL_Init(void)
{
  uint16_t tmp;
  char debug_info[128] = "OK";

  DelayMs(20);
  halTaskID = TMOS_ProcessEventRegister( HAL_ProcessEvent );
  HAL_TimeInit();
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
  WS2812_PWM_Init( );
#endif
#if (defined HAL_HW_I2C) && (HAL_HW_I2C == TRUE)
  HW_I2C_Init( );
#endif
#if (defined HAL_BATTADC) && (HAL_BATTADC == TRUE)
  BATTERY_Init( );
#endif
#if (defined HAL_PS2) && (HAL_PS2 == TRUE)
  PS2_Init(debug_info, TRUE); // PS/2中断实现
#endif
#if (defined HAL_KEYBOARD) && (HAL_KEYBOARD == TRUE)
  KEYBOARD_Init( );
#endif
#if (defined HAL_MOTOR) && (HAL_MOTOR == TRUE)
  MOTOR_Init( );
#endif
#if (defined HAL_OLED) && (HAL_OLED == TRUE)
  HAL_OLED_Init( );
#endif
#if (defined HAL_HW_I2C) && (HAL_HW_I2C == TRUE)  // 最后初始化保证上电延迟
#if (defined HAL_TPM) && (HAL_TPM == TRUE)
  TPM_init(debug_info);
#endif
#if (defined HAL_I2C_TP) && (HAL_I2C_TP == TRUE)  // I2C_TP is used by HW I2C
  I2C_TP_Init(debug_info);
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
  if ( g_Ready_Status.fatfs == FALSE ) {  // 文件系统无法挂载
    strcpy(debug_info, "FATFS-FAIL");
  }
  PRINT("TP78 init status: %s\n", debug_info);
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

  /* start task */
  tmos_start_task( halTaskID, MAIN_CIRCULATION_EVENT, 10 ); // 主循环
#if (defined HAL_KEYBOARD) && (HAL_KEYBOARD == TRUE)
  tmos_start_task( halTaskID, HAL_KEYBOARD_EVENT, MS1_TO_SYSTEM_TIME(20) ); // 键盘事件
#endif
#if ((defined HAL_MPR121_CAPMOUSE) && (HAL_MPR121_CAPMOUSE == TRUE)) || ((defined HAL_MPR121_TOUCHBAR) && (HAL_MPR121_TOUCHBAR == TRUE))
  tmos_start_task( halTaskID, MPR121_EVENT, MS1_TO_SYSTEM_TIME(30) );  // MPR121
#endif
#if ((defined HAL_PS2) && (HAL_PS2 == TRUE)) || ((defined HAL_I2C_TP) && (HAL_I2C_TP == TRUE)) || ((defined HAL_MPR121_CAPMOUSE) && (HAL_MPR121_CAPMOUSE == TRUE))
  tmos_start_task( halTaskID, HAL_MOUSE_EVENT, MS1_TO_SYSTEM_TIME(40) ); // 鼠标事件
#endif
#if (defined HAL_WS2812_PWM) && (HAL_WS2812_PWM == TRUE)
  tmos_start_task( halTaskID, WS2812_EVENT, MS1_TO_SYSTEM_TIME(50) );  // 背光控制
#endif
#if (defined HAL_OLED) && (HAL_OLED == TRUE)
  tmos_start_task( halTaskID, OLED_UI_EVENT, MS1_TO_SYSTEM_TIME(60) );  // OLED UI
#endif
#if ((defined HAL_HW_I2C) && (HAL_HW_I2C == TRUE)) && ((defined HAL_TPM) && (HAL_TPM == TRUE))
  tmos_start_task( halTaskID, TPM_EVENT, MS1_TO_SYSTEM_TIME(30) );  // 扩展模块
#endif
#if 0
  tmos_start_task( halTaskID, HAL_TEST_EVENT, 10 );
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
