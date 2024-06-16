/********************************** (C) COPYRIGHT *******************************
 * File Name          : RF_PHY.h
 * Author             : ChnMasterOG, WCH
 * Version            : V1.0
 * Date               : 2022/11/13
 * Description        : RF 2.4G应用程序
 * Copyright (c) 2023 ChnMasterOG
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#ifndef RF_PHY_H
  #define RF_PHY_H

  #ifdef __cplusplus
  extern "C" {
  #endif

  #define SBP_RF_START_DEVICE_EVT      0x1
  #define SBP_RF_PERIODIC_EVT          0x2
  #define SBP_RF_RF_RX_EVT             0x4
  #define SBP_RF_KEYBOARD_REPORT_EVT   0x8
  #define SBP_RF_MOUSE_REPORT_EVT      0x10
  #define SBP_RF_VOL_REPORT_EVT        0x20
  #define SBP_RF_JUMPBOOT_REPORT_EVT   0x40
  #define SBP_RF_HEARTBEAT_REPORT_EVT  0x80
  #define SBP_RF_CHECK_SEND_REPORT_EVT 0x100
  #define SBP_RF_CHANNEL_HOP_TX_EVT    (1 << 3)
  #define SBP_RF_CHANNEL_HOP_RX_EVT    (1 << 4)

  #define LLE_MODE_ORIGINAL_RX         (0x80) //如果配置LLEMODE时加上此宏，则接收第一字节为原始数据（原来为RSSI）

  extern void RF_Init(void);

  void DATAFLASH_Read_RFfreqlevel(void);
  void DATAFLASH_Write_RFfreqlevel(uint8_t rf_freq_level);
  void DATAFLASH_Read_CheckACKms(void);
  void DATAFLASH_Write_CheckACKms(uint8_t check_ack_ms);

  extern tmosTaskID RFTaskId;

  #ifdef __cplusplus
  }
  #endif

#endif
