/********************************** (C) COPYRIGHT *******************************
 * File Name          : LED.h
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2016/04/12
 * Description        :
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/******************************************************************************/
#ifndef __LED_H
#define __LED_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************
 * CONSTANTS
 */

/* LEDS - The LED number is the same as the bit position */
#define HAL_LED_1                      0x01
#define HAL_LED_2                      0x02
#define HAL_LED_3                      0x04
#define HAL_LED_4                      0x08
#define HAL_LED_ALL                    (HAL_LED_1 | HAL_LED_2 | HAL_LED_3 | HAL_LED_4)

/* Modes */
#define HAL_LED_MODE_OFF               0x00
#define HAL_LED_MODE_ON                0x01
#define HAL_LED_MODE_BLINK             0x02
#define HAL_LED_MODE_FLASH             0x04
#define HAL_LED_MODE_TOGGLE            0x08

/* Defaults */
#define HAL_LED_DEFAULT_MAX_LEDS       4
#define HAL_LED_DEFAULT_DUTY_CYCLE     5
#define HAL_LED_DEFAULT_FLASH_COUNT    50
#define HAL_LED_DEFAULT_FLASH_TIME     1000

/*********************************************************************
 * TYPEDEFS
 */

/* 连接一个LED用于监控演示程序的进度,低电平LED亮 */

/* 1 - LED */
#define LED1_BV                 BV(15)
#define LED2_BV
#define LED3_BV

#define LED_COL0_BV             GPIO_Pin_4
#define LED_COL1_BV             GPIO_Pin_5
#define LED_COL2_BV             GPIO_Pin_6
#define LED_COL3_BV             GPIO_Pin_0
#define LED_COL4_BV             GPIO_Pin_1
#define LED_COL5_BV             GPIO_Pin_2
#define LED_COL6_BV             GPIO_Pin_3
#define LED_COL7_BV             GPIO_Pin_15
#define LED_COL8_BV             GPIO_Pin_14
#define LED_COL9_BV             GPIO_Pin_13
#define LED_COL10_BV            GPIO_Pin_12
#define LED_COL11_BV            GPIO_Pin_11
#define LED_COL12_BV            GPIO_Pin_9
#define LED_COL13_BV            GPIO_Pin_7

#define LED_ROW0_BV             GPIO_Pin_6
#define LED_ROW1_BV             GPIO_Pin_5
#define LED_ROW2_BV             GPIO_Pin_3
#define LED_ROW3_BV             GPIO_Pin_2
#define LED_ROW4_BV             GPIO_Pin_1
#define LED_ROW5_BV             GPIO_Pin_0

#define LED_MOTOR_BV            GPIO_Pin_19
#define LED_I2C_SCL_BV          GPIO_Pin_13
#define LED_I2C_SDA_BV          GPIO_Pin_12
#define LED_MPR121_IRQ_BV       GPIO_Pin_19
#define LED_PS2_SCL_BV          GPIO_Pin_15
#define LED_PS2_SDA_BV          GPIO_Pin_14
#define LED_RGB_BV              GPIO_Pin_10
#define LED_BAT_ADC_BV          GPIO_Pin_8
#define LED_BAT_CHRG_BV         GPIO_Pin_16
#define LED_BAT_COM_BV          GPIO_Pin_17

#define LED1_OUT                (R32_PB_OUT)
#define LED2_OUT                0
#define LED3_OUT                0
#define LED4_OUT                0

#define LED_COL0_OUT            (R32_PA_OUT)
#define LED_COL1_OUT            (R32_PA_OUT)
#define LED_COL2_OUT            (R32_PA_OUT)
#define LED_COL3_OUT            (R32_PA_OUT)
#define LED_COL4_OUT            (R32_PA_OUT)
#define LED_COL5_OUT            (R32_PA_OUT)
#define LED_COL6_OUT            (R32_PA_OUT)
#define LED_COL7_OUT            (R32_PA_OUT)
#define LED_COL8_OUT            (R32_PA_OUT)
#define LED_COL9_OUT            (R32_PA_OUT)
#define LED_COL10_OUT           (R32_PA_OUT)
#define LED_COL11_OUT           (R32_PA_OUT)
#define LED_COL12_OUT           (R32_PA_OUT)
#define LED_COL13_OUT           (R32_PA_OUT)

#define LED_ROW0_OUT            (R32_PB_OUT)
#define LED_ROW1_OUT            (R32_PB_OUT)
#define LED_ROW2_OUT            (R32_PB_OUT)
#define LED_ROW3_OUT            (R32_PB_OUT)
#define LED_ROW4_OUT            (R32_PB_OUT)
#define LED_ROW5_OUT            (R32_PB_OUT)

#define LED_MOTOR_OUT           (R32_PB_OUT)
#define LED_I2C_SCL_OUT         (R32_PB_OUT)
#define LED_I2C_SDA_OUT         (R32_PB_OUT)
#define LED_MPR121_IRQ_OUT      (R32_PB_OUT)
#define LED_PS2_SCL_OUT         (R32_PB_OUT)
#define LED_PS2_SDA_OUT         (R32_PB_OUT)
#define LED_RGB_OUT             (R32_PA_OUT)
#define LED_BAT_ADC_OUT         (R32_PA_OUT)
#define LED_BAT_CHRG_OUT        (R32_PB_OUT)
#define LED_BAT_COM_OUT         (R32_PB_OUT)

