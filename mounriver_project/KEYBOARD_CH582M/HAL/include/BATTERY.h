/********************************** (C) COPYRIGHT *******************************
 * File Name          : BATTERY.h
 * Author             : ChnMasterOG
 * Version            : V1.0
 * Date               : 2022/1/26
 * Description        : 电池ADC采样头文件
 * Copyright (c) 2023 ChnMasterOG
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#ifndef BATTERY_H
#define BATTERY_H

  #define ADC_MAXBUFLEN       20

  #define BAT_MINADCVAL       3706  // 3.8V/2
  #define BAT_MAXADCVAL       3998  // 4.1V/2

  #define BAT_20PERCENT_VAL   (4*BAT_MINADCVAL/5+BAT_MAXADCVAL/5)   // 20%
  #define BAT_25PERCENT_VAL   (3*BAT_MINADCVAL/4+BAT_MAXADCVAL/4)   // 25%
  #define BAT_33PERCENT_VAL   (2*BAT_MINADCVAL/3+BAT_MAXADCVAL/3)   // 33%
  #define BAT_40PERCENT_VAL   (3*BAT_MINADCVAL/5+2*BAT_MAXADCVAL/5) // 40%
  #define BAT_50PERCENT_VAL   (BAT_MINADCVAL/2+BAT_MAXADCVAL/2)     // 50%
  #define BAT_60PERCENT_VAL   (2*BAT_MINADCVAL/5+3*BAT_MAXADCVAL/5) // 60%
  #define BAT_67PERCENT_VAL   (BAT_MINADCVAL/3+2*BAT_MAXADCVAL/3)   // 67%
  #define BAT_75PERCENT_VAL   (BAT_MINADCVAL/4+3*BAT_MAXADCVAL/4)   // 75%
  #define BAT_80PERCENT_VAL   (BAT_MINADCVAL/5+4*BAT_MAXADCVAL/5)   // 80%

  #define BAT_FLOATING_VAL    50    // ADC浮动超过该值则警告

  #define BAT_ADC_PIN         GPIO_Pin_8    // GPIOA
  #define BAT_CHRG_PIN        GPIO_Pin_16   // GPIOB
  #define BAT_COM_PIN         GPIO_Pin_17   // GPIOB
  #define BAT_IS_CHARGING     (GPIOB_ReadPortPin(BAT_CHRG_PIN) == 0)
  #define BAT_ADC_ENA()       GPIOB_SetBits(BAT_COM_PIN)
  #define BAT_ADC_DIS()       GPIOB_ResetBits(BAT_COM_PIN)

  void BATTERY_Init( void );
  void BATTERY_DMA_ENABLE( void );
  void BATTERY_ADC_Convert( void );
  void BATTERY_ADC_Calculation( void );
  void BATTERY_DrawBMP( void );

  extern UINT16 BAT_abcBuff[ADC_MAXBUFLEN];
  extern UINT32 BAT_adcVal;
  extern UINT32 BAT_adcHistory;

#endif
