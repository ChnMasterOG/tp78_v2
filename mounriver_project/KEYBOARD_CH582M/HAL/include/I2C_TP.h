/********************************** (C) COPYRIGHT *******************************
 * File Name          : I2C_TP.h
 * Author             : ChnMasterOG
 * Version            : V1.0
 * Date               : 2023/1/2
 * Description        : I2C小红点驱动
 * Copyright (c) 2023 ChnMasterOG
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#ifndef __I2C_TP_H
  #define __I2C_TP_H

  #include "CH58x_common.h"

  #define TPINT_Pin       GPIO_Pin_15   // the same as PS/2 scl pin
  #define TPINT_GPIO_(x)  GPIOB_ ## x
  #define I2C_TP_ADDR     0x2B

  void I2C_TP_Init(char* debug_info);
  uint8_t I2C_TP_SendCommand_Sleep(void);
  uint8_t I2C_TP_SendCommand_Wakeup(void);
  uint8_t I2C_TP_SendCommand_Reset(void);
  uint8_t I2C_TP_SendCommand_EnterExitIdleMode(uint8_t flag);
  uint8_t I2C_TP_ReadPacket(void);
  void I2C_TP_IT_handler(void);

#endif
