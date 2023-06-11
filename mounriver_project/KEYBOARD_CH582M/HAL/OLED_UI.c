/********************************** (C) COPYRIGHT *******************************
 * File Name          : OLED_UI.c
 * Author             : ChnMasterOG
 * Version            : V1.0
 * Date               : 2023/1/28
 * Description        : OLED UI接口
 * Copyright (c) 2023 ChnMasterOG
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#include "HAL.h"
#include <stdarg.h>

static oled_ui_task_structure oled_ui_task = { 0 };
static oled_ui_delay_task_structure oled_ui_delay_task = { 0 };
static oled_ui_slot_structure oled_ui_slot = { 0 };

uint8_t g_oled_fresh_rate = OLED_FRESH_RATE;  // OLED刷新率控制
BOOL g_oled_smooth_end_flag = TRUE;

static uint8_t oled_printf_history[OLED_UI_HIS_LEN][OLED_UI_HIS_DLEN+1] = {};  // 存放OLED_printf的历史记录
static uint8_t oled_printf_history_idx = 0;  // 存放OLED打印历史的下标

static uint8_t oled_ui_entry_idle_frame = 0;
static uint8_t *cur_menu_p = (uint8_t*)&main_menu; // 当前菜单指向的位置
static uint8_t menu_cur_idx = 0;

#ifdef OLED_0_66
static uint8_t oled_smooth_select_str0[OLED_UI_STR_LEN_MAX];
static uint8_t oled_smooth_select_str1[OLED_UI_STR_LEN_MAX];
static oled_ui_pos_len oled_smooth_select_pos_len[2];
static uint8_t oled_smooth_w_cnt = 0; // 用于选中框宽度计数
static uint8_t oled_smooth_y_cnt = 0; // 用于选中框y滑动变化计数
#endif

/*******************************************************************************
 * Function Name  : OLED_ShowOK
 * Description    : OLED显示OK(打勾)
 * Input          : x - x坐标; y - 页坐标; s - state, 显示打勾1或取消打勾0
 * Return         : None
 *******************************************************************************/
void OLED_UI_ShowOK(uint8_t x, uint8_t y, uint8_t s)
{
  uint8_t i;
  OLED_Set_Pos(x, y);
  if (FONT_SIZE == 8) {
    if (s != 0) for(i = 0; i < 6; i++) OLED_WR_Byte(F6x8[92][i], OLED_DATA);
    else for(i = 0; i < 6; i++) OLED_WR_Byte(0x00, OLED_DATA);
  }
}

/*******************************************************************************
 * Function Name  : OLED_ShowCapslock
 * Description    : OLED显示Capslock(大小写)
 * Input          : x - x坐标; y - 页坐标; s - state, 显示大小写被按下1或不显示0
 * Return         : None
 *******************************************************************************/
void OLED_UI_ShowCapslock(uint8_t x, uint8_t y, uint8_t s)
{
  uint8_t i;
  OLED_Set_Pos(x, y);
  if (FONT_SIZE == 8) {
    if (s != 0) for(i = 0; i < 6; i++) OLED_WR_Byte(F6x8[93][i], OLED_DATA);
    else for(i = 0; i < 6; i++) OLED_WR_Byte(F6x8[94][i], OLED_DATA);
  }
}

/*******************************************************************************
 * Function Name  : OLED_UI_TP78Info
 * Description    : TP78消息显示
 * Input          : *chr - 要显示的字符串
 * Return         : None
 *******************************************************************************/
static void OLED_UI_TP78Info(uint8_t *chr)
{
#if defined(OLED_0_91) || defined(OLED_0_66)
  // 清空原有信息
  uint8_t i;
  OLED_Set_Pos(0, 3);
  for (i = 0; i < OLED_WIDTH; i++) {
    OLED_WR_Byte(0x00, OLED_DATA);
  }
  // 定义左下角输出信息
  OLED_ShowString(OLED_Midx(strlen(chr), 0, OLED_WIDTH), 3, chr);
#endif
}

/*******************************************************************************
 * Function Name  : OLED_printf
 * Description    : OLED printf重定向函数
 * Input          : *pFormat - 要显示格式化的字符串
 * Return         : 输出长度
 *******************************************************************************/
int OLED_UI_printf(char *pFormat, ...)
{
  char pStr[OLED_UI_STR_LEN_MAX] = {'\0'};
  va_list ap;
  int res;

  va_start(ap, pFormat);
  res = vsprintf((char*)pStr, pFormat, ap);
  va_end(ap);

  OLED_UI_TP78Info(pStr);

  /* 记录至历史 */
  if (strlen(pStr) > OLED_UI_HIS_DLEN) { // 截取长度
    pStr[OLED_UI_HIS_DLEN-1] = '\0';
  }
  if (oled_printf_history_idx >= OLED_UI_HIS_LEN) {  // 缓存满 - 队列结构
    memcpy((uint8_t*)oled_printf_history[0], (uint8_t*)oled_printf_history[1], (OLED_UI_HIS_LEN-1)*(OLED_UI_HIS_DLEN));
    strcpy(oled_printf_history[OLED_UI_HIS_LEN-1], pStr);
  } else {
    oled_printf_history_idx++;
  }

  return res;
}

