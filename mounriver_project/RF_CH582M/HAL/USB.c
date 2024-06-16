/********************************** (C) COPYRIGHT *******************************
 * File Name          : USB.c
 * Author             : ChnMasterOG, WCH
 * Version            : V1.1
 * Date               : 2022/2/24
 * Description        : USB驱动源文件
 * Copyright (c) 2023 ChnMasterOG
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#include "HAL.h"
#include "CH58x_common.h"

// 设备描述符
const UINT8 MyDevDescr[] = { 0x12, 0x01, 0x10, 0x01, 0x00, 0x00, 0x00, DevEP0SIZE,
                             0x4A, 0x43,  // idVender=0x434A
                             0x40, 0x55,  // idProduct=0x5540
                             0x01, 0x01,  // bcdDevice=0x0101
                             0x01, 0x02, 0x00, 0x01
                           };
// 配置描述符
const UINT8 MyCfgDescr[] = { 0x09, 0x02, 0x6B, 0x00, 0x04, 0x01, 0x00, 0xA0, 0x32,            //配置描述符: 4个接口
                             0x09, 0x04, 0x00, 0x00, 0x01, 0x03, 0x01, 0x01, 0x00,            //接口描述符,键盘：接口编号0
                             0x09, 0x21, 0x11, 0x01, 0x00, 0x01, 0x22, 0x3e, 0x00,            //HID类描述符：下级描述符KeyRepDesc
                             0x07, 0x05, 0x81, 0x03, 0x08, 0x00, 0x0A,                        //端点描述符：端点地址0x81 即端点1
                             0x09, 0x04, 0x01, 0x00, 0x01, 0x03, 0x01, 0x02, 0x00,            //接口描述符,鼠标：接口编号1
                             0x09, 0x21, 0x10, 0x01, 0x00, 0x01, 0x22, 0x34, 0x00,            //HID类描述符：下级描述符MouseRepDesc
                             0x07, 0x05, 0x82, 0x03, 0x04, 0x00, 0x0A,                        //端点描述符：端点地址0x82 即端点2
                             0x09, 0x04, 0x02, 0x00, 0x01, 0x03, 0x01, 0x00, 0x00,            //接口描述符,音量：接口编号2
                             0x09, 0x21, 0x10, 0x01, 0x00, 0x01, 0x22, 0x21, 0x00,            //HID类描述符：下级描述符VolumeRepDesc
                             0x07, 0x05, 0x83, 0x03, 0x01, 0x00, 0x0A,                        //端点描述符：端点地址0x83 即端点3
                             0x09, 0x04, 0x03, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,            //接口描述符,自定义：接口编号3
                             0x07, 0x05, 0x84, 0x02, 0x40, 0x00, 0x00,                        //端点描述符：端点地址0x84
                             0x07, 0x05, 0x04, 0x02, 0x40, 0x00, 0x00,                        //端点描述符：端点地址0x04
                             //端点地址 Bit 3…0: The endpoint number Bit 6…4: Reserved, reset to zero
                           };
// 语言描述符
const UINT8 MyLangDescr[] = { 0x04, 0x03, 0x09, 0x04 };
// 厂家信息
const UINT8 MyManuInfo[] = { 0x0E, 0x03, 'l', 0, 'a', 0, 'b', 0, '4', 0, '1', 0, '8', 0 };
// 产品信息
const UINT8 MyProdInfo[] = { 0x0A, 0x03, 'T', 0, 'P', 0, '7', 0, '8', 0 };
/*HID类报表描述符*/
const UINT8 KeyRepDesc[] = { 0x05, 0x01, 0x09, 0x06, 0xA1, 0x01, 0x05, 0x07, 0x19, 0xe0, 0x29, 0xe7, 0x15, 0x00, 0x25,
                             0x01, 0x75, 0x01, 0x95, 0x08, 0x81, 0x02, 0x95, 0x01, 0x75, 0x08, 0x81, 0x01, 0x95, 0x03,
                             0x75, 0x01, 0x05, 0x08, 0x19, 0x01, 0x29, 0x03, 0x91, 0x02, 0x95, 0x05, 0x75, 0x01, 0x91,
                             0x01, 0x95, 0x06, 0x75, 0x08, 0x26, 0xff, 0x00, 0x05, 0x07, 0x19, 0x00, 0x29, 0x91, 0x81,
                             0x00, 0xC0 };
