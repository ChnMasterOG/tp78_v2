/********************************** (C) COPYRIGHT *******************************
 * File Name          : USB.c
 * Author             : ChnMasterOG, sakumisu
 * Version            : V3.0
 * Date               : 2024/7/19
 * Description        : CherryUSB驱动
 * Copyright (c) 2023 ChnMasterOG
 * Copyright (c) 2022, sakumisu
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#include "usbd_core.h"
#include "usbd_hid.h"
#include "usbd_msc.h"

#include "HAL.h"
#include "APP.h"

#ifndef WBVAL
#define WBVAL(x) (unsigned char)((x) & 0xFF), (unsigned char)(((x) >> 8) & 0xFF)
#endif

/*!< USBD CONFIG */
#define USBD_VERSION 0x0110
#define USBD_PRODUCT_VERSION 0x0001
#define USBD_VID 0x0418   //0x4D4C
#define USBD_PID 0x7802   //0xAD03
#define USBD_MAX_POWER 0xfa
#define USBD_LANGID_STRING 1033
#define USBD_CONFIG_DESCRIPTOR_SIZE 82

/*!< USBD ENDPOINT CONFIG */
// interface0 for HID keyboard/mouse/consumer/dial
#define USBD_IF0_AL0_EP0_ADDR 0x81
#define USBD_IF0_AL0_EP0_SIZE 0x40
#define USBD_IF0_AL0_EP0_INTERVAL 0x01

// interface1 for msc or VIA
#define USBD_IF1_AL0_EP0_ADDR 0x02
#define USBD_IF1_AL0_EP0_SIZE 0x40
#define USBD_IF1_AL0_EP0_INTERVAL 0x00

#define USBD_IF1_AL0_EP1_ADDR 0x82
#define USBD_IF1_AL0_EP1_SIZE 0x40
#define USBD_IF1_AL0_EP1_INTERVAL 0x00

#define USBD_RAW_IF1_AL0_EP0_ADDR 0x82
#define USBD_RAW_IF1_AL0_EP0_SIZE 0x20
#define USBD_RAW_IF1_AL0_EP0_INTERVAL 0x01

#define USBD_RAW_IF1_AL0_EP1_ADDR 0x02
#define USBD_RAW_IF1_AL0_EP1_SIZE 0x20
#define USBD_RAW_IF1_AL0_EP1_INTERVAL 0x01

// interface2 for windows lighting
#define USBD_IF2_AL0_EP0_ADDR 0x83
#define USBD_IF2_AL0_EP0_SIZE 0x40
#define USBD_IF2_AL0_EP0_INTERVAL 0x02

/*!< USBD HID CONFIG */
#define USBD_HID_VERSION 0x0111
#define USBD_HID_COUNTRY_CODE 0

/* desc size should be smaller than 256(4 * 64) */
#define USBD_IF0_AL0_HID_REPORT_DESC_SIZE (65+76+35+56)
#define RAW_HID_REPORT_DESC_SIZE 34
#define LIGHTING_HID_REPORT_DESC_SIZE 236

/*!< USBD Descriptor */
const unsigned char usbd_descriptor[] = {
/********************************************** Device Descriptor */
    0x12,                                       /*!< bLength */
    0x01,                                       /*!< bDescriptorType */
    WBVAL(USBD_VERSION),                        /*!< bcdUSB */
    0x00,                                       /*!< bDeviceClass */
    0x00,                                       /*!< bDeviceSubClass */
    0x00,                                       /*!< bDeviceProtocol */
    0x40,                                       /*!< bMaxPacketSize */
    WBVAL(USBD_VID),                            /*!< idVendor */
    WBVAL(USBD_PID),                            /*!< idProduct */
    WBVAL(USBD_PRODUCT_VERSION),                /*!< bcdDevice */
    0x01,                                       /*!< iManufacturer */
    0x02,                                       /*!< iProduct */
    0x03,                                       /*!< iSerial */
    0x01,                                       /*!< bNumConfigurations */
/********************************************** Config Descriptor */
    0x09,                                       /*!< bLength */
    0x02,                                       /*!< bDescriptorType */
    WBVAL(USBD_CONFIG_DESCRIPTOR_SIZE),         /*!< wTotalLength */
    0x03,                                       /*!< bNumInterfaces */
    0x01,                                       /*!< bConfigurationValue */
    0x00,                                       /*!< iConfiguration */
    0xa0,                                       /*!< bmAttributes */
    USBD_MAX_POWER,                             /*!< bMaxPower */
/********************************************** Interface 0 Alternate 0 Descriptor */
    0x09,                                       /*!< bLength */
    0x04,                                       /*!< bDescriptorType */
    0x00,                                       /*!< bInterfaceNumber */
    0x00,                                       /*!< bAlternateSetting */
    0x01,                                       /*!< bNumEndpoints */
    0x03,                                       /*!< bInterfaceClass */
    0x00,                                       /*!< bInterfaceSubClass */
    0x00,                                       /*!< bInterfaceProtocol */
    0x02,                                       /*!< iInterface */
/********************************************** Class Specific Descriptor of HID */
    0x09,                                       /*!< bLength */
    0x21,                                       /*!< bDescriptorType */
    WBVAL(USBD_HID_VERSION),                    /*!< bcdHID */
    USBD_HID_COUNTRY_CODE,                      /*!< bCountryCode */
    0x01,                                       /*!< bNumDescriptors */
    0x22,                                       /*!< bDescriptorType */
    WBVAL(USBD_IF0_AL0_HID_REPORT_DESC_SIZE),   /*!< wItemLength */
/********************************************** Endpoint 0 Descriptor */
    0x07,                                       /*!< bLength */
    0x05,                                       /*!< bDescriptorType */
    USBD_IF0_AL0_EP0_ADDR,                      /*!< bEndpointAddress */
    0x03,                                       /*!< bmAttributes */
    WBVAL(USBD_IF0_AL0_EP0_SIZE),               /*!< wMaxPacketSize */
    USBD_IF0_AL0_EP0_INTERVAL,                  /*!< bInterval */
/********************************************** Interface 1 Alternate 0 Descriptor */
    0x09,                                       /*!< bLength */
    0x04,                                       /*!< bDescriptorType */
    0x01,                                       /*!< bInterfaceNumber */
    0x00,                                       /*!< bAlternateSetting */
    0x02,                                       /*!< bNumEndpoints */
    0x08,                                       /*!< bInterfaceClass */
    0x06,                                       /*!< bInterfaceSubClass */
    0x50,                                       /*!< bInterfaceProtocol */
    0x02,                                       /*!< iInterface */
/********************************************** Class Specific Descriptor of MSC */
/********************************************** Endpoint 0 Descriptor */
    0x07,                                       /*!< bLength */
    0x05,                                       /*!< bDescriptorType */
    USBD_IF1_AL0_EP0_ADDR,                      /*!< bEndpointAddress */
    0x02,                                       /*!< bmAttributes */
    WBVAL(USBD_IF1_AL0_EP0_SIZE),               /*!< wMaxPacketSize */
    USBD_IF1_AL0_EP0_INTERVAL,                  /*!< bInterval */
/********************************************** Endpoint 1 Descriptor */
    0x07,                                       /*!< bLength */
    0x05,                                       /*!< bDescriptorType */
    USBD_IF1_AL0_EP1_ADDR,                      /*!< bEndpointAddress */
    0x02,                                       /*!< bmAttributes */
    WBVAL(USBD_IF1_AL0_EP1_SIZE),               /*!< wMaxPacketSize */
    USBD_IF1_AL0_EP1_INTERVAL,                  /*!< bInterval */
/********************************************** Interface 2 Alternate 0 Descriptor */
    0x09,                                       /*!< bLength */
    0x04,                                       /*!< bDescriptorType */
    0x02,                                       /*!< bInterfaceNumber */
    0x00,                                       /*!< bAlternateSetting */
    0x01,                                       /*!< bNumEndpoints */
    0x03,                                       /*!< bInterfaceClass */
    0x00,                                       /*!< bInterfaceSubClass */
    0x00,                                       /*!< bInterfaceProtocol */
    0x02,                                       /*!< iInterface */
/********************************************** Class Specific Descriptor of HID */
    0x09,                                       /*!< bLength */
    0x21,                                       /*!< bDescriptorType */
    WBVAL(USBD_HID_VERSION),                    /*!< bcdHID */
    USBD_HID_COUNTRY_CODE,                      /*!< bCountryCode */
    0x01,                                       /*!< bNumDescriptors */
    0x22,                                       /*!< bDescriptorType */
    WBVAL(LIGHTING_HID_REPORT_DESC_SIZE),       /*!< wItemLength */
/********************************************** Endpoint 0 Descriptor */
    0x07,                                       /*!< bLength */
    0x05,                                       /*!< bDescriptorType */
    USBD_IF2_AL0_EP0_ADDR,                      /*!< bEndpointAddress */
    0x03,                                       /*!< bmAttributes */
    WBVAL(USBD_IF2_AL0_EP0_SIZE),               /*!< wMaxPacketSize */
    USBD_IF2_AL0_EP0_INTERVAL,                  /*!< bInterval */
/********************************************** Language ID String Descriptor */
    0x04,                                       /*!< bLength */
    0x03,                                       /*!< bDescriptorType */
    WBVAL(USBD_LANGID_STRING),                  /*!< wLangID0 */
/********************************************** String 1 Descriptor */
/* CL_novel */
    0x12,                                       /*!< bLength */
    0x03,                                       /*!< bDescriptorType */
    0x43, 0x00,                                 /*!< 'C' wcChar0 */
    0x4c, 0x00,                                 /*!< 'L' wcChar1 */
    0x5f, 0x00,                                 /*!< '_' wcChar2 */
    0x6e, 0x00,                                 /*!< 'n' wcChar3 */
    0x6f, 0x00,                                 /*!< 'o' wcChar4 */
    0x76, 0x00,                                 /*!< 'v' wcChar5 */
    0x65, 0x00,                                 /*!< 'e' wcChar6 */
    0x6c, 0x00,                                 /*!< 'l' wcChar7 */
/********************************************** String 2 Descriptor */
/* TP78 */
    0x0a,                                       /*!< bLength */
    0x03,                                       /*!< bDescriptorType */
    0x54, 0x00,                                 /*!< 'T' wcChar0 */
    0x50, 0x00,                                 /*!< 'P' wcChar1 */
    0x37, 0x00,                                 /*!< '7' wcChar2 */
    0x38, 0x00,                                 /*!< '8' wcChar3 */
/********************************************** String 3 Descriptor */
/* v2.0 */
    0x0a,                                       /*!< bLength */
    0x03,                                       /*!< bDescriptorType */
    0x76, 0x00,                                 /*!< 'v' wcChar0 */
    0x32, 0x00,                                 /*!< '2' wcChar1 */
    0x2e, 0x00,                                 /*!< '.' wcChar2 */
    0x30, 0x00,                                 /*!< '0' wcChar3 */
    0x00
};