/*******************************************************************************
 * Function Name  : OLED_UI_add_task
 * Description    : OLED_UI添加任务
 * Input          : flag - new task flag;
 *                  pos_len - new task pos and len;
 *                  addr - pointer of BMP;
 *                  pstr - pointer of string.
 * Return         : 0 - success, 1 - failed
 *******************************************************************************/
uint8_t OLED_UI_add_task(oled_ui_data_flag flag, oled_ui_pos_len pos_len, uint8_t* addr, uint8_t* pstr)
{
  uint8_t index;

  if (oled_ui_task.size >= OLED_UI_TASK_MAX) {
    return 1;
  }
  index = oled_ui_task.head + oled_ui_task.size;
  if (index >= OLED_UI_TASK_MAX) {
    index = index - OLED_UI_TASK_MAX;
  }
  oled_ui_task.oled_ui_draw[index].flag = flag;
  oled_ui_task.oled_ui_draw[index].pos_len = pos_len;
  oled_ui_task.oled_ui_draw[index].addr = addr;
  strcpy(oled_ui_task.oled_ui_draw[index].pstr, pstr);  // assert strlen(pstr)+1 < OLED_UI_STR_LEN_MAX
  oled_ui_task.size++;
  return 0;
}

/*******************************************************************************
 * Function Name  : OLED_UI_add_CLEARPAGE_task
 * Description    : OLED_UI添加清空PAGE任务
 * Input          : y0 - start page;
 *                  y1 - end page;
 * Return         : 0 - success, 1 - failed
 *******************************************************************************/
uint8_t OLED_UI_add_CLEARPAGE_task(uint8_t y0, uint8_t y1)
{
  uint8_t index;
  oled_ui_pos_len pos_len = {.x = y0, .y = y1, .len = 0};

  if (oled_ui_task.size >= OLED_UI_TASK_MAX) {
    return 1;
  }
  index = oled_ui_task.head + oled_ui_task.size;
  if (index >= OLED_UI_TASK_MAX) {
    index = index - OLED_UI_TASK_MAX;
  }
  oled_ui_task.oled_ui_draw[index].flag = OLED_UI_FLAG_CLEAR_PAGE;
  oled_ui_task.oled_ui_draw[index].pos_len = pos_len;
  oled_ui_task.size++;
  return 0;
}

/*******************************************************************************
 * Function Name  : OLED_UI_add_SHOWSTRING_task
 * Description    : OLED_UI添加打印字符串任务
 * Input          : x - position X;
 *                  y - position Y;
 *                  pstr - pointer of string, and parameterization.
 * Return         : 0 - success, 1 - failed
 *******************************************************************************/
uint8_t OLED_UI_add_SHOWSTRING_task(uint8_t x, uint8_t y, char *pstr, ...)
{
  char pStr[OLED_UI_STR_LEN_MAX] = {'\0'};
  va_list ap;
  uint8_t index;
  oled_ui_pos_len pos_len = {.x = x, .y = y, .len = 0};

  if (oled_ui_task.size >= OLED_UI_TASK_MAX) {
    return 1;
  }

  va_start(ap, pstr);
  vsprintf((char*)pStr, pstr, ap);
  va_end(ap);

  index = oled_ui_task.head + oled_ui_task.size;
  if (index >= OLED_UI_TASK_MAX) {
    index = index - OLED_UI_TASK_MAX;
  }
  oled_ui_task.oled_ui_draw[index].flag = OLED_UI_FLAG_SHOW_STRING;
  oled_ui_task.oled_ui_draw[index].pos_len = pos_len;
  strcpy(oled_ui_task.oled_ui_draw[index].pstr, pStr);  // assert strlen(pstr)+1 < OLED_UI_STR_LEN_MAX
  oled_ui_task.size++;

  return 0;
}

/*******************************************************************************
 * Function Name  : OLED_UI_add_SHOWINFO_task
 * Description    : OLED_UI添加打印信息任务
 * Input          : pstr - pointer of string, and parameterization.
 * Return         : 0 - success, 1 - failed
 *******************************************************************************/
uint8_t OLED_UI_add_SHOWINFO_task(char *pstr, ...)
{
  char pStr[OLED_UI_STR_LEN_MAX] = {'\0'};
  va_list ap;
  uint8_t index;

  if (oled_ui_task.size >= OLED_UI_TASK_MAX) {
    return 1;
  }

  va_start(ap, pstr);
  vsprintf((char*)pStr, pstr, ap);
  va_end(ap);

  index = oled_ui_task.head + oled_ui_task.size;
  if (index >= OLED_UI_TASK_MAX) {
    index = index - OLED_UI_TASK_MAX;
  }
  oled_ui_task.oled_ui_draw[index].flag = OLED_UI_FLAG_SHOW_INFO;
  strcpy(oled_ui_task.oled_ui_draw[index].pstr, pStr);  // assert strlen(pstr)+1 < OLED_UI_STR_LEN_MAX
  oled_ui_task.size++;

  return 0;
}

/*******************************************************************************
 * Function Name  : OLED_UI_add_SHOW_ICONINFO_task
 * Description    : OLED_UI添加打印图标和信息任务
 * Input          : icon_idx - UI_Slot_Icon index; pstr - pointer of string, and parameterization.
 * Return         : 0 - success, 1 - failed
 *******************************************************************************/
