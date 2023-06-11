/********************************** (C) COPYRIGHT *******************************
 * File Name          : HAL.h
 * Author             : ChnMasterOG, WCH
 * Version            : V1.1
 * Date               : 2022/11/13
 * Description        : HAL层通用头文件
 * Copyright (c) 2023 ChnMasterOG
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/



/******************************************************************************/
#ifndef __HAL_H
#define __HAL_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "CH58x_common.h"
//#define FIRST_USED        // 出产设置

/* Mouse HID data format */
typedef union {
    struct {
        unsigned char LeftBtn : 1;
        unsigned char RightBtn : 1;
        unsigned char MiddleBtn : 1;
        unsigned char Always1 : 1;
        unsigned char Xsignbit : 1;
        unsigned char Xspinbit : 1;
        unsigned char Xoverflow : 1;
        unsigned char Yoverflow : 1;
        signed char XMovement : 8;
        signed char YMovement : 8;
        signed char ZMovement : 8;
    };
    uint8_t data[4];
}Mouse_Data_t;

/* Keyboard HID data format */
typedef union {
    struct {
        unsigned char LeftCTRL : 1;
        unsigned char LeftShift : 1;
        unsigned char LeftAlt : 1;
        unsigned char LeftGUI : 1;
        unsigned char RightCTRL : 1;
        unsigned char RightShift : 1;
        unsigned char RightAlt : 1;
        unsigned char RightGUI : 1;
        unsigned char Reserved : 8;
        unsigned char Key1 : 8;
        unsigned char Key2 : 8;
        unsigned char Key3 : 8;
        unsigned char Key4 : 8;
        unsigned char Key5 : 8;
        unsigned char Key6 : 8;
        };
    uint8_t data[8];
}Keyboard_Data_t;

#define MouseDat        ((Mouse_Data_t*)HIDMouse)
#define KeyboardDat     ((Keyboard_Data_t*)HIDKeyboard)

#include "config.h"
#include "TP78_MPR121_LIB.h"
#include "RTC.h"
#include "SLEEP.h"	
#include "LED.h"
#include "KEY.h"
#include "BLE.h"
#include "FATFS.h"
#include "USB.h"
#include "PS2.h"
#include "HW_I2C.h"
#include "I2C_TP.h"
#include "MPR121.h"
#include "OLED.h"
#include "OLED_UI.h"
#include "RF_PHY.h"
#include "KEYBOARD.h"
#include "BATTERY.h"
#include "WS2812.h"
#include "CORE.h"
#include "TPM.h"

/* hal task Event */
#define MAIN_CIRCULATION_EVENT              0x0001
#define HAL_MOUSE_EVENT                     0x0002
#define HAL_KEYBOARD_EVENT                  0x0004
#define HAL_TOUCHBAR_EVENT                  0x0008
#define BATTERY_EVENT                       0x0010
#define WS2812_EVENT                        0x0020
#define OLED_UI_EVENT                       0x0040
#define USB_READY_EVENT                     0x0080
#define MPR121_EVENT                        0x0100
#define FEEDDOG_EVENT                       0x0200
#define MOTOR_STOP_EVENT                    0x0400
#define MODULE_INITIAL_EVENT                0x0800
#define LED_BLINK_EVENT                     0x1000
#define KEY_EVENT                           0x2000
#define HAL_REG_INIT_EVENT                  0x4000
#define HAL_TEST_EVENT                      0x8000

/* hal sys_message */
#define MESSAGE_UART                        0xA0    // UART message
#define UART0_MESSAGE                       (MESSAGE_UART|0 )    // UART0 message
#define UART1_MESSAGE                       (MESSAGE_UART|1 )    // UART1 message

#define USB_MESSAGE              		        0xB0    // USB message

#define HID_MOUSE_DATA_LENGTH               4
#define HID_KEYBOARD_DATA_LENGTH            8
#define HID_VOLUME_DATA_LENGTH              1
#define HID_DATA_LENGTH                     16

/* CodeFlash 基地址0x00000 */
/* 0x0000~0x4FFFF 预留给程序 */
#define CODEFLASH_ADDR_START                (0x50000)
#define CODEFLASH_LENGTH                    (0x18000)     // 结束地址0x67FFF(预留0x7FFF)

