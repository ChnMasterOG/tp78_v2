/********************************** (C) COPYRIGHT *******************************
 * File Name          : HW_I2C.c
 * Author             : ChnMasterOG
 * Version            : V2.0
 * Date               : 2022/11/30
 * Description        : 硬件I2C驱动
 * Copyright (c) 2023 ChnMasterOG
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#include "HW_I2C.h"

/*******************************************************************************
 * Function Name  : HW_I2C_WaitUntilTimeout
 * Description    : 硬件I2C 等待条件表达式满足直到超时
 * Input          : expression_func: 条件表达式函数指针,
 *                  event_flag: 事件或标志位
 *                  flip: 翻转(可以看作flip满足一直循环)
 *                  timeout_val: 超时时间
 * Return         : 1: 条件表达式未满足, 0: 条件表达式满足, flip不为0则相反
 *******************************************************************************/
uint8_t HW_I2C_WaitUntilTimeout(expression_func exp_func, uint32_t event_flag, uint8_t flip, uint32_t timeout_val)
{
  volatile uint32_t timeout = timeout_val;

  if (!flip) {
    while (!exp_func(event_flag) && timeout > 0) {
      timeout--;
    }
  } else {
    while (exp_func(event_flag) && timeout > 0) {
       timeout--;
    }
  }
//  if (exp_func == (expression_func)I2C_GetFlagStatus && event_flag == I2C_FLAG_BUSY && timeout == 0) {
//    /* 总线异常 */
//    HW_I2C_Reset();
//  }
  return timeout == 0;
}

/*******************************************************************************
 * Function Name  : HW_I2C_Init
 * Description    : 硬件I2C GPIO初始化
 * Input          : None
 * Return         : None
 *******************************************************************************/
void HW_I2C_Init(void)
{
#ifdef HW_I2C_GPIO_REMAP
    GPIOPinRemap(ENABLE, RB_PIN_I2C);
#endif
    GPIOB_ModeCfg(HW_I2C_SCL_PIN | HW_I2C_SDA_PIN, GPIO_ModeIN_PU);
    I2C_Init(I2C_Mode_I2C, 400000, I2C_DutyCycle_2, I2C_Ack_Enable, I2C_AckAddr_7bit, HW_I2C_MASTER_ADDR);
    I2C_Cmd(ENABLE);
}

/*******************************************************************************
 * Function Name  : HW_I2C_Reset
 * Description    : 硬件I2C 复位总线
 * Input          : None
 * Return         : None
 *******************************************************************************/
void HW_I2C_Reset(void)
{
    uint16_t i2c_ctrl1, i2c_ctrl2, i2c_rtr, i2c_ckcfgr, i2c_addr;

    i2c_ctrl1 = (uint16_t)R16_I2C_CTRL1;
    i2c_ctrl2 = (uint16_t)R16_I2C_CTRL2;
    i2c_rtr = (uint16_t)R16_I2C_RTR;
    i2c_ckcfgr = (uint16_t)R16_I2C_CKCFGR;
    i2c_addr = (uint16_t)R16_I2C_OADDR1;
    R16_I2C_CTRL1 |= RB_I2C_SWRST;
    R16_I2C_CTRL1 &= ~RB_I2C_SWRST;
    R16_I2C_CTRL1 &= ~RB_I2C_PE;
    R16_I2C_RTR = i2c_rtr;
    R16_I2C_CKCFGR = i2c_ckcfgr;
    R16_I2C_OADDR1 = i2c_addr;
    R16_I2C_CTRL2 = i2c_ctrl2;
    R16_I2C_CTRL1 = i2c_ctrl1;
    R16_I2C_CTRL1 |= RB_I2C_PE;
}

/*******************************************************************************
 * Function Name  : HW_I2C_WR_Reg
 * Description    : 硬件I2C写寄存器
 * Input          : reg: 寄存器, dat: 要写的值, addr: 从机地址
 * Return         : 0: success, others: failed
 *******************************************************************************/