uint8_t OLED_UI_add_SHOW_ICONINFO_task(uint8_t icon_idx, char *pstr, ...)
{
  char pStr[OLED_UI_STR_LEN_MAX] = {'\0'};
  va_list ap;
  uint8_t index;

  if (oled_ui_task.size >= OLED_UI_TASK_MAX) {
    return 1;
  }

  va_start(ap, pstr);
  vsprintf((char*)pStr, pstr, ap);
  va_end(ap);

  index = oled_ui_task.head + oled_ui_task.size;
  if (index >= OLED_UI_TASK_MAX) {
    index = index - OLED_UI_TASK_MAX;
  }
  oled_ui_task.oled_ui_draw[index].flag = OLED_UI_FLAG_SHOW_ICONIFO;
  oled_ui_task.oled_ui_draw[index].addr = (uint8_t*)&UI_Slot_Icon[icon_idx];
  strcpy(oled_ui_task.oled_ui_draw[index].pstr, pStr);  // assert strlen(pstr)+1 < OLED_UI_STR_LEN_MAX
  oled_ui_task.size++;

  return 0;
}

/*******************************************************************************
 * Function Name  : OLED_UI_add_default_task
 * Description    : OLED_UI添加默认任务
 * Input          : flag - task flag;
 * Return         : 0 - success, 1 - failed
 *******************************************************************************/
uint8_t OLED_UI_add_default_task(oled_ui_data_flag flag)
{
  oled_ui_pos_len pos_len = {.x = 0, .y = 0, .len = 0};
  return OLED_UI_add_task(flag, pos_len, NULL, NULL);
}

/*******************************************************************************
 * Function Name  : OLED_UI_add_delay_task
 * Description    : OLED_UI添加延迟任务
 * Input          : flag - new task flag;
 *                  pos_len - new task pos and len;
 *                  addr - pointer of BMP;
 *                  pstr - pointer of string;
 *                  count - the delay counter(uints: ms).
 * Return         : 0 - success, 1 - failed
 *******************************************************************************/
uint8_t OLED_UI_add_delay_task(oled_ui_data_flag flag, oled_ui_pos_len pos_len, uint8_t* addr, uint8_t* pstr, uint32_t count)
{
  uint8_t i;

  count = count / g_oled_fresh_rate;  // transfer to fresh cycle

  for (i = 0; i < OLED_UI_DELAY_TASK_MAX; i++) {
    if (oled_ui_delay_task.oled_ui_draw[i].flag == OLED_UI_FLAG_DEFAULT) {
      break;
    }
  }
  if (i >= OLED_UI_DELAY_TASK_MAX) {
    return 1;
  }
  oled_ui_delay_task.oled_ui_draw[i].flag = flag;
  oled_ui_delay_task.oled_ui_draw[i].pos_len = pos_len;
  oled_ui_delay_task.oled_ui_draw[i].addr = addr;
  strcpy(oled_ui_delay_task.oled_ui_draw[i].pstr, pstr);  // assert strlen(pstr)+1 < OLED_UI_STR_LEN_MAX
  oled_ui_delay_task.count[i] = count;
  return 0;
}

/*******************************************************************************
 * Function Name  : OLED_UI_add_default_delay_task
 * Description    : OLED_UI添加默认延迟任务
 * Input          : flag - task flag; count - the delay counter(uints: ms).
 * Return         : 0 - success, 1 - failed
 *******************************************************************************/
uint8_t OLED_UI_add_default_delay_task(oled_ui_data_flag flag, uint32_t count)
{
  oled_ui_pos_len pos_len = {.x = 0, .y = 0, .len = 0};
  return OLED_UI_add_delay_task(flag, pos_len, NULL, NULL, count);
}

/*******************************************************************************
 * Function Name  : OLED_UI_add_CANCELINFO_delay_task
 * Description    : OLED_UI添加清除字符串延迟任务
 * Input          : pstr - pointer of string;
 *                  count - the delay counter(uints: ms).
 * Return         : 0 - success, 1 - failed
 *******************************************************************************/
uint8_t OLED_UI_add_CANCELINFO_delay_task(uint32_t count)
{
  uint8_t i;

  count = count / g_oled_fresh_rate;  // transfer to fresh cycle

  for (i = 0; i < OLED_UI_DELAY_TASK_MAX; i++) {
    if (oled_ui_delay_task.oled_ui_draw[i].flag == OLED_UI_FLAG_DEFAULT) {
      break;
    }
  }
  if (i >= OLED_UI_DELAY_TASK_MAX) {
    return 1;
  }
  oled_ui_delay_task.oled_ui_draw[i].flag = OLED_UI_FLAG_CANCEL_INFO;
  oled_ui_delay_task.count[i] = count;
  return 0;
}

/*******************************************************************************
 * Function Name  : OLED_UI_slot_add
 * Description    : 向OLED_UI槽添加一个icon
 * Input          : p - 要添加的icon内容指针
 * Return         : 0 - success, 1 - failed
 *******************************************************************************/
uint8_t OLED_UI_slot_add(uint8_t* p)
{
  if (p == NULL) {
    return 1;
  }
  if (oled_ui_slot.slot_size == OLED_UI_MAX_SLOT) {
    return 1;
  }
  oled_ui_slot.icon_p[oled_ui_slot.slot_size] = p;
  oled_ui_slot.slot_size++;
  return 0;
}

/*******************************************************************************
 * Function Name  : OLED_UI_slot_delete
 * Description    : 向OLED_UI槽删除一个icon
 * Input          : p - 要删除的icon内容指针
 * Return         : 0 - success, 1 - failed
 *******************************************************************************/
