/********************************** (C) COPYRIGHT *******************************
 * File Name          : MPR121.h
 * Author             : ChnMasterOG
 * Version            : V2.0
 * Date               : 2023/6/1
 * Description        : mpr121电容传感器驱动配置库
 * Copyright (c) 2023 ChnMasterOG
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#ifndef __MPR121_H
  #define __MPR121_LIB_H

  #include "HAL.h"

  /* MPR121 GPIO configurations */
  #define MPRINT_Pin            GPIO_Pin_18   // MPR121 INT
  #define MPRINT_GPIO_(x)       GPIOB_ ## x   // MPR121 IO

  /* MPR121 I2C interfaces */
  #define MPR121_WriteReg(reg, dat)           HW_I2C_WR_Reg(reg, dat, MPR121_ADDR)
  #define MPR121_ReadReg(reg, pdat)           HW_I2C_RD_Reg(reg, pdat, MPR121_ADDR)
  #define MPR121_ReadHalfWord(reg, pdat)      HW_I2C_Muti_RD_Reg(reg, (uint8*)pdat, MPR121_ADDR, 2)
  #define MPR121_ReadMutiReg(reg, pdat, len)  HW_I2C_Muti_RD_Reg(reg, pdat, MPR121_ADDR, len)

  /* MPR121 Default Parameters */
  #define CAP_MOUSE_TOU_THRESH  3             // cap_mouse touch threshold (default 3)
  #define CAP_MOUSE_REL_THRESH  2             // cap_mouse release threshold (default 2)
  #define CAP_MOUSE_MOVE_SPEED  8             // cap_mouse move speed (default 8)
  #define TOUCHBAR_TOU_THRESH   3             // touchbar touch threshold (default 3)
  #define TOUCHBAR_REL_THRESH   2             // touchbar release threshold (default 2)
  #define DOUBLE_TOUCH_CNT      20            // double touch counter (default 20)
  #define LONG_TOUCH_CNT        30            // long touch counter (default 30)

  /* MPR121 other configurations */
  #define MPR121_ADDR           0xB4          // 0x5A<<1, addr pin connects to GND
  //#define MPR121_Number         1             // MPR121 numbers
  #define MPR121_TASK_PERIOD    10            // units: 1ms

  void DATAFLASH_Read_MPR121_ALG_Parameter(mpr121_alg_param_t* p);
  void DATAFLASH_Write_MPR121_ALG_Parameter(mpr121_alg_param_t* p);
  void MPR121_GPIO_Init(void);
  void MPR121_Init(char* buf);
  void MPR121_Post_Operation(void);

#endif
