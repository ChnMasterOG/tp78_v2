/********************************** (C) COPYRIGHT *******************************
 * File Name          : OLED_UI.h
 * Author             : ChnMasterOG
 * Version            : V1.0
 * Date               : 2022/12/29
 * Description        : OLED UI接口
 * Copyright (c) 2023 ChnMasterOG
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#ifndef __OLED_UI_H
  #define __OLED_UI_H

  #include "CH58x_common.h"

  #define OLED_UI_TASK_MAX          3     // UI最大普通任务数
  #define OLED_UI_DELAY_TASK_MAX    3     // UI最大延迟任务数
  #define OLED_UI_STR_LEN_MAX       17    // UI打印字符串最长字符数
  #define OLED_UI_MAX_SLOT          4     // UI槽的最多显示个数
  #define OLED_UI_MENU_MAX_LEN      3     // UI菜单每级最多选项个数(暂时不支持超过OLED屏幕显示范围)

  #define OLED_UI_HIS_LEN           3                     // 保存OLED打印历史条数
  #define OLED_UI_HIS_DLEN          OLED_UI_STR_LEN_MAX   // 每条OLED打印历史长度

  #define OLED_UI_ICON_WIDTH        10    // uints: pixel
  #define OLED_UI_ICON_PIXEL_HEIGHT 12    // uints: pixel
  #define OLED_UI_ICON_HEIGHT       2     // units: page

  #define OLED_FRESH_RATE           30    // uints: 1Hz

  typedef enum {
    OLED_UI_FLAG_DEFAULT = 0,
    OLED_UI_FLAG_DRAW_OK,
    OLED_UI_FLAG_CANCEL_OK,
    OLED_UI_FLAG_CLEAR_PAGE,
    OLED_UI_FLAG_SHOW_STRING,
    OLED_UI_FLAG_SHOW_INFO,
    OLED_UI_FLAG_SHOW_ICONIFO,
    OLED_UI_FLAG_CANCEL_INFO,
    OLED_UI_FLAG_DRAW_BMP,
    OLED_UI_FLAG_DRAW_SLOT,
    OLED_UI_FLAG_IDLE_DRAW,
    OLED_UI_FLAG_CTL_STOP_SCOLL,
    OLED_UI_FLAG_SMOOTH_SELECT,
    OELD_UI_FLAG_REFRESH_MENU,
    /* only for normal task */
    OLED_UI_FLAG_BAT_CHARGE,
    OLED_UI_FLAG_BAT_CLR_CHARGE,
    OLED_UI_FLAG_BAT_LEVEL_1,   // 0.91寸-0%; 0.66寸-0%
    OLED_UI_FLAG_BAT_LEVEL_2,   // 0.91寸-20%; 0.66寸-33%
    OLED_UI_FLAG_BAT_LEVEL_3,   // 0.91寸-40%; 0.66寸-67%
    OLED_UI_FLAG_BAT_LEVEL_4,   // 0.91寸-60%; 0.66寸-100%
    OLED_UI_FLAG_BAT_LEVEL_5,   // 0.91寸-80%; 0.66寸-reserved
    OLED_UI_FLAG_BAT_LEVEL_6,   // 0.91寸-100%; 0.66寸-reserved
  }oled_ui_data_flag;

  typedef enum {
    OLED_UI_ICON_USB_UNCONNECT_IDX = 0,
    OLED_UI_ICON_USB_IDX,
    OLED_UI_ICON_RF_UNCONNECT_IDX,
    OLED_UI_ICON_RF_IDX,
    OLED_UI_ICON_MODULE_IDX,
    OLED_UI_ICON_NUMLOCK_IDX,
    OLED_UI_ICON_CAPSLOCK_IDX,
    OLED_UI_ICON_BLE_UNCONNECT1_IDX,
    OLED_UI_ICON_BLE_UNCONNECT2_IDX,
    OLED_UI_ICON_BLE_UNCONNECT3_IDX,
    OLED_UI_ICON_BLE_UNCONNECT4_IDX,
    OLED_UI_ICON_BLE1_IDX,
    OLED_UI_ICON_BLE2_IDX,
    OLED_UI_ICON_BLE3_IDX,
    OLED_UI_ICON_BLE4_IDX,
    OLED_UI_ICON_TP_IDX,
    OLED_UI_ICON_LED_STYLE_IDX,
    OLED_UI_ICON_MINIFOC_IDX,
  }oled_ui_icon_index;

  typedef enum {
    OLED_UI_MENU_REFRESH = 0,
    OLED_UI_SWIPE_UP,
    OLED_UI_SWIPE_DOWN,
    OLED_UI_SWIPE_LEFT,
    OLED_UI_SWIPE_RIGHT,
  }oled_ui_swipe;

  typedef enum {
    OLED_UI_TYPE_MENU = 0,
    OLED_UI_TYPE_ENTER_NUM,
    OLED_UI_TYPE_MPR121_STATUS,
    OLED_UI_TYPE_VAL_STATUS,
    OLED_UI_TYPE_EXECUTE,
  }oled_ui_menu_type;

  typedef struct {
    uint8_t x0;
    uint8_t y0;
    uint8_t x1;
    uint8_t y1;
  }oled_ui_pos;

  typedef struct {
    uint8_t x;
    uint8_t y;
    uint16_t len;
  }oled_ui_pos_len;

  typedef struct {
    oled_ui_data_flag flag;
    oled_ui_pos_len pos_len;
    uint8_t* addr;
    uint8_t pstr[OLED_UI_STR_LEN_MAX];
  }oled_ui_draw_structure;

  typedef struct {
    oled_ui_draw_structure oled_ui_draw[OLED_UI_TASK_MAX];
    uint8_t head;
    uint8_t size;
  }oled_ui_task_structure;

  typedef struct {
    oled_ui_draw_structure oled_ui_draw[OLED_UI_DELAY_TASK_MAX];
    uint32_t count[OLED_UI_DELAY_TASK_MAX];
  }oled_ui_delay_task_structure;

  typedef struct {
    uint8_t* icon_p[OLED_UI_MAX_SLOT];
    uint8_t slot_size;
  }oled_ui_slot_structure;

  typedef struct _oled_ui_menu_structure{
    oled_ui_menu_type type; // 菜单指针类型
    uint8_t* p[OLED_UI_MENU_MAX_LEN+3]; // 菜单指针(末三位分别表示: 上级、上翻页、下翻页)
    uint8_t text[OLED_UI_MENU_MAX_LEN][OLED_UI_STR_LEN_MAX];
    uint8_t menu_size;  // 菜单项数目
  }oled_ui_menu_structure;

  typedef struct _oled_ui_enter_num_structure{
    oled_ui_menu_type type; // 菜单指针类型
    uint8_t* p;  // 返回的菜单指向
    const uint8_t* preStr;
    const uint8_t* postStr;
    uint8_t pStr_len;
    uint8_t line; // 对应配置文件的行数
    uint8_t limit_len;
  }oled_ui_enter_num_structure;

  typedef struct _oled_ui_execute_structure{
    oled_ui_menu_type type; // 菜单指针类型
    uint8_t* p;  // 返回的菜单指向
    const uint8_t* preStr;
    uint8_t (*func)(void); // 执行的函数
  }oled_ui_execute_structure;

  typedef struct _oled_ui_mpr121_status_structure{
    oled_ui_menu_type type; // 菜单指针类型
    uint8_t* p;  // 返回的菜单指向
    uint8_t reg;  // 寄存器地址
    BOOL is_half_word;  // 为TRUE读取16bit, 否则8bit
  }oled_ui_mpr121_status_structure;

  typedef struct _oled_ui_val_status_structure{
    oled_ui_menu_type type; // 菜单指针类型
    uint8_t* p;  // 返回的菜单指向
    uint32_t* p_val;  // 变量数据地址
  }oled_ui_val_status_structure;

  #define P_MENU_T(x)         ((const oled_ui_menu_structure*)x)
  #define P_EN_T(x)           ((const oled_ui_enter_num_structure*)x)
  #define P_EXE_T(x)          ((const oled_ui_execute_structure*)x)
  #define P_MPR_STS_T(x)      ((const oled_ui_mpr121_status_structure*)x)
  #define P_VAL_STS_T(x)      ((const oled_ui_val_status_structure*)x)

  extern uint8_t g_oled_fresh_rate;
  extern BOOL g_oled_smooth_end_flag;

  /* declare in OLED_UI_CFG.c */
  extern const oled_ui_menu_structure cfg_menu_1;
  extern const oled_ui_menu_structure cfg_menu_2;
  extern const oled_ui_menu_structure cfg_menu_3;
  extern const oled_ui_menu_structure cfg_menu_4;
  extern const oled_ui_menu_structure key_status_menu_1;
  extern const oled_ui_menu_structure key_status_menu_2;
  extern const oled_ui_menu_structure key_status_menu_3;
  extern const oled_ui_menu_structure key_status_menu_4;
  extern const oled_ui_menu_structure main_menu;
  extern const oled_ui_val_status_structure adc_val_status;
  extern const oled_ui_mpr121_status_structure key_status_mousecap_U;
  extern const oled_ui_mpr121_status_structure key_status_mousecap_D;
  extern const oled_ui_mpr121_status_structure key_status_mousecap_L;
  extern const oled_ui_mpr121_status_structure key_status_mousecap_R;
  extern const oled_ui_mpr121_status_structure key_status_touchbar_1;
  extern const oled_ui_mpr121_status_structure key_status_touchbar_2;
  extern const oled_ui_mpr121_status_structure key_status_touchbar_3;
  extern const oled_ui_mpr121_status_structure key_status_touchbar_4;
  extern const oled_ui_mpr121_status_structure key_status_touchbar_5;
  extern const oled_ui_mpr121_status_structure key_status_touchbar_6;
  extern const oled_ui_mpr121_status_structure key_status_touchbar_7;
  extern const oled_ui_enter_num_structure bledevice_en;
  extern const oled_ui_enter_num_structure ledstyle_en;
  extern const oled_ui_enter_num_structure workmode_en;
  extern const oled_ui_enter_num_structure udiskmode_en;
  extern const oled_ui_enter_num_structure mprparam3_en;
  extern const oled_ui_enter_num_structure mprparam6_en;
  extern const oled_ui_enter_num_structure mprparam7_en;
  extern const oled_ui_enter_num_structure tp_speed_div_en;
  extern const oled_ui_enter_num_structure led_brightness_en;
  extern const oled_ui_enter_num_structure rf_freq_level_en;
  extern const oled_ui_execute_structure scan_tpm;

  void OLED_UI_ShowOK(uint8_t x, uint8_t y, uint8_t s);
  void OLED_UI_ShowCapslock(uint8_t x, uint8_t y, uint8_t s);
  int OLED_UI_printf(char *pFormat, ...);
  uint8_t OLED_UI_add_task(oled_ui_data_flag flag, oled_ui_pos_len pos_len, uint8_t* addr, uint8_t* pstr);
  uint8_t OLED_UI_add_CLEARPAGE_task(uint8_t y0, uint8_t y1);
  uint8_t OLED_UI_add_SHOWSTRING_task(uint8_t x, uint8_t y, char *pstr, ...);
  uint8_t OLED_UI_add_SHOWINFO_task(char *pstr, ...);
  uint8_t OLED_UI_add_SHOW_ICONINFO_task(uint8_t icon_idx, char *pstr, ...);
  uint8_t OLED_UI_add_default_task(oled_ui_data_flag flag);
  uint8_t OLED_UI_add_delay_task(oled_ui_data_flag flag, oled_ui_pos_len pos_len, uint8_t* addr, uint8_t* pstr, uint32_t count);
  uint8_t OLED_UI_add_default_delay_task(oled_ui_data_flag flag, uint32_t count);
  uint8_t OLED_UI_add_CANCELINFO_delay_task(uint32_t count);
  uint8_t OLED_UI_slot_add(uint8_t* p);
  uint8_t OLED_UI_slot_delete(uint8_t *p);
  uint8_t OLED_UI_slot_active(uint8_t *old_p, uint8_t *new_p);
  void OLED_UI_show_version(uint8_t ena);
  void OLED_UI_draw_empty_battery(void);
  void OLED_UI_draw_menu(oled_ui_swipe fresh_type);
  void OLED_UI_smooth_select_cfg(const uint8_t* str0, const uint8_t* str1, uint8_t y0, uint8_t y1);
  void OLED_UI_idle(uint8_t is_entrying);
  void OLED_UI_draw_thread_callback(void);