const UINT8 MouseRepDesc[] = { 0x05, 0x01, 0x09, 0x02, 0xA1, 0x01, 0x09, 0x01, 0xA1, 0x00, 0x05, 0x09, 0x19, 0x01, 0x29,
                               0x03, 0x15, 0x00, 0x25, 0x01, 0x75, 0x01, 0x95, 0x03, 0x81, 0x02, 0x75, 0x05, 0x95, 0x01,
                               0x81, 0x01, 0x05, 0x01, 0x09, 0x30, 0x09, 0x31, 0x09, 0x38, 0x15, 0x81, 0x25, 0x7f, 0x75,
                               0x08, 0x95, 0x03, 0x81, 0x06, 0xC0, 0xC0 };
const UINT8 VolumeRepDesc[] = { 0x05, 0x0C, 0x09, 0x01, 0xA1, 0x01, 0x09, 0xB0, 0x09, 0xB5, 0x09, 0xB6, 0x09, 0xE9, 0x09,
                                0xEA, 0x09, 0xE2, 0x09, 0xB1, 0x09, 0xB7, 0x15, 0x00, 0x25, 0x01, 0x95, 0x08, 0x75, 0x01,
                                0x81, 0x02, 0xC0 };

/**********************************************************/
UINT8 DevConfig;
UINT8 SetupReqCode;
UINT16 SetupReqLen;
const UINT8 *pDescr;

struct HIDdata HID_dat;

/******** 用户自定义分配端点RAM ****************************************/
__attribute__((aligned(4)))  UINT8 EP0_Databuf[64 + 64 + 64];   //ep0(64)+ep4_out(64)+ep4_in(64)
__attribute__((aligned(4)))  UINT8 EP1_Databuf[64 + 64];        //ep1_out(64)+ep1_in(64)
__attribute__((aligned(4)))  UINT8 EP2_Databuf[64 + 64];        //ep2_out(64)+ep2_in(64)
__attribute__((aligned(4)))  UINT8 EP3_Databuf[64 + 64];        //ep3_out(64)+ep3_in(64)

// Task id
tmosTaskID usbTaskID=INVALID_TASK_ID;
// USB ready flag
BOOL USB_Ready = FALSE;

/*******************************************************************************
* Function Name  : USB_ProcessEvent
* Description    : USB处理事件
* Input          : task_id： 任务id, events: USB事件
* Return         : tmosEvents
*******************************************************************************/
tmosEvents USB_ProcessEvent( tmosTaskID task_id, tmosEvents events )
{
  if ( events & START_USB_EVENT )
  {
    PFIC_EnableIRQ( USB_IRQn );
    return events ^ START_USB_EVENT;
  }

  if ( events & USB_MOUSE_EVENT )
  {
    memcpy(pEP2_IN_DataBuf, &HID_dat.Mouse[0], 4);
    DevEP2_IN_Deal( 4 );
    return events ^ USB_MOUSE_EVENT;
  }

  if ( events & USB_KEYBOARD_EVENT )
  {
    memcpy(pEP1_IN_DataBuf, &HID_dat.Keyboard[0], 8);
    DevEP1_IN_Deal( 8 );
    return events ^ USB_KEYBOARD_EVENT;
  }

  if ( events & USB_VOL_EVENT )
  {
    memcpy(pEP3_IN_DataBuf, &HID_dat.Volume[0], 1);
    DevEP3_IN_Deal( 1 );
    return events ^ USB_VOL_EVENT;
  }

  if ( events & USB_TEST_EVENT )
  {
    HID_dat.Mouse[1] = 2;
    HID_dat.Mouse[2] = 2;
    memcpy(pEP2_IN_DataBuf, &HID_dat.Mouse[0], 4);
    DevEP2_IN_Deal( 4 );
    tmos_start_task(usbTaskID, USB_TEST_EVENT, MS1_TO_SYSTEM_TIME(500));
    return events ^ USB_TEST_EVENT;
  }

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
  usbTaskID = TMOS_ProcessEventRegister( USB_ProcessEvent );
  pEP0_RAM_Addr = EP0_Databuf;
  pEP1_RAM_Addr = EP1_Databuf;
  pEP2_RAM_Addr = EP2_Databuf;
  pEP3_RAM_Addr = EP3_Databuf;
  USB_DeviceInit();
  tmos_start_task( usbTaskID, START_USB_EVENT, 0 );
}