uint8_t HW_I2C_WR_Reg(uint8_t reg, uint8_t dat, uint8_t addr)
{
  uint8_t err = 0;

  err += HW_I2C_WaitUntilTimeout((expression_func)I2C_GetFlagStatus, I2C_FLAG_BUSY, SET, HW_I2C_TIMOUT);
  I2C_GenerateSTART( ENABLE );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_MODE_SELECT, RESET, HW_I2C_TIMOUT);
  I2C_Send7bitAddress( addr, I2C_Direction_Transmitter );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, RESET, HW_I2C_ACK_TIMOUT);
  I2C_SendData(reg);
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET, HW_I2C_ACK_TIMOUT);
  I2C_SendData(dat);
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET, HW_I2C_ACK_TIMOUT);
  I2C_GenerateSTOP( ENABLE );

  return err;
}

/*******************************************************************************
 * Function Name  : HW_I2C_TPM_WR_Reg
 * Description    : TPM模块硬件I2C连续写寄存器
 * Input          : reg: 寄存器, *p_dat: 写指针, addr: 从机地址, len: 写的长度
 * Return         : 0: success, others: failed
 *******************************************************************************/
uint8_t HW_I2C_TPM_WR_Reg(uint8_t reg, uint8_t *p_dat, uint8_t addr, uint8_t len)
{
  uint8_t err = 0;

  err += HW_I2C_WaitUntilTimeout((expression_func)I2C_GetFlagStatus, I2C_FLAG_BUSY, SET, HW_I2C_TIMOUT);
  I2C_GenerateSTART( ENABLE );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_MODE_SELECT, RESET, HW_I2C_TIMOUT);
  I2C_Send7bitAddress( addr, I2C_Direction_Transmitter );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, RESET, HW_I2C_ACK_TIMOUT);
  I2C_SendData(reg);
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET, HW_I2C_ACK_TIMOUT);
  I2C_SendData(len);
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET, HW_I2C_ACK_TIMOUT);
  while (len) {
    I2C_SendData(*p_dat);
    err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET, HW_I2C_ACK_TIMOUT);
    len--;
    p_dat++;
  }
  I2C_GenerateSTOP( ENABLE );

  return err;
}

/*******************************************************************************
 * Function Name  : HW_I2C_RD_Reg
 * Description    : 硬件I2C读寄存器
 * Input          : reg: 寄存器, dat：读取结果指针, addr: 从机地址
 * Return         : 0: success, others: failed
 *******************************************************************************/
uint8_t HW_I2C_RD_Reg(uint8_t reg, uint8_t *dat, uint8_t addr)
{
  uint8_t err = 0;

  err += HW_I2C_WaitUntilTimeout((expression_func)I2C_GetFlagStatus, I2C_FLAG_BUSY, SET, HW_I2C_TIMOUT);
  I2C_GenerateSTART( ENABLE );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_MODE_SELECT, RESET, HW_I2C_TIMOUT);
  I2C_Send7bitAddress( addr, I2C_Direction_Transmitter );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, RESET, HW_I2C_ACK_TIMOUT);
  I2C_SendData(reg);
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET, HW_I2C_ACK_TIMOUT);

  /* restart */
  I2C_GenerateSTART( ENABLE );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_MODE_SELECT, RESET, HW_I2C_TIMOUT);
  I2C_Send7bitAddress( addr, I2C_Direction_Receiver );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED, RESET, HW_I2C_ACK_TIMOUT);
  I2C_GenerateSTOP(DISABLE);
  I2C_AcknowledgeConfig(DISABLE);
  err += HW_I2C_WaitUntilTimeout((expression_func)I2C_GetFlagStatus, I2C_FLAG_RXNE, RESET, HW_I2C_RD_TIMOUT);
  *dat = I2C_ReceiveData( );
  I2C_GenerateSTOP( ENABLE );
  I2C_AcknowledgeConfig(ENABLE);

  return err;
}

/*******************************************************************************
 * Function Name  : HW_I2C_2STOP_RD_Reg
 * Description    : 双STOP信号的硬件I2C读寄存器API
 * Input          : reg: 寄存器, dat读取结果指针, addr: 从机地址
 * Return         : 0: success, others: failed
 * Info           : 与HW_I2C_RD_Reg相比, 写寄存器数据后增加发送STOP
 *                  适配从机端通过检测STOP信号触发中断再读FIFO数据(例如ESP32 HAL库)
 *******************************************************************************/
