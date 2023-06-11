/********************************** (C) COPYRIGHT *******************************
 * File Name          : BLE.c
 * Author             : ChnMasterOG, WCH
 * Version            : V1.3
 * Date               : 2022/2/26
 * Description        : 蓝牙键鼠应用程序，初始化广播连接参数，然后广播，直至连接主机
 * Ref                : https://software-dl.ti.com/lprf/simplelink_cc2640r2_latest/docs/blestack/ble_user_guide/html/ble-stack-3.x-guide/index.html
 * Copyright (c) 2023 ChnMasterOG
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/


/*********************************************************************
 * INCLUDES
 */
#include "HAL.h"
#include "CONFIG.h"
#include "CH58x_common.h"
#include "devinfoservice.h"
#include "battservice.h"
#include "hiddev.h"
#include "hidkbmservice.h"

/*********************************************************************
 * MACROS
 */
// Selected HID mouse button values
#define MOUSE_BUTTON_NONE           0x00

// HID mouse input report length
#define HID_MOUSE_IN_RPT_LEN        HID_MOUSE_DATA_LENGTH

// HID keyboard input report length
#define HID_KEYBOARD_IN_RPT_LEN     HID_KEYBOARD_DATA_LENGTH

// HID LED output report length
#define HID_LED_OUT_RPT_LEN         1

// HID VOL input report length
#define HID_VOL_IN_RPT_LEN          2 //HID_VOLUME_DATA_LENGTH

/*********************************************************************
 * CONSTANTS
 */
// Param update delay
#define START_PARAM_UPDATE_EVT_DELAY          12800

// Param update delay
#define START_PHY_UPDATE_DELAY                1600

// HID idle timeout in msec; set to zero to disable timeout
#if (defined (BLE_BOND_KEYBOARD)) && (BLE_BOND_KEYBOARD == TRUE)
#define DEFAULT_HID_IDLE_TIMEOUT              0
#else
#define DEFAULT_HID_IDLE_TIMEOUT              60000
#endif

// Minimum connection interval (units of 1.25ms)
#define DEFAULT_DESIRED_MIN_CONN_INTERVAL     8

// Maximum connection interval (units of 1.25ms)
#define DEFAULT_DESIRED_MAX_CONN_INTERVAL     8

// Slave latency to use if parameter update request
#define DEFAULT_DESIRED_SLAVE_LATENCY         0

// Supervision timeout value (units of 10ms)
#define DEFAULT_DESIRED_CONN_TIMEOUT          500  // 5s

// Default passcode
#define DEFAULT_PASSCODE                      0

// Default GAP pairing mode
#define DEFAULT_PAIRING_MODE                  GAPBOND_PAIRING_MODE_WAIT_FOR_REQ

// Default MITM mode (TRUE to require passcode or OOB when pairing)
#if (defined (BLE_BOND_KEYBOARD)) && (BLE_BOND_KEYBOARD == TRUE)
#define DEFAULT_MITM_MODE                     TRUE
#else
#define DEFAULT_MITM_MODE                     FALSE
#endif

// Default bonding mode, TRUE to bond
#define DEFAULT_BONDING_MODE                  TRUE

// Default GAP bonding I/O capabilities
#if (defined (BLE_BOND_KEYBOARD)) && (BLE_BOND_KEYBOARD == TRUE)
#define DEFAULT_IO_CAPABILITIES               GAPBOND_IO_CAP_KEYBOARD_ONLY
#else
#define DEFAULT_IO_CAPABILITIES               GAPBOND_IO_CAP_NO_INPUT_NO_OUTPUT
#endif

// Battery level is critical when it is less than this %
#define DEFAULT_BATT_CRITICAL_LEVEL           10

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

// Volume code
const uint8_t Volume_to_Code[8] = {
    0xB0,   // Volume_Play
    0xB5,   // Volume_Next
    0xB6,   // Volume_Prev
    0xE9,   // Volume_Incr
    0xEA,   // Volume_Decr
    0xE2,   // Volume_Mute
    0xB1,   // Volume_Pause
    0xB7,   // Volume_Stop
};

// Task ID
tmosTaskID hidEmuTaskId = INVALID_TASK_ID;

// BLE device address
uint8_t DeviceAddress[6] = {0x84, 0xC2, 0xE5, 0x78, 0x73, 0x01};  // DeviceAddress[5] = 1 ~ 4

// Enter Passkey flag
BOOL EnterPasskey_flag = FALSE;

