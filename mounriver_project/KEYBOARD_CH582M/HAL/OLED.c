/********************************** (C) COPYRIGHT *******************************
 * File Name          : OLED.c
 * Author             : ChnMasterOG
 * Version            : V1.1
 * Date               : 2022/3/20
 * Description        : OLED 驱动源文件
 * Copyright (c) 2023 ChnMasterOG
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#include "HAL.h"

//OLED的显存
//存放格式如下.
//[0]0 1 2 3 ... 127
//[1]0 1 2 3 ... 127
//[2]0 1 2 3 ... 127
//[3]0 1 2 3 ... 127
//[4]0 1 2 3 ... 127
//[5]0 1 2 3 ... 127
//[6]0 1 2 3 ... 127
//[7]0 1 2 3 ... 127

/*******************************************************************************
 * Function Name  : OLED_SW_I2C_Delay
 * Description    : OLED软件模拟I2C-延迟函数
 * Input          : None
 * Return         : None
 *******************************************************************************/
static void OLED_SW_I2C_Delay(void)
{
	uint32_t i = 5;
	while( i-- );
}

/*******************************************************************************
 * Function Name  : OLED_SW_I2C_Start
 * Description    : OLED软件模拟I2C-Start信号
 * Input          : None
 * Return         : None
 *******************************************************************************/
static void OLED_SW_I2C_Start(void)
{
	OLED_IIC_SDIN_Set();
	I2C_DELAY;

	OLED_IIC_SCLK_Set() ;
	I2C_DELAY;

	OLED_IIC_SDIN_Clr();
	I2C_DELAY;

	OLED_IIC_SCLK_Clr();
	I2C_DELAY;
}

/*******************************************************************************
 * Function Name  : OLED_SW_I2C_Stop
 * Description    : OLED软件模拟I2C-Stop信号
 * Input          : None
 * Return         : None
 *******************************************************************************/
static void OLED_SW_I2C_Stop(void)
{
  OLED_IIC_SDIN_Clr();
	I2C_DELAY;

	OLED_IIC_SCLK_Set();
	I2C_DELAY;

	OLED_IIC_SDIN_Set();
	I2C_DELAY;
}

/*******************************************************************************
 * Function Name  : OLED_SW_I2C_SendByte
 * Description    : OLED软件模拟I2C-I2C发送1字节
 * Input          : Data
 * Return         : None
 *******************************************************************************/
static void OLED_SW_I2C_SendByte(uint8_t Data)
{
  uint8_t i;
	OLED_IIC_SCLK_Clr();
	for(i = 0; i < 8; i++)
	{  
	  if(Data & 0x80)	{
			OLED_IIC_SDIN_Set();
		}
		else {
			OLED_IIC_SDIN_Clr();
		} 
		Data <<= 1;
		I2C_DELAY;
		OLED_IIC_SCLK_Set();
		I2C_DELAY;
		OLED_IIC_SCLK_Clr();
		I2C_DELAY;
	}
	OLED_IIC_SDIN_Set();
	I2C_DELAY;
	OLED_IIC_SCLK_Set();
	I2C_DELAY;
	OLED_IIC_SCLK_Clr(); 
}

/*******************************************************************************
 * Function Name  : OLED_SW_I2C_WR_Byte
 * Description    : OLED软件模拟I2C-写寄存器
 * Input          : Data
 * Return         : None
 *******************************************************************************/
void OLED_SW_I2C_WR_Byte(uint8_t dat, uint8_t cmd)
{
  OLED_SW_I2C_Start();
  OLED_SW_I2C_SendByte(0x78);
  if(cmd == 0) {
    OLED_SW_I2C_SendByte(0x00);
  }
  else {
    OLED_SW_I2C_SendByte(0x40);
  }
  OLED_SW_I2C_SendByte(dat);
  OLED_SW_I2C_Stop();
}

/*******************************************************************************
 * Function Name  : OLED_Set_Pos
 * Description    : 设定OLED显示字符的位置
 * Input          : x, y - 横坐标和页坐标
 * Note(SSD1306)  :
 * B0~B7 1 0 1 1  0 X2 X1 X0 设置页地址 X[2:0]:0~7 对应页 0~7
 * 00~0F 0 0 0 0 X3 X2 X1 X0 设置列地址(低四位) 设置8位起始列地址的低四位
 * 10~1F 0 0 0 1 X3 X2 X1 X0 设置列地址(高四位) 设置8位起始列地址的高四位
 * Return         : None
 *******************************************************************************/
