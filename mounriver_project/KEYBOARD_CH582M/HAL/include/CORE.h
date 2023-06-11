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

  enum LP_Type {
    lp_mode_none = 0,
    lp_idle_mode,
    lp_sw_mode,
    lp_halt_mode,
    lp_sleep_mode,
    lp_shutdown_mode,
    lp_max_mode,
  };

  void APPJumpBoot(void);
  void SoftReset(void);
  void TP78Reinit(uint8_t type, enum LP_Type lp_type);
  void GotoLowpower(enum LP_Type type);
#endif
