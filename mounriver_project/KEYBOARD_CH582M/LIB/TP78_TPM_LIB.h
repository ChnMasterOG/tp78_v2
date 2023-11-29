/********************************** (C) COPYRIGHT *******************************
 * File Name          : TP78_TPM_LIB.h
 * Author             : ChnMasterOG
 * Version            : V1.1
 * Date               : 2023/5/24
 * Description        : TPM扩展模块通用接口驱动
 * Copyright (c) 2023 ChnMasterOG
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#ifndef __TP78_TPM_LIB_H
#define __TP78_TPM_LIB_H

#define MODULE_I2C_ADDR_MIN         0xC0
#define MODULE_I2C_ADDR_MAX         0xC4

/* 错误代码 */
#define MODULE_ERR_I2C_NO_READY     0x01

void TPM_init(char* debug_info);
uint8_t TPM_scan(void);
void TPM_notify_key_data(void);
void TPM_notify_led_data(uint8_t status);
void TPM_process_data(void);

#endif