/* DataFlash 基地址0x70000 */
#if 0    // for old version
#define DATAFLASH_ADDR_CustomKey            (8*1024)      // 从8K地址开始存放键盘布局，map空余空间：0x4~0x210C
#define DATAFLASH_ADDR_Extra_CustomKey      (9*1024)      // 从9K地址开始存放键盘额外布局
#define DATAFLASH_ADDR_LEDStyle             (10*1024)     // 背光样式
#define DATAFLASH_ADDR_BLEDevice            (10*1024+4)   // 蓝牙默认连接设备编号
#define DATAFLASH_ADDR_RForBLE              (10*1024+8)   // 启动默认RF模式或者BLE模式
#define DATAFLASH_ADDR_MPR121_ALG_Param     (10*1024+12)  // MPR121算法参数存储
#endif

#define FEEDDOG_PERIOD                      25            // 单位1ms
#define IDLE_MAX_PERIOD                     (180 * (1000 / FEEDDOG_PERIOD)) // idle_cnt大于该值则进入屏保
#define LP_MAX_PERIOD                       (240 * (1000 / FEEDDOG_PERIOD)) // idle_cnt大于该值则进入低功耗模式

#define MOTOR_PIN                           GPIO_Pin_19
#define MOTOR_RUN()                         { GPIOB_SetBits( MOTOR_PIN ); }
#define MOTOR_STOP()                        { GPIOB_ResetBits( MOTOR_PIN ); }
#define MOTOR_Init()                        { GPIOB_SetBits( MOTOR_PIN ); GPIOB_ModeCfg( MOTOR_PIN, GPIO_ModeOut_PP_5mA ); MOTOR_STOP(); }
#define MOTOR_GO()                          { MOTOR_RUN(); tmos_start_task( halTaskID, MOTOR_STOP_EVENT, MS1_TO_SYSTEM_TIME(100) ); }

/* CapsLock LEDOn Status */
#define USB_CAPSLOCK_LEDON_BIT              0x1
#define BLE_CAPSLOCK_LEDON_BIT              0x2
#define RF_CAPSLOCK_LEDON_BIT               0x4
#define UI_CAPSLOCK_LEDON_BIT               0x80

typedef struct tag_uart_package
{
  tmos_event_hdr_t hdr;
  uint8            *pData;
} uartPacket_t;

typedef struct _CapsLock_LEDOn_Status_t
{
    uint8_t usb : 1;
    uint8_t ble : 1;
    uint8_t rf : 1;
    uint8_t reserved : 4;
    uint8_t ui : 1;
}CapsLock_LEDOn_Status_t;

typedef struct _Ready_Status_t
{
    uint8_t usb : 1;
    uint8_t ble : 1;
    uint8_t rf : 1;
    uint8_t usb_l : 1;
    uint8_t ble_l : 1;
    uint8_t rf_l : 1;
    uint8_t fatfs : 1;
    uint8_t keyboard_key_data : 1;
    uint8_t keyboard_mouse_data : 1;
    uint8_t ps2_data : 1;
    uint8_t i2ctp_data : 1;
}Ready_Status_t;

typedef struct _Enable_Status_t
{
    uint8_t ble : 1;
    uint8_t motor : 1;
    uint8_t tp : 1;
    uint8_t paintedegg : 1;
    uint8_t reserved : 4;
}Enable_Status_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */
extern UINT8 HID_DATA[HID_DATA_LENGTH];
extern UINT8* HIDMouse;
extern UINT8* HIDKeyboard;
extern UINT8* HIDVolume;

extern BOOL priority_USB;
extern tmosTaskID halTaskID;

extern CapsLock_LEDOn_Status_t g_CapsLock_LEDOn_Status;
extern Ready_Status_t g_Ready_Status;
extern Enable_Status_t g_Enable_Status;
extern uint8_t g_TP_speed_div;
extern enum LP_Type g_lp_type;
extern uint32_t sys_time;

/*********************************************************************
 * GLOBAL FUNCTIONS
 */

void HID_KEYBOARD_Process( void );
void HID_PS2TP_Process( void );
void HID_I2CTP_Process( void );
void HID_CapMouse_Process( void );
void HID_VOL_Process( void );
void SW_PaintedEgg_Process( void );
void SW_OLED_Capslock_Process( void );
void SW_OLED_UBStatus_Process( void );
void HW_Battery_Process( void );
void HW_WS2812_Process( void );
void HW_MODULE_DEMO_Process( void );
void HW_TouchBar_Process( void );

extern void HAL_Init( );
extern tmosEvents HAL_ProcessEvent( tmosTaskID task_id, tmosEvents events );
extern void CH58X_BLEInit( void );
extern uint16 HAL_GetInterTempValue( void );
extern void Lib_Calibration_LSI( void );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif
