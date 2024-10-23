/********************************** (C) COPYRIGHT *******************************
 * File Name          : CORE.c
 * Author             : ChnMasterOG
 * Version            : V1.2
 * Date               : 2023/6/22
 * Description        : 上下电、MCU Sleep相关控制
 * Copyright (c) 2023 ChnMasterOG
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#include "HAL.h"

/*******************************************************************************
 * Function Name  : APPJumpKBoot
 * Description    : 跳转到kboot
 * Input          : 无
 * Return         : 无
 *******************************************************************************/
void APPJumpKBoot(void)
{
#if (defined HAL_WDG) && (HAL_WDG == TRUE)
  WWDG_ResetCfg(DISABLE); // 关看门狗
#endif
  SDRAM32(BOOT_FLAG_SDRAM_ADDRESS) = FLAG_APP_TO_BOOT;
  SYS_ResetExecute();
}

/*******************************************************************************
 * Function Name  : APPJumpBoot
 * Description    : 跳转到芯片bootloader
 * Input          : 无
 * Return         : 无
 *******************************************************************************/
#ifndef LOW_MEM
__HIGH_CODE
void APPJumpBoot(void)   //此段代码必须运行在RAM中
{
  uint32_t irq_status;

  SYS_DisableAllIrq( &irq_status );
#if (defined HAL_WDG) && (HAL_WDG == TRUE)
  WWDG_ResetCfg(DISABLE); // 关看门狗
#endif
  while(FLASH_ROM_ERASE(0, EEPROM_BLOCK_SIZE))
  {
    ;//ROM 擦4K1个单位，擦0地址起始
  }
  FLASH_ROM_SW_RESET();
  R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
  R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;  //安全访问模式
  SAFEOPERATE;
  R16_INT32K_TUNE = 0xFFFF;
  R8_RST_WDOG_CTRL |= RB_SOFTWARE_RESET;
  R8_SAFE_ACCESS_SIG = 0;//进入后执行复位，复位类型为上电复位
  while(1);//营造空片的现象，启动时就会停在BOOT，等烧写，超时时间10s
}
#endif

/*******************************************************************************
 * Function Name  : SoftReset
 * Description    : 软件复位
 * Input          : 无
 * Return         : 无
 *******************************************************************************/
void SoftReset(void)   //软件复位
{
#if (defined HAL_WDG) && (HAL_WDG == TRUE)
  WWDG_ResetCfg(DISABLE); // 关看门狗
#endif
  SYS_ResetExecute();
}

/*******************************************************************************
 * Function Name  : TP78_TMOS_Start
 * Description    : 开启所有线程
 * Input          : 无
 * Return         : 无
 *******************************************************************************/
void TP78_TMOS_Start(void)   //软件复位
{
  tmos_start_task( halTaskID, MAIN_CIRCULATION_EVENT, 10 ); // 主循环
  #if (defined HAL_KEYBOARD) && (HAL_KEYBOARD == TRUE)
    tmos_start_task( halTaskID, HAL_KEYBOARD_EVENT, MS1_TO_SYSTEM_TIME(20) ); // 键盘事件
  #endif
  #if ((defined HAL_MPR121_CAPMOUSE) && (HAL_MPR121_CAPMOUSE == TRUE)) || ((defined HAL_MPR121_TOUCHBAR) && (HAL_MPR121_TOUCHBAR == TRUE))
    tmos_start_task( halTaskID, MPR121_EVENT, MS1_TO_SYSTEM_TIME(30) );  // MPR121
  #endif
  #if ((defined HAL_PS2) && (HAL_PS2 == TRUE)) || ((defined HAL_I2C_TP) && (HAL_I2C_TP == TRUE)) || ((defined HAL_MPR121_CAPMOUSE) && (HAL_MPR121_CAPMOUSE == TRUE))
    tmos_start_task( halTaskID, HAL_MOUSE_EVENT, MS1_TO_SYSTEM_TIME(40) ); // 鼠标事件
  #endif
  #if (defined HAL_WS2812_PWM) && (HAL_WS2812_PWM == TRUE)
    tmos_start_task( halTaskID, WS2812_EVENT, MS1_TO_SYSTEM_TIME(50) );  // 背光控制
  #endif
  #if (defined HAL_OLED) && (HAL_OLED == TRUE)
    tmos_start_task( halTaskID, OLED_UI_EVENT, MS1_TO_SYSTEM_TIME(60) );  // OLED UI
  #endif
  #if ((defined HAL_HW_I2C) && (HAL_HW_I2C == TRUE)) && ((defined HAL_TPM) && (HAL_TPM == TRUE))
    tmos_start_task( halTaskID, TPM_EVENT, MS1_TO_SYSTEM_TIME(30) );  // 扩展模块
  #endif
  #if 0
    tmos_start_task( halTaskID, HAL_TEST_EVENT, 10 );
  #endif
}

