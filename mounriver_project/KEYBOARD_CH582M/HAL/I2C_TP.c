/********************************** (C) COPYRIGHT *******************************
 * File Name          : I2C_TP.c
 * Author             : ChnMasterOG
 * Version            : V1.0
 * Date               : 2023/1/2
 * Description        : i2c小红点驱动
 * Copyright (c) 2023 ChnMasterOG
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#include "HAL.h"

/*******************************************************************************
 * Function Name  : I2C_TP_Init
 * Description    : 初始化I2C小红点
 * Input          : buf - 错误信息
 * Return         : None
 *******************************************************************************/
void I2C_TP_Init(char* debug_info)
{
  uint8_t err;

  /* config PB15 as TP_INT */
  TPINT_GPIO_(SetBits)( TPINT_Pin );
  TPINT_GPIO_(ModeCfg)( TPINT_Pin, GPIO_ModeIN_PU );
#if 0
  TPINT_GPIO_(ITModeCfg)( TPINT_Pin, GPIO_ITMode_FallEdge );
  PFIC_EnableIRQ( GPIO_B_IRQn );  //TPINT_GPIO
#endif
  err = I2C_TP_SendCommand_Reset();
  if (err != 0) {
    strcpy(debug_info, "I2C_TP-ERR");
  }
}

/*******************************************************************************
 * Function Name  : I2C_TP_SendCommand_Sleep
 * Description    : I2C小红点发送Sleep命令
 * Input          : None
 * Return         : 0: success, others: failed
 *******************************************************************************/
