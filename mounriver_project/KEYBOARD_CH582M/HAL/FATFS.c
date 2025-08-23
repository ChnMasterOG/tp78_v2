/********************************** (C) COPYRIGHT *******************************
 * File Name          : FATFS.c
 * Author             : ChnMasterOG
 * Version            : V1.0
 * Date               : 2023/2/5
 * Description        : FatFs应用层驱动
 * Copyright (c) 2023 ChnMasterOG
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#include "diskio.h"
#include "string.h"
#include "HAL.h"

const BYTE WriteBuffer[] = {
    "---TP78改键说明---\n"
    "改键后需要重启键盘生效(Fn+R)\n"
    "---文件目录说明(数字均为十进制)---\n"
    "keyboard_cfg.txt---存放键盘配置(每行一个配置)\n"
    "LINE1:默认蓝牙设备(0~3)\n"
    "LINE2:默认LED模式(0~5)\n"
    "LINE3:工作模式(0-USB,1-BLE,2-RF,3-USB&BLE)\n"
    "LINE4:是否启动U盘(仅下次生效)\n"
    "LINE5:MPR_alg_magic\n"
    "LINE6:MPR_capmouse_tou_thr\n"
    "LINE7:MPR_capmouse_rel_thr\n"
    "LINE8:MPR_capmouse_move_speed\n"
    "LINE9:MPR_touchbar_tou_thr\n"
    "LINE10:MPR_touchbar_rel_thr\n"
    "LINE11:MPR_double_touch_cnt\n"
    "LINE12:MPR_long_touch_cnt\n"
    "LINE13:小红点减速系数(1~9)\n"
    "LINE14:亮度(1~255)\n"
    "LINE15:RF频段挡位(0~9)\n" // 每挡位+0.0025G频段(0~9 - 2.4G~2.4225G)
    "LINE16:是否使能触摸条触发鼠标按键(0~1)\n"
    "LINE17:进入屏保时间/s(1~65536)\n"
    "LINE18:进入休眠时间/s(1~65536)\n"
    "LINE19:RF模式下重传检查间隔/ms(1~255)\n"
    "LINE20:是否使能振动(0~1)\n"
    "LINE21:自动鼠标键周期(单位25ms)\n"
    "keyboard_spkey.txt---存放sp按键映射键位(每行8byte)\n"
    "LINE1:spKEY1的HID报表数据\n"
    "LINE2:spKEY2的HID报表数据\n"
    "LINE3:spKEY3的HID报表数据\n"
    "LINE4:spKEY4的HID报表数据\n"
    "LINE5:spKEY5的HID报表数据\n"
    "keyboard_mat.txt---存放键盘按键映射\n"
    "keyboard_ext_mat.txt---存放键盘层2按键映射\n"
};

/*******************************************************************************
 * Function Name  : HAL_Fs_Init
 * Description    : 初始化文件系统
 * Input          : None
 * Return         : None
 *******************************************************************************/
void HAL_Fs_Init(void)
{
  FATFS fs;                                                   // FatFs文件系统对象
  FIL fnew;                                                   // 文件对象
  FRESULT res_flash;                                          // 文件操作结果
  FILINFO fnow;                                               // 定义静态文件信息结构对象
  BYTE work[FF_MAX_SS];
  UINT fnum;                                                  // 文件成功读写数量

  res_flash = f_mount( &fs, "0:", 1 );                        // 挂载文件系统到分区 0

  if( res_flash == FR_NO_FILESYSTEM )                         // 没有文件系统
  {
      res_flash = f_mkfs( "0:", 0, work, sizeof(work) );      // 格式化 创建创建文件系统
      res_flash = f_mount( NULL, "0:", 1 );                   // 格式化后，先取消挂载
      res_flash = f_mount( &fs,  "0:", 1 );                   // 重新挂载
  }
  if( res_flash == FR_OK ) {                                  // 挂载成功
#ifdef FIRST_USED // 出产设置
      res_flash = f_open( &fnew, "0:readme.txt", FA_CREATE_ALWAYS | FA_WRITE );               // 以写入方式打开文件，若未发现文件则新建文件
      if( res_flash == FR_OK ) {
        /* 分两部分写 */
        res_flash = f_write( &fnew, WriteBuffer, sizeof(WriteBuffer)/2, &fnum );              // part1
        res_flash = f_write( &fnew, &WriteBuffer[sizeof(WriteBuffer)/2],
                             sizeof(WriteBuffer) - sizeof(WriteBuffer)/2 - 1, &fnum );        // part2
        f_close(&fnew);                                                                       // 关闭文件
        g_Ready_Status.fatfs = TRUE;
      }
#else
      g_Ready_Status.fatfs = TRUE;
#endif
  } else {
      g_Ready_Status.fatfs = FALSE;
  }
  f_mount( NULL, "0:", 1 );                                    // 卸载文件系统
}