/*******************************************************************************
 * Function Name  : TP78_TMOS_Stop
 * Description    : 停止所有线程
 * Input          : 无
 * Return         : 无
 *******************************************************************************/
void TP78_TMOS_Stop(void)   //停止所有线程
{
  tmos_stop_task( halTaskID, MAIN_CIRCULATION_EVENT ); // 主循环
  #if (defined HAL_KEYBOARD) && (HAL_KEYBOARD == TRUE)
    tmos_stop_task( halTaskID, HAL_KEYBOARD_EVENT ); // 键盘事件
  #endif
  #if ((defined HAL_MPR121_CAPMOUSE) && (HAL_MPR121_CAPMOUSE == TRUE)) || ((defined HAL_MPR121_TOUCHBAR) && (HAL_MPR121_TOUCHBAR == TRUE))
    tmos_stop_task( halTaskID, MPR121_EVENT );  // MPR121
  #endif
  #if ((defined HAL_PS2) && (HAL_PS2 == TRUE)) || ((defined HAL_I2C_TP) && (HAL_I2C_TP == TRUE)) || ((defined HAL_MPR121_CAPMOUSE) && (HAL_MPR121_CAPMOUSE == TRUE))
    tmos_stop_task( halTaskID, HAL_MOUSE_EVENT ); // 鼠标事件
  #endif
  #if (defined HAL_WS2812_PWM) && (HAL_WS2812_PWM == TRUE)
    tmos_stop_task( halTaskID, WS2812_EVENT );  // 背光控制
  #endif
  #if (defined HAL_OLED) && (HAL_OLED == TRUE)
    tmos_stop_task( halTaskID, OLED_UI_EVENT );  // OLED UI
  #endif
  #if ((defined HAL_HW_I2C) && (HAL_HW_I2C == TRUE)) && ((defined HAL_TPM) && (HAL_TPM == TRUE))
    tmos_stop_task( halTaskID, TPM_EVENT );  // 扩展模块
  #endif
  #if 0
    tmos_stop_task( halTaskID, HAL_TEST_EVENT );
  #endif
}

/*******************************************************************************
 * Function Name  : TP78Reinit
 * Description    : TP78键盘睡眠唤醒
 * Input          : mode - 0为进入sleep流程; 1为进入wakeup流程
 *                  lp_type - 低功耗模式
 * Return         : 无
 *******************************************************************************/
