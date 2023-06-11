/********************************** (C) COPYRIGHT *******************************
 * File Name          : TPM.c
 * Author             : ChnMasterOG
 * Version            : V1.0
 * Date               : 2023/5/24
 * Description        : TPM扩展模块通用接口驱动
 * Copyright (c) 2023 ChnMasterOG
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#include "HAL.h"
#include "malloc.h"

/*******************************************************************************
  TPM扩展模块通用接口驱动协议寄存器描述
  0x00 RO 固定字符，0x5A
  0x01 RO 模块地址，8 bit
  0x02 RO 模块固件版本号，16bit
  0x04 RO 模块名称长度(单位为byte)，8bit
  0x05 RW 主机名称长度(单位为byte)，8bit
  0x06 RO 模块类型1，8bit
          bit0 - 通用键盘模块
          bit1 - 通用鼠标模块
          bit2 - 通用旋钮控制模块
  0x10 WR Ready控制1，8bit
          bit0 - 键盘数据ready信号
          bit1 - 鼠标数据ready信号
          bit2 - 旋钮数据ready信号
  0x11 WR Ready控制2，8bit
  0x12 WR 通用控制1，8bit
          bit0 - 模块总ready信号
          bit1 - 模块使能
          bit2 - 低功耗使能
          bit3 - 主机心跳包信号
  0x20 WR 键盘模块数据，8byte
  0x28 WR 鼠标模块数据，4byte
  0x2C WR 旋钮模块数据1，2byte
  0x2E WR 旋钮模块数据2，2byte
  0x80 RO 模块名称，长度为0x04地址描述
  0xC0 WR 主机名称，长度为0x05地址描述
*******************************************************************************/

static uint8_t *tpm_module_addr;  // 记录模块的地址
static uint8_t *tpm_module_type;  // 记录模块类型
static uint8_t tpm_module_num = 0;  // 记录扩展模块的数量

/*******************************************************************************
 * Function Name  : TPM_data_free
 * Description    : TPM内存释放
 * Input          : 无
 * Return         : 无
 *******************************************************************************/
static inline void TPM_data_free(void)
{
  if (tpm_module_num == 0) return;
  free(tpm_module_addr);
  free(tpm_module_type);
  tpm_module_num = 0;
}

/*******************************************************************************
 * Function Name  : TPM_data_malloc
 * Description    : TPM内存申请
 * Input          : size - 模块数目
 * Return         : 无
 *******************************************************************************/
static inline void TPM_data_alloc(uint8_t size)
{
  tpm_module_addr = (uint8_t*)malloc(size * sizeof(uint8_t));
  tpm_module_type = (uint8_t*)malloc(size * sizeof(uint8_t));
  tpm_module_num = size;
}

/*******************************************************************************
 * Function Name  : TPM_Init
 * Description    : 初始化扩展模块
 * Input          : debug_info - 错误信息
 * Return         : 成功返回0，失败返回错误码
 *******************************************************************************/
uint8_t TPM_init(char* debug_info)
{
#if (defined HAL_HW_I2C) && (HAL_HW_I2C == TRUE)
  if ((R16_I2C_CTRL1 & RB_I2C_PE) == 0) {
    strcpy(debug_info, "TPM-ERR-01"); // 使用HW I2C，需要先初始化I2C并使能
    return MODULE_ERR_I2C_NO_READY;
  }
#endif
  tmos_start_task( halTaskID, MODULE_INITIAL_EVENT, MS1_TO_SYSTEM_TIME(100) );
  return 0;
}

/*******************************************************************************
 * Function Name  : TPM_Scan
 * Description    : 扫描扩展模块
 * Input          : 无
 * Return         : 有新增模块返回总模块数目，否则返回0
 *******************************************************************************/