/*******************************************************************************
 * Function Name  : unsigned_dec_to_string
 * Description    : 无符号类型转换为十进制字符串
 * Input          : num - u16类型; buff - char指针存放字符串;
 *                  type - 0默认, 1末尾放置空格, 2末尾放置换行
 * Return         : 字符串长度
 *******************************************************************************/
uint8_t unsigned_dec_to_string(uint16_t num, char *buff, uint8_t type)
{
  uint8_t len, ret;

  if (num <= 9) len = 1;
  else if (num <= 99) len = 2;
  else if (num <= 999) len = 3;
  else if (num <= 9999) len = 4;
  else len = 5; // u8 max 255, u16 max 65536

  if (type == 1) {
    buff[len] = ' ';
    buff[len + 1] = '\0';
    ret = len + 1;
  } else if (type == 2) {
    buff[len] = '\n';
    buff[len + 1] = '\0';
    ret = len + 1;
  } else {
    buff[len] = '\0';
    ret = len;
  }

  do {
    --len;
    buff[len] = num % 10 + '0';
    num /= 10;
  } while (num != 0);

  return ret;
}

/*******************************************************************************
 * Function Name  : string_dec_to_u8
 * Description    : 十进制字符串转换为u8类型
 * Input          : buff - char指针存放字符串; num - u8类型存放的指针
 * Return         : 指向数字末尾+1个字符
 *******************************************************************************/
uint8_t string_dec_to_u8(char *buff, uint8_t *num)
{
  uint8_t ret = 1;
  *num = 0;

  while (*buff != '\0' && *buff != ' ' && *buff != '\n') {
    *num *= 10;
    *num += *buff - '0';
    buff++;
    ret++;
  }

  return ret;
}

/*******************************************************************************
 * Function Name  : string_dec_to_u16
 * Description    : 十进制字符串转换为u16类型
 * Input          : buff - char指针存放字符串; num - u16类型存放的指针
 * Return         : 指向数字末尾+1个字符
 *******************************************************************************/
uint8_t string_dec_to_u16(char *buff, uint16_t *num)
{
  uint8_t ret = 1;
  *num = 0;

  while (*buff != '\0' && *buff != ' ' && *buff != '\n') {
    *num *= 10;
    *num += *buff - '0';
    buff++;
    ret++;
  }

  return ret;
}

/*******************************************************************************
 * Function Name  : HAL_Fs_Create_readme
 * Description    : 创建readme.txt
 * Input          : None
 * Return         : None
 *******************************************************************************/
