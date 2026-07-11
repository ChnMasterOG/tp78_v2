/********************************** (C) COPYRIGHT *******************************
 * File Name          : debug_log.c
 * Author             : ChnMasterOG
 * Version            : V1.0
 * Date               : 2026/7/7
 * Description        : debug logÔ´ÎÄ¼þ
 * Copyright (c) 2026 ChnMasterOG
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#include "debug_log.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static char log_buf[DEBUG_LOG_BUF_SIZE];
static uint16_t head = 0;
static uint16_t tail = 0;

void debug_log_write(const char *fmt, ...)
{
    char tmp[128];
    va_list args;
    va_start(args, fmt);
    vsnprintf(tmp, sizeof(tmp), fmt, args);
    va_end(args);

    size_t len = strlen(tmp);
    for (size_t i = 0; i < len; i++) {
        log_buf[head] = tmp[i];
        head = (head + 1) % DEBUG_LOG_BUF_SIZE;
        if (head == tail) {
            tail = (tail + 1) % DEBUG_LOG_BUF_SIZE;
        }
    }
}

uint8_t debug_log_read_buffer(uint8_t *buffer, uint8_t max_len)
{
    uint8_t count = 0;
    while (tail != head && count < max_len) {
        buffer[count++] = log_buf[tail];
        tail = (tail + 1) % DEBUG_LOG_BUF_SIZE;
    }
    return count;
}