void OLED_Set_Pos(uint8_t x, uint8_t y)
{ 
#if defined(OLED_0_42) || defined(OLED_0_66)
  x += 32;
#endif
	OLED_WR_Byte(0xb0 + y, OLED_CMD);            				//设置页地址
	OLED_WR_Byte(((x & 0xf0) >> 4) | 0x10, OLED_CMD);  	//设置列地址高4位
	OLED_WR_Byte((x & 0x0f) | 0x01, OLED_CMD);      		//设置列地址低4位
}   	  

/*******************************************************************************
 * Function Name  : OLED_Display_On
 * Description    : 开启OLED显示
 * Input          : None
 * Note(SSD1306)  :
 *   8D   1 0 0 0 1  1 0 1
 * A[7:0] * * 0 1 0 A2 0 0  电荷泵设置   A2=0,关闭电荷泵;A2=1,开启电荷泵
 * AE/AF  1 0 1 0 1  1 1 X0 设置显示开关 X0=0,关闭显示;X0=1,开启显示
 * Return         : None
 *******************************************************************************/
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D, OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X14, OLED_CMD);  //DCDC ON      //开启电荷泵
	OLED_WR_Byte(0XAF, OLED_CMD);  //DISPLAY ON   //开启显示
}

/*******************************************************************************
 * Function Name  : OLED_Display_Off
 * Description    : 关闭OLED显示
 * Input          : None
 * Return         : None
 *******************************************************************************/
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D, OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X10, OLED_CMD);  //DCDC OFF     //关闭电荷泵
	OLED_WR_Byte(0XAE, OLED_CMD);  //DISPLAY OFF  //关闭显示
}

/*******************************************************************************
 * Function Name  : OLED_Clear
 * Description    : OLED清屏函数
 * Input          : None
 * Return         : None
 *******************************************************************************/
void OLED_Clear(void)
{
	uint8_t i,n;		    
	for(i = 0; i < 8; i++)  
	{  
		OLED_WR_Byte(0xb0 + i, OLED_CMD);          //设置页地址（0~7）
		OLED_WR_Byte(0x00, OLED_CMD);              //设置显示位置—列低地址
		OLED_WR_Byte(0x10, OLED_CMD);              //设置显示位置—列高地址
		for(n=0;n<128;n++) OLED_WR_Byte(0, OLED_DATA); 
	}                                            //更新显示
}

/*******************************************************************************
 * Function Name  : OLED_ShowChar
 * Description    : OLED在指定位置显示一个字符,包括部分字符
 * Input          : x, y - 横坐标和页坐标; chr - 显示的字符
 * Return         : None
 *******************************************************************************/
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr)
{      	
	unsigned char c=0,i=0;	
	c = chr - ' ';                             //得到偏移后的值(对应ASCII码)
	if(x > Max_Column - 1) {
	  x = 0;
	  if (FONT_SIZE == 16) y += 2;
	  else y++;
	}
	if (FONT_SIZE == 16)                       //一个字符占8列16行(2页) 一块128*64像素屏一行能显示16个字符 能显示4行
	{
		OLED_Set_Pos(x, y);	                     //开始写第一页
			                                       //写入字符前64列(第一页先列后行)
		for(i = 0; i < 8; i++)                   //每次写8位 写8次
			OLED_WR_Byte(F8X16[c*16 + i], OLED_DATA);   
		OLED_Set_Pos(x, y + 1);                  //开始写第二页
			                                       //写入字符后64列(第二页先列后行)
		for(i = 0; i < 8; i++)                   //每次写8位 写8次
			OLED_WR_Byte(F8X16[c * 16 + i + 8], OLED_DATA); 
	}
	else
	{	
		OLED_Set_Pos(x, y);
		for(i = 0; i < 6; i++) OLED_WR_Byte(F6x8[c][i], OLED_DATA);
	}
}

/*******************************************************************************
 * Function Name  : OLED_Pow
 * Description    : 计算m^n
 * Input          : m, n
 * Return         : m^n
 *******************************************************************************/
inline uint32_t OLED_Pow(uint8_t m, uint8_t n)
{
	uint32_t result=1;
	while(n--) result*=m;    
	return result;
}				  

