/********************************** (C) COPYRIGHT *******************************
 * File Name          : PS2.h
 * Author             : ChnMasterOG
 * Version            : V2.0
 * Date               : 2022/11/30
 * Description        : PS/2驱动头文件
 * Copyright (c) 2023 ChnMasterOG
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#ifndef __PS2_H
  #define __PS2_H
 
  #include "HAL.h"
 
  #define PS2CLK_Pin		        GPIO_Pin_15
  #define PS2DATA_Pin		        GPIO_Pin_14
  #define PS2CLK_GPIO_(x)	      GPIOB_ ## x
  #define PS2DATA_GPIO_(x)	    GPIOB_ ## x
  
  #define PS2CLK_Clr()		      PS2CLK_GPIO_(ResetBits)(PS2CLK_Pin)
  #define PS2CLK_Set()		      PS2CLK_GPIO_(SetBits)(PS2CLK_Pin)
  #define PS2DATA_Clr()		      PS2DATA_GPIO_(ResetBits)(PS2DATA_Pin)
  #define PS2DATA_Set()		      PS2DATA_GPIO_(SetBits)(PS2DATA_Pin)
  #define PS2CLK_State()	      PS2CLK_GPIO_(ReadPortPin)(PS2CLK_Pin)
  #define PS2DATA_State()	      PS2DATA_GPIO_(ReadPortPin)(PS2DATA_Pin)
	
  #define PS2_TIMEOUT		        100000    // PS2超时时间

  #define PS_RESET		          0XFF      // 复位命令             回应0XFA
  #define RESEND	     	        0XFE      // 再次发送
  #define SET_DEFAULT	 	        0XF6      // 使用默认设置   回应0XFA
  #define DIS_DATA_REPORT   	  0XF5      // 禁用数据报告   回应0XFA
  #define EN_DATA_REPORT    	  0XF4      // 使能数据报告   回应0XFA
  #define SET_SAMPLE_RATE	      0XF3      // 设置采用速率   回应0XFA
  #define GET_DEVICE_ID		      0XF2      // 得到设备ID   回应0XFA+ID
  #define SET_REMOTE_MODE	      0XF0
  #define SET_WRAP_MODE       	0XEE
  #define RST_WRAP_MODE       	0XEC
  #define READ_DATA           	0XEB
  #define SET_STREAM_MODE     	0XEA
  #define STATUS_REQUEST      	0XE9
  #define SET_RESOLUTION      	0XE8
  #define SET_SCALING21       	0XE7
  #define SET_SCALING11       	0XE6

  extern uint8_t PS2_byte_cnt;

  //declare functions
  uint8_t PS2_ReadByte(uint8_t* dat);
  uint8_t PS2_WriteByte(uint8_t dat);
  uint8_t PS2_ReadMouseData(Mouse_Data_t* dat);
  uint8_t PS2_Config(uint8_t reg, uint8_t res);
  void PS2_En_Data_Report(void);
  void PS2_Dis_Data_Report(void);
  void PS2_IT_handler(void);
  uint8_t PS2_Init(char* debug_info, BOOL is_IT);
	
#endif
