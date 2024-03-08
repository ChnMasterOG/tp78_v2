/********************************** (C) COPYRIGHT *******************************
 * File Name          : PS2.c
 * Author             : ChnMasterOG
 * Version            : V2.0
 * Date               : 2022/11/30
 * Description        : PS/2驱动源文件
 * Copyright (c) 2023 ChnMasterOG
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#include "HAL.h"
#include <string.h>

#define Delay_us    DelayUs
#define Delay_ms    DelayMs

uint8_t PS2_bit_cnt = 0,
        PS2_byte_cnt = 0,
        PS2_byte = 0,
        PS2_high_cnt = 0;

/*******************************************************************************
 * Function Name  : PS2_WaitCLKState
 * Description    : PS/2协议等待CLK状态变化
 * Input          : wait_high - TRUE:等待CLK从低变高
 *                              FALSE:等待CLK从高变低
 * Return         : 成功返回0, 失败返回1
 *******************************************************************************/
uint8_t PS2_WaitCLKState( BOOL wait_high )
{
  uint32_t timeout = PS2_TIMEOUT;
  if ( wait_high ) {
    while (!PS2CLK_State()) {  //等待CLK高
      if (timeout == 0) return 1;
      timeout--;
      DelayUs(1);
    }
  } else {
    while (PS2CLK_State()) {  //等待CLK低
      if (timeout == 0) return 1;
      timeout--;
      DelayUs(1);
    }
  }
  return 0;
}

/*******************************************************************************
 * Function Name  : PS2_ReadByte
 * Description    : PS/2协议读一字节
 * Input          : dat - 将读取到的内容放到dat指针指向的空间中
 * Return         : 成功返回0, 失败返回1
 *******************************************************************************/
uint8_t PS2_ReadByte(uint8_t* dat)
{
  int i;
  uint8_t res = 0;
  uint8_t high = 0, parity;

  Delay_us(50);
  PS2_En_Data_Report();
  Delay_us(20);

  if ( PS2_WaitCLKState(FALSE) ) return 1;  //起始位
  for (i = 0; i < 8; i++) {
    if ( PS2_WaitCLKState(TRUE) ) return 1;
    if ( PS2_WaitCLKState(FALSE) ) return 1;	//等待一个下降沿
    res >>= 1;
    if (PS2DATA_State()) {
      res |= 0x80;
      ++high;
    }
  }
  if ( PS2_WaitCLKState(TRUE) ) return 1;
  if ( PS2_WaitCLKState(FALSE) ) return 1;	//校验位
  parity = PS2DATA_State();
  if ( PS2_WaitCLKState(TRUE) ) return 1;
  if ( PS2_WaitCLKState(FALSE) ) return 1;	//停止位
  PS2_Dis_Data_Report();
  //if ((high & 1) != (parity != 0)) return 1;
  *dat = res;
  return 0;
}

/*******************************************************************************
 * Function Name  : PS2_WriteByte
 * Description    : PS/2协议写一字节
 * Input          : dat - 要写入的值
 * Return         : 成功返回0, 失败返回1
 *******************************************************************************/
uint8_t PS2_WriteByte(uint8_t dat)
{
  int i, high = 0;
  PS2CLK_GPIO_(SetBits)( PS2CLK_Pin );
  PS2DATA_GPIO_(SetBits)( PS2DATA_Pin );
  PS2CLK_GPIO_(ModeCfg)( PS2CLK_Pin, GPIO_ModeOut_PP_5mA );
  PS2DATA_GPIO_(ModeCfg)( PS2DATA_Pin, GPIO_ModeOut_PP_5mA );

  PS2CLK_Clr();
  Delay_us(120);
  PS2DATA_Clr();
  Delay_us(10);
  PS2CLK_GPIO_(ModeCfg)( PS2CLK_Pin, GPIO_ModeIN_PU );	//释放时钟线

  if ( PS2_WaitCLKState(FALSE) ) return 1;

  for (i = 0; i < 8; i++) {
    if (dat & 0x01) {
      PS2DATA_Set();
      ++high;
    }
    else PS2DATA_Clr();
    dat >>= 1;
    if ( PS2_WaitCLKState(TRUE) ) return 1;
    if ( PS2_WaitCLKState(FALSE) ) return 1;
  }

  //上升沿-校验位
  if (high & 0x01) PS2DATA_Clr();
  else PS2DATA_Set();
  if ( PS2_WaitCLKState(TRUE) ) return 1;

  //上升沿-停止位
  if ( PS2_WaitCLKState(FALSE) ) return 1;
  PS2DATA_Set();
  if ( PS2_WaitCLKState(TRUE) ) return 1;

  PS2DATA_GPIO_(ModeCfg)( PS2DATA_Pin, GPIO_ModeIN_PU );	//释放数据线
  if ( PS2_WaitCLKState(FALSE) ) return 1;

  if (!PS2DATA_State()) {
    if ( PS2_WaitCLKState(TRUE) ) return 1;
  }
  else return 1;
  return 0;
}

/*******************************************************************************
 * Function Name  : PS2_ReadMouseData
 * Description    : PS/2协议接收一次鼠标数据
 * Input          : dat - 将接收到的内容放到dat指针指向的空间中(dat指向的空间应当占4byte)
 * Return         : 成功返回0, 失败返回1
 *******************************************************************************/
uint8_t PS2_ReadMouseData(Mouse_Data_t* dat)
{
  int i;
  if (PS2_ReadByte((uint8_t*)dat->data) != 0) return 1;
  if (PS2_ReadByte((uint8_t*)dat->data + 1) != 0) return 1;
  if (PS2_ReadByte((uint8_t*)dat->data + 2) != 0) return 1;
  return 0;
}

/*******************************************************************************
 * Function Name  : PS2_Config
 * Description    : 配置PS/2鼠标
 * Input          : reg - 配置值, res - 需要等待res的响应才算成功
 * Return         : 成功返回0, 失败返回非0
 *******************************************************************************/
uint8_t PS2_Config(uint8_t reg, uint8_t res)
{
  uint8_t dat, sta;
  sta = PS2_WriteByte(reg);
  if (sta != 0)	return 1;
  sta = PS2_ReadByte(&dat);
  if (sta != 0) return 1;
  else if (dat != res) return 2;
  return 0;
}

/*******************************************************************************
 * Function Name  : PS2_En_Data_Report
 * Description    : 使能PS/2数据传输
 * Input          : None
 * Return         : None
 *******************************************************************************/
void PS2_En_Data_Report(void)
{
  PS2CLK_Set();
  PS2CLK_GPIO_(ModeCfg)( PS2CLK_Pin, GPIO_ModeIN_PU );
}

/*******************************************************************************
 * Function Name  : PS2_Dis_Data_Report
 * Description    : 禁止PS/2数据传输
 * Input          : None
 * Return         : None
 *******************************************************************************/
void PS2_Dis_Data_Report(void)
{
  PS2CLK_GPIO_(SetBits)( PS2CLK_Pin );
  PS2CLK_GPIO_(ModeCfg)( PS2CLK_Pin, GPIO_ModeOut_PP_20mA );
  PS2CLK_Clr();
}

/*******************************************************************************
 * Function Name  : PS2_IT_handler
 * Description    : PS/2中断处理函数
 * Input          : None
 * Return         : None
 *******************************************************************************/
void PS2_IT_handler(void)
{
  if ( PS2CLK_GPIO_(ReadITFlagBit)( PS2CLK_Pin ) != 0 ) {
    PS2CLK_GPIO_(ClearITFlagBit)( PS2CLK_Pin );
    ++PS2_bit_cnt;
    if (PS2_bit_cnt == 1) { //起始位
      PS2_byte = 0;
      PS2_high_cnt = 0;
    } else if (PS2_bit_cnt <= 9) {
      PS2_byte >>= 1;
      if (PS2DATA_State()) {
        PS2_byte |= 0x80;
        ++PS2_high_cnt;
      }
    } else if (PS2_bit_cnt == 10) {   //校验位
//    if ((PS2_high_cnt & 1) == (PS2DATA_State() != 0)) {
//      //ERR??
//    }
    } else if (PS2_bit_cnt == 11) {   //停止位
      if ((PS2_byte_cnt == 0 && (PS2_byte & 0x8)) || PS2_byte_cnt > 0) {    //检查Always1位是否为1
        if ( PS2_byte_cnt > 0 ) MouseDat->data[PS2_byte_cnt] = PS2_byte;  //小红点不接受按键信息
        PS2_byte_cnt++;
      }
      PS2_Dis_Data_Report();
      PS2_bit_cnt = 0;
      g_Ready_Status.ps2_data = TRUE;
    }
  }
}

