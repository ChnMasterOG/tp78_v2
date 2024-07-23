/********************************** (C) COPYRIGHT *******************************
 * File Name          : USB.h
 * Author             : ChnMasterOG
 * Version            : V1.0
 * Date               : 2021/11/17
 * Description        : USB驱动头文件
 * Copyright (c) 2023 ChnMasterOG
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#ifndef __USB_H
    #define __USB_H

    #include "CH58x_common.h"

    #define DevEP0SIZE      0x40
    #define NumsLockLED     0x01
    #define CapsLockLED     0x02
    #define ScrollLockLED   0x04

    #define Volume_Play     (1 << 0)
    #define Volume_Next     (1 << 1)
    #define Volume_Prev     (1 << 2)
    #define Volume_Incr     (1 << 3)
    #define Volume_Decr     (1 << 4)
    #define Volume_Mute     (1 << 5)
    #define Volume_Pause    (1 << 6)
    #define Volume_Stop     (1 << 7)

    #define USB_MOUSE_EVENT       0x0001
    #define USB_KEYBOARD_EVENT    0x0002
    #define USB_VOL_EVENT         0x0004
    #define USB_SWITCH_EVENT      0x0008
    #define USB_TEST_EVENT        0x1000

    #define USB_ERR_LENGTH        0x01
    #define USB_DAT_INVALID       0x02
    #define USB_ERR_UNKNOWN       0x10

    enum Report_ID_E {
        RID_KEYBOARD = 1,
        RID_MOUSE,
        RID_CONSUMER_CONTROL, // Media, volume etc ..
        RID_DIAL,
        RID_LIGHTING_LAMP_ARRAY_ATTRIBUTES,
        RID_LIGHTING_LAMP_ATTRIBUTES_REQUEST,
        RID_LIGHTING_LAMP_ATTRIBUTES_RESPONSE,
        RID_LIGHTING_LAMP_MULTI_UPDATE,
        RID_LIGHTING_LAMP_RANGE_UPDATE,
        RID_LIGHTING_LAMP_ARRAY_CONTROL,
    };

    extern tmosTaskID usbTaskID;

    void usb_device_init( void );
    void HAL_USBInit( void );

#endif
