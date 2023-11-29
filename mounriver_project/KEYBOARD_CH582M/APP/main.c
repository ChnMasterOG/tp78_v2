/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : ChnMasterOG, WCH
 * Version            : V2.0
 * Date               : 2022/11/30
 * Description        : TP78v2 USB+蓝牙+2.4G三模小红点键盘应用主函数及任务系统初始化
 * Copyright (c) 2023 ChnMasterOG
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

/********************************** (C) COPYRIGHT *******************************
 *  Pin information:
 *  PB10 & PB11 --- USB1_D-/USB1_D+; [PA0~PA7 & PA9 & PA11~PA15] & [PB0/PB1/PB2/PB3/PB5/PB6] --- KeyBoard
 *  PB15(*) & PB14 --- PS/2 Reserved; PA10 --- WS2812(TMR1); PB13 & PB12 --- SCL/SDA
 *  PA8 --- Battery ADC; PB16 & PB17 --- Battery COM/CHRG; PB7 & PB4 --- TXD0/RXD0;
 *  PB21 & PB20 ---TXD3_/RXD3_; PB15(*) --- I2C TrackPoint IRQ; PB18 --- MPR121 IRQ; PB19 --- Motor CTL
 ********************************* (C) COPYRIGHT ********************************/

/*********************************************************************
 * INCLUDES
 */
#include "HAL.h"
#include "CONFIG.h"
#include "CH58x_common.h"
#include "hiddev.h"

/*********************************************************************
 * GLOBAL TYPEDEFS
 */
__attribute__((aligned(4))) u32 MEM_BUF[BLE_MEMHEAP_SIZE/4]; //定义长度为BLE_MEMHEAP_SIZE/4 的 u32 数组MEM_BUF,并按4字节对齐，MEM_BUF 蓝牙库专用ram

#if (defined (BLE_MAC)) && (BLE_MAC == TRUE)
u8C MacAddr[6] = {0x84, 0xC2, 0xE5, 0x78, 0x73, 0x01};
#endif

/*******************************************************************************
* Function Name  : Main_Circulation
* Description    : 主循环
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__HIGH_CODE
void Main_Circulation()
{
  // 开启TMOS任务调度
  while(1){
    TMOS_SystemProcess( );
  }
}

/*******************************************************************************
* Function Name  : main
* Description    : 主函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main( void )
{
#if (defined (DCDC_ENABLE)) && (DCDC_ENABLE == TRUE)
  PWR_DCDCCfg( ENABLE );
#endif
  SetSysClock( CLK_SOURCE_PLL_60MHz );
#if (defined (HAL_SLEEP)) && (HAL_SLEEP == TRUE)
  GPIOA_ModeCfg( GPIO_Pin_All, GPIO_ModeIN_PU );
  GPIOB_ModeCfg( GPIO_Pin_All, GPIO_ModeIN_PU );
#endif
#ifdef DEBUG  // DEBUG = 3(use UART3)
  GPIOB_SetBits(GPIO_Pin_21);
  GPIOB_ModeCfg(GPIO_Pin_20, GPIO_ModeIN_PU);       // RXD-配置上拉输入
  GPIOB_ModeCfg(GPIO_Pin_21, GPIO_ModeOut_PP_5mA);  // TXD-配置推挽输出，注意先让IO口输出高电平
  GPIOPinRemap(ENABLE, RB_PIN_UART3);
  UART3_DefInit();
#endif
  PRINT("%s\n",VER_LIB);
  FLASH_Init( );
  CH58X_BLEInit( ); //蓝牙初始化
  HAL_Init( ); //硬件初始化
#if (defined HAL_USB) && (HAL_USB == TRUE)
  if (g_Enable_Status.usb == TRUE) {
    HAL_USBInit( );
  }
#endif
  if (g_Enable_Status.ble == TRUE) {
    GAPRole_PeripheralInit( );  // 蓝牙层级决策功能初始化，闭源
    HidDev_Init( ); // 蓝牙广播服务初始化（注册和启动），闭源
    HidEmu_Init( ); // 蓝牙广播服务初始化（参数设置），闭源
  }
#if (defined (HAL_RF)) && (HAL_RF == TRUE)
  if (g_Enable_Status.rf == TRUE) {
    RF_RoleInit();
    RF_Init();
  }
#endif
  Main_Circulation();
}

/*******************************************************************************
* Function Name  : GPIOA_IRQHandler
* Description    : GPIOA外部中断
* Input          : None
* Return         : None
*******************************************************************************/
__INTERRUPT
__HIGH_CODE
void GPIOA_IRQHandler(void)
{
#if (ROW_SCAN_MODE)
  GPIOA_ClearITFlagBit(Colum_Pin_ALL);  // 用于唤醒
  TP78Reinit(1, g_lp_type);
  g_lp_type = lp_mode_none;
#endif
}

/*******************************************************************************
* Function Name  : GPIOB_IRQHandler
* Description    : GPIOB外部中断
* Input          : None
* Return         : None
*******************************************************************************/
__INTERRUPT
__HIGH_CODE
void GPIOB_IRQHandler( void )
{
#if (defined HAL_PS2) && (HAL_PS2 == TRUE)
  PS2_IT_handler();
#endif
#if 0
#if (defined HAL_I2C_TP) && (HAL_I2C_TP == TRUE)
  I2C_TP_IT_handler();
#endif
#endif
#if (!ROW_SCAN_MODE)
  if (Row_GPIO_(ReadITFlagBit)(Row_Pin_ALL) != 0) {
    GPIOB_ClearITFlagBit(Row_Pin_ALL);  // 用于唤醒
    TP78Reinit(1, g_lp_type);
    g_lp_type = lp_mode_none;
  }
#endif
}

/******************************** endfile @ main ******************************/