#define USBD_CONFIG_DESCRIPTOR_SIZE_NO_MSC 91

/*!< USBD Descriptor for normal connection */
const unsigned char usbd_descriptor_no_msc[] = {
/********************************************** Device Descriptor */
    0x12,                                       /*!< bLength */
    0x01,                                       /*!< bDescriptorType */
    WBVAL(USB_2_0),                             /*!< bcdUSB */
    0x00,                                       /*!< bDeviceClass */
    0x00,                                       /*!< bDeviceSubClass */
    0x00,                                       /*!< bDeviceProtocol */
    0x40,                                       /*!< bMaxPacketSize */
    WBVAL(USBD_VID),                            /*!< idVendor */
    WBVAL(USBD_PID),                            /*!< idProduct */
    WBVAL(USBD_PRODUCT_VERSION),                /*!< bcdDevice */
    0x01,                                       /*!< iManufacturer */
    0x02,                                       /*!< iProduct */
    0x03,                                       /*!< iSerial */
    0x01,                                       /*!< bNumConfigurations */
/********************************************** Config Descriptor */
    0x09,                                       /*!< bLength */
    0x02,                                       /*!< bDescriptorType */
    WBVAL(USBD_CONFIG_DESCRIPTOR_SIZE_NO_MSC),  /*!< wTotalLength */
    0x03,                                       /*!< bNumInterfaces */
    0x01,                                       /*!< bConfigurationValue */
    0x00,                                       /*!< iConfiguration */
    0xa0,                                       /*!< bmAttributes */
    USBD_MAX_POWER,                             /*!< bMaxPower */
/********************************************** Interface 0 Alternate 0 Descriptor */
    0x09,                                       /*!< bLength */
    0x04,                                       /*!< bDescriptorType */
    0x00,                                       /*!< bInterfaceNumber */
    0x00,                                       /*!< bAlternateSetting */
    0x01,                                       /*!< bNumEndpoints */
    0x03,                                       /*!< bInterfaceClass */
    0x00,                                       /*!< bInterfaceSubClass */
    0x00,                                       /*!< bInterfaceProtocol */
    0x02,                                       /*!< iInterface */
/********************************************** Class Specific Descriptor of HID */
    0x09,                                       /*!< bLength */
    0x21,                                       /*!< bDescriptorType */
    WBVAL(USBD_HID_VERSION),                    /*!< bcdHID */
    USBD_HID_COUNTRY_CODE,                      /*!< bCountryCode */
    0x01,                                       /*!< bNumDescriptors */
    0x22,                                       /*!< bDescriptorType */
    WBVAL(USBD_IF0_AL0_HID_REPORT_DESC_SIZE),   /*!< wItemLength */
/********************************************** Endpoint 0 Descriptor */
    0x07,                                       /*!< bLength */
    0x05,                                       /*!< bDescriptorType */
    USBD_IF0_AL0_EP0_ADDR,                      /*!< bEndpointAddress */
    0x03,                                       /*!< bmAttributes */
    WBVAL(USBD_IF0_AL0_EP0_SIZE),               /*!< wMaxPacketSize */
    USBD_IF0_AL0_EP0_INTERVAL,                  /*!< bInterval */
/********************************************** Interface 1 Alternate 0 Descriptor */
    0x09,                                       /*!< bLength */
    0x04,                                       /*!< bDescriptorType */
    0x01,                                       /*!< bInterfaceNumber */
    0x00,                                       /*!< bAlternateSetting */
    0x02,                                       /*!< bNumEndpoints */
    0x03,                                       /*!< bInterfaceClass */
    0x00,                                       /*!< bInterfaceSubClass */
    0x00,                                       /*!< bInterfaceProtocol */
    0x02,                                       /*!< iInterface */
/********************************************** Class Specific Descriptor of HID */
    0x09,                                       /*!< bLength */
    0x21,                                       /*!< bDescriptorType */
    WBVAL(USBD_HID_VERSION),                    /*!< bcdHID */
    USBD_HID_COUNTRY_CODE,                      /*!< bCountryCode */
    0x01,                                       /*!< bNumDescriptors */
    0x22,                                       /*!< bDescriptorType */
    WBVAL(RAW_HID_REPORT_DESC_SIZE),            /*!< wItemLength */
/********************************************** Endpoint 0 Descriptor */
    0x07,                                       /*!< bLength */
    0x05,                                       /*!< bDescriptorType */
    USBD_RAW_IF1_AL0_EP0_ADDR,                  /*!< bEndpointAddress */
    0x03,                                       /*!< bmAttributes */
    WBVAL(USBD_RAW_IF1_AL0_EP0_SIZE),           /*!< wMaxPacketSize */
    USBD_RAW_IF1_AL0_EP0_INTERVAL,              /*!< bInterval */
/********************************************** Endpoint 1 Descriptor */
    0x07,                                       /*!< bLength */
    0x05,                                       /*!< bDescriptorType */
    USBD_RAW_IF1_AL0_EP1_ADDR,                  /*!< bEndpointAddress */
    0x03,                                       /*!< bmAttributes */
    WBVAL(USBD_RAW_IF1_AL0_EP1_SIZE),           /*!< wMaxPacketSize */
    USBD_RAW_IF1_AL0_EP1_INTERVAL,              /*!< bInterval */
/********************************************** Interface 2 Alternate 0 Descriptor */
    0x09,                                       /*!< bLength */
    0x04,                                       /*!< bDescriptorType */
    0x02,                                       /*!< bInterfaceNumber */
    0x00,                                       /*!< bAlternateSetting */
    0x01,                                       /*!< bNumEndpoints */
    0x03,                                       /*!< bInterfaceClass */
    0x00,                                       /*!< bInterfaceSubClass */
    0x00,                                       /*!< bInterfaceProtocol */
    0x02,                                       /*!< iInterface */
/********************************************** Class Specific Descriptor of HID */
    0x09,                                       /*!< bLength */
    0x21,                                       /*!< bDescriptorType */
    WBVAL(USBD_HID_VERSION),                    /*!< bcdHID */
    USBD_HID_COUNTRY_CODE,                      /*!< bCountryCode */
    0x01,                                       /*!< bNumDescriptors */
    0x22,                                       /*!< bDescriptorType */
    WBVAL(LIGHTING_HID_REPORT_DESC_SIZE),       /*!< wItemLength */
/********************************************** Endpoint 0 Descriptor */
    0x07,                                       /*!< bLength */
    0x05,                                       /*!< bDescriptorType */
    USBD_IF2_AL0_EP0_ADDR,                      /*!< bEndpointAddress */
    0x03,                                       /*!< bmAttributes */
    WBVAL(USBD_IF2_AL0_EP0_SIZE),               /*!< wMaxPacketSize */
    USBD_IF2_AL0_EP0_INTERVAL,                  /*!< bInterval */
/********************************************** Language ID String Descriptor */
    0x04,                                       /*!< bLength */
    0x03,                                       /*!< bDescriptorType */
    WBVAL(USBD_LANGID_STRING),                  /*!< wLangID0 */
/********************************************** String 1 Descriptor */
/* CL_novel */
    0x12,                                       /*!< bLength */
    0x03,                                       /*!< bDescriptorType */
    0x43, 0x00,                                 /*!< 'C' wcChar0 */
    0x4c, 0x00,                                 /*!< 'L' wcChar1 */
    0x5f, 0x00,                                 /*!< '_' wcChar2 */
    0x6e, 0x00,                                 /*!< 'n' wcChar3 */
    0x6f, 0x00,                                 /*!< 'o' wcChar4 */
    0x76, 0x00,                                 /*!< 'v' wcChar5 */
    0x65, 0x00,                                 /*!< 'e' wcChar6 */
    0x6c, 0x00,                                 /*!< 'l' wcChar7 */
/********************************************** String 2 Descriptor */
/* TP78 */
    0x0a,                                       /*!< bLength */
    0x03,                                       /*!< bDescriptorType */
    0x54, 0x00,                                 /*!< 'T' wcChar0 */
    0x50, 0x00,                                 /*!< 'P' wcChar1 */
    0x37, 0x00,                                 /*!< '7' wcChar2 */
    0x38, 0x00,                                 /*!< '8' wcChar3 */
/********************************************** String 3 Descriptor */
/* v2.0 */
    0x0a,                                       /*!< bLength */
    0x03,                                       /*!< bDescriptorType */
    0x76, 0x00,                                 /*!< 'v' wcChar0 */
    0x32, 0x00,                                 /*!< '2' wcChar1 */
    0x2e, 0x00,                                 /*!< '.' wcChar2 */
    0x30, 0x00,                                 /*!< '0' wcChar3 */
    0x00
};

/*!< USBD HID REPORT 0 Descriptor - Keyboard + Mouse + Vol */
const unsigned char usbd_hid_0_report_descriptor[USBD_IF0_AL0_HID_REPORT_DESC_SIZE] = {
        /* Keyboard */
        0x05, 0x01, // USAGE_PAGE (Generic Desktop)
        0x09, 0x06, // USAGE (Keyboard)
        0xA1, 0x01, // COLLECTION (Application)

        0x85, RID_KEYBOARD, // Report ID
        0x05, 0x07, // USAGE_PAGE (Keyboard)
        0x19, 0xe0, // USAGE_MINIMUM (Keyboard LeftControl)
        0x29, 0xe7, // USAGE_MAXIMUM (Keyboard Right GUI)
        0x15, 0x00, // LOGICAL_MINIMUM (0)
        0x25, 0x01, // LOGICAL_MAXIMUM (1)
        0x75, 0x01, // REPORT_SIZE (1)
        0x95, 0x08, // REPORT_COUNT (8)
        0x81, 0x02, // INPUT (Data,Var,Abs)
        0x95, 0x01, // REPORT_COUNT (1)
        0x75, 0x08, // REPORT_SIZE (8)
        0x81, 0x03, // INPUT (Cnst,Var,Abs)
        0x95, 0x05, // REPORT_COUNT (5)
        0x75, 0x01, // REPORT_SIZE (1)
        0x05, 0x08, // USAGE_PAGE (LEDs)
        0x19, 0x01, // USAGE_MINIMUM (Num Lock)
        0x29, 0x05, // USAGE_MAXIMUM (Kana)
        0x91, 0x02, // OUTPUT (Data,Var,Abs)
        0x95, 0x01, // REPORT_COUNT (1)
        0x75, 0x03, // REPORT_SIZE (3)
        0x91, 0x03, // OUTPUT (Cnst,Var,Abs)
        0x95, 0x06, // REPORT_COUNT (6)
        0x75, 0x08, // REPORT_SIZE (8)
        0x15, 0x00, // LOGICAL_MINIMUM (0)
        0x25, 0xFF, // LOGICAL_MAXIMUM (255)
        0x05, 0x07, // USAGE_PAGE (Keyboard)
        0x19, 0x00, // USAGE_MINIMUM (Reserved (no event indicated))
        0x29, 0x65, // USAGE_MAXIMUM (Keyboard Application)
        0x81, 0x00, // INPUT (Data,Ary,Abs)
        0xC0,       // END_COLLECTION

        /* Mouse */
        0x05, 0x01, // USAGE_PAGE (Generic Desktop)
        0x09, 0x02, // USAGE (Mouse)
        0xA1, 0x01, // COLLECTION (Application)

        0x85, RID_MOUSE, // Report ID
        0x09, 0x01, // USAGE (Pointer)
        0xA1, 0x00, // COLLECTION (Physical)

        0x05, 0x09, // USAGE_PAGE (Button)
        0x19, 0x01, // USAGE_MINIMUM (Button 1)
        0x29, 0x03, // USAGE_MAXIMUM (Button 3)
        0x15, 0x00, // LOGICAL_MINIMUM (0)
        0x25, 0x01, // LOGICAL_MAXIMUM (1)
        0x95, 0x03, // REPORT_COUNT (3)
        0x75, 0x01, // REPORT_SIZE (1)
        0x81, 0x02, // INPUT (Data,Var,Abs)
        0x95, 0x01, // REPORT_COUNT (1)
        0x75, 0x05, // REPORT_SIZE (5)
        0x81, 0x01, // INPUT (Cnst,Var,Abs)
        0x05, 0x01, // USAGE_PAGE (Generic Desktop)
        0x09, 0x30, // USAGE (X)
        0x09, 0x31, // USAGE (Y)
        0x09, 0x38,
        0x15, 0x81, // LOGICAL_MINIMUM (-127)
        0x25, 0x7F, // LOGICAL_MAXIMUM (127)
        0x75, 0x08, // REPORT_SIZE (8)
        0x95, 0x03, // REPORT_COUNT (2)
        0x81, 0x06, // INPUT (Data,Var,Rel)
        0xC0,

        0x09, 0x3C,
        0x05, 0xFF,
        0x09, 0x01,
        0x15, 0x00,
        0x25, 0x01,
        0x75, 0x01,
        0x95, 0x02,
        0xB1, 0x22,
        0x75, 0x06,
        0x95, 0x01,
        0xB1, 0x01,
        0xC0, //   END_COLLECTION

        /* Vol */
        0x05, 0x0C,
        0x09, 0x01,
        0xA1, 0x01,

        0x85, RID_CONSUMER_CONTROL, // Report ID
        0x09, 0xB0,
        0x09, 0xB5,
        0x09, 0xB6,
        0x09, 0xE9,
        0x09, 0xEA,
        0x09, 0xE2,
        0x09, 0xB1,
        0x09, 0xB7,

        0x15, 0x00,
        0x25, 0x01,
        0x95, 0x08,
        0x75, 0x01,
        0x81, 0x02,
        0xC0,

        /* Dial */
        0x05, 0x01,
        0x09, 0x0E,
        0xA1, 0x01,

        0x85, RID_DIAL, // Report ID
        0x05, 0x0D,
        0x09, 0x21,
        0xA1, 0x00,

        0x05, 0x09, // USAGE_PAGE (Button)
        0x09, 0x01,
        0x95, 0x01, // REPORT_COUNT (1)
        0x75, 0x01, // REPORT_SIZE (1)
        0x15, 0x00, // LOGICAL_MINIMUM (0)
        0x25, 0x01, // LOGICAL_MAXIMUM (1)
        0x81, 0x02,
        0x05, 0x01,
        0x09, 0x37,
        0x95, 0x01,
        0x75, 0x0F,
        0x55, 0x0F,
        0x65, 0x14,
        0x36, 0xF0, 0xF1,
        0x46, 0x10, 0x0E,
        0x16, 0xF0, 0xF1,
        0x26, 0x10, 0x0E,
        0x81, 0x06,
        0xC0,
        0xC0,
};

