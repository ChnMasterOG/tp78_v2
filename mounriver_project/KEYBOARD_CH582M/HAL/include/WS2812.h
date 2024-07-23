/********************************** (C) COPYRIGHT *******************************
* File Name          : WS2812.h
* Author             : ChnMasterOG
* Version            : V1.0
* Date               : 2021/12/16
* Description        : WS2812驱动头文件
* Copyright (c) 2023 ChnMasterOG
* SPDX-License-Identifier: GPL-3.0
*******************************************************************************/

#ifndef WS2812_H
  #define WS2812_H

  #define USE_PWM1    //使用TMR1输出PWM

#ifdef USE_PWM1
  #define WS2812_Pin              GPIO_Pin_10
  #define WS2812_GPIO_(x)         GPIOA_ ## x
#else
  #define WS2812_Pin              GPIO_Pin_14
  #define WS2812_GPIO_(x)         GPIOB_ ## x
#endif

  #define TIMING_ONE              51
  #define TIMING_ZERO             24
  #define TIMING_RESET            0
  #define RESET_FRAME_SIZE        2  // uints: tick

  #define GREEN_INDEX             0
  #define RED_INDEX               1
  #define BLUE_INDEX              2

  #define BACKLIGHT_MODE_OFF      0
  #define BACKLIGHT_MODE_BREATH   1
  #define BACKLIGHT_MODE_WATERFUL 2
  #define BACKLIGHT_MODE_TOUCH    3
  #define BACKLIGHT_MODE_RAINBOW  4
  #define BACKLIGHT_MODE_NORMAL   5
  #define BACKLIGHT_MODE_CUSTOM   6

  #define PWM_SEND_HIGH_BIT()     TMR1_PWMActDataWidth(TIMING_ONE)    //0.85us
  #define PWM_SEND_LOW_BIT()      TMR1_PWMActDataWidth(TIMING_ZERO)   //0.4us
  #define PWM_SEND_RESET()        TMR1_PWMActDataWidth(TIMING_RESET)  //0us

  #define LED_NUMBER              83    // 83个灯 - 其他键盘布局虚修改此处
  #define LED_DEFAULT_BRIGHTNESS  15    // 默认亮度 - 15/255

  #define Waterful_Repeat_Times   2     // 控制周期*2 = 流水灯周期

  typedef void (*WS2812_Style_Func) ( void );   // WS2812变化样式对应函数

  // Get Reports
  uint16_t GetLampArrayAttributesReport(uint8_t* buffer);
  uint16_t GetLampAttributesReport(uint8_t* buffer);

  // Set Reports
  void SetLampAttributesId(const uint8_t* buffer);
  void SetMultipleLamps(const uint8_t* buffer);
  void SetLampRange(const uint8_t* buffer);
  void SetAutonomousMode(const uint8_t* buffer);

  extern UINT8 LED_BYTE_Buffer[LED_NUMBER][3];
  extern WS2812_Style_Func led_style_func;
  extern uint8_t g_LED_brightness;
  extern WS2812_Style_Func g_record_last_LED_style;

  uint8_t DATAFLASH_Read_LEDStyle( void );
  void DATAFLASH_Write_LEDStyle( uint16_t LED_Style_Number );
  void WS2812_PWM_Init( void );
  void WS2812_Style_Off( void );
  void WS2812_Style_Normal( void);
  void WS2812_Style_Breath( void );
  void WS2812_Style_Waterful( void );
  void WS2812_Style_Touch( void );
  void WS2812_Style_Rainbow( void );
  void WS2812_Style_Custom( void );
  void WS2812_Send( void );

#endif

