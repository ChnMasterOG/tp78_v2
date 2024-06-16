/********************************** (C) COPYRIGHT *******************************
 * File Name          : HW_I2C.h
 * Author             : ChnMasterOG
 * Version            : V2.0
 * Date               : 2022/11/30
 * Description        : Hardware I2C driver
 * Copyright (c) 2023 ChnMasterOG
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#ifndef __HW_I2C_H
  #define __HW_I2C_H

  #include "CH58x_common.h"

  #define HW_I2C_SCL_PIN        GPIO_Pin_13
  #define HW_I2C_SDA_PIN        GPIO_Pin_12
  #define HW_I2C_MASTER_ADDR    0x52
  #define HW_I2C_TIMOUT         0x20
  #define HW_I2C_ACK_TIMOUT     0x20
  #define HW_I2C_RD_TIMOUT      0x100
  //  #define HW_I2C_GPIO_REMAP

  typedef uint8_t (*expression_func)( uint32_t );

  void HW_I2C_Init( void );
  void HW_I2C_Reset( void );
  uint8_t HW_I2C_WaitUntilTimeout( expression_func exp_func, uint32_t event_flag, uint8_t flip, uint32_t timeout_val );
  uint8_t HW_I2C_WR_Reg( uint8_t reg, uint8_t dat, uint8_t addr );
  uint8_t HW_I2C_TPM_WR_Reg(uint8_t reg, uint8_t *p_dat, uint8_t addr, uint8_t len);
  uint8_t HW_I2C_RD_Reg( uint8_t reg, uint8_t *dat, uint8_t addr );
  uint8_t HW_I2C_2STOP_RD_Reg(uint8_t reg, uint8_t *dat, uint8_t addr);
  uint8_t HW_I2C_Muti_RD_Reg( uint8_t reg, uint8_t *dat, uint8_t addr, uint8_t len );
  uint8_t HW_I2C_2STOP_Muti_RD_Reg(uint8_t reg, uint8_t *dat, uint8_t addr, uint8_t len);
  uint8_t HW_I2C_TPM_RD_Reg(uint8_t reg, uint8_t *p_dat, uint8_t addr, uint8_t len);
#endif