#endif
/********************************** (C) COPYRIGHT *******************************
 * File Name          : OLED_UI.h
 * Author             : ChnMasterOG
 * Version            : V1.0
 * Date               : 2022/12/29
 * Description        : OLED UI接口
 * Copyright (c) 2023 ChnMasterOG
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#ifndef __OLED_UI_H
  #define __OLED_UI_H

  #include "CH58x_common.h"

  #define OLED_UI_TASK_MAX          5     // UI最大普通任务数
  #define OLED_UI_DELAY_TASK_MAX    3     // UI最大延迟任务数
  #define OLED_UI_STR_LEN_MAX       17    // UI打印字符串最长字符数
  #define OLED_UI_MAX_SLOT          4     // UI槽的最多显示个数
  #define OLED_UI_MENU_MAX_LEN      3     // UI菜单每级最多选项个数(暂时不支持超过OLED屏幕显示范围)

  #define OLED_UI_HIS_LEN           3                     // 保存OLED打印历史条数
  #define OLED_UI_HIS_DLEN          OLED_UI_STR_LEN_MAX   // 每条OLED打印历史长度

  #define OLED_UI_ICON_WIDTH        10    // uints: pixel
  #define OLED_UI_ICON_PIXEL_HEIGHT 12    // uints: pixel
  #define OLED_UI_ICON_HEIGHT       2     // units: page

  #define OLED_FRESH_RATE           30    // uints: 1Hz

  typedef enum {
    OLED_UI_FLAG_DEFAULT = 0,
    OLED_UI_FLAG_DRAW_OK,
    OLED_UI_FLAG_CANCEL_OK,
    OLED_UI_FLAG_CLEAR_PAGE,
    OLED_UI_FLAG_SHOW_STRING,
    OLED_UI_FLAG_SHOW_INFO,
    OLED_UI_FLAG_SHOW_ICONIFO,
    OLED_UI_FLAG_CANCEL_INFO,
    OLED_UI_FLAG_DRAW_BMP,
    OLED_UI_FLAG_DRAW_SLOT,
    OLED_UI_FLAG_IDLE_DRAW,
    OLED_UI_FLAG_CTL_STOP_SCOLL,
    OLED_UI_FLAG_SMOOTH_SELECT,
    OELD_UI_FLAG_REFRESH_MENU,
    /* only for normal task */
    OLED_UI_FLAG_BAT_CHARGE,
    OLED_UI_FLAG_BAT_CLR_CHARGE,
    OLED_UI_FLAG_BAT_LEVEL_1,   // 0.91寸-0%; 0.66寸-0%
    OLED_UI_FLAG_BAT_LEVEL_2,   // 0.91寸-20%; 0.66寸-33%
    OLED_UI_FLAG_BAT_LEVEL_3,   // 0.91寸-40%; 0.66寸-67%
    OLED_UI_FLAG_BAT_LEVEL_4,   // 0.91寸-60%; 0.66寸-100%
    OLED_UI_FLAG_BAT_LEVEL_5,   // 0.91寸-80%; 0.66寸-reserved
    OLED_UI_FLAG_BAT_LEVEL_6,   // 0.91寸-100%; 0.66寸-reserved
  }oled_ui_data_flag;

  typedef enum {
    OLED_UI_ICON_USB_IDX = 0,
    OLED_UI_ICON_RF_IDX,
    OLED_UI_ICON_CP_IDX,
    OLED_UI_ICON_CAPSLOCK_IDX,
    OLED_UI_ICON_BLE_UNCONNECT_IDX,
    OLED_UI_ICON_BLE1_IDX,
    OLED_UI_ICON_BLE2_IDX,
    OLED_UI_ICON_BLE3_IDX,
    OLED_UI_ICON_BLE4_IDX,
    OLED_UI_ICON_TP_IDX,
    OLED_UI_ICON_LED_STYLE_IDX,
  }oled_ui_icon_index;

  typedef enum {
    OLED_UI_MENU_REFRESH = 0,
    OLED_UI_SWIPE_UP,
    OLED_UI_SWIPE_DOWN,
    OLED_UI_SWIPE_LEFT,
    OLED_UI_SWIPE_RIGHT,
  }oled_ui_swipe;

  typedef enum {
    OLED_UI_TYPE_MENU = 0,
    OLED_UI_TYPE_ENTER_NUM,
    OLED_UI_TYPE_MPR121_STATUS,
    OLED_UI_TYPE_VAL_STATUS,
  }oled_ui_menu_type;

  typedef struct {
    uint8_t x0;
    uint8_t y0;
    uint8_t x1;
    uint8_t y1;
  }oled_ui_pos;

  typedef struct {
    uint8_t x;
    uint8_t y;
    uint16_t len;
  }oled_ui_pos_len;

  typedef struct {
    oled_ui_data_flag flag;
    oled_ui_pos_len pos_len;
    uint8_t* addr;
    uint8_t pstr[OLED_UI_STR_LEN_MAX];
  }oled_ui_draw_structure;

  typedef struct {
    oled_ui_draw_structure oled_ui_draw[OLED_UI_TASK_MAX];
    uint8_t head;
    uint8_t size;
  }oled_ui_task_structure;

  typedef struct {
    oled_ui_draw_structure oled_ui_draw[OLED_UI_DELAY_TASK_MAX];
    uint32_t count[OLED_UI_DELAY_TASK_MAX];
  }oled_ui_delay_task_structure;

  typedef struct {
    uint8_t* icon_p[OLED_UI_MAX_SLOT];
    uint8_t slot_size;
  }oled_ui_slot_structure;

  typedef struct _oled_ui_menu_structure{
    oled_ui_menu_type type; // 菜单指针类型
    uint8_t* p[OLED_UI_MENU_MAX_LEN+3]; // 菜单指针(末三位分别表示: 上级、上翻页、下翻页)
    uint8_t text[OLED_UI_MENU_MAX_LEN][OLED_UI_STR_LEN_MAX];
    uint8_t menu_size;  // 菜单项数目
  }oled_ui_menu_structure;

  typedef struct _oled_ui_enter_num_structure{
    oled_ui_menu_type type; // 菜单指针类型
    uint8_t* p;  // 返回的菜单指向
    const uint8_t* preStr;
    const uint8_t* postStr;
    uint8_t pStr_len;
    uint8_t line; // 对应配置文件的行数
    uint8_t limit_len;
  }oled_ui_enter_num_structure;

  typedef struct _oled_ui_mpr121_status_structure{
    oled_ui_menu_type type; // 菜单指针类型
    uint8_t* p;  // 返回的菜单指向
    uint8_t reg;  // 寄存器地址
    BOOL is_half_word;  // 为TRUE读取16bit, 否则8bit
  }oled_ui_mpr121_status_structure;

  typedef struct _oled_ui_val_status_structure{
    oled_ui_menu_type type; // 菜单指针类型
    uint8_t* p;  // 返回的菜单指向
    uint32_t* p_val;  // 变量数据地址
  }oled_ui_val_status_structure;

  #define P_MENU_T(x)         ((const oled_ui_menu_structure*)x)
  #define P_EN_T(x)           ((const oled_ui_enter_num_structure*)x)
  #define P_MPR_STS_T(x)      ((const oled_ui_mpr121_status_structure*)x)
  #define P_VAL_STS_T(x)      ((const oled_ui_val_status_structure*)x)

  extern uint8_t g_oled_fresh_rate;
  extern BOOL g_oled_smooth_end_flag;

  /* declare in OLED_UI_CFG.c */
  extern const oled_ui_menu_structure cfg_menu_1;
  extern const oled_ui_menu_structure cfg_menu_2;
  extern const oled_ui_menu_structure cfg_menu_3;
  extern const oled_ui_menu_structure cfg_menu_4;
  extern const oled_ui_menu_structure key_status_menu_1;
  extern const oled_ui_menu_structure key_status_menu_2;
  extern const oled_ui_menu_structure key_status_menu_3;
  extern const oled_ui_menu_structure key_status_menu_4;
  extern const oled_ui_menu_structure main_menu;
  extern const oled_ui_val_status_structure adc_val_status;
  extern const oled_ui_mpr121_status_structure key_status_mousecap_U;
  extern const oled_ui_mpr121_status_structure key_status_mousecap_D;
  extern const oled_ui_mpr121_status_structure key_status_mousecap_L;
  extern const oled_ui_mpr121_status_structure key_status_mousecap_R;
  extern const oled_ui_mpr121_status_structure key_status_touchbar_1;
  extern const oled_ui_mpr121_status_structure key_status_touchbar_2;
  extern const oled_ui_mpr121_status_structure key_status_touchbar_3;
  extern const oled_ui_mpr121_status_structure key_status_touchbar_4;
  extern const oled_ui_mpr121_status_structure key_status_touchbar_5;
  extern const oled_ui_mpr121_status_structure key_status_touchbar_6;
  extern const oled_ui_mpr121_status_structure key_status_touchbar_7;
  extern const oled_ui_enter_num_structure bledevice_en;
  extern const oled_ui_enter_num_structure ledstyle_en;
  extern const oled_ui_enter_num_structure rfenable_en;
  extern const oled_ui_enter_num_structure udiskmode_en;
  extern const oled_ui_enter_num_structure mprparam3_en;
  extern const oled_ui_enter_num_structure mprparam6_en;
  extern const oled_ui_enter_num_structure mprparam7_en;
  extern const oled_ui_enter_num_structure tp_speed_div_en;
  extern const oled_ui_enter_num_structure led_brightness_en;
  extern const oled_ui_enter_num_structure rf_freq_level_en;

  void OLED_UI_ShowOK(uint8_t x, uint8_t y, uint8_t s);
  void OLED_UI_ShowCapslock(uint8_t x, uint8_t y, uint8_t s);
  int OLED_UI_printf(char *pFormat, ...);
  uint8_t OLED_UI_add_task(oled_ui_data_flag flag, oled_ui_pos_len pos_len, uint8_t* addr, uint8_t* pstr);
  uint8_t OLED_UI_add_CLEARPAGE_task(uint8_t y0, uint8_t y1);
  uint8_t OLED_UI_add_SHOWSTRING_task(uint8_t x, uint8_t y, char *pstr, ...);
  uint8_t OLED_UI_add_SHOWINFO_task(char *pstr, ...);
  uint8_t OLED_UI_add_SHOW_ICONINFO_task(uint8_t icon_idx, char *pstr, ...);
  uint8_t OLED_UI_add_default_task(oled_ui_data_flag flag);
  uint8_t OLED_UI_add_delay_task(oled_ui_data_flag flag, oled_ui_pos_len pos_len, uint8_t* addr, uint8_t* pstr, uint32_t count);
  uint8_t OLED_UI_add_default_delay_task(oled_ui_data_flag flag, uint32_t count);
  uint8_t OLED_UI_add_CANCELINFO_delay_task(uint32_t count);
  uint8_t OLED_UI_slot_add(uint8_t* p);
  uint8_t OLED_UI_slot_delete(uint8_t *p);
  uint8_t OLED_UI_slot_active(uint8_t *old_p, uint8_t *new_p);
  void OLED_UI_show_version(uint8_t ena);
  void OLED_UI_draw_empty_battery(void);
  void OLED_UI_draw_menu(oled_ui_swipe fresh_type);
  void OLED_UI_smooth_select_cfg(const uint8_t* str0, const uint8_t* str1, uint8_t y0, uint8_t y1);
  void OLED_UI_idle(uint8_t is_entrying);
  void OLED_UI_draw_thread_callback(void);

#endif