void HAL_Fs_Create_readme(void)
{
  FATFS fs;
  FIL fnew;
  FRESULT res_flash;
  FILINFO fnow;
  UINT fnum;
  uint32_t wirte_num = 0;

  if (g_Ready_Status.fatfs == FALSE) return;

  res_flash = f_mount( &fs, "0:", 1 );
  if ( res_flash != FR_OK ) return;

  res_flash = f_open( &fnew, "0:readme.txt", FA_CREATE_ALWAYS | FA_WRITE );               // 以写入方式打开文件，若未发现文件则新建文件
  if( res_flash == FR_OK ) {
    /* 文件较大，分三部分写 */
    res_flash = f_write( &fnew, WriteBuffer, sizeof(WriteBuffer)/3, &fnum );              // part1
    wirte_num += sizeof(WriteBuffer)/3;
    res_flash = f_write( &fnew, &WriteBuffer[wirte_num],
                         sizeof(WriteBuffer)/3, &fnum );                                  // part2
    wirte_num += sizeof(WriteBuffer)/3;
    res_flash = f_write( &fnew, &WriteBuffer[wirte_num],
                         sizeof(WriteBuffer) - wirte_num - 1, &fnum );                    // part3
    f_close(&fnew);                                                                       // 关闭文件
    g_Ready_Status.fatfs = TRUE;
  }

  f_mount( NULL, "0:", 1 );   // 卸载文件系统
}

/*******************************************************************************
 * Function Name  : HAL_Fs_Create_keyboard_cfg
 * Description    : 创建keyboard_cfg.txt
 * Input          : len - 写入长度, 单位: 行数(至少为1); p_cfg - 参数指针
 * Return         : None
 *******************************************************************************/
void HAL_Fs_Create_keyboard_cfg(uint8_t len, uint16_t* p_cfg)
{
  FATFS fs;
  FIL fnew;
  FRESULT res_flash;
  FILINFO fnow;
  UINT fnum;

  uint8_t l = 0;
  uint8_t wr_buf[512];

  if (g_Ready_Status.fatfs == FALSE) return;

  res_flash = f_mount( &fs, "0:", 1 );
  if ( res_flash != FR_OK ) return;

  res_flash = f_open(&fnew, "0:keyboard_cfg.txt", FA_CREATE_ALWAYS | FA_WRITE );
  if ( res_flash != FR_OK ) goto fs_create_kbd_cfg_end;

  while (len) {
    l += unsigned_dec_to_string(*p_cfg, &wr_buf[l], 2);
    p_cfg++;
    len--;
  }
  f_write( &fnew, wr_buf, l, &fnum );

  f_close(&fnew);

  fs_create_kbd_cfg_end:
  f_mount( NULL, "0:", 1 );   // 卸载文件系统
}

/*******************************************************************************
 * Function Name  : HAL_Fs_Write_keyboard_cfg
 * Description    : 向keyboard_cfg.txt写参数
 * Input          : fs_line - 开始行数(0为第1行); len - 写入长度, 单位: 行数(至少为1); p_cfg - 参数指针
 * Return         : None
 *******************************************************************************/
void HAL_Fs_Write_keyboard_cfg(uint8_t fs_line, uint8_t len, uint16_t* p_cfg)
{
  FATFS fs;
  FIL fnew;
  FRESULT res_flash;
  FILINFO fnow;
  UINT fnum;

  uint8_t l = 0;
  uint8_t rd_buf[512];
  uint16_t temp;

  if (g_Ready_Status.fatfs == FALSE) return;

  res_flash = f_mount( &fs, "0:", 1 );
  if ( res_flash != FR_OK ) return;

  res_flash = f_open(&fnew, "0:keyboard_cfg.txt", FA_OPEN_EXISTING | FA_READ | FA_WRITE );
  if ( res_flash != FR_OK ) goto fs_write_kbd_cfg_end;    // 文件不存在

  res_flash = f_read(&fnew, rd_buf, sizeof(rd_buf), &fnum);
  while (fs_line) {
    l += string_dec_to_u16(&rd_buf[l], &temp);
    fs_line--;
  }
  fs_line = l;
  while (len) {
    l += unsigned_dec_to_string(*p_cfg, &rd_buf[l], 2);
    p_cfg++;
    len--;
  }
  f_lseek( &fnew, fs_line );
  f_write( &fnew, &rd_buf[fs_line], l - fs_line, &fnum );

  f_close(&fnew);

  fs_write_kbd_cfg_end:
  f_mount( NULL, "0:", 1 );   // 卸载文件系统
}

