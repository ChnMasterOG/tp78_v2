/********************************** (C) COPYRIGHT *******************************
 * File Name          : MPR121.c
 * Author             : ChnMasterOG
 * Version            : V2.0
 * Date               : 2023/6/1
 * Description        : mpr121电容传感器开源驱动
 * Copyright (c) 2023 ChnMasterOG
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#include "MPR121.h"

/*******************************************************************************
 * Function Name  : DATAFLASH_Read_MPR121_ALG_Parameter
 * Description    : 从DataFlash读取MPR121算法参数
 * Input          : p - 算法参数指针
 * Return         : None
 *******************************************************************************/
void DATAFLASH_Read_MPR121_ALG_Parameter(mpr121_alg_param_t* p)
{
  HAL_Fs_Read_keyboard_cfg(FS_LINE_MPR_ALG_MAGIC, 8, (uint16_t*)p);
  if (p->magic != ALG_PARAM_MAGIC) {
    // use default parameter
    p->magic = ALG_PARAM_MAGIC;
    p->cap_mouse_tou_thr = CAP_MOUSE_TOU_THRESH;
    p->cap_mouse_rel_thr = CAP_MOUSE_REL_THRESH;
    p->cap_mouse_move_speed = CAP_MOUSE_MOVE_SPEED;
    p->touchbar_tou_thr = TOUCHBAR_TOU_THRESH;
    p->touchbar_rel_thr = TOUCHBAR_REL_THRESH;
    p->double_touch_cnt = DOUBLE_TOUCH_CNT;
    p->long_touch_cnt = LONG_TOUCH_CNT;
  }
}

/*******************************************************************************
  * Function Name  : DATAFLASH_Write_MPR121_ALG_Parameter
  * Description    : 将MPR121算法参数写入DataFlash
  * Input          : p - 算法参数指针
  * Return         : None
  *******************************************************************************/
void DATAFLASH_Write_MPR121_ALG_Parameter(mpr121_alg_param_t* p)
{
  HAL_Fs_Write_keyboard_cfg(FS_LINE_MPR_ALG_MAGIC, 8, (uint16_t*)&p);
}

/*******************************************************************************
 * Function Name  : DATAFLASH_Read_MPR121_FUN_Parameter
 * Description    : 从DataFlash读取MPR121功能配置参数
 * Input          : None
 * Return         : None
 *******************************************************************************/
void DATAFLASH_Read_MPR121_FUN_Parameter(void)
{
  uint16_t val;

  HAL_Fs_Read_keyboard_cfg(FS_LINE_TOUCHBAR_BUTTON_CTL_ENA, 1, &val);
  g_Enable_Status.touchbar_button = val;
}

/*******************************************************************************
  * Function Name  : DATAFLASH_Write_MPR121_FUN_Parameter
  * Description    : 将MPR121功能配置参数写入DataFlash
  * Input          : None
  * Return         : None
  *******************************************************************************/
void DATAFLASH_Write_MPR121_FUN_Parameter(void)
{
  uint16_t val;

  val = g_Enable_Status.touchbar_button;
  HAL_Fs_Write_keyboard_cfg(FS_LINE_TOUCHBAR_BUTTON_CTL_ENA, 1, &val);
}

/*******************************************************************************
 * Function Name  : MPR121_GPIO_Init
 * Description    : MPR121初始化GPIO
 * Input          : None
 * Return         : None
 *******************************************************************************/
void MPR121_GPIO_Init(void)
{
  /* config MPRINT_Pin */
  MPRINT_GPIO_(SetBits)( MPRINT_Pin );
  MPRINT_GPIO_(ModeCfg)( MPRINT_Pin, GPIO_ModeIN_PU );

  /* MPR121 IRQ */
  /*
  MPRINT_GPIO_(ITModeCfg)( MPRINT_Pin, GPIO_ITMode_FallEdge );
  PFIC_EnableIRQ( GPIO_B_IRQn );  //MPRINT_GPIO
  */
}

/*******************************************************************************
 * Function Name  : MPR121_Init
 * Description    : 初始化MPR121
 * Input          : buf - 报错信息
 * Return         : None
 *******************************************************************************/
void MPR121_Init(char* buf)
{
  mpr121_interface_t mpr_interface;
  mpr121_alg_param_t mpr_alg_parameter;

  // 1. read mpr_alg_parameter & function configurations from flash
  DATAFLASH_Read_MPR121_ALG_Parameter(&mpr_alg_parameter);
  DATAFLASH_Read_MPR121_FUN_Parameter();
  // 2. config GPIO for MPR121
  MPR121_GPIO_Init();
  // 3. config mpr121_interface
  mpr_interface.i2c_write_reg = HW_I2C_WR_Reg;
  mpr_interface.i2c_muti_read = HW_I2C_Muti_RD_Reg;
  mpr_interface.i2c_addr = MPR121_ADDR;
  mpr_interface.HID_mouse_data = HIDMouse;
  // 4. run MPR121_lib_init
  MPR121_lib_init(buf, &mpr_interface, &mpr_alg_parameter);
}

/*******************************************************************************
 * Function Name  : MPR121_Post_Operation
 * Description    : MPR121操作结果计算后执行
 * Input          : None
 * Return         : None
 *******************************************************************************/
void MPR121_Post_Operation(void)
{
  mpr121_operation_data_t oper_dat;

  MPR121_get_result(&oper_dat);
  if (oper_dat.slide_left && oper_dat.slide_left_l == FALSE) {
    oper_dat.slide_left_l = TRUE;
    MPR121_set_result(&oper_dat);
    OLED_UI_add_SHOWINFO_task("<<<<<<");
    MOTOR_GO();
  } else if (oper_dat.slide_right && oper_dat.slide_right_l == FALSE) {
    oper_dat.slide_right_l = TRUE;
    MPR121_set_result(&oper_dat);
    OLED_UI_add_SHOWINFO_task(">>>>>>");
    MOTOR_GO();
  } else if (oper_dat.l_btn && oper_dat.l_btn_l == FALSE) {
    oper_dat.l_btn_l = TRUE;
    MPR121_set_result(&oper_dat);
    OLED_UI_add_SHOWINFO_task("L Btn");
    MOTOR_GO();
  } else if (oper_dat.m_btn && oper_dat.m_btn_l == FALSE) {
    oper_dat.m_btn_l = TRUE;
    MPR121_set_result(&oper_dat);
    OLED_UI_add_SHOWINFO_task("M Btn");
    MOTOR_GO();
  } else if (oper_dat.r_btn && oper_dat.r_btn_l == FALSE) {
    oper_dat.r_btn_l = TRUE;
    MPR121_set_result(&oper_dat);
    OLED_UI_add_SHOWINFO_task("R Btn");
    MOTOR_GO();
  } else return;
  OLED_UI_add_CANCELINFO_delay_task(2000);
}
