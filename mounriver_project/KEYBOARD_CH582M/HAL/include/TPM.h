/********************************** (C) COPYRIGHT *******************************
 * File Name          : TPM.h
 * Author             : ChnMasterOG
 * Version            : V1.0
 * Date               : 2023/5/24
 * Description        : TPM扩展模块通用接口驱动
 * Copyright (c) 2023 ChnMasterOG
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#ifndef __MSG_TPM_H
  #define __MSG_TPM_H

  #include "HAL.h"

  #define MODULE_WAKEUP_PIN         HW_I2C_SDA_PIN  // 唤醒脚为SDA
  #define MODULE_WAKEUP_GPIO(x)     GPIOB_ ## x
  #define MODULE_I2C_ADDR_MIN       0xB0
  #define MODULE_I2C_ADDR_MAX       0xC0

  /* 扩展模块register宏 */
  #define MODULE_SPECIFIC_ID        0x5A
  #define MODULE_SPECIFIC_ID_REG    0x00
  #define MODULE_ADDR_REG           0x01
  #define MODULE_FIRMWARE_REG       0x02
  #define MODULE_NAME_LENGTH_REG    0x04
  #define HOST_NAME_LENGTH_REG      0x05
  #define MODULE_TYPE_REG           0x06
  #define MODULE_READY_CONTROL1_REG 0x10
  #define MODULE_READY_CONTROL2_REG 0x11
  #define MODULE_CONTROL1_REG       0x12
  #define MODULE_KBD_DATA_REG       0x20
  #define MODULE_MOUSE_DATA_REG     0x28
  #define MODULE_SCROLL_DATA1_REG   0x2C
  #define MODULE_SCROLL_DATA2_REG   0x2E
  #define MODULE_NAME_REG           0x80
  #define HOST_NAME_REG             0xC0

  /* 扩展模块register bit宏 */
  #define MODULE_BIT_KEYBOARD_READY 0x01
  #define MODULE_BIT_MOUSE_READY    0x02
  #define MODULE_BIT_SWITCH_READY   0x04

  /* I2C标准接口定义 */
  #define MODULE_I2C_WR_Reg(reg, dat, addr)               HW_I2C_WR_Reg(reg, dat, addr)
  #define MODULE_I2C_RD_Reg(reg, p_dat, addr)             HW_I2C_RD_Reg(reg, p_dat, addr)
  #define MODULE_I2C_Muti_RD_Reg(reg, p_dat, addr, len)   HW_I2C_Muti_RD_Reg(reg, p_dat, addr, len)

  /* 错误代码 */
  #define MODULE_ERR_I2C_NO_READY   0x01

  uint8_t TPM_init(char* debug_info);

#endif