/*!< USBD RAW HID REPORT Descriptor - Raw */
const unsigned char usbd_raw_hid_report_descriptor[RAW_HID_REPORT_DESC_SIZE] = {
        0x06, 0x60, 0xFF, // Vendor Defined
        0x09, 0x61, // Vendor Defined
        0xA1, 0x01, // Application

        0x09, 0x62,
        0x15, 0x00, // LOGICAL_MINIMUM (0)
        0x26, 0xFF, 0x00, // LOGICAL_MAXIMUM (0xFF)
        0x95, 0x20, // REPORT_COUNT(RAW_EPSIZE = 32)
        0x75, 0x08, // REPORT_SIZE
        0x81, 0x02,

        0x09, 0x63,
        0x15, 0x00, // LOGICAL_MINIMUM (0)
        0x26, 0xFF, 0x00, // LOGICAL_MAXIMUM (0xFF)
        0x95, 0x20, // REPORT_COUNT(RAW_EPSIZE = 32)
        0x75, 0x08, // REPORT_SIZE
        0x91, 0x02,

        0xC0
};

/*!< USBD LIGHTING HID REPORT Descriptor 1 */
const unsigned char usbd_lighting_hid_report_descriptor[LIGHTING_HID_REPORT_DESC_SIZE] = {
        0x05, 0x59,                      // UsagePage(Lighting And Illumination[0x0059])
        0x09, 0x01,                      // UsageId(LampArray[0x0001])
        0xA1, 0x01,                      // Collection(Application)
        0x85, RID_LIGHTING_LAMP_ARRAY_ATTRIBUTES,
                                         //     ReportId
        0x09, 0x02,                      //     UsageId(LampArrayAttributesReport[0x0002])
        0xA1, 0x02,                      //     Collection(Logical)
        0x09, 0x03,                      //         UsageId(LampCount[0x0003])
        0x15, 0x00,                      //         LogicalMinimum(0)
        0x27, 0xFF, 0xFF, 0x00, 0x00,    //         LogicalMaximum(65,535)
        0x95, 0x01,                      //         ReportCount(1)
        0x75, 0x10,                      //         ReportSize(16)
        0xB1, 0x03,                      //         Feature(Constant, Variable, Absolute, NoWrap, Linear, PreferredState, NoNullPosition, NonVolatile, BitField)
        0x09, 0x04,                      //         UsageId(BoundingBoxWidthInMicrometers[0x0004])
        0x09, 0x05,                      //         UsageId(BoundingBoxHeightInMicrometers[0x0005])
        0x09, 0x06,                      //         UsageId(BoundingBoxDepthInMicrometers[0x0006])
        0x09, 0x07,                      //         UsageId(LampArrayKind[0x0007])
        0x09, 0x08,                      //         UsageId(MinUpdateIntervalInMicroseconds[0x0008])
        0x27, 0xFF, 0xFF, 0xFF, 0x7F,    //         LogicalMaximum(2,147,483,647)
        0x95, 0x05,                      //         ReportCount(5)
        0x75, 0x20,                      //         ReportSize(32)
        0xB1, 0x03,                      //         Feature(Constant, Variable, Absolute, NoWrap, Linear, PreferredState, NoNullPosition, NonVolatile, BitField)
        0xC0,                            //     EndCollection()
        0x85, RID_LIGHTING_LAMP_ATTRIBUTES_REQUEST,
                                         //     ReportId
        0x09, 0x20,                      //     UsageId(LampAttributesRequestReport[0x0020])
        0xA1, 0x02,                      //     Collection(Logical)
        0x09, 0x21,                      //         UsageId(LampId[0x0021])
        0x27, 0xFF, 0xFF, 0x00, 0x00,    //         LogicalMaximum(65,535)
        0x95, 0x01,                      //         ReportCount(1)
        0x75, 0x10,                      //         ReportSize(16)
        0xB1, 0x02,                      //         Feature(Data, Variable, Absolute, NoWrap, Linear, PreferredState, NoNullPosition, NonVolatile, BitField)
        0xC0,                            //     EndCollection()
        0x85, RID_LIGHTING_LAMP_ATTRIBUTES_RESPONSE,
                                         //     ReportId
        0x09, 0x22,                      //     UsageId(LampAttributesResponseReport[0x0022])
        0xA1, 0x02,                      //     Collection(Logical)
        0x09, 0x21,                      //         UsageId(LampId[0x0021])
        0xB1, 0x02,                      //         Feature(Data, Variable, Absolute, NoWrap, Linear, PreferredState, NoNullPosition, NonVolatile, BitField)
        0x09, 0x23,                      //         UsageId(PositionXInMicrometers[0x0023])
        0x09, 0x24,                      //         UsageId(PositionYInMicrometers[0x0024])
        0x09, 0x25,                      //         UsageId(PositionZInMicrometers[0x0025])
        0x09, 0x27,                      //         UsageId(UpdateLatencyInMicroseconds[0x0027])
        0x09, 0x26,                      //         UsageId(LampPurposes[0x0026])
        0x27, 0xFF, 0xFF, 0xFF, 0x7F,    //         LogicalMaximum(2,147,483,647)
        0x95, 0x05,                      //         ReportCount(5)
        0x75, 0x20,                      //         ReportSize(32)
        0xB1, 0x02,                      //         Feature(Data, Variable, Absolute, NoWrap, Linear, PreferredState, NoNullPosition, NonVolatile, BitField)
        0x09, 0x28,                      //         UsageId(RedLevelCount[0x0028])
        0x09, 0x29,                      //         UsageId(GreenLevelCount[0x0029])
        0x09, 0x2A,                      //         UsageId(BlueLevelCount[0x002A])
        0x09, 0x2B,                      //         UsageId(IntensityLevelCount[0x002B])
        0x09, 0x2C,                      //         UsageId(IsProgrammable[0x002C])
        0x09, 0x2D,                      //         UsageId(InputBinding[0x002D])
        0x26, 0xFF, 0x00,                //         LogicalMaximum(255)
        0x95, 0x06,                      //         ReportCount(6)
        0x75, 0x08,                      //         ReportSize(8)
        0xB1, 0x02,                      //         Feature(Data, Variable, Absolute, NoWrap, Linear, PreferredState, NoNullPosition, NonVolatile, BitField)
        0xC0,                            //     EndCollection()
//        0x85, RID_LIGHTING_LAMP_MULTI_UPDATE,
//                                         //     ReportId
//        0x09, 0x50,                      //     UsageId(LampMultiUpdateReport[0x0050])
//        0xA1, 0x02,                      //     Collection(Logical)
//        0x09, 0x03,                      //         UsageId(LampCount[0x0003])
//        0x25, 0x08,                      //         LogicalMaximum(8)
//        0x95, 0x01,                      //         ReportCount(1)
//        0xB1, 0x02,                      //         Feature(Data, Variable, Absolute, NoWrap, Linear, PreferredState, NoNullPosition, NonVolatile, BitField)
//        0x09, 0x55,                      //         UsageId(LampUpdateFlags[0x0055])
//        0x25, 0x01,                      //         LogicalMaximum(1)
//        0xB1, 0x02,                      //         Feature(Data, Variable, Absolute, NoWrap, Linear, PreferredState, NoNullPosition, NonVolatile, BitField)
//        0x09, 0x21,                      //         UsageId(LampId[0x0021])
//        0x27, 0xFF, 0xFF, 0x00, 0x00,    //         LogicalMaximum(65,535)
//        0x95, 0x08,                      //         ReportCount(8)
//        0x75, 0x10,                      //         ReportSize(16)
//        0xB1, 0x02,                      //         Feature(Data, Variable, Absolute, NoWrap, Linear, PreferredState, NoNullPosition, NonVolatile, BitField)
//        0x09, 0x51,                      //         UsageId(RedUpdateChannel[0x0051])
//        0x09, 0x52,                      //         UsageId(GreenUpdateChannel[0x0052])
//        0x09, 0x53,                      //         UsageId(BlueUpdateChannel[0x0053])
//        0x09, 0x54,                      //         UsageId(IntensityUpdateChannel[0x0054])
//        0x09, 0x51,                      //         UsageId(RedUpdateChannel[0x0051])
//        0x09, 0x52,                      //         UsageId(GreenUpdateChannel[0x0052])
//        0x09, 0x53,                      //         UsageId(BlueUpdateChannel[0x0053])
//        0x09, 0x54,                      //         UsageId(IntensityUpdateChannel[0x0054])
//        0x09, 0x51,                      //         UsageId(RedUpdateChannel[0x0051])
//        0x09, 0x52,                      //         UsageId(GreenUpdateChannel[0x0052])
//        0x09, 0x53,                      //         UsageId(BlueUpdateChannel[0x0053])
//        0x09, 0x54,                      //         UsageId(IntensityUpdateChannel[0x0054])
//        0x09, 0x51,                      //         UsageId(RedUpdateChannel[0x0051])
//        0x09, 0x52,                      //         UsageId(GreenUpdateChannel[0x0052])
//        0x09, 0x53,                      //         UsageId(BlueUpdateChannel[0x0053])
//        0x09, 0x54,                      //         UsageId(IntensityUpdateChannel[0x0054])
//        0x09, 0x51,                      //         UsageId(RedUpdateChannel[0x0051])
//        0x09, 0x52,                      //         UsageId(GreenUpdateChannel[0x0052])
//        0x09, 0x53,                      //         UsageId(BlueUpdateChannel[0x0053])
//        0x09, 0x54,                      //         UsageId(IntensityUpdateChannel[0x0054])
//        0x09, 0x51,                      //         UsageId(RedUpdateChannel[0x0051])
//        0x09, 0x52,                      //         UsageId(GreenUpdateChannel[0x0052])
//        0x09, 0x53,                      //         UsageId(BlueUpdateChannel[0x0053])
//        0x09, 0x54,                      //         UsageId(IntensityUpdateChannel[0x0054])
//        0x09, 0x51,                      //         UsageId(RedUpdateChannel[0x0051])
//        0x09, 0x52,                      //         UsageId(GreenUpdateChannel[0x0052])
//        0x09, 0x53,                      //         UsageId(BlueUpdateChannel[0x0053])
//        0x09, 0x54,                      //         UsageId(IntensityUpdateChannel[0x0054])
//        0x09, 0x51,                      //         UsageId(RedUpdateChannel[0x0051])
//        0x09, 0x52,                      //         UsageId(GreenUpdateChannel[0x0052])
//        0x09, 0x53,                      //         UsageId(BlueUpdateChannel[0x0053])
//        0x09, 0x54,                      //         UsageId(IntensityUpdateChannel[0x0054])
//        0x26, 0xFF, 0x00,                //         LogicalMaximum(255)
//        0x95, 0x20,                      //         ReportCount(32)
//        0x75, 0x08,                      //         ReportSize(8)
//        0xB1, 0x02,                      //         Feature(Data, Variable, Absolute, NoWrap, Linear, PreferredState, NoNullPosition, NonVolatile, BitField)
//        0xC0,                            //     EndCollection()

        /////
        0x85, RID_LIGHTING_LAMP_MULTI_UPDATE,
                                         //     ReportId
        0x09, 0x50,                      //     UsageId(LampMultiUpdateReport[0x0050])
        0xA1, 0x02,                      //     Collection(Logical)
        0x09, 0x03,                      //         UsageId(LampCount[0x0003])
        0x25, 0x08,                      //         LogicalMaximum(8)
        0x95, 0x01,                      //         ReportCount(1)
        0xB1, 0x02,                      //         Feature(Data, Variable, Absolute, NoWrap, Linear, PreferredState, NoNullPosition, NonVolatile, BitField)
        0x09, 0x55,                      //         UsageId(LampUpdateFlags[0x0055])
        0x25, 0x01,                      //         LogicalMaximum(1)
        0xB1, 0x02,                      //         Feature(Data, Variable, Absolute, NoWrap, Linear, PreferredState, NoNullPosition, NonVolatile, BitField)
        0x09, 0x21,                      //         UsageId(LampId[0x0021])
        0x27, 0xFF, 0xFF, 0x00, 0x00,    //         LogicalMaximum(65,535)
        0x95, 0x08,                      //         ReportCount(8)
        0x75, 0x10,                      //         ReportSize(16)
        0xB1, 0x02,                      //         Feature(Data, Variable, Absolute, NoWrap, Linear, PreferredState, NoNullPosition, NonVolatile, BitField)
        0x09, 0x51,                      //         UsageId(RedUpdateChannel[0x0051])
        0x09, 0x52,                      //         UsageId(GreenUpdateChannel[0x0052])
        0x09, 0x53,                      //         UsageId(BlueUpdateChannel[0x0053])
        0x09, 0x54,                      //         UsageId(IntensityUpdateChannel[0x0054])
        0x26, 0xFF, 0x00,                //         LogicalMaximum(255)
        0x95, 0x20,                      //         ReportCount(32)
        0x75, 0x08,                      //         ReportSize(8)
        0xB1, 0x02,                      //         Feature(Data, Variable, Absolute, NoWrap, Linear, PreferredState, NoNullPosition, NonVolatile, BitField)
        0xC0,                            //     EndCollection()
        /////
        0x85, RID_LIGHTING_LAMP_RANGE_UPDATE,                      //     ReportId
        0x09, 0x60,                      //     UsageId(LampRangeUpdateReport[0x0060])
        0xA1, 0x02,                      //     Collection(Logical)
        0x09, 0x55,                      //         UsageId(LampUpdateFlags[0x0055])
        0x25, 0x01,                      //         LogicalMaximum(1)
        0x95, 0x01,                      //         ReportCount(1)
        0xB1, 0x02,                      //         Feature(Data, Variable, Absolute, NoWrap, Linear, PreferredState, NoNullPosition, NonVolatile, BitField)
        0x09, 0x61,                      //         UsageId(LampIdStart[0x0061])
        0x09, 0x62,                      //         UsageId(LampIdEnd[0x0062])
        0x27, 0xFF, 0xFF, 0x00, 0x00,    //         LogicalMaximum(65,535)
        0x95, 0x02,                      //         ReportCount(2)
        0x75, 0x10,                      //         ReportSize(16)
        0xB1, 0x02,                      //         Feature(Data, Variable, Absolute, NoWrap, Linear, PreferredState, NoNullPosition, NonVolatile, BitField)
        0x09, 0x51,                      //         UsageId(RedUpdateChannel[0x0051])
        0x09, 0x52,                      //         UsageId(GreenUpdateChannel[0x0052])
        0x09, 0x53,                      //         UsageId(BlueUpdateChannel[0x0053])
        0x09, 0x54,                      //         UsageId(IntensityUpdateChannel[0x0054])
        0x26, 0xFF, 0x00,                //         LogicalMaximum(255)
        0x95, 0x04,                      //         ReportCount(4)
        0x75, 0x08,                      //         ReportSize(8)
        0xB1, 0x02,                      //         Feature(Data, Variable, Absolute, NoWrap, Linear, PreferredState, NoNullPosition, NonVolatile, BitField)
        0xC0,                            //     EndCollection()
        0x85, RID_LIGHTING_LAMP_ARRAY_CONTROL,                      //     ReportId
        0x09, 0x70,                      //     UsageId(LampArrayControlReport[0x0070])
        0xA1, 0x02,                      //     Collection(Logical)
        0x09, 0x71,                      //         UsageId(AutonomousMode[0x0071])
        0x25, 0x01,                      //         LogicalMaximum(1)
        0x95, 0x01,                      //         ReportCount(1)
        0xB1, 0x02,                      //         Feature(Data, Variable, Absolute, NoWrap, Linear, PreferredState, NoNullPosition, NonVolatile, BitField)
        0xC0,                            //     EndCollection()
        0xC0,
};

#define HID_STATE_IDLE 0
#define HID_STATE_BUSY 1

/*!< hid state ! Data can be sent only when state is idle  */
static uint8_t hid_state = HID_STATE_IDLE;

// Task id
tmosTaskID usbTaskID = INVALID_TASK_ID;

/* function ------------------------------------------------------------------*/
static void usbd_hid_kmvd_in_callback(uint8_t ep, uint32_t nbytes)
{
    hid_state = HID_STATE_IDLE;
}

USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t raw_buffer[32] = { 0 };

static void usbd_hid_raw_in_callback(uint8_t ep, uint32_t nbytes)
{

}

/* support via */
static void usbd_hid_raw_out_callback(uint8_t ep, uint32_t nbytes)
{
  via_data_processing(raw_buffer, 32);
  usbd_ep_start_write(USBD_RAW_IF1_AL0_EP0_ADDR, raw_buffer, sizeof(raw_buffer));
  usbd_ep_start_read(USBD_RAW_IF1_AL0_EP1_ADDR, raw_buffer, sizeof(raw_buffer));
}

static void usbd_hid_lighting_in_callback(uint8_t ep, uint32_t nbytes)
{

}

/*!< endpoint call back */
static struct usbd_endpoint hid_kmvd_in_ep = {
    .ep_cb = usbd_hid_kmvd_in_callback,
    .ep_addr = USBD_IF0_AL0_EP0_ADDR
};