/*******************************************************************************
 * Function Name  : HAL_Fs_Read_keyboard_cfg
 * Description    : 读取keyboard_cfg.txt参数
 * Input          : fs_line - 开始行数(0为第1行); len - 读取长度, 单位: 行数(至少为1); p_cfg - 参数指针
 * Return         : None
 *******************************************************************************/
void HAL_Fs_Read_keyboard_cfg(uint8_t fs_line, uint8_t len, uint16_t* p_cfg)
{
  FATFS fs;
  FIL fnew;
  FRESULT res_flash;
  FILINFO fnow;
  UINT fnum;

  uint8_t l = 0;
  uint8_t rd_buf[512];

  if (g_Ready_Status.fatfs == FALSE) return;

  res_flash = f_mount( &fs, "0:", 1 );
  if ( res_flash != FR_OK ) return;

  res_flash = f_open(&fnew, "0:keyboard_cfg.txt", FA_OPEN_EXISTING | FA_READ );
  if ( res_flash != FR_OK ) goto fs_read_kbd_cfg_end;    // 文件不存在

  res_flash = f_read(&fnew, rd_buf, sizeof(rd_buf), &fnum);
  while (fs_line) {
    l += string_dec_to_u16(&rd_buf[l], p_cfg);
    fs_line--;
  }
  while (len) {
    l += string_dec_to_u16(&rd_buf[l], p_cfg);
    p_cfg++;
    len--;
  }

  f_close(&fnew);

  fs_read_kbd_cfg_end:
  f_mount( NULL, "0:", 1 );   // 卸载文件系统
}

/*******************************************************************************
 * Function Name  : HAL_Fs_Write_ble_device_info
 * Description    : 向ble_device_info.txt写BLE绑定主机的设备信息
 * Input          : ble_device_info - 绑定的主机设备信息指针
 * Return         : None
 *******************************************************************************/
/*
void HAL_Fs_Write_ble_device_info(BLE_Device_Info_t *ble_device_info)
{
  FATFS fs;
  FIL fnew;
  FRESULT res_flash;
  FILINFO fnow;
  UINT fnum;

  uint8_t i, j, len = 0;
  uint8_t wr_buf[512];
  uint16_t temp;

  if (g_Ready_Status.fatfs == FALSE) return;


  res_flash = f_mount( &fs, "0:", 1 );
  if ( res_flash != FR_OK ) return;

  res_flash = f_open(&fnew, "0:ble_device_info.txt", FA_CREATE_ALWAYS | FA_WRITE );
  if ( res_flash != FR_OK ) goto fs_write_ble_device_info_end;

  for (i = 0; i < BLE_DEVICE_NUM; i++) {
    len += unsigned_dec_to_string(ble_device_info[i].magic, &wr_buf[len], 1);
    len += unsigned_dec_to_string(ble_device_info[i].addr_type, &wr_buf[len], 2);
    for (j = 0; j < 6; j++) {
      if (j == 5) len += unsigned_dec_to_string(ble_device_info[i].mac_addr[j], &wr_buf[len], 2);
      else len += unsigned_dec_to_string(ble_device_info[i].mac_addr[j], &wr_buf[len], 1);
    }
  }

  f_write( &fnew, wr_buf, len, &fnum );

  f_close(&fnew);

  fs_write_ble_device_info_end:
  f_mount( NULL, "0:", 1 );   // 卸载文件系统
}
*/

/*******************************************************************************
 * Function Name  : HAL_Fs_Read_ble_device_info
 * Description    : 从ble_device_info.txt读BLE绑定主机的设备信息
 * Input          : ble_device_info - 绑定的主机设备信息指针
 * Return         : None
 *******************************************************************************/
