/********************************** (C) COPYRIGHT *******************************
 * File Name          : debug_log.h
 * Author             : ChnMasterOG
 * Version            : V1.0
 * Date               : 2026/7/7
 * Description        : debug logÍ·ÎÄ¼þ
 * Copyright (c) 2026 ChnMasterOG
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#ifndef __DEBUG_LOG_H
#define __DEBUG_LOG_H

#include "CH58x_common.h"

#define DEBUG_LOG_BUF_SIZE 32

#define verbose_log(fmt, ...) { debug_log_write("[V]" fmt "\n", ##__VA_ARGS__); }
#define debug_log(fmt, ...) { debug_log_write("[D]" fmt "\n", ##__VA_ARGS__); }
#define error_log(fmt, ...) { debug_log_write("[E]" fmt "\n", ##__VA_ARGS__); }

void debug_log_write(const char *fmt, ...);
uint8_t debug_log_read_buffer(uint8_t *buffer, uint8_t max_len);

#endif