static struct usbd_endpoint hid_raw_in_ep = {
    .ep_cb = usbd_hid_raw_in_callback,
    .ep_addr = USBD_RAW_IF1_AL0_EP0_ADDR
};

static struct usbd_endpoint hid_raw_out_ep = {
    .ep_cb = usbd_hid_raw_out_callback,
    .ep_addr = USBD_RAW_IF1_AL0_EP1_ADDR
};

static struct usbd_endpoint hid_lighting_1_in_ep = {
    .ep_cb = usbd_hid_lighting_in_callback,
    .ep_addr = USBD_IF2_AL0_EP0_ADDR
};

struct usbd_interface intf0;
struct usbd_interface intf1;
struct usbd_interface intf2;

static BOOL u_disk_mode = FALSE;
BOOL usb_host_suspend_flag = FALSE;

void usb_device_init()
{
    if (u_disk_mode) usbd_desc_register(usbd_descriptor);
    else usbd_desc_register(usbd_descriptor_no_msc);
    usbd_add_interface(usbd_hid_init_intf(&intf0, usbd_hid_0_report_descriptor, USBD_IF0_AL0_HID_REPORT_DESC_SIZE));
    usbd_add_endpoint(&hid_kmvd_in_ep);
    if (u_disk_mode) usbd_add_interface(usbd_msc_init_intf(&intf1, USBD_IF1_AL0_EP0_ADDR, USBD_IF1_AL0_EP1_ADDR));
    else {  // support raw
      usbd_add_interface(usbd_hid_init_intf(&intf1, usbd_raw_hid_report_descriptor, RAW_HID_REPORT_DESC_SIZE));
      usbd_add_endpoint(&hid_raw_in_ep);
      usbd_add_endpoint(&hid_raw_out_ep);
    }
    usbd_add_interface(usbd_hid_init_intf(&intf2, usbd_lighting_hid_report_descriptor, LIGHTING_HID_REPORT_DESC_SIZE));
    usbd_add_endpoint(&hid_lighting_1_in_ep);

    usbd_initialize();
}