uint8_t OLED_UI_slot_delete(uint8_t *p)
{
  uint8_t i, j;
  if (p == NULL) {
    return 1;
  }
  for (i = 0; i < oled_ui_slot.slot_size; i++) {
    if (oled_ui_slot.icon_p[i] == p) {
      for (j = i; j < oled_ui_slot.slot_size - 1; j++) {
        oled_ui_slot.icon_p[j] = oled_ui_slot.icon_p[j + 1];
      }
      oled_ui_slot.icon_p[oled_ui_slot.slot_size - 1] = NULL;
      oled_ui_slot.slot_size--;
      return 0;
    }
  }
  return 1;
}

/*******************************************************************************
 * Function Name  : OLED_UI_slot_active
 * Description    : 激活OLED_UI槽中的一个icon(先删除一个icon后添加一个icon)
 * Input          : old_p - 要删除的icon内容指针, new_p - 要新加的icon内容指针
 * Return         : 0 - success, others - failed
 *******************************************************************************/
uint8_t OLED_UI_slot_active(uint8_t *old_p, uint8_t *new_p)
{
  uint8_t res = 0;
  if (new_p == NULL || old_p == NULL) {
    return 1;
  }
  res += OLED_UI_slot_delete(old_p);
  res += OLED_UI_slot_add(new_p);
  return res;
}

/*******************************************************************************
 * Function Name  : OLED_UI_slot_draw
 * Description    : 绘制OLED UI槽
 * Input          : None
 * Return         : None
 *******************************************************************************/
void OLED_UI_slot_draw(void)
{
  uint8_t i;
  for (i = 0; i < OLED_UI_MAX_SLOT; i++) {
    if (oled_ui_slot.icon_p[i] == NULL) {
      OLED_Clr(OLED_WIDTH - (i + 1) * OLED_UI_ICON_WIDTH, 0,
               OLED_WIDTH - i * OLED_UI_ICON_WIDTH, OLED_UI_ICON_HEIGHT);
    } else {
      OLED_DrawBMP(OLED_WIDTH - (i + 1) * OLED_UI_ICON_WIDTH, 0,
                   OLED_WIDTH - i * OLED_UI_ICON_WIDTH, OLED_UI_ICON_HEIGHT,
                   oled_ui_slot.icon_p[i]);
    }
  }
}

/*******************************************************************************
 * Function Name  : OLED_UI_show_version
 * Description    : 打印固件版本信息
 * Input          : ena - 0表示失能, 1表示使能
 * Return         : None
 *******************************************************************************/
void OLED_UI_show_version(uint8_t ena)
{
#ifdef OLED_0_66
  if (ena) {
    OLED_ShowString(OLED_Midx(8, 0, 64), 2, "Firmware");
    OLED_ShowString(OLED_Midx(6, 0, 64), 3, FIRMWARE_VERSION);
    OLED_ShowString(OLED_Midx(8, 0, 64), 4, "Designers");
    OLED_ShowString(0, 5, "Qi.C, Chengwei.L");
    OLED_Scroll(5, 5, 16, 48, 2, 1, 0);
  } else {
    OLED_Clr(0, 2, 63, 8);
    OLED_WR_Byte(0x2E, OLED_CMD);   // 停止滚动
  }
#endif
}

/*******************************************************************************
 * Function Name  : OLED_UI_draw_empty_battery
 * Description    : 绘制空电池
 * Input          : None
 * Return         : None
 *******************************************************************************/
void OLED_UI_draw_empty_battery(void)
{
#ifdef OLED_0_91
  extern const unsigned char BatteryBMP[][96];
  OLED_DrawBMP(91, 0, 91 + 32, 3, (uint8_t*)BatteryBMP[0]);
#endif
#ifdef OLED_0_66
  extern const unsigned char BatteryBMP[][46];
  OLED_DrawBMP(0, 0, 23, 2, (uint8_t*)BatteryBMP[0]);
#endif
}

/*******************************************************************************
 * Function Name  : OLED_UI_draw_menu
 * Description    : 绘制菜单界面
 * Input          : fresh_type - 菜单更新方式
 * Return         : None
 *******************************************************************************/