uint8_t I2C_TP_SendCommand_Sleep(void)
{
  uint8_t err = 0;

  err += HW_I2C_WaitUntilTimeout((expression_func)I2C_GetFlagStatus, I2C_FLAG_BUSY, SET);
  I2C_GenerateSTART( ENABLE );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_MODE_SELECT, RESET);
  I2C_Send7bitAddress( I2C_TP_ADDR, I2C_Direction_Transmitter );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, RESET);
  I2C_SendData( 0x22 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_SendData( 0x00 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_SendData( 0x01 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_SendData( 0x08 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_GenerateSTOP( ENABLE );

  return err;
}

/*******************************************************************************
 * Function Name  : I2C_TP_SendCommand_Wakeup
 * Description    : I2C小红点发送Wakeup命令
 * Input          : None
 * Return         : 0: success, others: failed
 *******************************************************************************/
uint8_t I2C_TP_SendCommand_Wakeup(void)
{
  uint8_t err = 0;

  err += HW_I2C_WaitUntilTimeout((expression_func)I2C_GetFlagStatus, I2C_FLAG_BUSY, SET);
  I2C_GenerateSTART( ENABLE );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_MODE_SELECT, RESET);
  I2C_Send7bitAddress( I2C_TP_ADDR, I2C_Direction_Transmitter );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, RESET);
  I2C_SendData( 0x22 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_SendData( 0x00 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_SendData( 0x00 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_SendData( 0x08 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_GenerateSTOP( ENABLE );

  return err;
}

/*******************************************************************************
 * Function Name  : I2C_TP_SendCommand_Reset
 * Description    : I2C小红点发送Reset命令
 * Input          : None
 * Return         : 0: success, others: failed
 *******************************************************************************/
uint8_t I2C_TP_SendCommand_Reset(void)
{
  uint8_t err = 0;

  err += HW_I2C_WaitUntilTimeout((expression_func)I2C_GetFlagStatus, I2C_FLAG_BUSY, SET);
  I2C_GenerateSTART( ENABLE );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_MODE_SELECT, RESET);
  I2C_Send7bitAddress( I2C_TP_ADDR, I2C_Direction_Transmitter );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, RESET);
  I2C_SendData( 0x25 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_SendData( 0x00 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_SendData( 0x06 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_SendData( 0x00 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_SendData( 0x29 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_SendData( 0x77 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_SendData( 0x77 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_SendData( 0x77 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_GenerateSTOP( ENABLE );

  return err;
}

/*******************************************************************************
 * Function Name  : I2C_TP_SendCommand_EnterExitIdleMode
 * Description    : I2C小红点发送进入或退出idle命令
 * Input          : flag: 0x1 或  0x0
 * Return         : 0: success, others: failed
 *******************************************************************************/
uint8_t I2C_TP_SendCommand_EnterExitIdleMode(uint8_t flag)
{
  uint8_t err = 0;

  err += HW_I2C_WaitUntilTimeout((expression_func)I2C_GetFlagStatus, I2C_FLAG_BUSY, SET);
  I2C_GenerateSTART( ENABLE );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_MODE_SELECT, RESET);
  I2C_Send7bitAddress( I2C_TP_ADDR, I2C_Direction_Transmitter );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, RESET);
  I2C_SendData( 0x25 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_SendData( 0x00 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_SendData( 0x06 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_SendData( 0x00 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_SendData( 0x29 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_SendData( 0x06 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_SendData( 0x06 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_SendData( flag );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_GenerateSTOP( ENABLE );

  return err;
}

/*******************************************************************************
 * Function Name  : I2C_TP_ReadPacket
 * Description    : I2C小红点读取数据包
 * Input          : None
 * Return         : 0: success, others: failed
 *******************************************************************************/
uint8_t I2C_TP_ReadPacket(void)
{
  volatile uint8_t packet_check[4] = { 0x07, 0x00, 0x01, 0x0 };
  uint8_t err = 0;

  err += HW_I2C_WaitUntilTimeout((expression_func)I2C_GetFlagStatus, I2C_FLAG_BUSY, SET);
  I2C_GenerateSTART( ENABLE );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_MODE_SELECT, RESET);
  I2C_Send7bitAddress( I2C_TP_ADDR, I2C_Direction_Receiver );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED, RESET);
  I2C_GenerateSTOP(DISABLE);
  I2C_AcknowledgeConfig(ENABLE);
  /* packet check 0 */
  err += HW_I2C_WaitUntilTimeout((expression_func)I2C_GetFlagStatus, I2C_FLAG_RXNE, RESET);
  if (I2C_ReceiveData( ) != packet_check[0]) err = 1;
  /* packet check 1 */
  err += HW_I2C_WaitUntilTimeout((expression_func)I2C_GetFlagStatus, I2C_FLAG_RXNE, RESET);
  if (I2C_ReceiveData( ) != packet_check[1]) err = 1;
  /* packet check 2 */
  err += HW_I2C_WaitUntilTimeout((expression_func)I2C_GetFlagStatus, I2C_FLAG_RXNE, RESET);
  if (I2C_ReceiveData( ) != packet_check[2]) err = 1;
  /* receive packet */
  /* left/right/middle btn */
  err += HW_I2C_WaitUntilTimeout((expression_func)I2C_GetFlagStatus, I2C_FLAG_RXNE, RESET);
  packet_check[3] = I2C_ReceiveData( );
  /* Xmovement */
  err += HW_I2C_WaitUntilTimeout((expression_func)I2C_GetFlagStatus, I2C_FLAG_RXNE, RESET);
  HIDMouse[1] = I2C_ReceiveData( );
  /* Ymovement */
  err += HW_I2C_WaitUntilTimeout((expression_func)I2C_GetFlagStatus, I2C_FLAG_RXNE, RESET);
  HIDMouse[2] = I2C_ReceiveData( );
  /* Reserved */
  I2C_AcknowledgeConfig(DISABLE);
  err += HW_I2C_WaitUntilTimeout((expression_func)I2C_GetFlagStatus, I2C_FLAG_RXNE, RESET);
  packet_check[3] = I2C_ReceiveData( );
  I2C_GenerateSTOP(ENABLE);
  I2C_AcknowledgeConfig(ENABLE);

  return err;
}
#if 0
/*******************************************************************************
 * Function Name  : I2C_TP_IT_handler
 * Description    : I2C小红点中断处理函数
 * Input          : None
 * Return         : None
 *******************************************************************************/
void I2C_TP_IT_handler(void)
{
  if ( TPINT_GPIO_(ReadITFlagBit)( TPINT_Pin ) != 0 ) {
    TPINT_GPIO_(ClearITFlagBit)( TPINT_Pin );
    g_Ready_Status.i2ctp_data = TRUE;
  }
}
#endif
