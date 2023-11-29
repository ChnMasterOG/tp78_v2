/********************************** (C) COPYRIGHT *******************************
 * File Name          : RF_PHY.c
 * Author             : ChnMasterOG, WCH
 * Version            : V1.0
 * Date               : 2022/11/13
 * Description        : RF 2.4G应用程序
 * Copyright (c) 2023 ChnMasterOG
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

/******************************************************************************/
/* 头文件包含 */
#include "HAL.h"
#include "CONFIG.h"

/*********************************************************************
 * GLOBAL TYPEDEFS
 */
tmosTaskID RFTaskId;
uint8_t JUMP_BOOT_DATA[1] = {0x7A}; // 规定发送0x7A接收器进BOOT
uint8_t HEARTBEAT_DATA[1] = {0x78}; // 规定发送0x78为心跳包
uint8_t TX_DATA[2] = {0xff, 0xff};
static uint8_t RF_freq_level = 0;  // 频段挡位

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*******************************************************************************
 * Function Name  : DATAFLASH_Read_RFfreq
 * Description    : 从DataFlash读取RF频段挡位
 * Input          : None
 * Return         : None
 *******************************************************************************/
void DATAFLASH_Read_RFfreqlevel(void)
{
  uint16_t freq_level;
  HAL_Fs_Read_keyboard_cfg(FS_LINE_RF_FREQ_LEVEL, 1, &freq_level);
  if (freq_level > 10) freq_level = 10;
  RF_freq_level = freq_level;
}

/*******************************************************************************
 * Function Name  : DATAFLASH_Read_RFfreq
 * Description    : 从DataFlash读取RF频段挡位
 * Input          : rf_freq_level: RF频段挡位
 * Return         : None
 *******************************************************************************/
void DATAFLASH_Write_RFfreqlevel(uint8_t rf_freq_level)
{
  uint16_t freq_level = rf_freq_level;
  HAL_Fs_Write_keyboard_cfg(FS_LINE_RF_FREQ_LEVEL, 1, &freq_level);
}

/*********************************************************************
 * @fn      RF_2G4StatusCallBack
 *
 * @brief   RF 状态回调，注意：不可在此函数中直接调用RF接收或者发送API，需要使用事件的方式调用
 *
 * @param   sta     - 状态类型
 * @param   crc     - crc校验结果
 * @param   rxBuf   - 数据buf指针
 *
 * @return  none
 */
void RF_2G4StatusCallBack(uint8_t sta, uint8_t crc, uint8_t *rxBuf)
{
    switch(sta)
    {
        case TX_MODE_TX_FINISH:
        {
            break;
        }
        case TX_MODE_TX_FAIL:
        {
            break;
        }
        case TX_MODE_RX_DATA:
        {
            RF_Shut();
            if(crc == 1)
            {
                PRINT("crc error\n");
            }
            else if(crc == 2)
            {
                PRINT("match type error\n");
            }
            else
            {
                PRINT("tx recv,rssi:%d\n", (int8_t)rxBuf[0]);
                if (rxBuf[1] == 2) {
                  switch (rxBuf[2]) {
                    case 0x3:
                      g_CapsLock_LEDOn_Status.rf = rxBuf[3] ? TRUE : FALSE;
                      break;
                    case 0x4:
                      g_NumLock_LEDOn_Status.rf = rxBuf[3] ? TRUE : FALSE;
                      break;
                    default:
                      break;
                  }
                }
            }
            break;
        }
        case TX_MODE_RX_TIMEOUT: // Timeout is about 200us
        {
            break;
        }
        case RX_MODE_RX_DATA:
        {
            if(crc == 1)
            {
                PRINT("crc error\n");
            }
            else if(crc == 2)
            {
                PRINT("match type error\n");
            }
            else
            {
                PRINT("rx recv, rssi: %d\n", (int8_t)rxBuf[0]);
                if (rxBuf[1] == 2) {
                  switch (rxBuf[2]) {
                    case 0x3:
                      g_CapsLock_LEDOn_Status.rf = rxBuf[3] ? TRUE : FALSE;
                      break;
                    case 0x4:
                      g_NumLock_LEDOn_Status.rf = rxBuf[3] ? TRUE : FALSE;
                      break;
                    default:
                      break;
                  }
                }
            }
            tmos_set_event(RFTaskId, SBP_RF_RF_RX_EVT);
            break;
        }
        case RX_MODE_TX_FINISH:
        {
            tmos_set_event(RFTaskId, SBP_RF_RF_RX_EVT);
            break;
        }
        case RX_MODE_TX_FAIL:
        {
            break;
        }
    }
    PRINT("STA: %x\n", sta);
}

/*******************************************************************************
 * Function Name  : RF_ProcessTMOSMsg
 * Description    : Process an incoming task message.
 * Input          : pMsg - message to process
 * Return         : tmosEvents
 *******************************************************************************/