uint8_t TPM_scan(void)
{
  uint8_t addr, dat, ret, size = 0;

  for (addr = MODULE_I2C_ADDR_MIN; addr < MODULE_I2C_ADDR_MAX; addr+=2) {
    ret = MODULE_I2C_RD_Reg(MODULE_SPECIFIC_ID_REG, &dat, addr);
    if (ret == 0 && dat == MODULE_SPECIFIC_ID) size++;
  }
  if (size > tpm_module_num) {  // 新增模块
    TPM_data_free();
    TPM_data_alloc(size);
    size = 0;
    for (addr = MODULE_I2C_ADDR_MIN; addr < MODULE_I2C_ADDR_MAX; addr+=2) {
      ret = MODULE_I2C_RD_Reg(MODULE_SPECIFIC_ID_REG, &dat, addr);
      if (ret == 0 || dat == MODULE_SPECIFIC_ID) {
        tpm_module_addr[size] = addr;
        MODULE_I2C_RD_Reg(MODULE_TYPE_REG, &dat, addr);
        tpm_module_type[size] = dat;
        size++;
      }
    }
    return size;
  } else return 0;
}

/*******************************************************************************
 * Function Name  : TPM_process_keyboard
 * Description    : 扩展模块处理键盘数据
 * Input          : addr - 扩展模块对应的地址
 * Return         : 发送键盘数据返回1，否则返回0
 *******************************************************************************/
uint8_t TPM_process_keyboard(uint8_t addr)
{
  uint8_t dat, ret;
  uint8_t kbd_dat[HID_KEYBOARD_DATA_LENGTH];

  ret = MODULE_I2C_RD_Reg(MODULE_READY_CONTROL1_REG, &dat, addr);
  if (ret == 0 && (dat & MODULE_BIT_KEYBOARD_READY)) {
    MODULE_I2C_Muti_RD_Reg(MODULE_KBD_DATA_REG, kbd_dat, addr, HID_KEYBOARD_DATA_LENGTH);
    /////////////////
    /* 键盘事件 */
    if ( g_Ready_Status.usb == TRUE && priority_USB == TRUE ) {
      tmos_set_event( usbTaskID, USB_KEYBOARD_EVENT );  // USB键盘事件
    } else if ( g_Ready_Status.ble == TRUE ) {
      tmos_set_event( hidEmuTaskId, START_KEYBOARD_REPORT_EVT );  // 蓝牙键盘事件
    } else if ( g_Ready_Status.rf == TRUE ) {
      tmos_set_event( RFTaskId, SBP_RF_KEYBOARD_REPORT_EVT );  // RF键盘事件
    }
    return 1;
  } else return 0;
}

/*******************************************************************************
 * Function Name  : TPM_process_mouse
 * Description    : 扩展模块处理鼠标数据
 * Input          : addr - 扩展模块对应的地址
 * Return         : 发送鼠标数据返回1，否则返回0
 *******************************************************************************/
uint8_t TPM_process_mouse(uint8_t addr)
{
  uint8_t dat, ret;

  ret = MODULE_I2C_RD_Reg(MODULE_READY_CONTROL1_REG, &dat, addr);
  if (ret == 0 && (dat & MODULE_BIT_MOUSE_READY)) {
    MODULE_I2C_Muti_RD_Reg(MODULE_MOUSE_DATA_REG, HIDMouse, addr, HID_MOUSE_DATA_LENGTH);
    /* 鼠标事件 */
    if ( g_Ready_Status.usb == TRUE && priority_USB == TRUE ) {
      tmos_set_event( usbTaskID, USB_MOUSE_EVENT );  //USB鼠标事件
    } else if ( g_Ready_Status.ble == TRUE ) {
      tmos_set_event( hidEmuTaskId, START_MOUSE_REPORT_EVT );  //蓝牙鼠标事件
    } else if ( g_Ready_Status.rf == TRUE ) {
      tmos_set_event( RFTaskId, SBP_RF_MOUSE_REPORT_EVT );  // RF鼠标事件
    }
    return 1;
  } else return 0;
}