/*--------------------------- define for hid ---------------------------*/

void usb_suspend_wake_up_cb(uint8_t type)
{
  if (type) { // wake up
    g_Ready_Status.usb = TRUE;
  } else {
    g_Ready_Status.usb = FALSE;
  }
}

void usbd_configure_done_callback(void)
{
  if (u_disk_mode == FALSE) {
    usbd_ep_start_read(USBD_RAW_IF1_AL0_EP1_ADDR, raw_buffer, sizeof(raw_buffer));
  }
  usb_suspend_wake_up_cb(1);
}

void usbh_hid_set_idle(uint8_t intf, uint8_t report_id, uint8_t duration)
{

}

void usbh_hid_set_protocol(uint8_t intf, uint8_t protocol)
{

}

void usbh_hid_set_report(uint8_t intf, uint8_t report_id, uint8_t report_type, uint8_t *report, uint8_t report_len)
{
    if (intf == 0) {
      switch (report[0]) {
        /* LED data output */
        case RID_KEYBOARD:
          g_NumLock_LEDOn_Status.usb = (report[1] & 0x1) ? TRUE : FALSE;
          g_CapsLock_LEDOn_Status.usb = (report[1] & 0x2) ? TRUE : FALSE;
          TPM_notify_led_data(report[1]);
          break;
        default: {
            break;
        }
      }
    } else if (intf == 2) {
      switch (report[0]) {
        case RID_LIGHTING_LAMP_ATTRIBUTES_REQUEST: {
            SetLampAttributesId(&report[1]);
            break;
        }
        case RID_LIGHTING_LAMP_MULTI_UPDATE: {
            SetMultipleLamps(&report[1]);
            break;
        }
        case RID_LIGHTING_LAMP_RANGE_UPDATE: {
            SetLampRange(&report[1]);
            break;
        }
        case RID_LIGHTING_LAMP_ARRAY_CONTROL: {
            SetAutonomousMode(&report[1]);
            break;
        }
        default: {
            break;
        }
      }
    }
}

uint8_t usbh_hid_get_report(uint8_t intf, uint8_t report_id, uint8_t report_type, uint8_t *report, uint8_t report_len)
{
  if (intf == 2) {
    switch (report[0]) {
      case RID_LIGHTING_LAMP_ARRAY_ATTRIBUTES: {
          return GetLampArrayAttributesReport(&report[1]) + 1;
          break;
      }
      case RID_LIGHTING_LAMP_ATTRIBUTES_RESPONSE: {
          return GetLampAttributesReport(&report[1]) + 1;
          break;
      }
      default: {
          break;
      }
    }
  }

  return 1;
}

/*--------------------------- define for msc ---------------------------*/

#include "fatfs_usbd.h"

#define BLOCK_SIZE    512
#define BLOCK_COUNT   60  // reserved 2k

void usbd_msc_get_cap(uint8_t lun, uint32_t *block_num, uint16_t *block_size)
{
    *block_num = BLOCK_COUNT; //Pretend having so many buffer,not has actually.
    *block_size = BLOCK_SIZE;
}

int usbd_msc_sector_read(uint32_t sector, uint8_t *buffer, uint32_t length)
{
//    if (sector < 64) {
    USB_disk_read(buffer, sector, length);
//    }
    return 0;
}

int usbd_msc_sector_write(uint32_t sector, uint8_t *buffer, uint32_t length)
{
//    if (sector < 64) {
    USB_disk_write(buffer, sector, length);
//    }
    return 0;
}

#if 0

void hid_mouse_test(void)
{
    HIDMouse[1] = HIDMouse[2] = 5;

    while (hid_state.mouse_hid_state == HID_STATE_BUSY) {}
    int ret = usbd_ep_start_write(USBD_IF1_AL0_EP0_ADDR, HIDMouse, HID_MOUSE_DATA_LENGTH);
    if (ret >= 0) {
        hid_state.mouse_hid_state = HID_STATE_BUSY;
    }
}