/*******************************************************************************
 * Function Name  : PS2_Init
 * Description    : 初始化PS/2鼠标
 * Input          : debug_info - 错误信息; is_IT - 是否使用中断
 * Return         : 成功返回0，失败返回1
 *******************************************************************************/
uint8_t PS2_Init(char* debug_info, BOOL is_IT)
{ 	
  uint8_t res, sta;

  // config gpio
  PS2CLK_GPIO_(SetBits)( PS2CLK_Pin );
  PS2CLK_GPIO_(ModeCfg)( PS2CLK_Pin, GPIO_ModeOut_PP_20mA );
  PS2_Dis_Data_Report();
  PS2DATA_GPIO_(ModeCfg)( PS2DATA_Pin, GPIO_ModeIN_PU );

  // config mode
  sta = PS2_Config(PS_RESET, 0xFA);
  if (sta != 0) {
    strcpy(debug_info, "PS2-ERR-01");
    return 1;
  }
  sta = PS2_ReadByte(&res);   //AA
  if (sta != 0 || res != 0xAA) {
    strcpy(debug_info, "PS2-ERR-02");
    return 1;
  } else if (res != 0xAA) {
    strcpy(debug_info, "PS2-WAR-02");
  }
  sta = PS2_ReadByte(&res);   //ID号：0
  if (sta != 0) {
    strcpy(debug_info, "PS2-ERR-03");
    return 1;
  } else if (res != 0x00) {
    strcpy(debug_info, "PS2-WAR-03");
  }
  sta = PS2_Config(SET_DEFAULT, 0xFA);
  if (sta != 0) {
    strcpy(debug_info, "PS2-ERR-04");
    return 1;
  }
  sta = PS2_Config(EN_DATA_REPORT, 0xFA);
  if (sta != 0) {
    strcpy(debug_info, "PS2-ERR-05");
    return 1;
  }
  if (is_IT) {
    PS2_En_Data_Report();
    DelayMs(10);   //等待稳定
    PS2CLK_GPIO_(ITModeCfg)( PS2CLK_Pin, GPIO_ITMode_FallEdge );
    PFIC_EnableIRQ( GPIO_B_IRQn );  //PS2CLK_GPIO
  }
  return 0;
}

/****************************************************************
 *
 *  //初始化写法
 *  if (PS2_Init(buf, 1) != 0) {
 *      printf("%s\n", buf);
 *      while (1);
 *  } else printf("mouse ready\n");
 *
 *  //主循环中断写法
 *  while (1) {
 *      if (g_Ready_Status.ps2_data == TRUE) {
 *          g_Ready_Status.ps2_data = FALSE;
 *          if (PS2_byte_cnt == 3) {
 *              PS2_byte_cnt = 0;
 *              printf("%d %d %d %d\n", PS2dat.LeftBtn, PS2dat.RightBtn, PS2dat.XMovement, PS2dat.YMovement);
 *          }
 *          PS2_En_Data_Report();
 *      }
 *      DelayUs(50);
 *  }
 *
 *  //主循环非中断写法
 *  while (1) {
 *      PS2_ReadMouseData(&PS2dat);
 *      printf("%d %d %d %d\n", PS2dat.LeftBtn, PS2dat.RightBtn, PS2dat.XMovement, PS2dat.YMovement);
 *  }
 *
 *  //外部中断调用
 *  PS2_IT_handler();
 *
 ****************************************************************/