uint8_t HW_I2C_2STOP_RD_Reg(uint8_t reg, uint8_t *dat, uint8_t addr)
{
  uint8_t err = 0;

  err += HW_I2C_WaitUntilTimeout((expression_func)I2C_GetFlagStatus, I2C_FLAG_BUSY, SET, HW_I2C_TIMOUT);
  I2C_GenerateSTART( ENABLE );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_MODE_SELECT, RESET, HW_I2C_TIMOUT);
  I2C_Send7bitAddress( addr, I2C_Direction_Transmitter );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, RESET, HW_I2C_ACK_TIMOUT);
  I2C_SendData(reg);
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET, HW_I2C_ACK_TIMOUT);
  I2C_GenerateSTOP( ENABLE ); // add stop signal to wait the slave device reads data

  /* restart */
  I2C_GenerateSTART( ENABLE );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_MODE_SELECT, RESET, HW_I2C_TIMOUT);
  I2C_Send7bitAddress( addr, I2C_Direction_Receiver );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED, RESET, HW_I2C_ACK_TIMOUT);
  I2C_GenerateSTOP(DISABLE);
  I2C_AcknowledgeConfig(DISABLE);
  err += HW_I2C_WaitUntilTimeout((expression_func)I2C_GetFlagStatus, I2C_FLAG_RXNE, RESET, HW_I2C_RD_TIMOUT);
  *dat = I2C_ReceiveData( );
  I2C_GenerateSTOP( ENABLE );
  I2C_AcknowledgeConfig(ENABLE);

  return err;
}

/*******************************************************************************
 * Function Name  : HW_I2C_Muti_RD_Reg
 * Description    : 硬件I2C连续读寄存器
 * Input          : reg: 寄存器, *dat: 要读的值的起始地址, addr: 从机地址, len: 读的长度
 * Return         : 0: success, others: failed
 *******************************************************************************/
uint8_t HW_I2C_Muti_RD_Reg(uint8_t reg, uint8_t *dat, uint8_t addr, uint8_t len)
{
  uint8_t err = 0;

  err += HW_I2C_WaitUntilTimeout((expression_func)I2C_GetFlagStatus, I2C_FLAG_BUSY, SET, HW_I2C_TIMOUT);
  I2C_GenerateSTART( ENABLE );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_MODE_SELECT, RESET, HW_I2C_TIMOUT);
  I2C_Send7bitAddress( addr, I2C_Direction_Transmitter );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, RESET, HW_I2C_ACK_TIMOUT);
  I2C_SendData(reg);
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET, HW_I2C_ACK_TIMOUT);

  /* restart */
  I2C_GenerateSTART( ENABLE );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_MODE_SELECT, RESET, HW_I2C_TIMOUT);
  I2C_Send7bitAddress( addr, I2C_Direction_Receiver );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED, RESET, HW_I2C_ACK_TIMOUT);
  I2C_GenerateSTOP( DISABLE );
  while (len-- != 0) {
    if (len == 0) {
      I2C_AcknowledgeConfig( DISABLE );
    } else {
      I2C_AcknowledgeConfig( ENABLE );
    }
    err += HW_I2C_WaitUntilTimeout((expression_func)I2C_GetFlagStatus, I2C_FLAG_RXNE, RESET, HW_I2C_RD_TIMOUT);
    *dat++ = I2C_ReceiveData( );
  }

  I2C_GenerateSTOP( ENABLE );
  I2C_AcknowledgeConfig( ENABLE );

  return err;
}

/*******************************************************************************
 * Function Name  : HW_I2C_2STOP_Muti_RD_Reg
 * Description    : 双STOP信号的硬件I2C连续读寄存器API
 * Input          : reg: 寄存器, *dat: 要读的值的起始地址, addr: 从机地址, len: 读的长度
 * Return         : 0: success, others: failed
 * Info           : 与HW_I2C_Muti_RD_Reg相比, 写寄存器数据后增加发送STOP
 *                  适配从机端通过检测STOP信号触发中断再读FIFO数据(例如ESP32 HAL库)
 *******************************************************************************/
