/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : ChnMasterOG, WCH
 * Version            : V1.0
 * Date               : 2022/11/08
 * Description        :
 * Copyright (c) 2023 ChnMasterOG
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

/******************************************************************************/
/* 头文件包含 */
#include "CONFIG.h"
#include "HAL.h"

/*********************************************************************
 * GLOBAL TYPEDEFS
 */
tmosTaskID RFtaskID;
uint8_t LEDOUT_DATA[2] = {0, 0};    // index 0 - numlock, index 1 - capslock
uint32_t heartbeat_cnt = 0;
BOOL lastevent[3] = {FALSE, FALSE, FALSE};

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
    uint8_t err = 0;
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
#if 0
                uint8_t i;
                PRINT("tx recv,rssi:%d\n", (int8_t)rxBuf[0]);
                PRINT("len:%d-", rxBuf[1]);
                if (rxBuf[1] <= sizeof(HID_dat)) {
                    if (rxBuf[2] == 0x7A) {   //jump to boot
                        PRINT("jump to boot\n");
                        APPJumpBoot();
                    } else {
                        for(i = 0; i < rxBuf[1]; i++) {
                            PRINT("%x ", rxBuf[i + 2]);
                            *((uint8_t*)&HID_dat + i) = rxBuf[i + 2];
                        }
                        if (g_Ready_Status.usb == TRUE) {
                            tmos_set_event(usbTaskID, 1<<HID_dat.label);
                        }
                        PRINT("\n");
                    }
                } else {
                    PRINT("receive err!\n");
                }
#endif
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
                err = 1;
            }
            else if(crc == 2)
            {
                PRINT("match type error\n");
                err = 2;
            }
            else
            {
                uint8_t i;
                PRINT("rx recv, rssi: %d\n", (int8_t)rxBuf[0]);
                PRINT("len:%d-", rxBuf[1]);
                if (rxBuf[1] <= sizeof(HID_dat)) {
                    if (rxBuf[2] == 0x7A) {   //jump to boot
                        PRINT("jump to boot\n");
                        APPJumpBoot();
                    } else if (rxBuf[2] == 0x78) {   //heartbeat
                        heartbeat_cnt = 0;
                    } else {
                        for(i = 0; i < rxBuf[1]; i++) {
                            PRINT("%x ", rxBuf[i + 2]);
                            *((uint8_t*)&HID_dat + i) = rxBuf[i + 2];
                        }
                        if (USB_Ready == TRUE) {
                            lastevent[1<<(HID_dat.label - 1)] = TRUE;
                            tmos_set_event(usbTaskID, 1<<(HID_dat.label - 1));
                        }
                        PRINT("\n");
                    }
                } else {
                    PRINT("receive err!\n");
                    err = 3;
                }
            }
            if (err == 0)
                tmos_set_event(RFtaskID, SBP_RF_RF_RX_EVT);
            break;
        }
        case RX_MODE_TX_FINISH:
        {
            tmos_set_event(RFtaskID, SBP_RF_RF_RX_EVT);
            break;
        }
        case RX_MODE_TX_FAIL:
        {
            break;
        }
    }
    PRINT("STA: %x\n", sta);
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
    if(events & SYS_EVENT_MSG)
    {
        uint8_t *pMsg;

        if((pMsg = tmos_msg_receive(task_id)) != NULL)
        {
            // Release the TMOS message
            tmos_msg_deallocate(pMsg);
        }
        // return unprocessed events
        return (events ^ SYS_EVENT_MSG);
    }
