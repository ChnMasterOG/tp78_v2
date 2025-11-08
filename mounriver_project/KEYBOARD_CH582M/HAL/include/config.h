/********************************** (C) COPYRIGHT *******************************
 * File Name          : CONFIG.h
 * Author             : ChnMasterOG, WCH
 * Version            : V1.10
 * Date               : 2022/11/13
 * Description        : 配置说明及默认值，建议在工程配置里的预处理中修改当前值
 * Copyright (c) 2023 ChnMasterOG
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

/******************************************************************************/
#ifndef __CONFIG_H
#define __CONFIG_H

#define	ID_CH582M							0x82

#define CHIP_ID								ID_CH582M

#ifdef CH57xBLE_ROM
#include "CH58xBLE_ROM.H"
#else
#include "CH58xBLE_LIB.H"
#endif
/*********************************************************************
 【MAC】
 BLE_MAC                      - 是否自定义蓝牙Mac地址 ( 默认:FALSE - 使用芯片Mac地址 )，需要在main.c修改Mac地址定义

 【DCDC】
 DCDC_ENABLE                  - 是否使能DCDC ( 默认:FALSE )

 【SLEEP】
 HAL_SLEEP                    - 是否开启睡眠功能 ( 默认:FALSE )
 WAKE_UP_RTC_MAX_TIME         - 等待32M晶振稳定时间，根据不同睡眠类型取值可分为： 睡眠模式/下电模式 - 45(默认); 暂停模式 -	45; 空闲模式 - 5
 
 【TEMPERATION】
 TEM_SAMPLE                   - 是否打开根据温度变化校准的功能，单次校准耗时小于10ms( 默认:TRUE )
 
 【CALIBRATION】
 BLE_CALIBRATION_ENABLE       - 是否打开定时校准的功能，单次校准耗时小于10ms( 默认:TRUE )
 BLE_CALIBRATION_PERIOD       - 定时校准的周期，单位ms( 默认:120000 )
 
 【SNV】
 BLE_SNV                      - 是否开启SNV功能，用于储存绑定信息( 默认:TRUE )
 BLE_SNV_ADDR                 - SNV信息保存地址，使用data flash最后( 默认:0x77E00 )
                              - 如果配置了SNVNum参数，则需要对应修改Lib_Write_Flash函数内擦除的flash大小，大小为SNVBlock*SNVNum

 【RTC】
 CLK_OSC32K                   - RTC时钟选择，如包含主机角色必须使用外部32K( 默认:0 外部(32768Hz), 1：内部(32000Hz), 2：内部(32768Hz), 0x80: HSE(32MHz) )

 【MEMORY】
 BLE_MEMHEAP_SIZE             - 蓝牙协议栈使用的RAM大小，不小于6K ( 默认:(1024*6) )

 【DATA】
 BLE_BUFF_MAX_LEN             - 单个连接最大包长度( 默认:27 (ATT_MTU=23)，取值范围[27~251] )
 BLE_BUFF_NUM                 - 控制器缓存的包数量( 默认:5 )
 BLE_TX_NUM_EVENT             - 单个连接事件最多可以发多少个数据包( 默认:1 )
 BLE_TX_POWER                 - 发射功率( 默认:LL_TX_POWEER_0_DBM (0dBm) )
 
 【MULTICONN】
 PERIPHERAL_MAX_CONNECTION    - 最多可同时做多少从机角色( 默认:1 )
 CENTRAL_MAX_CONNECTION       - 最多可同时做多少主机角色( 默认:3 )
**********************************************************************/

/*********************************************************************
 * 默认配置值
 */