/*******************************************************************************
* Function Name  : USB_DevTransProcess
* Description    : USB中断传输处理
* Input          : None
* Return         : None
*******************************************************************************/
void USB_DevTransProcess( void )
{
  UINT8 len, chtype;
  UINT8 intflag, errflag = 0;

  intflag = R8_USB_INT_FG;
  if ( intflag & RB_UIF_TRANSFER )
  {
    if ( ( R8_USB_INT_ST & MASK_UIS_TOKEN ) != MASK_UIS_TOKEN )    // 非空闲
    {
      switch ( R8_USB_INT_ST & ( MASK_UIS_TOKEN | MASK_UIS_ENDP ) )
      // 分析操作令牌和端点号
      {
        case UIS_TOKEN_IN : //USB传输事务令牌PID 10b 端点0
        {
          switch ( SetupReqCode )
          {
            case USB_GET_DESCRIPTOR :
              len = SetupReqLen >= DevEP0SIZE ?
                  DevEP0SIZE : SetupReqLen;    // 本次传输长度
              memcpy( pEP0_DataBuf, pDescr, len ); /* 加载上传数据 */
              SetupReqLen -= len;
              pDescr += len;
              R8_UEP0_T_LEN = len;
              R8_UEP0_CTRL ^= RB_UEP_T_TOG;                             // 翻转
              break;
            case USB_SET_ADDRESS :
              R8_USB_DEV_AD = ( R8_USB_DEV_AD & RB_UDA_GP_BIT ) | SetupReqLen;
              R8_UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
              break;
            default :
              R8_UEP0_T_LEN = 0;                                      // 状态阶段完成中断或者是强制上传0长度数据包结束控制传输
              R8_UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
              break;
          }
        }
          break;

        case UIS_TOKEN_OUT : //USB传输事务令牌PID 00b 端点0
        {
          len = R8_USB_RX_LEN;
          if ( SetupReqCode == 0x09 )
          {
            if ( (pEP0_DataBuf[0] & (1<<0)) == 0 ) {
                LEDOUT_DATA[0] = 0;    // Light off Num Lock LED
            } else {
                LEDOUT_DATA[0] = 1;     // Light on Num Lock LED
            }
            if ( (pEP0_DataBuf[0] & (1<<1)) == 0 ) {
                LEDOUT_DATA[1] = 0;    // Light off Caps Lock LED
            } else {
                LEDOUT_DATA[1] = 1;     // Light on Caps Lock LED
            }
            tmos_start_task(RFtaskID, SBP_RF_LEDOUT_TX_EVT, 10);
          }
        }
          break;

        case UIS_TOKEN_OUT | 1 :    //USB传输事务令牌PID 10b 端点1
        {
          if ( R8_USB_INT_ST & RB_UIS_TOG_OK )
          {                       // 不同步的数据包将丢弃
            len = R8_USB_RX_LEN;
            DevEP1_OUT_Deal( len );
          }
        }
          break;

        case UIS_TOKEN_IN | 1 :     //USB传输事务令牌PID 00b 端点1
          R8_UEP1_CTRL = ( R8_UEP1_CTRL & ~MASK_UEP_T_RES ) | UEP_T_RES_NAK;
          break;

        case UIS_TOKEN_OUT | 2 :    //USB传输事务令牌PID 10b 端点2
        {
          if ( R8_USB_INT_ST & RB_UIS_TOG_OK )
          {                       // 不同步的数据包将丢弃
            len = R8_USB_RX_LEN;
            DevEP2_OUT_Deal( len );
          }
        }
          break;

        case UIS_TOKEN_IN | 2 :     //USB传输事务令牌PID 00b 端点2
          R8_UEP2_CTRL = ( R8_UEP2_CTRL & ~MASK_UEP_T_RES ) | UEP_T_RES_NAK;
          break;

        case UIS_TOKEN_OUT | 3 :    //USB传输事务令牌PID 10b 端点3
        {
          if ( R8_USB_INT_ST & RB_UIS_TOG_OK )
          {                       // 不同步的数据包将丢弃
            len = R8_USB_RX_LEN;
            DevEP3_OUT_Deal( len );
          }
        }
          break;

        case UIS_TOKEN_IN | 3 :     //USB传输事务令牌PID 00b 端点3
          R8_UEP3_CTRL = ( R8_UEP3_CTRL & ~MASK_UEP_T_RES ) | UEP_T_RES_NAK;
          break;

        case UIS_TOKEN_OUT | 4 :    //USB传输事务令牌PID 10b 端点4
        {
          if ( R8_USB_INT_ST & RB_UIS_TOG_OK )
          {
            R8_UEP4_CTRL ^= RB_UEP_R_TOG;
            len = R8_USB_RX_LEN;
            DevEP4_OUT_Deal( len );
          }
        }
          break;

        case UIS_TOKEN_IN | 4 :     //USB传输事务令牌PID 00b 端点4
          R8_UEP4_CTRL ^= RB_UEP_T_TOG;
          R8_UEP4_CTRL = ( R8_UEP4_CTRL & ~MASK_UEP_T_RES ) | UEP_T_RES_NAK;
          break;

        default :
          break;
      }
      R8_USB_INT_FG = RB_UIF_TRANSFER;
    }
    if ( R8_USB_INT_ST & RB_UIS_SETUP_ACT )                  // Setup包处理
    {
      R8_UEP0_CTRL = RB_UEP_R_TOG | RB_UEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;
      SetupReqLen = pSetupReqPak->wLength;
      SetupReqCode = pSetupReqPak->bRequest;
      chtype = pSetupReqPak->bRequestType;

      len = 0;
      errflag = 0;
      if ( ( pSetupReqPak->bRequestType & USB_REQ_TYP_MASK ) != USB_REQ_TYP_STANDARD )
      {
        switch ( SetupReqCode )
        {
          case 0x0a :
            break;        //这个一定要有
          case 0x09 :
            break;
          default :
            errflag = 0xFF;
        }
      }
      else /* 标准请求 */
      {
        switch ( SetupReqCode )
        {
          case USB_GET_DESCRIPTOR :
          {
            switch ( ( ( pSetupReqPak->wValue ) >> 8 ) )
            {
              case USB_DESCR_TYP_DEVICE :
              {
                pDescr = MyDevDescr;
                len = MyDevDescr[0];
              }
                break;

              case USB_DESCR_TYP_CONFIG :
              {
                pDescr = MyCfgDescr;
                len = MyCfgDescr[2];
              }
                break;

              case USB_DESCR_TYP_REPORT :
              {
                if ( ( ( pSetupReqPak->wIndex ) & 0xff ) == 0 )         //接口0报表描述符
                {
                  pDescr = KeyRepDesc;                                  //数据准备上传
                  len = sizeof( KeyRepDesc );
                }
                else if ( ( ( pSetupReqPak->wIndex ) & 0xff ) == 1 )    //接口1报表描述符
                {
                  pDescr = MouseRepDesc;                                //数据准备上传
                  len = sizeof( MouseRepDesc );
                }
                else if ( ( ( pSetupReqPak->wIndex ) & 0xff ) == 2 )    //接口2报表描述符
                {
                  pDescr = VolumeRepDesc;                               //数据准备上传
                  len = sizeof( VolumeRepDesc );                        //如果有更多接口，该标准位应该在最后一个接口配置完成后有效
//                  tmos_start_task(usbTaskID, USB_TEST_EVENT, MS1_TO_SYSTEM_TIME(500));
                }
                else
                {
                  len = 0xff;                                           //本程序只有4个接口，这句话正常不可能执行
                }
              }
                break;

              case USB_DESCR_TYP_STRING :
              {
                switch ( ( pSetupReqPak->wValue ) & 0xff )
                {
                  case 1 :
                    pDescr = MyManuInfo;
                    len = MyManuInfo[0];
                    break;
                  case 2 :
                    pDescr = MyProdInfo;
                    len = MyProdInfo[0];
                    break;
                  case 0 :
                    pDescr = MyLangDescr;
                    len = MyLangDescr[0];
                    break;
                  default :
                    errflag = 0xFF;                               // 不支持的字符串描述符
                    break;
                }
              }
                break;

              default :
                errflag = 0xff;
                break;
            }
            if ( SetupReqLen > len )
              SetupReqLen = len;      //实际需上传总长度
            len = ( SetupReqLen >= DevEP0SIZE ) ?
                DevEP0SIZE : SetupReqLen;
            memcpy( pEP0_DataBuf, pDescr, len );
            pDescr += len;
          }
            break;

          case USB_SET_ADDRESS :
            SetupReqLen = ( pSetupReqPak->wValue ) & 0xff;
            break;

          case USB_GET_CONFIGURATION :
            pEP0_DataBuf[0] = DevConfig;
            if ( SetupReqLen > 1 )
              SetupReqLen = 1;
            break;

          case USB_SET_CONFIGURATION :
            DevConfig = ( pSetupReqPak->wValue ) & 0xff;
            break;

          case USB_CLEAR_FEATURE :
          {
            if ( ( pSetupReqPak->bRequestType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )    // 端点
            {
              switch ( ( pSetupReqPak->wIndex ) & 0xff )
              {
                case 0x82 :
                  R8_UEP2_CTRL = ( R8_UEP2_CTRL & ~( RB_UEP_T_TOG | MASK_UEP_T_RES ) ) | UEP_T_RES_NAK;
                  break;
                case 0x02 :
                  R8_UEP2_CTRL = ( R8_UEP2_CTRL & ~( RB_UEP_R_TOG | MASK_UEP_R_RES ) ) | UEP_R_RES_ACK;
                  break;
                case 0x81 :
                  R8_UEP1_CTRL = ( R8_UEP1_CTRL & ~( RB_UEP_T_TOG | MASK_UEP_T_RES ) ) | UEP_T_RES_NAK;
                  break;
                case 0x01 :
                  R8_UEP1_CTRL = ( R8_UEP1_CTRL & ~( RB_UEP_R_TOG | MASK_UEP_R_RES ) ) | UEP_R_RES_ACK;
                  break;
                default :
                  errflag = 0xFF;                                 // 不支持的端点
                  break;
              }
            }
            else
              errflag = 0xFF;
          }
            break;

          case USB_GET_INTERFACE :
            pEP0_DataBuf[0] = 0x00;
            if ( SetupReqLen > 1 )
              SetupReqLen = 1;
            break;

          case USB_GET_STATUS :
            pEP0_DataBuf[0] = 0x00;
            pEP0_DataBuf[1] = 0x00;
            if ( SetupReqLen > 2 )
              SetupReqLen = 2;
            break;

          default :
            errflag = 0xff;
            break;
        }
      }
      if ( errflag == 0xff )        // 错误或不支持
      {
//                  SetupReqCode = 0xFF;
        R8_UEP0_CTRL = RB_UEP_R_TOG | RB_UEP_T_TOG | UEP_R_RES_STALL | UEP_T_RES_STALL;    // STALL
      }
      else
      {
        if ( chtype & 0x80 )     // 上传
        {
          len = ( SetupReqLen > DevEP0SIZE ) ?
              DevEP0SIZE : SetupReqLen;
          SetupReqLen -= len;
        }
        else
          len = 0;        // 下传
        R8_UEP0_T_LEN = len;
        R8_UEP0_CTRL = RB_UEP_R_TOG | RB_UEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;    // 默认数据包是DATA1
      }

      R8_USB_INT_FG = RB_UIF_TRANSFER;
    }
  }
  else if ( intflag & RB_UIF_BUS_RST )
  {
    R8_USB_DEV_AD = 0;
    R8_UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
    R8_UEP1_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK | RB_UEP_AUTO_TOG;
    R8_UEP2_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK | RB_UEP_AUTO_TOG;
    R8_UEP3_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK | RB_UEP_AUTO_TOG;
    R8_USB_INT_FG = RB_UIF_BUS_RST;
  }
  else if ( intflag & RB_UIF_SUSPEND )
  {
    if ( R8_USB_MIS_ST & RB_UMS_SUSPEND )
    {
        USB_Ready = FALSE;
    }   // 挂起
    else
    {
        USB_Ready = TRUE;
    }   // 唤醒
    R8_USB_INT_FG = RB_UIF_SUSPEND;
  }
  else
  {
    R8_USB_INT_FG = intflag;
  }
}

/*******************************************************************************
* Function Name  : DevEP1_OUT_Deal
* Description    : 端点1数据处理(HID键盘-不使用)
* Input          : None
* Return         : None
*******************************************************************************/
void DevEP1_OUT_Deal( UINT8 l )
{
  UINT8 i;

  for ( i = 0; i < l; i++ )
  {
    pEP1_IN_DataBuf[i] = ~pEP1_OUT_DataBuf[i];
  }
  DevEP1_IN_Deal( l );
}

/*******************************************************************************
* Function Name  : DevEP2_OUT_Deal
* Description    : 端点2数据处理(HID鼠标-不使用)
* Input          : None
* Return         : None
*******************************************************************************/
void DevEP2_OUT_Deal( UINT8 l )
{
  UINT8 i;

  for ( i = 0; i < l; i++ )
  {
    pEP2_IN_DataBuf[i] = ~pEP2_OUT_DataBuf[i];
  }
  DevEP2_IN_Deal( l );
}

/*******************************************************************************
* Function Name  : DevEP3_OUT_Deal
* Description    : 端点3数据处理(HID音量控制-不使用)
* Input          : None
* Return         : None
*******************************************************************************/
void DevEP3_OUT_Deal( UINT8 l )
{
  UINT8 i;

  for ( i = 0; i < l; i++ )
  {
    pEP3_IN_DataBuf[i] = ~pEP3_OUT_DataBuf[i];
  }
  DevEP3_IN_Deal( l );
}

/*******************************************************************************
* Function Name  : DevEP4_OUT_Deal
* Description    : 预留
* Input          : None
* Return         : None
*******************************************************************************/
void DevEP4_OUT_Deal( UINT8 l )
{
  UINT8 i;

  for ( i = 0; i < l; i++ )
  {
    pEP4_IN_DataBuf[i] = ~pEP4_OUT_DataBuf[i];
  }
  DevEP4_IN_Deal( l );
}

__INTERRUPT
__HIGH_CODE
void USB_IRQHandler( void )     //USB中断服务程序,使用寄存器组1
{
    USB_DevTransProcess();
}