/*******************************************************************************
 * Function Name  : OLED_ShowNum
 * Description    : 显示1个数字
 * Input          : x, y - 横坐标和页坐标; num - 显示的数字; len - 数字的长度
 * Return         : None
 *******************************************************************************/
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len)
{         	
	uint8_t t, temp;
	uint8_t enshow = 0;
	for(t = 0; t < len; t++)
	{
		temp=(num / OLED_Pow(10, len-t-1)) % 10;	//从最高位依次将每一位(十进制)取出
		if(enshow == 0 && t < (len - 1))
		{
			if(temp == 0)
			{
			  if ( FONT_SIZE == 16 ) OLED_ShowChar(x + 8 * t, y, ' ');
			  else OLED_ShowChar(x + 6 * t, y, ' '); //将开头为0的位略成' '
				continue;
			}else enshow = 1; 
		}
		if ( FONT_SIZE == 16 ) OLED_ShowChar(x + 8 * t, y, temp + '0');	//打印数字
		else OLED_ShowChar(x + 6 * t, y, temp + '0'); //打印数字
	}
}

/*******************************************************************************
 * Function Name  : OLED_ShowString
 * Description    : OLED显示字符串
 * Input          : x, y - 横坐标和页坐标; *chr - 要显示的字符串
 * Return         : None
 *******************************************************************************/
void OLED_ShowString(uint8_t x, uint8_t y, const uint8_t *chr)
{
	uint8_t j=0;
	while(chr[j]!='\0')                         //该位不为空则打印
	{
		OLED_ShowChar(x, y, chr[j]);        			//从第x列第y页打印字符
    if ( FONT_SIZE == 16 ) {
      x += 8;                                 //字宽8列 打印完跳8列
      if (x > 128 - 8) {
        x = 0; y += 2;
      }
    } else {
      x += 6;                                 //字宽6列 打印完跳6列
      if (x > 128 - 6) {
        x = 0; y += 1;
      }
    }
    j++;
	}
}

/*******************************************************************************
 * Function Name  : OLED_ShowString_f
 * Description    : OLED反向显示字符串
 * Input          : x, y - 横坐标和页坐标; *chr - 要显示的字符串
 * Return         : None
 *******************************************************************************/
void OLED_ShowString_f(uint8_t x, uint8_t y, const uint8_t *chr)
{
  uint8_t j=0;
  unsigned char c=0,i=0;

  while(chr[j] != '\0')                       //该位不为空则打印
  {
    c = chr[j] - ' ';                         //得到偏移后的值(对应ASCII码)
    if(x > Max_Column - 1) {
      x = 0;
      if (FONT_SIZE == 16) y += 2;
      else y++;
    }
    if (FONT_SIZE == 16) {                    //一个字符占8列16行(2页) 一块128*64像素屏一行能显示16个字符 能显示4行
      OLED_Set_Pos(x, y);
      for(i = 0; i < 8; i++) OLED_WR_Byte(~F8X16[c * 16 + i], OLED_DATA);
      OLED_Set_Pos(x, y + 1);
      for(i = 0; i < 8; i++) OLED_WR_Byte(~F8X16[c * 16 + i + 8], OLED_DATA);
    } else {
      OLED_Set_Pos(x, y);
      for(i = 0; i < 6; i++) OLED_WR_Byte(~F6x8[c][i], OLED_DATA);
    }
    if ( FONT_SIZE == 16 ) {
      x += 8;           //字宽8列 打印完跳8列
      if (x > 128 - 8) {
        x = 0; y += 2;
      }
    } else {
      x += 6;           //字宽6列 打印完跳6列
      if (x > 128 - 6) {
        x = 0; y += 1;
      }
    }
    j++;
  }
}

/*******************************************************************************
 * Function Name  : OLED_ShowCHinese
 * Description    : OLED显示中文
 * Input          : x, y - 横坐标和页坐标; no - oledfont.h中存放汉字数组Hzk的序号
 * Return         : None
 *******************************************************************************/
void OLED_ShowCHinese(uint8_t x, uint8_t y, uint8_t no)
{      			    
	uint8_t t,adder=0;
	OLED_Set_Pos(x,y);	
  for(t=0;t<16;t++)                           //打印中文第一页部分
  {
    OLED_WR_Byte(Hzk[2*no][t],OLED_DATA);
    adder+=1;
  }
	OLED_Set_Pos(x,y+1);	
  for(t=0;t<16;t++)                           //打印中文第二页部分
	{
		OLED_WR_Byte(Hzk[2*no+1][t],OLED_DATA);
		adder+=1;
  }
}

/*******************************************************************************
 * Function Name  : OLED_DrawBMP
 * Description    : 显示位图
 * Input          : x0为起始列坐标; y0为其实页坐标; x1为终止列坐标; y1位终止页坐标; BMP[]为存放图片的数组
 * Return         : None
 *******************************************************************************/