void hid_keyboard_test(void)
{
    HIDKeyboard[2] = HID_KBD_USAGE_A;

    while (hid_state.keyboard_hid_state == HID_STATE_BUSY) {}
    int ret = usbd_ep_start_write(USBD_IF0_AL0_EP0_ADDR, HIDKeyboard, HID_KEYBOARD_DATA_LENGTH);
    if (ret >= 0) {
        hid_state.keyboard_hid_state = HID_STATE_BUSY;
    }

    while (hid_state.keyboard_hid_state == HID_STATE_BUSY) {}
    DelayMs(30);
    HIDKeyboard[2] = 0;
    ret = usbd_ep_start_write(USBD_IF0_AL0_EP0_ADDR, HIDKeyboard, HID_KEYBOARD_DATA_LENGTH);
    if (ret >= 0) {
        hid_state.keyboard_hid_state = HID_STATE_BUSY;
    }
}
#endif

/*******************************************************************************
 * Function Name  : USB_ProcessTMOSMsg
 * Description    : Process an incoming task message.
 * Input          : pMsg - message to process
 * Return         : tmosEvents
 *******************************************************************************/
static void USB_ProcessTMOSMsg( tmos_event_hdr_t *pMsg )
{
  if (usb_host_suspend_flag) {
    USB_DevWakeup();
    usb_host_suspend_flag = FALSE;
  }
  switch ( pMsg->event )
  {
    case KEY_MESSAGE: {
        SendMSG_t *msg = (SendMSG_t *) pMsg;
        msg->hdr.status ? tmos_set_event( usbTaskID, USB_KEYBOARD_EVENT ) : 0;
        break;
    }
    case MOUSE_MESSAGE: {
        SendMSG_t *msg = (SendMSG_t *) pMsg;
        msg->hdr.status ? tmos_set_event( usbTaskID, USB_MOUSE_EVENT ) : 0;
        break;
    }
    case VOL_MESSAGE: {
        SendMSG_t *msg = (SendMSG_t *) pMsg;
        msg->hdr.status ? tmos_set_event( usbTaskID, USB_VOL_EVENT ) : 0;
        break;
    }
    case SWITCH_MESSAGE: {
        SendMSG_t *msg = (SendMSG_t *) pMsg;
        msg->hdr.status ? tmos_set_event( usbTaskID, USB_SWITCH_EVENT ) : 0;
        break;
    }
    default:
      break;
  }
}

/*******************************************************************************
 * Function Name  : USB_DevWakeup
 * Description    : USB唤醒主机
 * Input          : None
 * Return         : None
 *******************************************************************************/
void USB_DevWakeup(void)
{
    R16_PIN_ANALOG_IE &= ~RB_PIN_USB_DP_PU;
    R8_UDEV_CTRL |= RB_UD_LOW_SPEED;
    mDelaymS(2);
    R16_PIN_ANALOG_IE |= RB_PIN_USB_DP_PU;
    R8_UDEV_CTRL &= ~RB_UD_LOW_SPEED;
}

/*******************************************************************************
 * Function Name  : USB_ProcessEvent
 * Description    : USB处理事件
 * Input          : task_id： 任务id, events: USB事件
 * Return         : tmosEvents
 *******************************************************************************/
tmosEvents USB_ProcessEvent( tmosTaskID task_id, tmosEvents events )
{
  int ret;

  if ( events & SYS_EVENT_MSG )
  {
    uint8 *pMsg;

    if ( (pMsg = tmos_msg_receive( usbTaskID )) != NULL )
    {
      USB_ProcessTMOSMsg( (tmos_event_hdr_t *)pMsg );

      // Release the TMOS message
      tmos_msg_deallocate( pMsg );
    }

    return events ^ SYS_EVENT_MSG;
  }

  if ( events & USB_KEYBOARD_EVENT )
  {
    if (hid_state != HID_STATE_BUSY) {
      hid_state = HID_STATE_BUSY;
      ret = usbd_ep_start_write(USBD_IF0_AL0_EP0_ADDR, HIDKeyboard - 1, HID_KEYBOARD_DATA_LENGTH + 1);
      if (ret < 0) {
          hid_state = HID_STATE_IDLE;
      }
    }
    return events ^ USB_KEYBOARD_EVENT;
  }

  if ( events & USB_MOUSE_EVENT )
  {
    if (hid_state != HID_STATE_BUSY) {
      hid_state = HID_STATE_BUSY;
      ret = usbd_ep_start_write(USBD_IF0_AL0_EP0_ADDR, HIDMouse - 1, HID_MOUSE_DATA_LENGTH + 1);
      if (ret < 0) {
          hid_state = HID_STATE_IDLE;
      }
    }
    return events ^ USB_MOUSE_EVENT;
  }

  if ( events & USB_VOL_EVENT )
  {
    if (hid_state != HID_STATE_BUSY) {
      hid_state = HID_STATE_BUSY;
      ret = usbd_ep_start_write(USBD_IF0_AL0_EP0_ADDR, HIDVolume - 1, HID_VOLUME_DATA_LENGTH + 1);
      if (ret < 0) {
          hid_state = HID_STATE_IDLE;
      }
    }
    return events ^ USB_VOL_EVENT;
  }

  if ( events & USB_SWITCH_EVENT )
  {
    if (hid_state != HID_STATE_BUSY) {
      hid_state = HID_STATE_BUSY;
      ret = usbd_ep_start_write(USBD_IF0_AL0_EP0_ADDR, HIDSwitch - 1, HID_SWITCH_DATA_LENGTH + 1);
      if (ret < 0) {
          hid_state = HID_STATE_IDLE;
      }
    }
    return events ^ USB_SWITCH_EVENT;
  }

#if DEBUG
  if ( events & USB_TEST_EVENT )
  {
#if 0
    hid_keyboard_test();
    hid_mouse_test();
#endif
    tmos_start_task(usbTaskID, USB_TEST_EVENT, MS1_TO_SYSTEM_TIME(500));
    return events ^ USB_TEST_EVENT;
  }
#endif

  return 0;
}

/*******************************************************************************
 * Function Name  : HAL_USBInit
 * Description    : USB初始化
 * Input          : None
 * Return         : None
 *******************************************************************************/
void HAL_USBInit( void )
{
  uint16_t Udisk_mode = 0;

  usbTaskID = TMOS_ProcessEventRegister( USB_ProcessEvent );

  /* usb device init */
#ifdef FIRST_USED
  u_disk_mode = TRUE;
  usb_device_init();
#else
  HAL_Fs_Read_keyboard_cfg(FS_LINE_UDISK_MODE, 1, &Udisk_mode);
  if (Udisk_mode) {
    Udisk_mode = 0;
    HAL_Fs_Write_keyboard_cfg(FS_LINE_UDISK_MODE, 1, &Udisk_mode);
    u_disk_mode = TRUE;
  } else {
    u_disk_mode = FALSE;
  }
  usb_device_init();
#endif
  while (!usb_device_is_configured());
}

void usb_dc_low_level_init(void)
{
  extern void USB_IRQHandler(void);
  PFIC_EnableIRQ(USB_IRQn);
  PFIC_EnableFastINT0(USB_IRQn, (uint32_t)(void *)USB_IRQHandler);
}

void usb_hc_low_level_init(void)
{
//  PFIC_EnableIRQ(USB2_IRQn);
}

int usb_dc_deinit(void)
{
    return 0;
}
