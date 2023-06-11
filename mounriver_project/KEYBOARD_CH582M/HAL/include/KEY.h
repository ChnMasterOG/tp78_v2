/********************************** (C) COPYRIGHT *******************************
* File Name          : KEY.h
* Author             : ChnMasterOG
* Version            : V1.0
* Date               : 2021/12/7
* Description        : 
*******************************************************************************/



/******************************************************************************/
#ifndef __KEY_H
#define __KEY_H

#ifdef __cplusplus
extern "C"
{
#endif

	


/**************************************************************************************************
 *                                              MACROS
 **************************************************************************************************/



  #define HAL_KEY_POLLING_VALUE   	   100

  /* Switches (keys) */
  #define HAL_KEY_SW_1 0x01  // key1
  #define HAL_KEY_SW_2 0x02  // key2
  #define HAL_KEY_SW_3 0x04  // key3
  #define HAL_KEY_SW_4 0x08  // key4

  /* 按键定义 */
  
  /* 连接一个按键 */
  #define KEY1_BV         BV(1)
  #define KEY2_BV
  #define KEY3_BV           
  #define KEY4_BV           

  #define KEY1_PD        	(R32_PB_PD_DRV |= KEY1_BV)  // 上拉R32_PB_PU
  #define KEY2_PD        	()
  #define KEY3_PD        	()
  #define KEY4_PD        	()

  #define KEY1_DIR       	(R32_PB_DIR &= ~KEY1_BV)
  #define KEY2_DIR        ()
  #define KEY3_DIR        ()
  #define KEY4_DIR        ()

  #define KEY1_IN        	(ACTIVE_HIGH(R32_PB_PIN&KEY1_BV)) // 检测低ACTIVE_LOW
  #define KEY2_IN        	()
  #define KEY3_IN        	()
  #define KEY4_IN        	()

#define HAL_PUSH_BUTTON1()        ( KEY1_IN ) //添加自定义按键
#define HAL_PUSH_BUTTON2()        ( 0 )
#define HAL_PUSH_BUTTON3()        ( 0 ) 
#define HAL_PUSH_BUTTON4()        ( 0 ) 






/**************************************************************************************************
 * TYPEDEFS
 **************************************************************************************************/
typedef void (*halKeyCBack_t) (uint8 keys );

typedef struct
{
  uint8  keys;  // keys
} keyChange_t;

/**************************************************************************************************
 *                                             GLOBAL VARIABLES
 **************************************************************************************************/

/*********************************************************************
 * FUNCTIONS
 */

/*
 * Initialize the Key Service
 */
void HAL_KeyInit( void );

/*
 * This is for internal used by hal_driver
 */
void HAL_KeyPoll( void );

/*
 * Configure the Key Service
 */
void HalKeyConfig( const halKeyCBack_t cback);

/*
 * Read the Key callback
 */
void HalKeyCallback ( uint8 keys );

/*
 * Read the Key status
 */
uint8 HalKeyRead( void);

/**************************************************************************************************
**************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif
