/********************************** (C) COPYRIGHT *******************************
 * File Name          : rf_phy.h
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2018/11/12
 * Description        :
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#ifndef CENTRAL_H
#define CENTRAL_H

#ifdef __cplusplus
extern "C" {
#endif

#define SBP_RF_START_DEVICE_EVT    0x1
#define SBP_RF_PERIODIC_EVT        0x2
#define SBP_RF_RF_RX_EVT           0x4
#define SBP_RF_LEDOUT_TX_EVT       0x8

#define HEARTBEAT_CNT_MAX          20       // 单位：ms
#define LLE_MODE_ORIGINAL_RX       (0x80)   // 如果配置LLEMODE时加上此宏，则接收第一字节为原始数据（原来为RSSI）

extern tmosTaskID RFtaskID;
extern uint8_t LEDOUT_DATA[2];

extern void RF_Init(void);

#ifdef __cplusplus
}
#endif

#endif