void OLED_UI_draw_menu(oled_ui_swipe fresh_type)
{
#ifdef OLED_0_66
  static uint8_t preStr[10];
  uint32_t val;
  uint8_t i;

  menu_fresh_start:
  switch (fresh_type)
  {
    case OLED_UI_MENU_REFRESH:  // 直接刷新
      if (*(oled_ui_menu_type*)cur_menu_p == OLED_UI_TYPE_MENU) { // 菜单类
        OLED_Clr(0, 2, OLED_WIDTH, 5);
        for (i = 0; i < OLED_UI_MENU_MAX_LEN; i++) {
          if (P_MENU_T(cur_menu_p)->text[i][0] != 0) {
            if (i == menu_cur_idx) {
              OLED_ShowString_f(0, 2 + i, P_MENU_T(cur_menu_p)->text[i]);
            } else {
              OLED_ShowString(0, 2 + i, P_MENU_T(cur_menu_p)->text[i]);
            }
          }
        }
      } else if (*(oled_ui_menu_type*)cur_menu_p == OLED_UI_TYPE_ENTER_NUM) { // 输入项目
        i = KEYBOARD_EnterNumber(&val, preStr, P_EN_T(cur_menu_p)->postStr, P_EN_T(cur_menu_p)->limit_len);
        if (i == 0) { // 输入完成
          HAL_Fs_Write_keyboard_cfg(P_EN_T(cur_menu_p)->line, 1, (uint16_t*)&val);
          cur_menu_p = P_EN_T(cur_menu_p)->p; // 返回菜单类
          goto menu_fresh_start;
        } else if (i == 0xFF) { // 清空输入
          cur_menu_p = P_EN_T(cur_menu_p)->p; // 返回菜单类
          goto menu_fresh_start;
        }
      } else if (*(oled_ui_menu_type*)cur_menu_p == OLED_UI_TYPE_VAL_STATUS) { // 变量数值通用项目
        OLED_PRINT("%d", *P_VAL_STS_T(cur_menu_p)->p_val);
        OLED_UI_add_default_delay_task(OELD_UI_FLAG_REFRESH_MENU, 20);
      } else if (*(oled_ui_menu_type*)cur_menu_p == OLED_UI_TYPE_MPR121_STATUS) { // MPR121状态项目
        val = 0;
        if (P_MPR_STS_T(cur_menu_p)->type == TRUE) MPR121_ReadHalfWord(P_MPR_STS_T(cur_menu_p)->reg, (uint16_t*)&val);
        else MPR121_ReadReg(P_MPR_STS_T(cur_menu_p)->reg, (uint8_t*)&val);
        OLED_PRINT("%d", val);
        OLED_UI_add_default_delay_task(OELD_UI_FLAG_REFRESH_MENU, 20);
      }
      break;
    case OLED_UI_SWIPE_UP:  // 上滑
      if (*(oled_ui_menu_type*)cur_menu_p == OLED_UI_TYPE_MENU) { // 菜单类
        if (menu_cur_idx == 0) {
          if (P_MENU_T(cur_menu_p)->p[OLED_UI_MENU_MAX_LEN+1] != NULL) {
            cur_menu_p = P_MENU_T(cur_menu_p)->p[OLED_UI_MENU_MAX_LEN+1];
            menu_cur_idx = P_MENU_T(cur_menu_p)->menu_size - 1;
            fresh_type = OLED_UI_MENU_REFRESH;
            goto menu_fresh_start;
          }
          break;
        }
        g_oled_smooth_end_flag = FALSE;
        OLED_UI_smooth_select_cfg(P_MENU_T(cur_menu_p)->text[menu_cur_idx], P_MENU_T(cur_menu_p)->text[menu_cur_idx - 1],
                                  menu_cur_idx + 2, menu_cur_idx + 1);
        OLED_UI_add_default_delay_task(OLED_UI_FLAG_SMOOTH_SELECT, 30);
        menu_cur_idx--;
      }
      break;
    case OLED_UI_SWIPE_DOWN:  // 下滑
      if (*(oled_ui_menu_type*)cur_menu_p == OLED_UI_TYPE_MENU) { // 菜单类
        if (menu_cur_idx == P_MENU_T(cur_menu_p)->menu_size - 1) {
          if (P_MENU_T(cur_menu_p)->p[OLED_UI_MENU_MAX_LEN+2] != NULL) {
            cur_menu_p = P_MENU_T(cur_menu_p)->p[OLED_UI_MENU_MAX_LEN+2];
            menu_cur_idx = 0;
            fresh_type = OLED_UI_MENU_REFRESH;
            goto menu_fresh_start;
          }
          break;
        }
        g_oled_smooth_end_flag = FALSE;
        OLED_UI_smooth_select_cfg(P_MENU_T(cur_menu_p)->text[menu_cur_idx], P_MENU_T(cur_menu_p)->text[menu_cur_idx + 1],
                                  menu_cur_idx + 2, menu_cur_idx + 3);
        OLED_UI_add_default_delay_task(OLED_UI_FLAG_SMOOTH_SELECT, 30);
        menu_cur_idx++;
      }
      break;
    case OLED_UI_SWIPE_LEFT:  // 左滑
      if (*(oled_ui_menu_type*)cur_menu_p == OLED_UI_TYPE_MENU) { // 菜单类
        if (P_MENU_T(cur_menu_p)->p[OLED_UI_MENU_MAX_LEN] == NULL) break;
        cur_menu_p = P_MENU_T(cur_menu_p)->p[OLED_UI_MENU_MAX_LEN];
        menu_cur_idx = 0;
        fresh_type = OLED_UI_MENU_REFRESH;
        goto menu_fresh_start;
      } else if (*(oled_ui_menu_type*)cur_menu_p == OLED_UI_TYPE_VAL_STATUS) {  // 变量数值通用项目
        if (P_VAL_STS_T(cur_menu_p)->p == NULL) break; // 错误的配置
        cur_menu_p = P_VAL_STS_T(cur_menu_p)->p;
        menu_cur_idx = 0;
        fresh_type = OLED_UI_MENU_REFRESH;
        goto menu_fresh_start;
      } else if (*(oled_ui_menu_type*)cur_menu_p == OLED_UI_TYPE_MPR121_STATUS) {  // MPR121状态项目
        if (P_MPR_STS_T(cur_menu_p)->p == NULL) break; // 错误的配置
        cur_menu_p = P_MPR_STS_T(cur_menu_p)->p;
        menu_cur_idx = 0;
        fresh_type = OLED_UI_MENU_REFRESH;
        goto menu_fresh_start;
      }
      break;
    case OLED_UI_SWIPE_RIGHT:  // 右滑
      if (*(oled_ui_menu_type*)cur_menu_p == OLED_UI_TYPE_MENU) { // 菜单类
        if (P_MENU_T(cur_menu_p)->p[menu_cur_idx] == NULL) break;
        cur_menu_p = P_MENU_T(cur_menu_p)->p[menu_cur_idx];
        menu_cur_idx = 0;
        if (*(oled_ui_menu_type*)cur_menu_p == OLED_UI_TYPE_ENTER_NUM) {  // 右滑后为输入项目
          HAL_Fs_Read_keyboard_cfg(P_EN_T(cur_menu_p)->line, 1, (uint16_t*)&val);
          memcpy(preStr, P_EN_T(cur_menu_p)->preStr, P_EN_T(cur_menu_p)->pStr_len);
          unsigned_dec_to_string(val, &preStr[P_EN_T(cur_menu_p)->pStr_len], 0);
          OLED_Clr(0, 2, OLED_WIDTH, 5);
          OLED_PRINT("%s", preStr);
        } else if (*(oled_ui_menu_type*)cur_menu_p == OLED_UI_TYPE_VAL_STATUS) {  // 右滑后为变量数值通用项目
          OLED_Clr(0, 2, OLED_WIDTH, 5);
        } else if (*(oled_ui_menu_type*)cur_menu_p == OLED_UI_TYPE_MPR121_STATUS) {  // 右滑后为MPR121状态项目
          OLED_Clr(0, 2, OLED_WIDTH, 5);
        }
        fresh_type = OLED_UI_MENU_REFRESH;
        goto menu_fresh_start;
      }
      break;
  }
#endif
}