// BLE Pair Passkey context
uint32_t BLE_Passkey = 0;

// Select host device index (0 ~ BLE_DEVICE_NUM-1)
static uint8_t BLE_SelectHostIndex = 0;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

// GAP Profile - Name attribute for SCAN RSP data 扫描应答数据
static uint8 scanRspData[] =
{
  0x07,                             // length of this data
  GAP_ADTYPE_LOCAL_NAME_COMPLETE,   // AD Type = Complete local name
  'T',
  'P',
  '7',
  '8',
  'v',
  '2',
  0x05,   // length of this data
  GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE, //连接间隔
  LO_UINT16( DEFAULT_DESIRED_MIN_CONN_INTERVAL ),
  HI_UINT16( DEFAULT_DESIRED_MIN_CONN_INTERVAL ),
  LO_UINT16( DEFAULT_DESIRED_MAX_CONN_INTERVAL ),
  HI_UINT16( DEFAULT_DESIRED_MAX_CONN_INTERVAL ),

  // service UUIDs
  0x05,   // length of this data
  GAP_ADTYPE_16BIT_MORE,
  LO_UINT16(HID_SERV_UUID),
  HI_UINT16(HID_SERV_UUID),
  LO_UINT16(BATT_SERV_UUID),
  HI_UINT16(BATT_SERV_UUID),

  // Tx power level
  0x02,   // length of this data
  GAP_ADTYPE_POWER_LEVEL,//发送功率
  0       // 0dBm
};

// Advertising data
static uint8 advertData[] =
{
  // flags
  0x02,   // 
  GAP_ADTYPE_FLAGS, //广播标志数据，让主机知道从机是持续广播还是只广播一段时间
  GAP_ADTYPE_FLAGS_LIMITED | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,

  // appearance
  0x03,   // length of this data
  GAP_ADTYPE_APPEARANCE,
  LO_UINT16(GAP_APPEARE_GENERIC_HID),
  HI_UINT16(GAP_APPEARE_GENERIC_HID), //通用HID设备(键盘+鼠标)
};

// Device name attribute value
static CONST uint8 attDeviceName[GAP_DEVICE_NAME_LEN] = "TP78v2";

// HID Dev configuration
static hidDevCfg_t hidEmuCfg =
{
  DEFAULT_HID_IDLE_TIMEOUT,   // Idle timeout
  HID_FEATURE_FLAGS           // HID feature flags
};

static uint16 hidEmuConnHandle = GAP_CONNHANDLE_INIT;

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static void hidEmu_ProcessTMOSMsg( tmos_event_hdr_t *pMsg );
static void hidEmuSendMouseReport( uint8 buttons ,uint8 X_data ,uint8 Y_data );
static void hidEmuSendKbdReport( uint8 keycode );
static uint8 hidEmuRcvReport( uint8 len, uint8 *pData );
static uint8 hidEmuRptCB( uint8 id, uint8 type, uint16 uuid,
                             uint8 oper, uint16 *pLen, uint8 *pData );
static void hidEmuEvtCB( uint8 evt );
static void hidEmuStateCB( gapRole_States_t newState , gapRoleEvent_t * pEvent );

/*********************************************************************
 * PROFILE CALLBACKS
 */

