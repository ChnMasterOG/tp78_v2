/********************************** (C) COPYRIGHT *******************************
 * File Name          : SLEEP.c
 * Author             : WCH
 * Version            : V1.1
 * Date               : 2019/11/05
 * Description        : 睡眠配置及其初始化
 *******************************************************************************/

/******************************************************************************/
/* 头文件包含 */
#include "HAL.h"
#include "CH58x_common.h"

/*******************************************************************************
 * @fn          CH58X_LowPower
 *
 * @brief       启动睡眠
 *
 * input parameters
 *
 * @param       time-唤醒的时间点（RTC绝对值）
 *
 * output parameters
 *
 * @param       
 *
 * @return      None.
 */
u32 CH58X_LowPower( u32 time )
{
#if (defined (HAL_SLEEP)) && (HAL_SLEEP == TRUE)
  u32 tmp, irq_status;

  if (g_Enable_Status.sleep == FALSE) {
    return 2; // 不满足睡眠条件
  }

  SYS_DisableAllIrq( &irq_status );
  tmp = RTC_GetCycle32k();
  if ( ( time < tmp ) || ( ( time - tmp ) < 30 ) )
  {    // 检测睡眠的最短时间
    SYS_RecoverIrq( irq_status );
    return 2;
  }
  RTC_SetTignTime( time ); // 设置RTC的唤醒时间
  SYS_RecoverIrq( irq_status ); // 保存当前中断状态
#if( DEBUG == Debug_UART1 )  // 使用其他串口输出打印信息需要修改这行代码
  while( ( R8_UART1_LSR & RB_LSR_TX_ALL_EMP ) == 0 )
  {
    __nop();
  }
#endif
// LOW POWER-sleep模式
  if ( !RTCTigFlag )
  {
//    LowPower_Sleep( RB_PWR_RAM2K | RB_PWR_RAM30K | RB_PWR_EXTEND );
    LowPower_Halt();
    if( !RTCTigFlag )   // 注意如果使用了RTC以外的唤醒方式，需要注意此时32M晶振未稳定
    {
      time += WAKE_UP_RTC_MAX_TIME;
      if( time > 0xA8C00000 )   time -= 0xA8C00000;
      RTC_SetTignTime( time );
      LowPower_Idle();
    }
    HSECFG_Current( HSE_RCur_100 ); // 降为额定电流(低功耗函数中提升了HSE偏置电流)
  }
  else
  {
    return 3;
  }
#endif
  return 0;
}

/*******************************************************************************
 * @fn          HAL_SleepInit
 *
 * @brief       配置睡眠唤醒的方式-RTC唤醒，触发模式
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       
 *
 * @return      None.
 */
void HAL_SleepInit( void )
{
#if (defined (HAL_SLEEP)) && (HAL_SLEEP == TRUE)
  if (g_lp_type == lp_no_sleep_mode)
    return;
  sys_safe_access_enable();
  if (g_lp_type == lp_sw_mode) {
    R8_SLP_WAKE_CTRL |= RB_SLP_RTC_WAKE | RB_SLP_GPIO_WAKE;    // RTC唤醒 + GPIO唤醒
  } else {
    R8_SLP_WAKE_CTRL |= RB_SLP_GPIO_WAKE;    // GPIO唤醒
  }
  sys_safe_access_disable();
  sys_safe_access_enable();
  R8_RTC_MODE_CTRL |= RB_RTC_TRIG_EN;    // 触发模式
  sys_safe_access_disable();

  if (g_lp_type == lp_sw_mode) {
    PFIC_EnableIRQ( RTC_IRQn );
  }

//  if (g_Enable_Status.usb == TRUE) {
//    PWR_PeriphWakeUpCfg( ENABLE, RB_SLP_USB_WAKE, 0 );
//  }
#endif
}