static void RF_ProcessTMOSMsg( tmos_event_hdr_t *pMsg )
{
  switch ( pMsg->event )
  {
    case KEY_MESSAGE: {
        SendMSG_t *msg = (SendMSG_t *) pMsg;
        msg->hdr.status ? tmos_set_event( RFTaskId, SBP_RF_KEYBOARD_REPORT_EVT ) : 0;
        break;
    }
    case MOUSE_MESSAGE: {
        SendMSG_t *msg = (SendMSG_t *) pMsg;
        msg->hdr.status ? tmos_set_event( RFTaskId, SBP_RF_MOUSE_REPORT_EVT ) : 0;
        break;
    }
    case VOL_MESSAGE: {
        SendMSG_t *msg = (SendMSG_t *) pMsg;
        msg->hdr.status ? tmos_set_event( RFTaskId, SBP_RF_VOL_REPORT_EVT ) : 0;
        break;
    }
    case HEARTBEAT_MESSAGE: {
        SendMSG_t *msg = (SendMSG_t *) pMsg;
        msg->hdr.status ? tmos_set_event( RFTaskId, SBP_RF_HEARTBEAT_REPORT_EVT ) : 0;
        break;
    }
    default:
      break;
  }
}

/*********************************************************************
 * @fn      RF_ProcessEvent
 *
 * @brief   RF 事件处理
 *
 * @param   task_id - 任务ID
 * @param   events  - 事件标志
 *
 * @return  未完成事件
 */
uint16_t RF_ProcessEvent(uint8_t task_id, uint16_t events)
{
    if (events & SYS_EVENT_MSG)
    {
        uint8_t *pMsg;

        if((pMsg = tmos_msg_receive(task_id)) != NULL)
        {
            RF_ProcessTMOSMsg( (tmos_event_hdr_t *)pMsg );

            // Release the TMOS message
            tmos_msg_deallocate(pMsg);
        }
        // return unprocessed events
        return (events ^ SYS_EVENT_MSG);
    }
//    if (events & SBP_RF_START_DEVICE_EVT)
//    {
//        tmos_start_task(RFTaskId, SBP_RF_PERIODIC_EVT, 1000);
//        return events ^ SBP_RF_START_DEVICE_EVT;
//    }
//    if (events & SBP_RF_PERIODIC_EVT)
//    {
//        RF_Shut();
//        TX_DATA[0]+=2;
//        RF_Tx(TX_DATA, 10, 0xFF, 0xFF);
//        tmos_start_task(RFTaskId, SBP_RF_PERIODIC_EVT, 1000);
//        return events ^ SBP_RF_PERIODIC_EVT;
//    }
    if (events & SBP_RF_RF_RX_EVT)
    {
        uint8_t state;
        RF_Shut();
        state = RF_Rx(TX_DATA, 2, 0xFF, 0xFF);
        PRINT("RX mode.state = %x\n", state);
        return events ^ SBP_RF_RF_RX_EVT;
    }
    if (events & SBP_RF_MOUSE_REPORT_EVT)
    {
        RF_Shut();
        RF_Tx(HIDMouse - 1, HID_MOUSE_DATA_LENGTH + 1, 0xFF, 0xFF);
        return events ^ SBP_RF_MOUSE_REPORT_EVT;
    }
    if (events & SBP_RF_KEYBOARD_REPORT_EVT)
    {
        RF_Shut();
        RF_Tx(HIDKeyboard - 1, HID_KEYBOARD_DATA_LENGTH + 1, 0xFF, 0xFF);
        return events ^ SBP_RF_KEYBOARD_REPORT_EVT;
    }
    if (events & SBP_RF_VOL_REPORT_EVT)
    {
        RF_Shut();
        RF_Tx(HIDVolume - 1, HID_VOLUME_DATA_LENGTH + 1, 0xFF, 0xFF);
        return events ^ SBP_RF_VOL_REPORT_EVT;
    }
    if (events & SBP_RF_JUMPBOOT_REPORT_EVT)
    {
        RF_Shut();
        RF_Tx(JUMP_BOOT_DATA, 1, 0xFF, 0xFF);
        return events ^ SBP_RF_JUMPBOOT_REPORT_EVT;
    }
    if (events & SBP_RF_HEARTBEAT_REPORT_EVT)
    {
        RF_Shut();
        RF_Tx(HEARTBEAT_DATA, 1, 0xFF, 0xFF);
        return events ^ SBP_RF_HEARTBEAT_REPORT_EVT;
    }
    return 0;
}

/*********************************************************************
 * @fn      RF_Init
 *
 * @brief   RF 初始化
 *
 * @return  none
 */
void RF_Init(void)
{
    uint8_t    state;
    rfConfig_t rfConfig;

    tmos_memset(&rfConfig, 0, sizeof(rfConfig_t));
    RFTaskId = TMOS_ProcessEventRegister(RF_ProcessEvent);
    rfConfig.accessAddress = 0x71764129; // 禁止使用0x55555555以及0xAAAAAAAA ( 建议不超过24次位反转，且不超过连续的6个0或1 )
    rfConfig.CRCInit = 0x555555;
    rfConfig.Channel = 8;
    rfConfig.Frequency = 2400000 + (uint32)RF_freq_level * 2500;
    rfConfig.LLEMode = LLE_MODE_AUTO | LLE_MODE_EX_CHANNEL; // 使能 LLE_MODE_EX_CHANNEL 表示 选择 rfConfig.Frequency 作为通信频点
    rfConfig.rfStatusCB = RF_2G4StatusCallBack;
    rfConfig.RxMaxlen = 251;
    state = RF_Config(&rfConfig);
    PRINT("rf 2.4g init: %x\n", state);
}

/******************************** endfile @ main ******************************/