#define LED1_DDR                (R32_PB_DIR |= LED1_BV)
#define LED2_DDR                0
#define LED3_DDR                0

#define LED_COL0_DDR            (R32_PA_DIR |= LED_COL0_BV)
#define LED_COL1_DDR            (R32_PA_DIR |= LED_COL1_BV)
#define LED_COL2_DDR            (R32_PA_DIR |= LED_COL2_BV)
#define LED_COL3_DDR            (R32_PA_DIR |= LED_COL3_BV)
#define LED_COL4_DDR            (R32_PA_DIR |= LED_COL4_BV)
#define LED_COL5_DDR            (R32_PA_DIR |= LED_COL5_BV)
#define LED_COL6_DDR            (R32_PA_DIR |= LED_COL6_BV)
#define LED_COL7_DDR            (R32_PA_DIR |= LED_COL7_BV)
#define LED_COL8_DDR            (R32_PA_DIR |= LED_COL8_BV)
#define LED_COL9_DDR            (R32_PA_DIR |= LED_COL9_BV)
#define LED_COL10_DDR           (R32_PA_DIR |= LED_COL10_BV)
#define LED_COL11_DDR           (R32_PA_DIR |= LED_COL11_BV)
#define LED_COL12_DDR           (R32_PA_DIR |= LED_COL12_BV)
#define LED_COL13_DDR           (R32_PA_DIR |= LED_COL13_BV)

#define LED_ROW0_DDR            (R32_PB_DIR |= LED_ROW0_BV)
#define LED_ROW1_DDR            (R32_PB_DIR |= LED_ROW1_BV)
#define LED_ROW2_DDR            (R32_PB_DIR |= LED_ROW2_BV)
#define LED_ROW3_DDR            (R32_PB_DIR |= LED_ROW3_BV)
#define LED_ROW4_DDR            (R32_PB_DIR |= LED_ROW4_BV)
#define LED_ROW5_DDR            (R32_PB_DIR |= LED_ROW5_BV)

#define LED_MOTOR_DDR           (R32_PB_DIR |= LED_MOTOR_BV)
#define LED_I2C_SCL_DDR         (R32_PB_DIR |= LED_I2C_SCL_BV)
#define LED_I2C_SDA_DDR         (R32_PB_DIR |= LED_I2C_SDA_BV)
#define LED_MPR121_IRQ_DDR      (R32_PB_DIR |= LED_MPR121_IRQ_BV)
#define LED_PS2_SCL_DDR         (R32_PB_DIR |= LED_PS2_SCL_BV)
#define LED_PS2_SDA_DDR         (R32_PB_DIR |= LED_PS2_SDA_BV)
#define LED_RGB_DDR             (R32_PA_DIR |= LED_RGB_BV)
#define LED_BAT_ADC_DDR         (R32_PA_DIR |= LED_BAT_ADC_BV)
#define LED_BAT_CHRG_DDR        (R32_PB_DIR |= LED_BAT_CHRG_BV)
#define LED_BAT_COM_DDR         (R32_PB_DIR |= LED_BAT_COM_BV)

#define HAL_TURN_OFF_LED1()     (LED1_OUT |= LED1_BV)
#define HAL_TURN_OFF_LED2()
#define HAL_TURN_OFF_LED3()
#define HAL_TURN_OFF_LED4()

#define HAL_TURN_OFF_LED(x)     (LED_ ## x ## _OUT &= (~LED_## x ## _BV))

#define HAL_TURN_ON_LED1()      (LED1_OUT &= (~LED1_BV))
#define HAL_TURN_ON_LED2()
#define HAL_TURN_ON_LED3()
#define HAL_TURN_ON_LED4()

#define HAL_TURN_ON_LED(x)      (LED_ ## x ## _OUT |= LED_## x ## _BV)

#define HAL_STATE_LED1()        0
#define HAL_STATE_LED2()        0
#define HAL_STATE_LED3()        0
#define HAL_STATE_LED4()        0

/*********************************************************************
 * GLOBAL VARIABLES
 */

/**
 * @brief   Initialize LED Service.
 */
void HAL_LedInit(void);

void HalLedTest(void);

/**
 * @brief   update time LED Service.
 */
void HalLedUpdate(void);

/**
 * @brief   Turn ON/OFF/TOGGLE given LEDs
 *
 * @param   led     - bit mask value of leds to be turned ON/OFF/TOGGLE
 * @param   mode    - BLINK, FLASH, TOGGLE, ON, OFF
 */
extern uint8_t HalLedSet(uint8_t led, uint8_t mode);

/**
 * @brief   Blink the leds
 *
 * @param   led         - bit mask value of leds to be turned ON/OFF/TOGGLE
 * @param   numBlinks   - number of blinks
 * @param   percent     - the percentage in each period where the led will be on
 * @param   period      - length of each cycle in milliseconds
 */
extern void HalLedBlink(uint8_t leds, uint8_t cnt, uint8_t duty, uint16_t time);

/**
 * @brief   Put LEDs in sleep state - store current values
 */
extern void HalLedEnterSleep(void);

/**
 * @brief   Retore LEDs from sleep state
 */
extern void HalLedExitSleep(void);

/**
 * @brief   Return LED state
 */
extern uint8_t HalLedGetState(void);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif
