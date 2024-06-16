/********************************** (C) COPYRIGHT *******************************
* File Name          : ISP.c
* Author             : ChnMasterOG
* Version            : V1.0
* Date               : 2022/7/17
* Description        : ��תBOOT����ISP����-Դ�ļ�
* Copyright (c) 2023 ChnMasterOG
* SPDX-License-Identifier: GPL-3.0
*******************************************************************************/

#include "HAL.h"
#include "CONFIG.h"

__HIGH_CODE
void APPJumpBoot(void)   //�˶δ������������RAM��
{
  while(FLASH_ROM_ERASE(0, EEPROM_BLOCK_SIZE))
  {
    ;//ROM ��4K1����λ����0��ַ��ʼ
  }
  FLASH_ROM_SW_RESET();
  R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
  R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;  //��ȫ����ģʽ
  SAFEOPERATE;
  R16_INT32K_TUNE = 0xFFFF;
  R8_RST_WDOG_CTRL |= RB_SOFTWARE_RESET;
  R8_SAFE_ACCESS_SIG = 0;//�����ִ�и�λ����λ����Ϊ�ϵ縴λ
  while(1);//Ӫ���Ƭ����������ʱ�ͻ�ͣ��BOOT������д����ʱʱ��10s
}

__HIGH_CODE
void SoftReset(void)   //�����λ
{
  R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
  R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;  //��ȫ����ģʽ
  SAFEOPERATE;
  R16_INT32K_TUNE = 0xFFFF;
  R8_RST_WDOG_CTRL |= RB_SOFTWARE_RESET;
  R8_SAFE_ACCESS_SIG = 0;
  while(1);
}