uint8_t HW_I2C_2STOP_Muti_RD_Reg(uint8_t reg, uint8_t *dat, uint8_t addr, uint8_t len)
{
  uint8_t err = 0;

  err += HW_I2C_WaitUntilTimeout((expression_func)I2C_GetFlagStatus, I2C_FLAG_BUSY, SET, HW_I2C_TIMOUT);
  I2C_GenerateSTART( ENABLE );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_MODE_SELECT, RESET, HW_I2C_TIMOUT);
  I2C_Send7bitAddress( addr, I2C_Direction_Transmitter );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, RESET, HW_I2C_ACK_TIMOUT);
  I2C_SendData(reg);
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET, HW_I2C_ACK_TIMOUT);
  I2C_GenerateSTOP( ENABLE ); // add stop signal to wait the slave device reads data

  /* restart */
  I2C_GenerateSTART( ENABLE );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_MODE_SELECT, RESET, HW_I2C_TIMOUT);
  I2C_Send7bitAddress( addr, I2C_Direction_Receiver );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED, RESET, HW_I2C_ACK_TIMOUT);
  I2C_GenerateSTOP( DISABLE );
  while (len-- != 0) {
    if (len == 0) {
      I2C_AcknowledgeConfig( DISABLE );
    } else {
      I2C_AcknowledgeConfig( ENABLE );
    }
    err += HW_I2C_WaitUntilTimeout((expression_func)I2C_GetFlagStatus, I2C_FLAG_RXNE, RESET, HW_I2C_RD_TIMOUT);
    *dat++ = I2C_ReceiveData( );
  }

  I2C_GenerateSTOP( ENABLE );
  I2C_AcknowledgeConfig( ENABLE );

  return err;
}

/*******************************************************************************
 * Function Name  : HW_I2C_TPM_RD_Reg
 * Description    : TPM模块硬件I2C连续读寄存器
 * Input          : reg: 寄存器, *p_dat: 要读的值的起始地址, addr: 从机地址, len: 读的长度
 * Return         : 0: success, others: failed
 * Info           : 与HW_I2C_Muti_RD_Reg相比, 写寄存器数据后增加发送STOP
 *                  适配从机端通过检测STOP信号触发中断再读FIFO数据(例如ESP32 HAL库)
 *******************************************************************************/
uint8_t HW_I2C_TPM_RD_Reg(uint8_t reg, uint8_t *p_dat, uint8_t addr, uint8_t len)
{
  uint8_t err = 0;

  err += HW_I2C_WaitUntilTimeout((expression_func)I2C_GetFlagStatus, I2C_FLAG_BUSY, SET, HW_I2C_TIMOUT);
  I2C_GenerateSTART( ENABLE );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_MODE_SELECT, RESET, HW_I2C_TIMOUT);
  I2C_Send7bitAddress( addr, I2C_Direction_Transmitter );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, RESET, HW_I2C_ACK_TIMOUT);
  I2C_SendData(reg);
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET, HW_I2C_ACK_TIMOUT);
  I2C_SendData(len);
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET, HW_I2C_ACK_TIMOUT);
  I2C_GenerateSTOP( ENABLE ); // add stop signal to wait the slave device reads data

  /* restart */
  I2C_GenerateSTART( ENABLE );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_MODE_SELECT, RESET, HW_I2C_TIMOUT);
  I2C_Send7bitAddress( addr, I2C_Direction_Receiver );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED, RESET, HW_I2C_ACK_TIMOUT);
  I2C_GenerateSTOP( DISABLE );
  while (len-- != 0) {
    if (len == 0) {
      I2C_AcknowledgeConfig( DISABLE );
    } else {
      I2C_AcknowledgeConfig( ENABLE );
    }
    err += HW_I2C_WaitUntilTimeout((expression_func)I2C_GetFlagStatus, I2C_FLAG_RXNE, RESET, HW_I2C_RD_TIMOUT);
    *p_dat++ = I2C_ReceiveData( );
  }

  I2C_GenerateSTOP( ENABLE );
  I2C_AcknowledgeConfig( ENABLE );

  return err;
}