void OLED_DrawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t* BMP)
{ 	
  //BMP大小((x1-x0+1)*(y1-y0+1))
  unsigned int j=0;
  unsigned char x,y;

  for (y = y0; y < y1; y++) {
    OLED_Set_Pos(x0,y);
    for (x = x0; x < x1; x++) {
      OLED_WR_Byte(BMP[j++],OLED_DATA);	      //逐点显示，先行后列
    }
  }
} 

/*******************************************************************************
 * Function Name  : OLED_Fill
 * Description    : OLED填充
 * Input          : x0为起始列坐标; y0为其实页坐标; x1为终止列坐标; y1位终止页坐标
 * Return         : None
 *******************************************************************************/
void OLED_Fill(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
  //BMP大小((x1-x0+1)*(y1-y0+1))
  unsigned int j=0;
  unsigned char x,y;

  for(y=y0;y<y1;y++)
  {
    OLED_Set_Pos(x0,y);
    for(x=x0;x<x1;x++)
    {
      OLED_WR_Byte(0xFF, OLED_DATA);       //逐点显示，先行后列
    }
  }
}

/*******************************************************************************
 * Function Name  : OLED_Clr
 * Description    : OLED清空(OLED_Fill反操作)
 * Input          : x0为起始列坐标; y0为其实页坐标; x1为终止列坐标; y1位终止页坐标
 * Return         : None
 *******************************************************************************/
void OLED_Clr(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
  //BMP大小((x1-x0+1)*(y1-y0+1))
  unsigned int j=0;
  unsigned char x,y;

  for(y=y0;y<y1;y++)
  {
    OLED_Set_Pos(x0,y);
    for(x=x0;x<x1;x++)
    {
      OLED_WR_Byte(0x00, OLED_DATA);       //逐点显示，先行后列
    }
  }
}

/*******************************************************************************
 * Function Name  : OLED_Midx
 * Description    : OLED计算字符串/数字居中位置x坐标
 * Input          : length - 文字长度; xstart, xend - 显示区域起点、终点
 * Return         : None
 *******************************************************************************/
inline uint8_t OLED_Midx(uint8_t length, uint8_t xstart, uint8_t xend)
{
  uint8_t w = FONT_SIZE == 16 ? 8 : 6;
	if (length * w > xend - xstart) return 0;
	return xstart + (xend - xstart - length * w) / 2;
}

/*******************************************************************************
 * Function Name  : OLED_Set_Scroll_ENA
 * Description    : 设置OLED是否使能滚屏
 * Input          : 0 - 失能; 1 - 使能
 * Return         : None
 *******************************************************************************/
void OLED_Set_Scroll_ENA(uint8_t is_enable)
{
  if (is_enable) {
    OLED_WR_Byte(0x2F, OLED_CMD);
  } else {
    OLED_WR_Byte(0x2E, OLED_CMD);
  }
}

/*******************************************************************************
 * Function Name  : OLED_Scroll
 * Description    : 设置OLED滚屏
 * Input          : h_y0 - 水平滚动起始页地址; h_y1 - 水平滚动终止页地址;
 *                  v_ystart - 垂直滚动起始点阵(0~64); v_len - 垂直滚动长度(0~64);
 *                  frame - 帧单位; scroll_offset - 滚动偏移; dir - 方向(0表示向左, 1表示向右)
 * Return         : None
 *******************************************************************************/
void OLED_Scroll(uint8_t h_y0, uint8_t h_y1, uint8_t v_ystart, uint8_t v_len,
                 uint8_t frame, uint8_t scroll_offset, uint8_t dir)
{
  OLED_WR_Byte(0x2E, OLED_CMD);   // 停止滚动

  OLED_WR_Byte(0xA3, OLED_CMD);
  OLED_WR_Byte(v_ystart, OLED_CMD);
  OLED_WR_Byte(v_len, OLED_CMD);

  OLED_WR_Byte(dir ? 0x29 : 0x2A, OLED_CMD);
  OLED_WR_Byte(0x00, OLED_CMD);
  OLED_WR_Byte(h_y0 & 0x7, OLED_CMD);
  OLED_WR_Byte(frame & 0x7, OLED_CMD);
  OLED_WR_Byte(h_y1 & 0x7, OLED_CMD);
  OLED_WR_Byte(scroll_offset, OLED_CMD);

  OLED_WR_Byte(0x2F, OLED_CMD);   // 启用滚动
}

