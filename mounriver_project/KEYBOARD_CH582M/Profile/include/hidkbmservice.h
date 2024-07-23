/********************************** (C) COPYRIGHT *******************************
 * File Name          : hidkbmservice.h
 * Author             : ChnMasterOG, WCH
 * Version            : V1.0
 * Date               : 2021/12/4
 * Description        : 键鼠服务头文件
 * Copyright (c) 2023 ChnMasterOG
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#ifndef HIDKBMSERVICE_H
#define HIDKBMSERVICE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "CONFIG.h"

/*********************************************************************
 * CONSTANTS
 */

// Number of HID reports defined in the service
#define HID_NUM_REPORTS          9

// HID Report IDs for the service
#define HID_RPT_ID_MOUSE_IN      1  // Mouse input report ID
#define HID_RPT_ID_VOL_IN        2  // Volume report ID
#define HID_RPT_ID_DIAL_IN       3  // Dial input report ID
#define HID_RPT_ID_KEY_IN        4  // Keyboard input report ID
#define HID_RPT_ID_LED_OUT       4  // LED output report ID
#define HID_RPT_ID_FEATURE       4  // Feature report ID

// HID feature flags
#define HID_FEATURE_FLAGS             HID_FLAGS_REMOTE_WAKE

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * Profile Callbacks
 */


/*********************************************************************
 * API FUNCTIONS
 */

/*********************************************************************
 * @fn      Hid_AddService
 *
 * @brief   Initializes the HID service for keyboard by registering
 *          GATT attributes with the GATT server.
 *
 * @param   none
 *
 * @return  Success or Failure
 */
extern bStatus_t Hid_AddService(void);

/*********************************************************************
 * @fn      Hid_SetParameter
 *
 * @brief   Set a HID Kbd parameter.
 *
 * @param   id     - HID report ID.
 * @param   type   - HID report type.
 * @param   uuid   - attribute uuid.
 * @param   len    - length of data to right.
 * @param   pValue - pointer to data to write.  This is dependent on
 *          the input parameters and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 *
 * @return  GATT status code.
 */
extern uint8 Hid_SetParameter(uint8 id, uint8 type, uint16 uuid, uint8 len, 
                                 void *pValue);

/*********************************************************************
 * @fn      Hid_GetParameter
 *
 * @brief   Get a HID Kbd parameter.
 *
 * @param   id     - HID report ID.
 * @param   type   - HID report type.
 * @param   uuid   - attribute uuid.
 * @param   pLen   - length of data to be read.
 * @param   pValue - pointer to data to get.  This is dependent on
 *          the input parameters and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 *
 * @return  GATT status code.
 */
extern uint8 Hid_GetParameter(uint8 id, uint8 type, uint16 uuid, uint16 *pLen, void *pValue);


/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* HIDKBDSERVICE_H */