#ifndef FIRMWARE_VERSION
#define FIRMWARE_VERSION            "v2_1_12"
#endif
#ifndef BLE_MAC
#define BLE_MAC                     FALSE
#endif
#ifndef BLE_BOND_KEYBOARD
#define BLE_BOND_KEYBOARD           FALSE
#endif
#ifndef BLE_VOL
#define BLE_VOL                     TRUE
#endif
#ifndef BLE_DIAL
#define BLE_DIAL                    TRUE
#endif
#ifndef DCDC_ENABLE
#define DCDC_ENABLE                 FALSE
#endif
#ifndef HAL_SLEEP
#define HAL_SLEEP                   TRUE
#endif
#ifndef WAKE_UP_RTC_MAX_TIME
#define WAKE_UP_RTC_MAX_TIME        45
#endif
#ifndef HAL_KEY
#define HAL_KEY                     FALSE           // 板载按键
#endif
#ifndef HAL_LED
#define HAL_LED                     FALSE           // 板载LED
#endif
#ifndef HAL_WDG
#define HAL_WDG                     FALSE           // 看门狗 - 打开蓝牙连接会出问题
#endif
#ifndef HAL_USB
#define HAL_USB                     TRUE            // USB
#endif
#ifndef TP_Reverse
#define TP_Reverse                  TRUE            // 小红点是否反转
#endif
#ifndef HAL_PS2
#define HAL_PS2                     FALSE           // PS/2小红点
#endif
#ifndef HAL_KEYBOARD
#define HAL_KEYBOARD                TRUE            // 键盘
#endif
#ifndef HAL_OLED
#define HAL_OLED                    TRUE            // OLED
#endif
#ifndef HAL_BATTADC
#define HAL_BATTADC                 TRUE            // 电池ADC
#endif
#ifndef HAL_WS2812_PWM
#define HAL_WS2812_PWM              TRUE            // WS2812 PWM驱动
#endif
#ifndef HAL_MOTOR
#define HAL_MOTOR                   TRUE            // 震动马达
#endif
#ifndef HAL_RF
#define HAL_RF                      TRUE            // RF 2.4G
#endif
#ifndef HAL_HW_I2C
#define HAL_HW_I2C                  TRUE            // 硬件I2C
#endif
#ifndef SW_PAINTEDEGG
#define SW_PAINTEDEGG               TRUE            // 彩蛋
#endif
#ifndef HAL_I2C_TP
#define HAL_I2C_TP                  TRUE            // I2C小红点
#endif
#ifndef HAL_MPR121_CAPMOUSE
#define HAL_MPR121_CAPMOUSE         FALSE           // MPR121触摸板 - 注意：修改此处还需同步库文件修改
#endif
#ifndef HAL_MPR121_TOUCHBAR
#define HAL_MPR121_TOUCHBAR         TRUE            // MPR121触摸条 - 注意：修改此处还需同步库文件修改
#endif
#ifndef HAL_TPM
#define HAL_TPM                     TRUE            // 扩展模块
#endif
#ifndef TEM_SAMPLE
#define TEM_SAMPLE                  TRUE            // 温度采样
#endif
#ifndef BLE_CALIBRATION_ENABLE
#define BLE_CALIBRATION_ENABLE      TRUE            // BLE校准
#endif
#ifndef BLE_CALIBRATION_PERIOD
#define BLE_CALIBRATION_PERIOD      120000          // BLE校准周期(ms)
#endif
#ifndef BLE_SNV
#define BLE_SNV                     TRUE
#endif
#ifndef BLE_SNV_NUM
#define BLE_SNV_NUM                 1
#endif
#ifndef CUR_VERSION_ADDR
#define CUR_VERSION_ADDR            0x7B00
#endif
#ifndef LAST_VERSION_ADDR
#define LAST_VERSION_ADDR           0x7B80
#endif
#ifndef BLE_SNV_ADDR
#define BLE_SNV_ADDR                (0x77C00-FLASH_ROM_MAX_SIZE)
#define BLE_SNV_ADDR_D1             0x7C00
#define BLE_SNV_ADDR_D2             0x7D00
#define BLE_SNV_ADDR_D3             0x7E00
#define BLE_SNV_ADDR_D4             0x7F00
#endif
#ifndef CLK_OSC32K
#define CLK_OSC32K                  2               // 该项请勿在此修改，必须在工程配置里的预处理中修改，如包含主机角色必须使用外部32K
#endif
#ifndef BLE_MEMHEAP_SIZE
#define BLE_MEMHEAP_SIZE            (1024*5)        // (1024*6)
#endif
#ifndef BLE_DEVICE_NUM
#define BLE_DEVICE_NUM              4               // 记住蓝牙主机设备数目
#endif
#ifndef BLE_BUFF_MAX_LEN
#define BLE_BUFF_MAX_LEN            27              // ATT_MTU = BLE_BUFF_MAX_LEN - 4
#endif
#ifndef BLE_BUFF_NUM
#define BLE_BUFF_NUM                5
#endif
#ifndef BLE_TX_NUM_EVENT
#define BLE_TX_NUM_EVENT            1
#endif
#ifndef BLE_TX_POWER
#define BLE_TX_POWER                LL_TX_POWEER_0_DBM
#endif
#ifndef PERIPHERAL_MAX_CONNECTION
#define PERIPHERAL_MAX_CONNECTION   1
#endif
#ifndef CENTRAL_MAX_CONNECTION
#define CENTRAL_MAX_CONNECTION      3
#endif

#ifdef HAL_OLED
#define OLED_PRINT(X...)            OLED_UI_printf(X)
#else
#define OLED_PRINT(X...)
#endif

extern u32 MEM_BUF[BLE_MEMHEAP_SIZE / 4];
extern u8C MacAddr[6];

#endif

/******************************* endfile @ config ******************************/