void TP78Reinit(uint8_t mode, enum LP_Type lp_type)
{
  uint32_t irq_status;

  if (lp_type == lp_mode_none) return;

  if (mode == 0) {  // 进入睡眠
    SYS_DisableAllIrq( &irq_status );
#if (defined HAL_WDG) && (HAL_WDG == TRUE)
    WWDG_ResetCfg(DISABLE); // 关看门狗
#endif
#ifdef HAL_WS2812_PWM
    g_record_last_LED_style = led_style_func;
    led_style_func = WS2812_Style_Off; g_keyboard_status.changeBL = TRUE; WS2812_Send(); // WS2812 OFF
#endif
#if (defined HAL_OLED) && (HAL_OLED == TRUE)
    OLED_WR_Byte(0xAE, OLED_CMD);  // OLED display off
#endif
#if (defined HAL_HW_I2C) && (HAL_HW_I2C == TRUE)
#if (defined HAL_I2C_TP) && (HAL_I2C_TP == TRUE)
    I2C_TP_SendCommand_Sleep();   // 小红点sleep
#endif
#if ((defined HAL_MPR121_CAPMOUSE) && (HAL_MPR121_CAPMOUSE == TRUE)) || ((defined HAL_MPR121_TOUCHBAR) && (HAL_MPR121_TOUCHBAR == TRUE))
    MPR121_WriteReg(MPR121_REG_FG_CDT, 0x7);
#endif
#ifdef HAL_BATTADC
    BAT_ADC_DIS();  // MOS关闭
#endif
#endif
#if (ROW_SCAN_MODE)
    Row_GPIO_(ModeCfg)( Row_Pin_ALL, GPIO_ModeOut_PP_5mA );
    Row_GPIO_(SetBits)( Row_Pin_ALL );
    DelayMs(25);
    Colum_GPIO_(ModeCfg)( Colum_Pin_ALL, GPIO_ModeIN_PD );
#else
    Colum_GPIO_(ModeCfg)( Colum_Pin_ALL, GPIO_ModeOut_PP_20mA );
    Colum_GPIO_(ResetBits)( Colum_Pin_ALL );
    DelayMs(25);
    Row_GPIO_(ModeCfg)( Row_Pin_ALL, GPIO_ModeIN_PU );
#endif
#if (ROW_SCAN_MODE)
    GPIOA_ClearITFlagBit( Colum_Pin_ALL );
    Colum_GPIO_(ITModeCfg)( Colum_Pin_ALL, GPIO_ITMode_RiseEdge ); // 上升沿触发唤醒
    PFIC_EnableIRQ(GPIO_A_IRQn);  // colum GPIO is GPIOA
#else
    GPIOA_ClearITFlagBit( Row_Pin_ALL );  // 用于唤醒
    Row_GPIO_(ITModeCfg)( Row_Pin_ALL, GPIO_ITMode_FallEdge ); // 下降沿触发唤醒
    PFIC_EnableIRQ(GPIO_B_IRQn);  // row GPIO is GPIOB
#endif
    SYS_RecoverIrq( irq_status );
    g_Enable_Status.sleep = TRUE;
  } else {  // 唤醒键盘
    if (lp_type != lp_shutdown_mode) { // 恢复现场
#if (defined HAL_WS2812_PWM)
      led_style_func = g_record_last_LED_style; g_keyboard_status.changeBL = TRUE;
#endif
#if (defined HAL_HW_I2C) && (HAL_HW_I2C == TRUE)
#if (defined HAL_I2C_TP) && (HAL_I2C_TP == TRUE)
      I2C_TP_SendCommand_Wakeup();   // 小红点wake up
#endif
#endif
#if (defined HAL_OLED) && (HAL_OLED == TRUE)
      OLED_WR_Byte(0xAF, OLED_CMD);  // OLED display on
#endif
#if ((defined HAL_MPR121_CAPMOUSE) && (HAL_MPR121_CAPMOUSE == TRUE)) || ((defined HAL_MPR121_TOUCHBAR) && (HAL_MPR121_TOUCHBAR == TRUE))
      MPR121_WriteReg(MPR121_REG_FG_CDT, 0x24);
#endif
#if (defined HAL_WDG) && (HAL_WDG == TRUE)
      WWDG_ResetCfg(ENABLE);  // 开看门狗
#endif
#if (ROW_SCAN_MODE)
      Row_GPIO_(ResetBits)( Row_Pin_ALL );
#else
      Colum_GPIO_(SetBits)( Colum_Pin_ALL );
#endif
    } else SoftReset();
#if (ROW_SCAN_MODE)
    PFIC_DisableIRQ(GPIO_A_IRQn);  // colum GPIO is GPIOA
#else
    PFIC_DisableIRQ(GPIO_B_IRQn);  // row GPIO is GPIOB
#endif
    g_Enable_Status.sleep = FALSE;
  }
}

/*******************************************************************************
 * Function Name  : GotoLowpower
 * Description    : MCU进Low Power模式
 * Input          : type: 低功耗模式类型
 * Return         : 无
 *******************************************************************************/
void GotoLowpower(enum LP_Type type)
{
#if (defined HAL_KEYBOARD) && (HAL_KEYBOARD == TRUE)
  if (type > lp_max_mode) return; // error type
#if (defined MODULE_DEMO) && (MODULE_DEMO == TRUE)
  MODULE_I2C_WR_Reg(reg, dat, addr);  // 控制扩展模块进低功耗模式
#endif
  switch (type)
  {
    case lp_idle_mode: // 空闲模式 - 1.6mA
      TP78Reinit(0, type);
      LowPower_Idle();
      break;
    case lp_sw_mode: // 软件低功耗处理
    case lp_no_sleep_mode:
      TP78Reinit(0, type);
      break;
    case lp_halt_mode: // 暂停模式 - 320uA
      TP78Reinit(0, type);
      LowPower_Halt();
      break;
#ifndef LOW_MEM
    case lp_sleep_mode:  // 睡眠模式 - 0.7uA~2.8uA
      TP78Reinit(0, type);
      LowPower_Sleep(RB_PWR_RAM30K | RB_PWR_RAM2K | RB_PWR_EXTEND);
      break;
    case lp_shutdown_mode: // 下电模式 - 0.2uA~2.3uA
      TP78Reinit(0, type);
      LowPower_Shutdown(0);
      break;
#endif
    default:  // 不睡眠
      return;
  }
#else
  return;
#endif
}