/*
void HAL_Fs_Read_ble_device_info(BLE_Device_Info_t *ble_device_info)
{
  FATFS fs;
  FIL fnew;
  FRESULT res_flash;
  FILINFO fnow;
  UINT fnum;

  uint8_t i, j, len = 0;
  uint8_t rd_buf[512];
  uint16_t temp;

  if (g_Ready_Status.fatfs == FALSE) return;

  res_flash = f_mount( &fs, "0:", 1 );
  if ( res_flash != FR_OK ) return;

  res_flash = f_open(&fnew, "0:ble_device_info.txt", FA_OPEN_EXISTING | FA_READ );
  if ( res_flash != FR_OK ) goto fs_read_ble_device_info_end;    // 文件不存在

  res_flash = f_read(&fnew, rd_buf, sizeof(rd_buf), &fnum);
  for (i = 0; i < BLE_DEVICE_NUM; i++) {
    len += string_dec_to_u8(&rd_buf[len], &(ble_device_info[i].magic));
    len += string_dec_to_u8(&rd_buf[len], &(ble_device_info[i].addr_type));
    for (j = 0; j < 6; j++) {
      len += string_dec_to_u8(&rd_buf[len], &(ble_device_info[i].mac_addr[j]));
    }
  }

  f_close(&fnew);

  fs_read_ble_device_info_end:
  f_mount( NULL, "0:", 1 );   // 卸载文件系统
}
*/

/*******************************************************************************
 * Function Name  : HAL_Fs_Write_keyboard_spkey
 * Description    : 写入键盘sp按键映射
 * Input          : hid_arr: HID keyboard报文, 每个sp按键8字节, 一共SP_KEY_NUMBER*8字节
 * Return         : None
 *******************************************************************************/
void HAL_Fs_Write_keyboard_spkey(uint8_t* hid_arr)
{
  FATFS fs;
  FIL fnew;
  FRESULT res_flash;
  FILINFO fnow;
  UINT fnum;

  uint8_t i, j, len;
  uint8_t wr_buf[512];  // 最大4*COL_SIZE*ROW_SIZE

  if (g_Ready_Status.fatfs == FALSE) return;

  res_flash = f_mount( &fs, "0:", 1 );
  if ( res_flash != FR_OK ) return;

  res_flash = f_open( &fnew, "0:keyboard_spkey.txt", FA_CREATE_ALWAYS | FA_WRITE );
  if ( res_flash != FR_OK ) goto fs_write_kbd_spkey_end;

  len = 0;
  for (i = 0; i < SP_KEY_NUMBER; i++) {
    for (j = 0; j < 8; j++) {
      if (j == 7) len += unsigned_dec_to_string(hid_arr[j + i * 8], &wr_buf[len], 2);
      else len += unsigned_dec_to_string(hid_arr[j + i * 8], &wr_buf[len], 1);
    }
  }

  f_write( &fnew, wr_buf, len, &fnum );

  f_close(&fnew);

  fs_write_kbd_spkey_end:
  f_mount( NULL, "0:", 1 );   // 卸载文件系统
}

/*******************************************************************************
 * Function Name  : HAL_Fs_Read_keyboard_spkey
 * Description    : 读取键盘sp按键映射
 * Input          : hid_arr: HID keyboard报文, 每个sp按键8字节, 一共SP_KEY_NUMBER*8字节
 * Return         : None
 *******************************************************************************/
void HAL_Fs_Read_keyboard_spkey(uint8_t* hid_arr)
{
  FATFS fs;
  FIL fnew;
  FRESULT res_flash;
  FILINFO fnow;
  UINT fnum;

  uint8_t i, j, len;
  uint8_t rd_buf[512];  // 最大4*COL_SIZE*ROW_SIZE

  if (g_Ready_Status.fatfs == FALSE) return;

  res_flash = f_mount( &fs, "0:", 1 );
  if ( res_flash != FR_OK ) return;

  res_flash = f_open(&fnew, "0:keyboard_spkey.txt", FA_OPEN_EXISTING | FA_READ );
  if ( res_flash != FR_OK ) goto fs_read_kbd_spkey_end;    // 文件不存在

  res_flash = f_read(&fnew, rd_buf, sizeof(rd_buf), &fnum);
  if ( res_flash == FR_OK ) {
    len = 0;
    for (i = 0; i < SP_KEY_NUMBER; i++) {
      for (j = 0; j < 8; j++) {
        len += string_dec_to_u8(&rd_buf[len], &hid_arr[j + i * 8]);
      }
    }
  }

  f_close(&fnew);

  fs_read_kbd_spkey_end:
  f_mount( NULL, "0:", 1 );   // 卸载文件系统
}