//    if(events & SBP_RF_START_DEVICE_EVT)
//    {
//        tmos_start_task(RFtaskID, SBP_RF_PERIODIC_EVT, 1000);
//        return events ^ SBP_RF_START_DEVICE_EVT;
//    }
//    if(events & SBP_RF_PERIODIC_EVT)
//    {
//        RF_Shut();
//        RF_Tx(TX_DATA, 2, 0xFF, 0xFF);
//        tmos_start_task(RFtaskID, SBP_RF_PERIODIC_EVT, 1000);
//        return events ^ SBP_RF_PERIODIC_EVT;
//    }
    if(events & SBP_RF_RF_RX_EVT)
    {
        uint8_t state;
        RF_Shut();
        state = RF_Rx(LEDOUT_DATA, 2, 0xFF, 0xFF);
        PRINT("RX mode.state = %x\n", state);
        return events ^ SBP_RF_RF_RX_EVT;
    }
    if(events & SBP_RF_LEDOUT_TX_EVT)
    {
        uint8_t state;
        RF_Shut();
        state = RF_Rx(LEDOUT_DATA, 2, 0xFF, 0xFF);
        PRINT("RX mode.state = %x\n", state);
        return events ^ SBP_RF_LEDOUT_TX_EVT;
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
    RFtaskID = TMOS_ProcessEventRegister(RF_ProcessEvent);
    rfConfig.accessAddress = 0x71764129; // 禁止使用0x55555555以及0xAAAAAAAA ( 建议不超过24次位反转，且不超过连续的6个0或1 )
    rfConfig.CRCInit = 0x555555;
    rfConfig.Channel = 8;
    rfConfig.Frequency = 2405000;
    rfConfig.LLEMode = LLE_MODE_AUTO | LLE_MODE_EX_CHANNEL; // 使能 LLE_MODE_EX_CHANNEL 表示 选择 rfConfig.Frequency 作为通信频点
    rfConfig.rfStatusCB = RF_2G4StatusCallBack;
    rfConfig.RxMaxlen = 251;
    state = RF_Config(&rfConfig);
    PRINT("rf 2.4g init: %x\n", state);
    { // RX mode
        state = RF_Rx(LEDOUT_DATA, 2, 0xFF, 0xFF);
        PRINT("RX mode.state = %x\n", state);
    }

    /* enable TMR0 */
    TMR0_TimerInit(FREQ_SYS / 1000);
    TMR0_ITCfg(ENABLE, TMR0_3_IT_CYC_END);
    PFIC_EnableIRQ(TMR0_IRQn);
    PFIC_SetPriority(TMR0_IRQn, 20);

    //	{ // TX mode
    //		tmos_set_event( RFtaskID , SBP_RF_PERIODIC_EVT );
    //	}
}

__INTERRUPT
__HIGH_CODE
void TMR0_IRQHandler(void)
{
  /*!< Timer 0 IRQ */
  if (TMR0_GetITFlag(TMR0_3_IT_CYC_END)) {
    /*!< Clear Pending flag */
    TMR0_ClearITFlag(TMR0_3_IT_CYC_END);

    /*!< Updata the ms count */
    if (heartbeat_cnt >= HEARTBEAT_CNT_MAX) {
        heartbeat_cnt = 0;
        tmos_memset(&HID_dat, 0, sizeof(HID_dat));
        if (lastevent[0] == TRUE) {
            lastevent[0] = FALSE;
            tmos_set_event(usbTaskID, USB_MOUSE_EVENT);
        }
        if (lastevent[1] == TRUE) {
            lastevent[1] = FALSE;
            tmos_set_event(usbTaskID, USB_KEYBOARD_EVENT);
        }
        if (lastevent[2] == TRUE) {
            lastevent[2] = FALSE;
            tmos_set_event(usbTaskID, USB_VOL_EVENT);
        }
    } else heartbeat_cnt++;
    /*!< Set timing time 1ms */
    R32_TMR0_CNT_END = GetSysClock() / 1000;
    R8_TMR0_CTRL_MOD = RB_TMR_ALL_CLEAR;
    R8_TMR0_CTRL_MOD = RB_TMR_COUNT_EN;

    /*!< Enable interrupt */
    TMR0_ITCfg(ENABLE, TMR0_3_IT_CYC_END);
  }
}

/******************************** endfile @ main ******************************/