static hidDevCB_t hidEmuHidCBs =
{
  hidEmuRptCB,
  hidEmuEvtCB,
  NULL,
  hidEmuStateCB
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*******************************************************************************
* Function Name  : DATAFLASH_Read_DeviceID
* Description    : 从DataFlash读取设备ID
* Input          : None
* Return         : None
*******************************************************************************/
void DATAFLASH_Read_DeviceID( void )
{
  uint16_t DeviceID;
  HAL_Fs_Read_keyboard_cfg(FS_LINE_BLE_DEVICE, 1, &DeviceID);
  DeviceAddress[5] = DeviceID >= BLE_DEVICE_NUM ? 1 : DeviceID + 1;
  BLE_SelectHostIndex = DeviceAddress[5] - 1;
}

/*******************************************************************************
* Function Name  : DATAFLASH_Write_DeviceID
* Description    : 将设备ID写入DataFlash
* Input          : DeviceID
* Return         : None
*******************************************************************************/
void DATAFLASH_Write_DeviceID( uint8_t DeviceID )
{
  uint16_t w_DeviceID = DeviceID;
  HAL_Fs_Write_keyboard_cfg(FS_LINE_BLE_DEVICE, 1, &w_DeviceID);
}

/*********************************************************************
 * @fn      HidEmu_Init
 *
 * @brief   Initialization function for the HidEmuKbd App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notificaiton ... ).
 *
 * @param   task_id - the ID assigned by TMOS.  This ID should be
 *                    used to send messages and set timers.
 *
 * @return  none
 */
void HidEmu_Init( )
{
  hidEmuTaskId = TMOS_ProcessEventRegister(HidEmu_ProcessEvent);

  // Setup the GAP Peripheral Role Profile
  {
      uint8_t initial_advertising_enable = TRUE;

      // Set the GAP Role Parameters
      GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t), &initial_advertising_enable);

      GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(advertData), advertData);//广播数据
      GAPRole_SetParameter(GAPROLE_SCAN_RSP_DATA, sizeof(scanRspData), scanRspData);//扫描应答数据
  }

  // Set the GAP Characteristics
  GGS_SetParameter(GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN, (void *)attDeviceName);//设置设备名称

  // Setup the GAP Bond Manager 配对绑定相关参数
  {
      uint32_t passkey = DEFAULT_PASSCODE;
      uint8_t  pairMode = DEFAULT_PAIRING_MODE;
      uint8_t  mitm = DEFAULT_MITM_MODE;
      uint8_t  ioCap = DEFAULT_IO_CAPABILITIES;
      uint8_t  bonding = DEFAULT_BONDING_MODE;
      GAPBondMgr_SetParameter(GAPBOND_PERI_DEFAULT_PASSCODE, sizeof(uint32_t), &passkey);
      GAPBondMgr_SetParameter(GAPBOND_PERI_PAIRING_MODE, sizeof(uint8_t), &pairMode);
      GAPBondMgr_SetParameter(GAPBOND_PERI_MITM_PROTECTION, sizeof(uint8_t), &mitm);//防止密钥被第三方破解
      GAPBondMgr_SetParameter(GAPBOND_PERI_IO_CAPABILITIES, sizeof(uint8_t), &ioCap);//密钥显示、输入的模式
      GAPBondMgr_SetParameter(GAPBOND_PERI_BONDING_ENABLED, sizeof(uint8_t), &bonding);//是否保存绑定信息
  }

  // Setup Battery Characteristic Values 电量低于critical就会弹窗？
  {
      uint8_t critical = DEFAULT_BATT_CRITICAL_LEVEL;
      Batt_SetParameter(BATT_PARAM_CRITICAL_LEVEL, sizeof(uint8_t), &critical);
  }

  // Set up HID keyboard service
  Hid_AddService();

  // Register for HID Dev callback
  HidDev_Register(&hidEmuCfg, &hidEmuHidCBs);

  // Setup a delayed profile startup
  tmos_set_event(hidEmuTaskId, START_DEVICE_EVT);
}

/*********************************************************************
 * @fn      HidEmu_ProcessEvent
 *
 * @brief   HidEmuKbd Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The TMOS assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  events not processed
 */
