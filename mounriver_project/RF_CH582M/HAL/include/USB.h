/********************************** (C) COPYRIGHT *******************************
 * File Name          : USB.h
 * Author             : ChnMasterOG
 * Version            : V1.0
 * Date               : 2021/11/17
 * Description        : USB����ͷ�ļ�
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

    #define USB_KEYBOARD_EVENT    0x0001
    #define USB_MOUSE_EVENT       0x0002
    #define USB_VOL_EVENT         0x0004
    #define USB_DIAL_EVENT        0x0008
    #define START_USB_EVENT       0x0010
    #define USB_CONNECT_EVENT     0x0020
    #define USB_TEST_EVENT        0x1000

    #define USB_ERR_LENGTH        0x01
    #define USB_DAT_INVALID       0x02
    #define USB_ERR_UNKNOWN       0x10

    struct HIDdata {
        UINT8 label;
        union {
            UINT8 Keyboard[8];
            UINT8 Mouse[4];
            UINT8 Volume[1];
            UINT8 Dial[2];
            UINT8 data[8];
        };
    };

    enum {
        KEYBOARD_REPORT_ID = 1,
        MOUSE_REPORT_ID,
        VOL_REPORT_ID,
        DIAL_REPORT_ID,
    };

    extern struct HIDdata HID_dat;

    extern const UINT8 MyDevDescr[];
    extern const UINT8 MyCfgDescr[];
    extern const UINT8 MyLangDescr[];
    extern const UINT8 MyManuInfo[];
    extern const UINT8 MyProdInfo[] ;
    extern UINT8 DevConfig;
    extern UINT8 SetupReqCode;
    extern UINT16 SetupReqLen;
    extern const UINT8 *pDescr;

    extern tmosTaskID usbTaskID;
    extern BOOL USB_Ready;
    extern BOOL USB_CapsLock_LEDOn;

    void HAL_USBInit( void );

#endif
