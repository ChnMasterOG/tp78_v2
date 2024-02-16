/********************************** (C) COPYRIGHT *******************************
 * File Name          : TP78_MPR121_LIB.h
 * Author             : ChnMasterOG
 * Version            : V2.0
 * Date               : 2023/6/3
 * Description        : mpr121电容传感器驱动库，编译生成TP78_MPR121_LIB.a
 * Copyright (c) 2023 ChnMasterOG
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#ifndef __TP78_MPR121_LIB_H
  #define __TP78_MPR121_LIB_H

  #include <stdio.h>
  #include <string.h>

  //#define CAP_MOUSE_ENABLE                      1
  #define TOUCHBAR_ENABLE                       1
  #define TOUCHBAR_Reverse                      0

  /* type definitions */
  #ifndef _UINT8_T_DECLARED
    typedef unsigned char uint8_t;
  #endif
  #ifndef _UINT16_T_DECLARED
    typedef unsigned short uint16_t;
  #endif
  #ifndef _UINT32_T_DECLARED
    typedef unsigned int uint32_t;
  #endif
  #ifndef FALSE
    #define FALSE                               0
  #endif
  #ifndef TRUE
    #define TRUE                                (!FALSE)
  #endif

  /* CapMouse Pinmux */
  #define MPR121_PINMUX_CapMouseU               1
  #define MPR121_PINMUX_CapMouseD               2
  #define MPR121_PINMUX_CapMouseL               3
  #define MPR121_PINMUX_CapMouseR               0

  /* Touchbar Pinmux */
  #if (defined TOUCHBAR_ENABLE) && (TOUCHBAR_Reverse == TRUE)
    #define MPR121_PINMUX_L1                    10
    #define MPR121_PINMUX_L2                    9
    #define MPR121_PINMUX_L3                    8
    #define MPR121_PINMUX_M                     7
    #define MPR121_PINMUX_R1                    4
    #define MPR121_PINMUX_R2                    5
    #define MPR121_PINMUX_R3                    6
  #elif (defined TOUCHBAR_ENABLE)
    #define MPR121_PINMUX_L1                    4
    #define MPR121_PINMUX_L2                    5
    #define MPR121_PINMUX_L3                    6
    #define MPR121_PINMUX_M                     9
    #define MPR121_PINMUX_R1                    10
    #define MPR121_PINMUX_R2                    7
    #define MPR121_PINMUX_R3                    8
  #else
    #define MPR121_PINMUX_L1                    0
    #define MPR121_PINMUX_L2                    0
    #define MPR121_PINMUX_L3                    0
    #define MPR121_PINMUX_M                     0
    #define MPR121_PINMUX_R1                    0
    #define MPR121_PINMUX_R2                    0
    #define MPR121_PINMUX_R3                    0
  #endif

  #define MPR121_PINMUX_TO_ELE(x)             (x * 2 + MPR121_REG_EFD0LB)

  /* Registers */
  #define MPR121_REG_STS0       0x00  // ELE0~ELE7 Touch Status
  #define MPR121_REG_STS1       0x01  // ELE8~ELE11, ELEPROX Touch Status

  #define MPR121_REG_EFD0LB     0x04  // ELE0 Electrode Filtered Data LSB
  #define MPR121_REG_EFD0HB     0x05  // ELE0 Electrode Filtered Data MSB
  #define MPR121_REG_EFD1LB     0x06  // ELE1 Electrode Filtered Data LSB
  #define MPR121_REG_EFD1HB     0x07  // ELE1 Electrode Filtered Data MSB
  #define MPR121_REG_EFD2LB     0x08  // ELE2 Electrode Filtered Data LSB
  #define MPR121_REG_EFD2HB     0x09  // ELE2 Electrode Filtered Data MSB
  #define MPR121_REG_EFD3LB     0x0A  // ELE3 Electrode Filtered Data LSB
  #define MPR121_REG_EFD3HB     0x0B  // ELE3 Electrode Filtered Data MSB
  #define MPR121_REG_EFD4LB     0x0C  // ELE4 Electrode Filtered Data LSB
  #define MPR121_REG_EFD4HB     0x0D  // ELE4 Electrode Filtered Data MSB
  #define MPR121_REG_EFD5LB     0x0E  // ELE5 Electrode Filtered Data LSB
  #define MPR121_REG_EFD5HB     0x0F  // ELE5 Electrode Filtered Data MSB
  #define MPR121_REG_EFD6LB     0x10  // ELE6 Electrode Filtered Data LSB
  #define MPR121_REG_EFD6HB     0x11  // ELE6 Electrode Filtered Data MSB
  #define MPR121_REG_EFD7LB     0x12  // ELE7 Electrode Filtered Data LSB
  #define MPR121_REG_EFD7HB     0x13  // ELE7 Electrode Filtered Data MSB
  #define MPR121_REG_EFD8LB     0x14  // ELE8 Electrode Filtered Data LSB
  #define MPR121_REG_EFD8HB     0x15  // ELE8 Electrode Filtered Data MSB
  #define MPR121_REG_EFD9LB     0x16  // ELE9 Electrode Filtered Data LSB
  #define MPR121_REG_EFD9HB     0x17  // ELE9 Electrode Filtered Data MSB
  #define MPR121_REG_EFD10LB    0x18  // ELE10 Electrode Filtered Data LSB
  #define MPR121_REG_EFD10HB    0x19  // ELE10 Electrode Filtered Data MSB
  #define MPR121_REG_EFD11LB    0x1A  // ELE10 Electrode Filtered Data LSB
  #define MPR121_REG_EFD11HB    0x1B  // ELE10 Electrode Filtered Data MSB
  #define MPR121_REG_EFDPROXLB  0x1C  // ELEPROX Electrode Filtered Data LSB
  #define MPR121_REG_EFDPROXHB  0x1D  // ELEPROX Electrode Filtered Data MSB

  #define MPR121_REG_MHDR       0x2B  // Maximum Half Delta Rising(1~63).
  #define MPR121_REG_NHDR       0x2C  // Noise Half Delta Rising(1~63).
  #define MPR121_REG_NCLR       0x2D  // Noise Count Limit Rising(0~255).
  #define MPR121_REG_FDLR       0x2E  // Filter Delay Count Limit Rising(0~255).
  #define MPR121_REG_MHDF       0x2F  // Maximum Half Delta Falling(1~63).
  #define MPR121_REG_NHDF       0x30  // Noise Half Delta Falling(1~63).
  #define MPR121_REG_NCLF       0x31  // Noise Count Limit Falling(0~255).
  #define MPR121_REG_FDLF       0x32  // Filter Delay Count Limit Falling(0~255).
  #define MPR121_REG_NHDT       0x33  // NHD Amount Touched
  #define MPR121_REG_NCLT       0x34  // NCL Touched
  #define MPR121_REG_FDLT       0x35  // FDL Touched
  #define MPR121_REG_MHDPROXR   0x36  // ELEPROX MHD Rising
  #define MPR121_REG_NHDPROXR   0x37  // ELEPROX NHD Amount Rising
  #define MPR121_REG_NCLPROXR   0x38  // ELEPROX NCL Rising
  #define MPR121_REG_FDLPROXR   0x39  // ELEPROX FDL Rising
  #define MPR121_REG_MHDPROXF   0x3A  // ELEPROX MHD Falling
  #define MPR121_REG_NHDPROXF   0x3B  // ELEPROX NHD Amount Falling
  #define MPR121_REG_NCLPROXF   0x3C  // ELEPROX NCL Falling
  #define MPR121_REG_FDLPROXF   0x3D  // ELEPROX FDL Falling
  #define MPR121_REG_NHDPROXT   0x3E  // ELEPROX NHD Amount Touched
  #define MPR121_REG_NCLPROXT   0x3F  // ELEPROX NCL Touched
  #define MPR121_REG_FDLPROXT   0x40  // ELEPROX FDL Touched

  #define MPR121_REG_E0TTH      0x41  // ELE0 Touch Threshold
  #define MPR121_REG_E0RTH      0x42  // ELE0 Release Threshold
  #define MPR121_REG_E1TTH      0x43  // ELE1 Touch Threshold
  #define MPR121_REG_E1RTH      0x44  // ELE1 Release Threshold
  #define MPR121_REG_E2TTH      0x45  // ELE2 Touch Threshold
  #define MPR121_REG_E2RTH      0x46  // ELE2 Release Threshold
  #define MPR121_REG_E3TTH      0x47  // ELE3 Touch Threshold
  #define MPR121_REG_E3RTH      0x48  // ELE3 Release Threshold
  #define MPR121_REG_E4TTH      0x49  // ELE4 Touch Threshold
  #define MPR121_REG_E4RTH      0x4A  // ELE4 Release Threshold
  #define MPR121_REG_E5TTH      0x4B  // ELE5 Touch Threshold
  #define MPR121_REG_E5RTH      0x4C  // ELE5 Release Threshold
  #define MPR121_REG_E6TTH      0x4D  // ELE6 Touch Threshold
  #define MPR121_REG_E6RTH      0x4E  // ELE6 Release Threshold
  #define MPR121_REG_E7TTH      0x4F  // ELE7 Touch Threshold
  #define MPR121_REG_E7RTH      0x50  // ELE7 Release Threshold
  #define MPR121_REG_E8TTH      0x51  // ELE8 Touch Threshold
  #define MPR121_REG_E8RTH      0x52  // ELE8 Release Threshold
  #define MPR121_REG_E9TTH      0x53  // ELE9 Touch Threshold
  #define MPR121_REG_E9RTH      0x54  // ELE9 Release Threshold
  #define MPR121_REG_E10TTH     0x55  // ELE10 Touch Threshold
  #define MPR121_REG_E10RTH     0x56  // ELE10 Release Threshold
  #define MPR121_REG_E11TTH     0x57  // ELE11 Touch Threshold
  #define MPR121_REG_E11RTH     0x58  // ELE11 Release Threshold

  #define MPR121_REG_EPROXTTH   0x59  // ELEPROX Touch Threshold
  #define MPR121_REG_EPROXRTH   0x5A  // ELEPROX Release Threshold
  #define MPR121_REG_DTR        0x5B  // Debounce Touch & Release
  #define MPR121_REG_FG_CDC     0x5C  // Filter/Global CDC Configuration
  #define MPR121_REG_FG_CDT     0x5D  // Filter/Global CDT Configuration

  #define MPR121_REG_ACCR0      0x7B  // AUTO-CONFIG Control Register 0
  #define MPR121_REG_ACCR1      0x7C  // AUTO-CONFIG Control Register 1
  #define MPR121_REG_ACUSL      0x7D  // AUTO-CONFIG USL Register
  #define MPR121_REG_ACLSL      0x7E  // AUTO-CONFIG LSL Register
  #define MPR121_REG_ACTLR      0x7F  // AUTO-CONFIG Target Level Register

  #define MPR121_REG_ECR        0x5E  // D7~D6:CL, D5~D4:ELEPROX_EN, D3~D0:ELE_EN
  #define MPR121_REG_SRST       0x80  // write 0x63 to reset

  #define FILTER_CNT            2     // filter cnt which is less than this value
  #define LEFT_BTN_DATA_BIT     0x1   // left btn index of btn_dat/dbtn_dat
  #define MIDDLE_BTN_DATA_BIT   0x2   // middle btn index of btn_dat/dbtn_dat
  #define RIGHT_BTN_DATA_BIT    0x4   // right btn index of btn_dat/dbtn_dat

  /* Global Definitions */
  #define __LIMIT__(data, limit_val)          ((data) >= (limit_val) ? (limit_val) : (data))
  #define __ABS_LIMIT__(data, abs_limit_val)  ((data) > 0 ? ((data) >= (abs_limit_val) ? (abs_limit_val) : (data)) : ((data) <= -(abs_limit_val) ? -(abs_limit_val) : (data)))
  #define __SIGN_STATE__(data)                ((data) > 0 ? 1 : ((data) == 0 ? 0 : -1))
  #define __MIN__(a, b)                       ((a) >= (b) ? (b) : (a))
  #define __BOOL__(exp)                       ((exp) != 0)
  #define __iBOOL__(exp)                      ((exp) == 0)
  #define __REGISTER_SET__(r, v)              (r |= v)
  #define __REGISTER_CLR__(r, v)              (r &= ~v)

  /* MPR121 Algorithm */
  #define ALG_COLLECT_CNT                     2
  #define ALG_BUF_LEN                         2 // must be 2

  #define ALG_MOVE_UP_INDEX                   0
  #define ALG_MOVE_DOWN_INDEX                 1
  #define ALG_MOVE_LEFT_INDEX                 2
  #define ALG_MOVE_RIGHT_INDEX                3

  #define ALG_PARAM_MAGIC                     0x78
  #define ALG_MAX_MOVEMENT                    10

  #define ALG_PINMUX_BIT(x)                   (1 << x)
  #define ALG_CAP_MOUSE_PINMUX_BIT(idx)       (1 << MPR121_Cap_Mouse_Pinmux[idx])
  #define ALG_TOUCHBAR_PINMUX_BIT(idx)        (1 << MPR121_TouchBar_Pinmux[idx])

  #define ALG_ENABLE_TOUCHBAR_SLIDE           0x1
  #define ALG_ENABLE_TOUCHBAR_TOUCH           0x2

  /* HID mouse data */
  #define HID_MOUSE_LBTN_BIT                  0x1
  #define HID_MOUSE_RBTN_BIT                  0x2
  #define HID_MOUSE_MBTN_BIT                  0x4
  #define HID_MOUSE_XSIGN_BIT                 0x10
  #define HID_MOUSE_XSPIN_BIT                 0x20
  #define HID_XOVERFLOW_BIT                   0x40
  #define HID_YOVERFLOW_BIT                   0x80
  #define HID_MOUSE_BTN_IDX                   0
  #define HID_MOUSE_XMOVEMENT_IDX             2
  #define HID_MOUSE_YMOVEMENT_IDX             3

  typedef struct alg_ListNode{
    uint16_t dat;
    struct alg_ListNode* next;
    struct alg_ListNode* prev;
  }mpr121_alg_list_t;

  typedef struct mpr121_alg_param{
    uint16_t magic;
    uint16_t cap_mouse_tou_thr;
    uint16_t cap_mouse_rel_thr;
    uint16_t cap_mouse_move_speed;
    uint16_t touchbar_tou_thr;
    uint16_t touchbar_rel_thr;
    uint16_t double_touch_cnt;
    uint16_t long_touch_cnt;
    uint16_t btn_dat;
    uint16_t dbtn_dat;
    uint16_t l_cnt_dat;
    uint16_t m_cnt_dat;
    uint16_t r_cnt_dat;
  }mpr121_alg_param_t;

  typedef enum {
    DIRECT_OTHER = 0,
    DIRECT_LEFT,
    DIRECT_RIGHT,
    DIRECT_LEFT2,
    DIRECT_RIGHT2,
    DIRECT_LEFT3,
    DIRECT_RIGHT3,
    DIRECT_JUDGE_LEFT,
    DIRECT_JUDGE_RIGHT,
  }mpr121_alg_touchbar_direction_t;

  /* touchbar data format */
  typedef struct mpr121_operation_data {
    /* newest operation(5 bit) */
    uint8_t slide_left : 1;
    uint8_t slide_right : 1;
    uint8_t l_btn : 1;
    uint8_t m_btn : 1;
    uint8_t r_btn : 1;
    /* status(2 bit) */
    uint8_t cap_mouse_data_change : 1;
    uint8_t touchbar_data_change : 1;
    /* last operation(5 bit) */
    uint8_t slide_left_l : 1;
    uint8_t slide_right_l : 1;
    uint8_t l_btn_l : 1;
    uint8_t m_btn_l : 1;
    uint8_t r_btn_l : 1;
    uint8_t reserved : 4;
  }mpr121_operation_data_t;

  typedef struct _mpr121_interface_t{
    /* i2c interface */
    uint8_t (*i2c_write_reg)(uint8_t, uint8_t, uint8_t);  // i2c_write_reg(reg, dat, addr)
    uint8_t (*i2c_muti_read)(uint8_t, uint8_t*, uint8_t, uint8_t);  // i2c_muti_read(reg, *dat, addr, len)
    uint8_t i2c_addr;
    /* HID mouse data pointer */
    uint8_t *HID_mouse_data;
  }mpr121_interface_t;

  #if defined(CAP_MOUSE_ENABLE) && (CAP_MOUSE_ENABLE != 0)
    extern const uint8_t MPR121_Cap_Mouse_Pinmux[4];
  #endif
  #if defined(TOUCHBAR_ENABLE) && (TOUCHBAR_ENABLE != 0)
    extern const uint8_t MPR121_TouchBar_Pinmux[7];
  #endif

  /*******************************************************************************
   * Function Name  : MPR121_lib_init
   * Description    : 初始化MPR121库
   * Input          : buf - 错误信息
   *                  p_interface - mpr121接口指针
   *                  p_alg_parameter - mpr121算法参数指针
   * Return         : None
   *******************************************************************************/
  void MPR121_lib_init(char* buf, mpr121_interface_t* p_interface, mpr121_alg_param_t* p_alg_parameter);

  /*******************************************************************************
   * Function Name  : MPR121_get_result
   * Description    : 获取MPR121操作结果
   * Input          : p - 操作结果指针
   * Return         : None
   *******************************************************************************/
  void MPR121_get_result(mpr121_operation_data_t* p);

  /*******************************************************************************
   * Function Name  : MPR121_set_result
   * Description    : 设置MPR121操作结果
   * Input          : p - 操作结果指针
   * Return         : None
   *******************************************************************************/
  void MPR121_set_result(mpr121_operation_data_t* p);

  /*******************************************************************************
   * Function Name  : MPR121_update_data
   * Description    : 更新MPR121数据
   * Input          : None
   * Return         : None
   *******************************************************************************/
  void MPR121_update_data(void);

  #if defined(CAP_MOUSE_ENABLE) && (CAP_MOUSE_ENABLE != 0)
    /*******************************************************************************
     * Function Name  : MPR121_alg_judge_cap_mouse
     * Description    : MPR121算法判断触摸板
     * Input          : None
     * Return         : None
     *******************************************************************************/
    void MPR121_alg_judge_cap_mouse(void);
  #endif
  #if defined(TOUCHBAR_ENABLE) && (TOUCHBAR_ENABLE != 0)
    /*******************************************************************************
     * Function Name  : MPR121_alg_judge_touchbar
     * Description    : MPR121算法判断触摸条
     * Input          : mode - 模式：bit 0 - 使能滑动判断; bit 1 - 使能触摸判断
     * Return         : None
     *******************************************************************************/
    void MPR121_alg_judge_touchbar(uint8_t mode);
  #endif
#endif