uint16 HidEmu_ProcessEvent( uint8 task_id, uint16 events )
{
  static u8 send_char=4;

  if ( events & SYS_EVENT_MSG )
  {
    uint8 *pMsg;

    if ( (pMsg = tmos_msg_receive( hidEmuTaskId )) != NULL )
    {
      hidEmu_ProcessTMOSMsg( (tmos_event_hdr_t *)pMsg );

      // Release the TMOS message
      tmos_msg_deallocate( pMsg );
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  if ( events & START_MOUSE_REPORT_EVT )
  {
    HidDev_Report( HID_RPT_ID_MOUSE_IN, HID_REPORT_TYPE_INPUT,
                   HID_MOUSE_IN_RPT_LEN, HIDMouse );    // HID鼠标report
    return ( events ^ START_MOUSE_REPORT_EVT );
  }

  if ( events & START_KEYBOARD_REPORT_EVT )
  {
    HidDev_Report( HID_RPT_ID_KEY_IN, HID_REPORT_TYPE_INPUT,
                   HID_KEYBOARD_IN_RPT_LEN, HIDKeyboard );   // HID键盘report
    return ( events ^ START_KEYBOARD_REPORT_EVT );
  }

  if ( events & START_VOL_REPORT_EVT )
  {
#if (defined (BLE_VOL)) && (BLE_VOL == TRUE)
    UINT8 _hidvol[HID_VOL_IN_RPT_LEN] = { 0 }, i, j = 0;
    for (i = 0; i < 8 && j < 2; i++) {
      if (((UINT8)*HIDVolume & (1 << i))) _hidvol[j++] = Volume_to_Code[i];
    }
    HidDev_Report( HID_RPT_ID_VOL_IN, HID_REPORT_TYPE_INPUT,
                   HID_VOL_IN_RPT_LEN, _hidvol );     // HID音量report
#endif
    return ( events ^ START_VOL_REPORT_EVT );
  }

  if ( events & START_ENTER_PASSKEY_EVT )
  {
//    OLED_Set_Scroll_ENA(0);
    OLED_UI_add_SHOWINFO_task("Passkey=?");
    EnterPasskey_flag = TRUE;
    return ( events ^ START_ENTER_PASSKEY_EVT );
  }

  if ( events & START_SEND_PASSKEY_EVT )
  {
    GAPBondMgr_PasscodeRsp( hidEmuConnHandle, SUCCESS, BLE_Passkey ); // 发送密码

    return ( events ^ START_SEND_PASSKEY_EVT );
  }

  if ( events & START_DEVICE_EVT )
  {
    return ( events ^ START_DEVICE_EVT );
  }

  if ( events & START_PARAM_UPDATE_EVT )
  {
    // Send connect param update request
    GAPRole_PeripheralConnParamUpdateReq( hidEmuConnHandle,
                                          DEFAULT_DESIRED_MIN_CONN_INTERVAL,
                                          DEFAULT_DESIRED_MAX_CONN_INTERVAL,
                                          DEFAULT_DESIRED_SLAVE_LATENCY,
                                          DEFAULT_DESIRED_CONN_TIMEOUT,
                                          hidEmuTaskId);
    return (events ^ START_PARAM_UPDATE_EVT);
  }

  if ( events & START_PHY_UPDATE_EVT )
  {
    // start phy update
    PRINT("Send Phy Update %x...\n", GAPRole_UpdatePHY( hidEmuConnHandle, 0, GAP_PHY_BIT_LE_2M,
                                                        GAP_PHY_BIT_LE_2M, 0 ) );
    return ( events ^ START_PHY_UPDATE_EVT );
  }

  if ( events & CHANGE_ADDR_EVT )  // 切换蓝牙设备地址
  {
    /* disable advertising */
    uint8 param = FALSE, is_adv;
    bStatus_t status;
    GAPRole_GetParameter( GAPROLE_ADVERT_ENABLED, &is_adv );
    if ( is_adv == TRUE ) {
      GAPRole_SetParameter( GAPROLE_ADVERT_ENABLED, sizeof( uint8 ), &param );
    }
    status = GAP_ConfigDeviceAddr( ADDRTYPE_STATIC, DeviceAddress );
    if ( status == SUCCESS ) {
#ifdef OLED_0_91
      OLED_ShowNum(44, 1, DeviceAddress[5], 1);
#endif
      if ( hidEmuConnHandle != GAP_CONNHANDLE_INIT ) {
        GAPRole_TerminateLink( hidEmuConnHandle );  // disconnect
        hidEmuConnHandle = GAP_CONNHANDLE_INIT;
        // bug - 切换DeviceAddress后，OLED无法删除切换前的icon，因此需要手动删除
#ifdef OLED_0_66
        OLED_UI_slot_delete((uint8_t*)UI_Slot_Icon[OLED_UI_ICON_BLE_UNCONNECT_IDX + BLE_SelectHostIndex + 1]);
#endif
      }
      BLE_SelectHostIndex = DeviceAddress[5] - 1;
    } else {
      DeviceAddress[5] = BLE_SelectHostIndex + 1;
//      OLED_PRINT("[Error]Status: %d", status);
    }
    if ( is_adv == TRUE ) { // 恢复广播
      param = TRUE;
      GAPRole_SetParameter( GAPROLE_ADVERT_ENABLED, sizeof( uint8 ), &param );
    }
    return ( events ^ CHANGE_ADDR_EVT );
  }

  if ( events & BLE_DISCONNECT_EVT )  // 断开连接事件
  {
    GAPRole_TerminateLink( hidEmuConnHandle );  // disconnect
    return ( events ^ BLE_DISCONNECT_EVT );
  }

  if ( events & BLE_EN_ADVERT_EVT )  // 使能广播事件
  {
    uint8 param = TRUE;
    GAPRole_SetParameter( GAPROLE_ADVERT_ENABLED, sizeof( uint8 ), &param );
    return ( events ^ BLE_EN_ADVERT_EVT );
  }

  if ( events & BLE_DIS_ADVERT_EVT )  // 关闭广播事件
  {
    uint8 param = FALSE;
    GAPRole_SetParameter( GAPROLE_ADVERT_ENABLED, sizeof( uint8 ), &param );
    return ( events ^ BLE_DIS_ADVERT_EVT );
  }

  if ( events & BLE_TEST_REPORT_EVT ) // 测试事件
  {
    hidEmuSendKbdReport( send_char );
    send_char++;
    if(send_char>=29) send_char = 4;
    hidEmuSendKbdReport( 0x00 );
    hidEmuSendMouseReport( MOUSE_BUTTON_NONE , 2 , 2 );
    tmos_start_task( hidEmuTaskId, BLE_TEST_REPORT_EVT, 2000 );
    return ( events ^ BLE_TEST_REPORT_EVT );
  }

  return 0;
}

/*********************************************************************
 * @fn      hidEmu_ProcessTMOSMsg
 *
 * @brief   Process an incoming task message.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static void hidEmu_ProcessTMOSMsg( tmos_event_hdr_t *pMsg )
{
  switch ( pMsg->event )
  {
    default:
      break;
  }
}

/*********************************************************************
 * @fn      hidEmuSendMouseReport
 *
 * @brief   Build and send a HID mouse report.
 *
 * @param   buttons - Mouse button code
 *          X_data - X axis move data
 *          Y_data - Y axis move data
 *
 * @return  none
 */
static void hidEmuSendMouseReport( uint8 buttons ,uint8 X_data ,uint8 Y_data )
{
  uint8 buf[HID_MOUSE_IN_RPT_LEN];

  buf[0] = buttons;   // Buttons
  buf[1] = X_data;    // X
  buf[2] = Y_data;    // Y
  buf[3] = 0;         // Wheel

  HidDev_Report( HID_RPT_ID_MOUSE_IN, HID_REPORT_TYPE_INPUT,
                 HID_MOUSE_IN_RPT_LEN, buf );
}

/*********************************************************************
 * @fn      hidEmuSendKbdReport
 *
 * @brief   Build and send a HID keyboard report.
 *
 * @param   keycode - HID keycode.
 *
 * @return  none
 */
static void hidEmuSendKbdReport( uint8 keycode )
{
  uint8 buf[HID_KEYBOARD_IN_RPT_LEN];

  buf[0] = 0;         // Modifier keys
  buf[1] = 0;         // Reserved
  buf[2] = keycode;   // Keycode 1
  buf[3] = 0;         // Keycode 2
  buf[4] = 0;         // Keycode 3
  buf[5] = 0;         // Keycode 4
  buf[6] = 0;         // Keycode 5
  buf[7] = 0;         // Keycode 6

  HidDev_Report( HID_RPT_ID_KEY_IN, HID_REPORT_TYPE_INPUT,
                HID_KEYBOARD_IN_RPT_LEN, buf );
}

/*********************************************************************
 * @fn      hidEmuStateCB
 *
 * @brief   GAP state change callback.
 *
 * @param   newState - new state
 *
 * @return  none
 */
static void hidEmuStateCB( gapRole_States_t newState , gapRoleEvent_t * pEvent )
{
  switch ( newState&GAPROLE_STATE_ADV_MASK )
  {
    case GAPROLE_STARTED:
      {
        GAP_ConfigDeviceAddr( ADDRTYPE_STATIC, DeviceAddress );
        PRINT( "Initialized..\n" );
      }
      break;

    case GAPROLE_ADVERTISING:
      if( pEvent->gap.opcode == GAP_MAKE_DISCOVERABLE_DONE_EVENT )
      {
        PRINT( "Advertising..\n" );
      }
      break;

    case GAPROLE_CONNECTED:
      if( pEvent->gap.opcode == GAP_LINK_ESTABLISHED_EVENT )
      {
        gapEstLinkReqEvent_t *event = (gapEstLinkReqEvent_t *) pEvent;

        // get connection handle
        hidEmuConnHandle = event->connectionHandle;

//        if ( g_Enable_Status.ble == FALSE ) {
//          GAPRole_TerminateLink( hidEmuConnHandle );  // disconnect
//        }

        tmos_start_task( hidEmuTaskId, START_PARAM_UPDATE_EVT, START_PARAM_UPDATE_EVT_DELAY );
//        tmos_start_task( hidEmuTaskId, START_PHY_UPDATE_EVT, START_PHY_UPDATE_DELAY);

//        g_Ready_Status.ble = TRUE;
        PRINT( "Connected..\n" );
      }
      break;

    case GAPROLE_CONNECTED_ADV:
      if( pEvent->gap.opcode == GAP_MAKE_DISCOVERABLE_DONE_EVENT )
      {
        PRINT( "Connected Advertising..\n" );
      }
      break;

    case GAPROLE_WAITING:
      if( pEvent->gap.opcode == GAP_END_DISCOVERABLE_DONE_EVENT )
      {
        PRINT( "Waiting for advertising..\n" );
      }
      else if( pEvent->gap.opcode == GAP_LINK_TERMINATED_EVENT )
      {
//        g_Ready_Status.ble = FALSE;
        PRINT( "Disconnected.. Reason:%x\n",pEvent->linkTerminate.reason );
      }
      else if( pEvent->gap.opcode == GAP_LINK_ESTABLISHED_EVENT )
      {
        PRINT( "Advertising timeout..\n" );
      }
      // Enable advertising
      {
        uint8 initial_advertising_enable = TRUE;
        // Set the GAP Role Parameters
        GAPRole_SetParameter( GAPROLE_ADVERT_ENABLED, sizeof( uint8 ), &initial_advertising_enable );
      }
      break;

    case GAPROLE_ERROR:
      PRINT( "Error %x ..\n",pEvent->gap.opcode );
      break;

    default:
      break;
  }
}

/*********************************************************************
 * @fn      hidEmuRcvReport
 *
 * @brief   Process an incoming HID keyboard report.
 *
 * @param   len - Length of report.
 * @param   pData - Report data.
 *
 * @return  status
 */
static uint8 hidEmuRcvReport( uint8 len, uint8 *pData )
{
  // verify data length
  if ( len == HID_LED_OUT_RPT_LEN )
  {
    // set LEDs
    /*
        (pData[0] & 0x01) ? set_led_num(1) : set_led_num(0);
        (pData[0] & 0x02) ? set_led_cap(1) : set_led_cap(0);
        (pData[0] & 0x04) ? set_led_scr(1) : set_led_scr(0);
     */
    if ((pData[0] & 0x02) != 0)
      g_CapsLock_LEDOn_Status.ble = TRUE;     // Light on Caps Lock LED
    else
      g_CapsLock_LEDOn_Status.ble = FALSE;    // Light off Caps Lock LED
    return SUCCESS;
  }
  else
  {
    return ATT_ERR_INVALID_VALUE_SIZE;
  }
}

/*********************************************************************
 * @fn      hidEmuRptCB
 *
 * @brief   HID Dev report callback.
 *
 * @param   id - HID report ID.
 * @param   type - HID report type.
 * @param   uuid - attribute uuid.
 * @param   oper - operation:  read, write, etc.
 * @param   len - Length of report.
 * @param   pData - Report data.
 *
 * @return  GATT status code.
 */
static uint8 hidEmuRptCB( uint8 id, uint8 type, uint16 uuid,
                             uint8 oper, uint16 *pLen, uint8 *pData )
{
  uint8 status = SUCCESS;

  // write
  if ( oper == HID_DEV_OPER_WRITE )
  {
    if ( uuid == REPORT_UUID )
    {
      // process write to LED output report; ignore others
      if ( type == HID_REPORT_TYPE_OUTPUT )
      {
        status = hidEmuRcvReport( *pLen, pData );
      }
    }

    if ( status == SUCCESS )
    {
      status = Hid_SetParameter( id, type, uuid, *pLen, pData );
    }
  }
  // read
  else if ( oper == HID_DEV_OPER_READ )
  {
    status = Hid_GetParameter( id, type, uuid, pLen, pData );
  }
  // notifications enabled
  else if ( oper == HID_DEV_OPER_ENABLE )
  {
//    tmos_start_task( hidEmuTaskId, BLE_TEST_REPORT_EVT, 500 );  //测试report
  }
  return status;
}

/*********************************************************************
 * @fn      hidEmuEvtCB
 *
 * @brief   HID Dev event callback.
 *
 * @param   evt - event ID.
 *
 * @return  HID response code.
 */
static void hidEmuEvtCB( uint8 evt )
{
  // process enter/exit suspend or enter/exit boot mode
  return;
}

/*********************************************************************
*********************************************************************/

