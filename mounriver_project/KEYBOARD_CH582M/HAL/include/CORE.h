/********************************** (C) COPYRIGHT *******************************
 * File Name          : CORE.h
 * Author             : ChnMasterOG
 * Version            : V1.1
 * Date               : 2022/12/24
 * Description        : 上下电、低功耗相关控制
 * Copyright (c) 2023 ChnMasterOG
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#ifndef __CORE_H
  #define __CORE_H

  #include "CH58x_common.h"

  #define BOOT_FLAG_SDRAM_ADDRESS     (0x20008000-4)
  #define FLAG_APP_TO_BOOT            0x5555AAAA
  #define SDRAM32(x)                  (*((volatile uint32_t *)(x)))

  enum LP_Type {
    lp_mode_none = 0,
    lp_idle_mode,
    lp_sw_mode,
    lp_halt_mode,
    lp_sleep_mode,
    lp_shutdown_mode,
    lp_max_mode,
  };

  void APPJumpKBoot(void);
  void APPJumpBoot(void);
  void SoftReset(void);
  void TP78_TMOS_Start(void);
  void TP78_TMOS_Stop(void);
  void TP78Reinit(uint8_t type, enum LP_Type lp_type);
  void GotoLowpower(enum LP_Type type);
#endif