/*******************************************************************************
 * Function Name  : HAL_OLED_Init
 * Description    : 初始化SSD1306
 * Input          : None
 * Return         : None
 *******************************************************************************/
void HAL_OLED_Init(void)
{ 	
  //IO
#if !(defined HAL_HW_I2C) || (HAL_HW_I2C != TRUE)
  OLED_CLK_GPIO_(SetBits)( OLED_CLK_Pin );
  OLED_SDA_GPIO_(SetBits)( OLED_SDA_Pin );
  OLED_CLK_GPIO_(ModeCfg)( OLED_CLK_Pin, GPIO_ModeOut_PP_5mA );
  OLED_SDA_GPIO_(ModeCfg)( OLED_SDA_Pin, GPIO_ModeOut_PP_5mA );
#endif

#ifdef OLED_0_42
  //display off
  OLED_WR_Byte(0xAE,OLED_CMD);
  OLED_WR_Byte(0xD5,OLED_CMD);
  OLED_WR_Byte(0xF0,OLED_CMD);
  OLED_WR_Byte(0xA8,OLED_CMD);
  OLED_WR_Byte(0x27,OLED_CMD);
  OLED_WR_Byte(0xD3,OLED_CMD);
  OLED_WR_Byte(0x00,OLED_CMD);
  OLED_WR_Byte(0x40,OLED_CMD);
  OLED_WR_Byte(0x8D,OLED_CMD);
  OLED_WR_Byte(0x14,OLED_CMD);
  OLED_WR_Byte(0x20,OLED_CMD);
  OLED_WR_Byte(0x02,OLED_CMD);
  OLED_WR_Byte(0xA1,OLED_CMD);
  OLED_WR_Byte(0xC8,OLED_CMD);
  OLED_WR_Byte(0xDA,OLED_CMD);
  OLED_WR_Byte(0x12,OLED_CMD);
  OLED_WR_Byte(0xAD,OLED_CMD);
  OLED_WR_Byte(0x30,OLED_CMD);

  //对比度
  OLED_WR_Byte(0x81,OLED_CMD);
  OLED_WR_Byte(0x2F,OLED_CMD);

  //Pre-Charge Period
  OLED_WR_Byte(0xD9,OLED_CMD);
  OLED_WR_Byte(0x22,OLED_CMD);

  //Vcomh
  OLED_WR_Byte(0xDB,OLED_CMD);
  OLED_WR_Byte(0x20,OLED_CMD);

  //整体显示(输出跟随RAM)
  OLED_WR_Byte(0xA4,OLED_CMD);

  //正常显示
  OLED_WR_Byte(0xA6,OLED_CMD);

  //列开始：140
  OLED_WR_Byte(0x0C,OLED_CMD);
  OLED_WR_Byte(0x11,OLED_CMD);
#endif

#ifdef OLED_0_66
#if 0
  OLED_WR_Byte(0xAE,OLED_CMD);  // display off

  OLED_WR_Byte(0xD5,OLED_CMD);  // set display clock divide ratio/oscillator frequency
  OLED_WR_Byte(0xF0,OLED_CMD);  // divide ratio = 0x0 + 1, oscillator frequency = 0xF

  OLED_WR_Byte(0xA8,OLED_CMD);  // set multiplex ratio
  OLED_WR_Byte(0x27,OLED_CMD);  // 28 MUX

  OLED_WR_Byte(0xD3,OLED_CMD);  // set vertical shift by COM
  OLED_WR_Byte(0x00,OLED_CMD);  // vertical shift = 0x0

  OLED_WR_Byte(0x40,OLED_CMD);  // start line 0x40 - 0x40 = 0
  OLED_WR_Byte(0xB0,OLED_CMD);  // start page 0xB0 - 0xB0 = 0

  OLED_WR_Byte(0x8D,OLED_CMD);
  OLED_WR_Byte(0x14,OLED_CMD);

  OLED_WR_Byte(0x20,OLED_CMD);  // set memory addressing mode
  OLED_WR_Byte(0x02,OLED_CMD);  // page address mode

  OLED_WR_Byte(0xA1,OLED_CMD);  // column 127 is mapped to SEG0

  OLED_WR_Byte(0xC8,OLED_CMD);  // set COM output scan direction. scan from COM[N-1] to COM0

  OLED_WR_Byte(0xDA,OLED_CMD);  // set COM pins hardware configuration
  OLED_WR_Byte(0x12,OLED_CMD);  // alternative COM pin configuration. disable COM Left/Right remap

  OLED_WR_Byte(0xAD,OLED_CMD);
  OLED_WR_Byte(0x30,OLED_CMD);

  OLED_WR_Byte(0x81,OLED_CMD);  // set contrast control
  OLED_WR_Byte(0x2F,OLED_CMD);

  OLED_WR_Byte(0xD9,OLED_CMD);  // set pre-charge period
  OLED_WR_Byte(0x22,OLED_CMD);

  OLED_WR_Byte(0xDB,OLED_CMD);  // set Vcomh
  OLED_WR_Byte(0x20,OLED_CMD);

  OLED_WR_Byte(0xA4,OLED_CMD);  // output folows RAM content

  OLED_WR_Byte(0xA6,OLED_CMD);  // set normal display (not inverse)
#endif
  OLED_WR_Byte(0xAE,OLED_CMD); /*display off*/
  OLED_WR_Byte(0x00,OLED_CMD); /*set lower column address*/
  OLED_WR_Byte(0x12,OLED_CMD); /*set higher column address*/
  OLED_WR_Byte(0x40,OLED_CMD); /*set display start line*/
  OLED_WR_Byte(0xB0,OLED_CMD); /*set page address*/
  OLED_WR_Byte(0x81,OLED_CMD); /*contract control*/
  OLED_WR_Byte(0xff,OLED_CMD); /*128*/
  OLED_WR_Byte(0xA1,OLED_CMD); /*set segment remap*/
  OLED_WR_Byte(0xA6,OLED_CMD); /*normal / reverse*/
  OLED_WR_Byte(0xA8,OLED_CMD); /*multiplex ratio*/
  OLED_WR_Byte(0x2F,OLED_CMD); /*duty = 1/48*/
  OLED_WR_Byte(0xC8,OLED_CMD); /*Com scan direction*/
  OLED_WR_Byte(0xD3,OLED_CMD); /*set display offset*/
  OLED_WR_Byte(0x00,OLED_CMD);
  OLED_WR_Byte(0xD5,OLED_CMD); /*set osc division*/
  OLED_WR_Byte(0x80,OLED_CMD);
  OLED_WR_Byte(0xD9,OLED_CMD); /*set pre-charge period*/
  OLED_WR_Byte(0x21,OLED_CMD);
  OLED_WR_Byte(0xDA,OLED_CMD); /*set COM pins*/
  OLED_WR_Byte(0x12,OLED_CMD);
  OLED_WR_Byte(0xdb,OLED_CMD); /*set vcomh*/
  OLED_WR_Byte(0x40,OLED_CMD);
  OLED_WR_Byte(0x8d,OLED_CMD); /*set charge pump enable*/
  OLED_WR_Byte(0x14,OLED_CMD);
#endif

#ifdef OLED_0_96
  OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel

  OLED_WR_Byte(0x40,OLED_CMD);//---set low column address
  OLED_WR_Byte(0xB0,OLED_CMD);//---set high column address

  OLED_WR_Byte(0xC8,OLED_CMD);//-not offset

  OLED_WR_Byte(0x81,OLED_CMD);//设置对比度
  OLED_WR_Byte(0xff,OLED_CMD);

  OLED_WR_Byte(0xa1,OLED_CMD);//段重定向设置

  OLED_WR_Byte(0xa6,OLED_CMD);//

  OLED_WR_Byte(0xa8,OLED_CMD);//设置驱动路数
  OLED_WR_Byte(0x1f,OLED_CMD);

  OLED_WR_Byte(0xd3,OLED_CMD);
  OLED_WR_Byte(0x00,OLED_CMD);

  OLED_WR_Byte(0xd5,OLED_CMD);
  OLED_WR_Byte(0xf0,OLED_CMD);

  OLED_WR_Byte(0xd9,OLED_CMD);
  OLED_WR_Byte(0x22,OLED_CMD);

  OLED_WR_Byte(0xda,OLED_CMD);
  OLED_WR_Byte(0x02,OLED_CMD);

  OLED_WR_Byte(0xdb,OLED_CMD);
  OLED_WR_Byte(0x49,OLED_CMD);

  OLED_WR_Byte(0x8d,OLED_CMD);
  OLED_WR_Byte(0x14,OLED_CMD);
#endif

  //display ON
  OLED_Clear();
  OLED_WR_Byte(0xAF, OLED_CMD);
	OLED_Set_Pos(0,0);
}
/*********************************************END OF FILE**********************/