/*******************************************************************************
 * Function Name  : OLED_UI_smooth_select_cfg
 * Description    : UI菜单丝滑切换配置
 * Input          : str0/str1 - 起始/终止字符串; y0/y1 - 起始/终止页数
 * Return         : None
 *******************************************************************************/
void OLED_UI_smooth_select_cfg(const uint8_t* str0, const uint8_t* str1, uint8_t y0, uint8_t y1)
{
  uint8_t len0, len1;

  for (len0 = 0; str0[len0] != '\0'; len0++) {
    oled_smooth_select_str0[len0] = str0[len0];
  }
  for (len1 = 0; str1[len1] != '\0'; len1++) {
    oled_smooth_select_str1[len1] = str1[len1];
  }

  oled_smooth_select_pos_len[0].y = y0;
  oled_smooth_select_pos_len[1].y = y1;
  oled_smooth_select_pos_len[0].len = len0;
  oled_smooth_select_pos_len[1].len = len1;
  oled_smooth_w_cnt = len0 * 6;
  oled_smooth_y_cnt = y0 < y1 ? 0 : 7;
}

/*******************************************************************************
 * Function Name  : OLED_UI_smooth_select
 * Description    : UI菜单丝滑切换选项
 * Input          : None
 * Return         : None
 *******************************************************************************/
void OLED_UI_smooth_select(void)
{
#if defined(OLED_0_66) && (FONT_SIZE == 8)
  uint8_t i, j, temp, w_buf, dir;
  uint32_t maxlen, minlen;

  dir = oled_smooth_select_pos_len[0].y < oled_smooth_select_pos_len[1].y;
  temp = dir ? ((1 << (oled_smooth_y_cnt + 1)) - 1) : ~((1 << (oled_smooth_y_cnt + 1)) - 1);
  maxlen = MAX(oled_smooth_select_pos_len[0].len, oled_smooth_select_pos_len[1].len);
  minlen = MIN(oled_smooth_select_pos_len[0].len, oled_smooth_select_pos_len[1].len);

  OLED_Set_Pos(0, oled_smooth_select_pos_len[0].y);
  for (i = 0; i < maxlen; i++) {
    for (j = 0; j < 6; j++) {
      if (i < oled_smooth_select_pos_len[0].len) {
        if (i * 6 + j < oled_smooth_w_cnt) {
          w_buf = ((~F6x8[oled_smooth_select_str0[i] - ' '][j] & ~temp) | (F6x8[oled_smooth_select_str0[i] - ' '][j] & temp));
        } else {
          w_buf = F6x8[oled_smooth_select_str0[i] - ' '][j];
        }
      } else {
        if (i * 6 + j < oled_smooth_w_cnt) {
          w_buf = ~temp;
        } else {
          w_buf = 0;
        }
      }
      OLED_WR_Byte(w_buf, OLED_DATA);
    }
  }
  OLED_Set_Pos(0, oled_smooth_select_pos_len[1].y);
  for (i = 0; i < maxlen; i++) {
    for (j = 0; j < 6; j++) {
      if (i < oled_smooth_select_pos_len[1].len) {
        if (i * 6 + j < oled_smooth_w_cnt) {
          w_buf = ((~F6x8[oled_smooth_select_str1[i] - ' '][j] & temp) | (F6x8[oled_smooth_select_str1[i] - ' '][j] & ~temp));
        } else {
          w_buf = F6x8[oled_smooth_select_str1[i] - ' '][j];
        }
      } else {
        if (i * 6 + j < oled_smooth_w_cnt) {
          w_buf = temp;
        } else {
          w_buf = 0;
        }
      }
      OLED_WR_Byte(w_buf, OLED_DATA);
    }
  }

  if ((oled_smooth_y_cnt != 8 && dir) || (oled_smooth_y_cnt != 0xFF && !dir)) {
    if (oled_smooth_select_pos_len[0].len > oled_smooth_select_pos_len[1].len) {
      if (dir) oled_smooth_w_cnt = oled_smooth_select_pos_len[1].len * 6 + (maxlen - minlen) * (7 - oled_smooth_y_cnt) * 6 / 8;
      else oled_smooth_w_cnt = oled_smooth_select_pos_len[1].len * 6 + (maxlen - minlen) * oled_smooth_y_cnt * 6 / 8;
    } else {
      if (dir) oled_smooth_w_cnt = oled_smooth_select_pos_len[0].len * 6 + (maxlen - minlen) * (oled_smooth_y_cnt + 1) * 6 / 8;
      else oled_smooth_w_cnt = oled_smooth_select_pos_len[0].len * 6 + (maxlen - minlen) * (8 - oled_smooth_y_cnt) * 6 / 8;
    }
    if (dir) oled_smooth_y_cnt++;
    else oled_smooth_y_cnt--;
    OLED_UI_add_default_delay_task(OLED_UI_FLAG_SMOOTH_SELECT, 5);
  } else g_oled_smooth_end_flag = TRUE;
#endif
}