/*******************************************************************************
 * Function Name  : HAL_Fs_Write_keyboard_mat
 * Description    : 写入键盘布局
 * Input          : fp - 文件系统路径: 如"0:keyboard_mat.txt", "0:keyboard_ext_mat.txt"
 *                  keyarr - RAM zone按键信息矩阵: 如CustomKey, Extra_CustomKey
 * Return         : None
 *******************************************************************************/
void HAL_Fs_Write_keyboard_mat(const uint8_t* fp, const uint8_t* key_arr)
{
  FATFS fs;
  FIL fnew;
  FRESULT res_flash;
  FILINFO fnow;
  UINT fnum;

  uint8_t i, j, len;
  uint8_t wr_buf[512];  // 最大4*COL_SIZE*ROW_SIZE

  if (g_Ready_Status.fatfs == FALSE) return;

  res_flash = f_mount( &fs, "0:", 1 );
  if ( res_flash != FR_OK ) return;

  res_flash = f_open( &fnew, fp, FA_CREATE_ALWAYS | FA_WRITE );
  if ( res_flash != FR_OK ) goto fs_write_kbd_mat_end;

  len = 0;
  for (i = 0; i < ROW_SIZE; i++) {
    for (j = 0; j < COL_SIZE; j++) {
      if (j == COL_SIZE - 1) len += unsigned_dec_to_string(key_arr[j + i * COL_SIZE], &wr_buf[len], 2);
      else len += unsigned_dec_to_string(key_arr[j + i * COL_SIZE], &wr_buf[len], 1);
    }
  }

  f_write( &fnew, wr_buf, len, &fnum );

  f_close(&fnew);

  fs_write_kbd_mat_end:
  f_mount( NULL, "0:", 1 );   // 卸载文件系统
}

/*******************************************************************************
 * Function Name  : HAL_Fs_Read_keyboard_mat
 * Description    : 读取键盘布局
 * Input          : fp - 文件系统路径: 如"0:keyboard_mat.txt", "0:keyboard_ext_mat.txt"
 *                  keyarr - RAM zone按键信息矩阵: 如CustomKey, Extra_CustomKey
 * Return         : None
 *******************************************************************************/
void HAL_Fs_Read_keyboard_mat(const uint8_t* fp, uint8_t* key_arr)
{
  FATFS fs;
  FIL fnew;
  FRESULT res_flash;
  FILINFO fnow;
  UINT fnum;

  uint8_t i, j, len;
  uint8_t rd_buf[512];  // 最大4*COL_SIZE*ROW_SIZE

  if (g_Ready_Status.fatfs == FALSE) return;

  res_flash = f_mount( &fs, "0:", 1 );
  if ( res_flash != FR_OK ) return;

  res_flash = f_open(&fnew, fp, FA_OPEN_EXISTING | FA_READ );
  if ( res_flash != FR_OK ) goto fs_read_kbd_mat_end;    // 文件不存在

  res_flash = f_read(&fnew, rd_buf, sizeof(rd_buf), &fnum);
  if ( res_flash == FR_OK ) {
    len = 0;
    for (i = 0; i < ROW_SIZE; i++) {
      for (j = 0; j < COL_SIZE; j++) {
        len += string_dec_to_u8(&rd_buf[len], &key_arr[j + i * COL_SIZE]);
      }
    }
  }

  f_close(&fnew);

  fs_read_kbd_mat_end:
  f_mount( NULL, "0:", 1 );   // 卸载文件系统
}