/*******************************************************************************
 * Function Name  : OLED_UI_entry_idle
 * Description    : 进入/退出待机界面
 * Input          : is_entrying - 1进入待机界面, 0退出待机界面
 * Return         : None
 *******************************************************************************/
void OLED_UI_idle(uint8_t is_entrying)
{
  oled_ui_pos_len pos_len;
  oled_ui_entry_idle_frame = is_entrying ? 1 : 0;
  if (is_entrying) {
    OLED_UI_add_delay_task(OLED_UI_FLAG_IDLE_DRAW, pos_len, (uint8_t*)UI_Oled_Idle, NULL, 80);
  }
}

/*******************************************************************************
 * Function Name  : OLED_UI_draw_thread_callback
 * Description    : OLED_UI绘图回调函数
 * Input          : None
 * Return         : None
 *******************************************************************************/
void OLED_UI_draw_thread_callback(void)
{
#ifdef OLED_0_91
  extern const unsigned char BatteryBMP[][96];
#endif
#ifdef OLED_0_66
  extern const unsigned char BatteryBMP[][46];
#endif
  uint8_t i, j;
  /* normal task */
  switch (oled_ui_task.oled_ui_draw[oled_ui_task.head].flag)
  {
    case OLED_UI_FLAG_DRAW_OK:
      break;
    case OLED_UI_FLAG_CANCEL_OK:
      break;
    case OLED_UI_FLAG_CLEAR_PAGE:
      OLED_Clr(0, oled_ui_task.oled_ui_draw[oled_ui_task.head].pos_len.x,
               OLED_WIDTH, oled_ui_task.oled_ui_draw[oled_ui_task.head].pos_len.y);
      break;
    case OLED_UI_FLAG_SHOW_STRING:
      OLED_ShowString(oled_ui_task.oled_ui_draw[oled_ui_task.head].pos_len.x,
                      oled_ui_task.oled_ui_draw[oled_ui_task.head].pos_len.y,
                      oled_ui_task.oled_ui_draw[oled_ui_task.head].pstr);
      break;
    case OLED_UI_FLAG_SHOW_INFO:
      OLED_PRINT("%s", oled_ui_task.oled_ui_draw[oled_ui_task.head].pstr);
      break;
    case OLED_UI_FLAG_SHOW_ICONIFO:
#ifdef OLED_0_66
      j = OLED_Midx(strlen(oled_ui_task.oled_ui_draw[oled_ui_task.head].pstr), 0, OLED_WIDTH);
      // Draw page1
      OLED_Set_Pos(j - 5, 2);
      for (i = 0; i < OLED_UI_ICON_WIDTH; i++) OLED_WR_Byte((uint8_t)(oled_ui_task.oled_ui_draw[oled_ui_task.head].addr[i] << (12 - OLED_UI_ICON_PIXEL_HEIGHT / 2)), OLED_DATA);
      // Draw page2
      OLED_Set_Pos(j - 5, 3);
      for (i = 0; i < OLED_UI_ICON_WIDTH; i++) {
        OLED_WR_Byte((uint8_t)(oled_ui_task.oled_ui_draw[oled_ui_task.head].addr[i] >> (OLED_UI_ICON_PIXEL_HEIGHT / 2 - 4)) |
                     (uint8_t)(oled_ui_task.oled_ui_draw[oled_ui_task.head].addr[i+OLED_UI_ICON_WIDTH] << (12 - OLED_UI_ICON_PIXEL_HEIGHT / 2)),
                     OLED_DATA);
      }
      // Draw page3
      OLED_Set_Pos(j - 5, 4);
      for (i = 0; i < OLED_UI_ICON_WIDTH; i++) OLED_WR_Byte((uint8_t)(oled_ui_task.oled_ui_draw[oled_ui_task.head].addr[i+OLED_UI_ICON_WIDTH] >> (OLED_UI_ICON_PIXEL_HEIGHT / 2 - 4)), OLED_DATA);
      // Draw info string
      OLED_ShowString(j + 5, 3, oled_ui_task.oled_ui_draw[oled_ui_task.head].pstr);
#endif
      break;
    case OLED_UI_FLAG_CANCEL_INFO:
#ifdef OLED_0_66
      OLED_Clr(0, 2, OLED_WIDTH, 5);
#endif
      break;
    case OLED_UI_FLAG_DRAW_SLOT:
      OLED_UI_slot_draw();
      break;
    case OLED_UI_FLAG_BAT_CHARGE:
#ifdef OLED_0_91
      OLED_DrawBMP(72, 0, 81, 4, (uint8_t*)ChargeBattery);
#endif
#ifdef OLED_0_66
      OLED_DrawBMP(0, 0, 23, 2, (uint8_t*)ChargeBattery);
#endif
      break;
    case OLED_UI_FLAG_BAT_CLR_CHARGE:
#ifdef OLED_0_91
      OLED_Clr(72, 0, 81, 4);
#endif
#ifdef OLED_0_66
      BAT_adcHistory = 0; // clear adc history
#endif
      break;
    case OLED_UI_FLAG_BAT_LEVEL_1:
    case OLED_UI_FLAG_BAT_LEVEL_2:
    case OLED_UI_FLAG_BAT_LEVEL_3:
    case OLED_UI_FLAG_BAT_LEVEL_4:
    case OLED_UI_FLAG_BAT_LEVEL_5:
    case OLED_UI_FLAG_BAT_LEVEL_6:
#ifdef OLED_0_91
      OLED_DrawBMP(91, 0, 91 + 32, 3, (uint8_t*)BatteryBMP[oled_ui_task.oled_ui_draw[oled_ui_task.head].flag - OLED_UI_FLAG_BAT_LEVEL_1]);
#endif
#ifdef OLED_0_66
      OLED_DrawBMP(0, 0, 23, 2, (uint8_t*)BatteryBMP[oled_ui_task.oled_ui_draw[oled_ui_task.head].flag - OLED_UI_FLAG_BAT_LEVEL_1]);
#endif
      break;
    default:
      goto normal_task_process_end;
  }
  // remove task
  oled_ui_task.oled_ui_draw[oled_ui_task.head].flag = OLED_UI_FLAG_DEFAULT;
  oled_ui_task.head = (oled_ui_task.head == OLED_UI_TASK_MAX - 1) ? 0 : oled_ui_task.head + 1;
  oled_ui_task.size--;
  normal_task_process_end:
  /* delay task */
  for (i = 0; i < OLED_UI_DELAY_TASK_MAX; i++) {
    if (oled_ui_delay_task.count[i] != 0) {
      oled_ui_delay_task.count[i]--;
      continue;
    }
    switch (oled_ui_delay_task.oled_ui_draw[i].flag)
    {
      case OLED_UI_FLAG_DRAW_OK:
        break;
      case OLED_UI_FLAG_CANCEL_OK:
        break;
      case OLED_UI_FLAG_CLEAR_PAGE:
        OLED_Clr(0, oled_ui_delay_task.oled_ui_draw[i].pos_len.x,
                 OLED_WIDTH, oled_ui_delay_task.oled_ui_draw[i].pos_len.y);
        break;
      case OLED_UI_FLAG_SHOW_STRING:
        OLED_ShowString(oled_ui_delay_task.oled_ui_draw[i].pos_len.x,
                        oled_ui_delay_task.oled_ui_draw[i].pos_len.y,
                        oled_ui_delay_task.oled_ui_draw[i].pstr);
        break;
      case OLED_UI_FLAG_SHOW_INFO:
        OLED_PRINT("%s", oled_ui_delay_task.oled_ui_draw[i].pstr);
        break;
      case OLED_UI_FLAG_CANCEL_INFO:
#ifdef OLED_0_66
        OLED_Clr(0, 2, OLED_WIDTH, 5);
#endif
        break;
      case OLED_UI_FLAG_DRAW_SLOT:
        OLED_UI_slot_draw();
        break;
      case OLED_UI_FLAG_IDLE_DRAW:
#ifdef OLED_0_66
        if (oled_ui_entry_idle_frame) {
          OLED_DrawBMP(0, 2, 64, 5, oled_ui_delay_task.oled_ui_draw[i].addr);
          OLED_UI_add_delay_task(OLED_UI_FLAG_IDLE_DRAW, oled_ui_delay_task.oled_ui_draw[i].pos_len, (uint8_t*)UI_Oled_Idle[oled_ui_entry_idle_frame], NULL, 200);
          oled_ui_entry_idle_frame++;
          if (oled_ui_entry_idle_frame == 8) {
            oled_ui_entry_idle_frame = 1;
          }
        } else {
          OLED_Clr(0, 2, 64, 5);
        }
#endif
        break;
      case OLED_UI_FLAG_CTL_STOP_SCOLL:
        OLED_Set_Scroll_ENA(0);
        break;
      case OLED_UI_FLAG_SMOOTH_SELECT:
        OLED_UI_smooth_select();
        break;
      case OELD_UI_FLAG_REFRESH_MENU:
        OLED_UI_draw_menu(OLED_UI_MENU_REFRESH);
        break;
      default:
        continue;
    }
    // remove delay task
    oled_ui_delay_task.oled_ui_draw[i].flag = OLED_UI_FLAG_DEFAULT;
  }
//  OLED_Set_Pos(oled_ui_data.x0, oled_ui_data.y0++);
}
