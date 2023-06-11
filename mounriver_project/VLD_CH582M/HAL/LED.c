/********************************** (C) COPYRIGHT *******************************
 * File Name          : LED.c
 * Author             : WCH, ChnMasterOG
 * Version            : V1.2
 * Date               : 2023/06/04
 * Description        :
 *********************************************************************************
 * Copyright (c) 2023 ChnMasterOG
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/******************************************************************************/
/* 头文件包含 */
#include "HAL.h"

/* LED control structure */
typedef struct
{
    uint8_t  mode;  /* Operation mode */
    uint8_t  todo;  /* Blink cycles left */
    uint8_t  onPct; /* On cycle percentage */
    uint16_t time;  /* On/off cycle time (msec) */
    uint32_t next;  /* Time for next change */
} HalLedControl_t;

typedef struct
{
    HalLedControl_t HalLedControlTable[HAL_LED_DEFAULT_MAX_LEDS];
    uint8_t         sleepActive;
} HalLedStatus_t;

/***************************************************************************************************
 *                                           GLOBAL VARIABLES
 ***************************************************************************************************/
static uint8_t HalLedState; // LED state at last set/clr/blink update

static uint8_t preBlinkState; // Original State before going to blink mode
                              // bit 0, 1, 2, 3 represent led 0, 1, 2, 3
static HalLedStatus_t HalLedStatusControl;

/***************************************************************************************************
 *                                            LOCAL FUNCTION
 ***************************************************************************************************/
void HalLedOnOff(uint8_t leds, uint8_t mode);

/***************************************************************************************************
 *                                            FUNCTIONS - API
 ***************************************************************************************************/

/*********************************************************************
 * @fn      HAL_LedInit
 *
 * @brief   Initialize LED Service
 *
 * @return  none
 */
void HAL_LedInit(void)
{
#if 0
    /* Initialize all LEDs to OFF */
    LED1_DDR;
    HalLedSet(HAL_LED_ALL, HAL_LED_MODE_OFF);
    // just test
    HalLedBlink(HAL_LED_1, 10, 30, 4000);
    /* Initialize sleepActive to FALSE */
    HalLedStatusControl.sleepActive = FALSE;
#endif
    HAL_TURN_OFF_LED(COL0);
    HAL_TURN_OFF_LED(COL1);
    HAL_TURN_OFF_LED(COL2);
    HAL_TURN_OFF_LED(COL3);
    HAL_TURN_OFF_LED(COL4);
    HAL_TURN_OFF_LED(COL5);
    HAL_TURN_OFF_LED(COL6);
    HAL_TURN_OFF_LED(COL7);
    HAL_TURN_OFF_LED(COL8);
    HAL_TURN_OFF_LED(COL9);
    HAL_TURN_OFF_LED(COL10);
    HAL_TURN_OFF_LED(COL11);
    HAL_TURN_OFF_LED(COL12);
    HAL_TURN_OFF_LED(COL13);
    HAL_TURN_OFF_LED(ROW0);
    HAL_TURN_OFF_LED(ROW1);
    HAL_TURN_OFF_LED(ROW2);
    HAL_TURN_OFF_LED(ROW3);
    HAL_TURN_OFF_LED(ROW4);
    HAL_TURN_OFF_LED(ROW5);
    HAL_TURN_OFF_LED(MOTOR);
    HAL_TURN_OFF_LED(PS2_SCL);
    HAL_TURN_OFF_LED(PS2_SDA);
    HAL_TURN_OFF_LED(RGB);
    HAL_TURN_OFF_LED(BAT_ADC);
    HAL_TURN_OFF_LED(BAT_CHRG);
    HAL_TURN_OFF_LED(BAT_COM);

    LED_COL0_DDR;
    LED_COL1_DDR;
    LED_COL2_DDR;
    LED_COL3_DDR;
    LED_COL4_DDR;
    LED_COL5_DDR;
    LED_COL6_DDR;
    LED_COL7_DDR;
    LED_COL8_DDR;
    LED_COL9_DDR;
    LED_COL10_DDR;
    LED_COL11_DDR;
    LED_COL12_DDR;
    LED_COL13_DDR;
    LED_ROW0_DDR;
    LED_ROW1_DDR;
    LED_ROW2_DDR;
    LED_ROW3_DDR;
    LED_ROW4_DDR;
    LED_ROW5_DDR;
    LED_MOTOR_DDR;
    LED_I2C_SCL_DDR;
    LED_I2C_SDA_DDR;
    LED_MPR121_IRQ_DDR;
    LED_PS2_SCL_DDR;
    LED_PS2_SDA_DDR;
    LED_RGB_DDR;
    LED_BAT_ADC_DDR;
    LED_BAT_CHRG_DDR;
    LED_BAT_COM_DDR;
}

void HalLedTest(void)
{
    static uint8_t led_num = 0;

    switch(led_num)
    {
        case 0:
            HAL_TURN_OFF_LED(BAT_COM);
            HAL_TURN_ON_LED(COL0);
            break;
        case 1:
            HAL_TURN_OFF_LED(COL0);
            HAL_TURN_ON_LED(COL1);
            break;
        case 2:
            HAL_TURN_OFF_LED(COL1);
            HAL_TURN_ON_LED(COL2);
            break;
        case 3:
            HAL_TURN_OFF_LED(COL2);
            HAL_TURN_ON_LED(COL3);
            break;
        case 4:
            HAL_TURN_OFF_LED(COL3);
            HAL_TURN_ON_LED(COL4);
            break;
        case 5:
            HAL_TURN_OFF_LED(COL4);
            HAL_TURN_ON_LED(COL5);
            break;
        case 6:
            HAL_TURN_OFF_LED(COL5);
            HAL_TURN_ON_LED(COL6);
            break;
        case 7:
            HAL_TURN_OFF_LED(COL6);
            HAL_TURN_ON_LED(COL7);
            break;
        case 8:
            HAL_TURN_OFF_LED(COL7);
            HAL_TURN_ON_LED(COL8);
            break;
        case 9:
            HAL_TURN_OFF_LED(COL8);
            HAL_TURN_ON_LED(COL9);
            break;
        case 10:
            HAL_TURN_OFF_LED(COL9);
            HAL_TURN_ON_LED(COL10);
            break;
        case 11:
            HAL_TURN_OFF_LED(COL10);
            HAL_TURN_ON_LED(COL11);
            break;
        case 12:
            HAL_TURN_OFF_LED(COL11);
            HAL_TURN_ON_LED(COL12);
            break;
        case 13:
            HAL_TURN_OFF_LED(COL12);
            HAL_TURN_ON_LED(COL13);
            break;
        case 14:
            HAL_TURN_OFF_LED(COL13);
            HAL_TURN_ON_LED(ROW0);
            break;
        case 15:
            HAL_TURN_OFF_LED(ROW0);
            HAL_TURN_ON_LED(ROW1);
            break;
        case 16:
            HAL_TURN_OFF_LED(ROW1);
            HAL_TURN_ON_LED(ROW2);
            break;
        case 17:
            HAL_TURN_OFF_LED(ROW2);
            HAL_TURN_ON_LED(ROW3);
            break;
        case 18:
            HAL_TURN_OFF_LED(ROW3);
            HAL_TURN_ON_LED(ROW4);
            break;
        case 19:
            HAL_TURN_OFF_LED(ROW4);
            HAL_TURN_ON_LED(ROW5);
            break;
        case 20:
            HAL_TURN_OFF_LED(ROW5);
            HAL_TURN_ON_LED(MOTOR);
            break;
        case 21:
            HAL_TURN_OFF_LED(MOTOR);
            HAL_TURN_ON_LED(I2C_SCL);
            break;
        case 22:
            HAL_TURN_OFF_LED(I2C_SCL);
            HAL_TURN_ON_LED(I2C_SDA);
            break;
        case 23:
            HAL_TURN_OFF_LED(I2C_SDA);
            HAL_TURN_ON_LED(MPR121_IRQ);
            break;
        case 24:
            HAL_TURN_OFF_LED(MPR121_IRQ);
            HAL_TURN_ON_LED(PS2_SCL);
            break;
        case 25:
            HAL_TURN_OFF_LED(PS2_SCL);
            HAL_TURN_ON_LED(PS2_SDA);
            break;
        case 26:
            HAL_TURN_OFF_LED(PS2_SDA);
            HAL_TURN_ON_LED(RGB);
            break;
        case 27:
            HAL_TURN_OFF_LED(RGB);
            HAL_TURN_ON_LED(BAT_ADC);
            break;
        case 28:
            HAL_TURN_OFF_LED(BAT_ADC);
            HAL_TURN_ON_LED(BAT_CHRG);
            break;
        case 29:
            HAL_TURN_OFF_LED(BAT_CHRG);
            HAL_TURN_ON_LED(BAT_COM);
            break;
        default:
            break;
    }

    led_num++;
    if (led_num >= 30) led_num = 0;
}

/*********************************************************************
 * @fn      HalLedSet
 *
 * @brief   Turn ON/OFF/TOGGLE given LEDs
 *
 * @param   led     - bit mask value of leds to be turned ON/OFF/TOGGLE
 * @param   mode    - BLINK, FLASH, TOGGLE, ON, OFF
 *
 * @return  0
 */
uint8_t HalLedSet(uint8_t leds, uint8_t mode)
{
    uint8_t          led;
    HalLedControl_t *sts;

    switch(mode)
    {
        case HAL_LED_MODE_BLINK:
        {
            /* Default blink, 1 time, D% duty cycle */
            HalLedBlink(leds, 1, HAL_LED_DEFAULT_DUTY_CYCLE, HAL_LED_DEFAULT_FLASH_TIME);
            break;
        }

        case HAL_LED_MODE_FLASH:
        {
            /* Default flash, N times, D% duty cycle */
            HalLedBlink(leds, HAL_LED_DEFAULT_FLASH_COUNT, HAL_LED_DEFAULT_DUTY_CYCLE, HAL_LED_DEFAULT_FLASH_TIME);
            break;
        }

        case HAL_LED_MODE_ON:
        case HAL_LED_MODE_OFF:
        case HAL_LED_MODE_TOGGLE:
        {
            led = HAL_LED_1;
            leds &= HAL_LED_ALL;
            sts = HalLedStatusControl.HalLedControlTable;
            while(leds)
            {
                if(leds & led)
                {
                    if(mode != HAL_LED_MODE_TOGGLE)
                    {
                        sts->mode = mode; /* ON or OFF */
                    }
                    else
                    {
                        sts->mode ^= HAL_LED_MODE_ON; /* Toggle */
                    }
                    HalLedOnOff(led, sts->mode);
                    leds ^= led;
                }
                led <<= 1;
                sts++;
            }
            break;
        }

        default:
            break;
    }
    return (0);
}

/*********************************************************************
 * @fn      HalLedBlink
 *
 * @brief   Blink the leds
 *
 * @param   led         - bit mask value of leds to be turned ON/OFF/TOGGLE
 * @param   numBlinks   - number of blinks
 * @param   percent     - the percentage in each period where the led will be on
 * @param   period      - length of each cycle in milliseconds
 *
 * @return  none
 */
void HalLedBlink(uint8_t leds, uint8_t numBlinks, uint8_t percent, uint16_t period)
{
    uint8_t          led;
    HalLedControl_t *sts;

    if(leds && percent && period)
    {
        if(percent < 100)
        {
            led = HAL_LED_1;
            leds &= HAL_LED_ALL;
            sts = HalLedStatusControl.HalLedControlTable;
            while(leds)
            {
                if(leds & led)
                {
                    /* Store the current state of the led before going to blinking */
                    preBlinkState |= (led & HalLedState);
                    sts->mode = HAL_LED_MODE_OFF; /* Stop previous blink */
                    sts->time = period;           /* Time for one on/off cycle */
                    sts->onPct = percent;         /* % of cycle LED is on */
                    sts->todo = numBlinks;        /* Number of blink cycles */
                    if(!numBlinks)
                    {
                        sts->mode |= HAL_LED_MODE_FLASH; /* Continuous */
                    }
                    sts->next = TMOS_GetSystemClock(); /* Start now */
                    sts->mode |= HAL_LED_MODE_BLINK;   /* Enable blinking */
                    leds ^= led;
                }
                led <<= 1;
                sts++;
            }
            tmos_start_task(halTaskID, LED_BLINK_EVENT, 0);
        }
        else
        {
            HalLedSet(leds, HAL_LED_MODE_ON); /* >= 100%, turn on */
        }
    }
    else
    {
        HalLedSet(leds, HAL_LED_MODE_OFF); /* No on time, turn off */
    }
}

/*********************************************************************
 * @fn      HalLedUpdate
 *
 * @brief   Update leds to work with blink
 *
 * @return  none
 */
void HalLedUpdate(void)
{
    uint8_t          led, pct, leds;
    uint16_t         next, wait;
    uint32_t         time;
    HalLedControl_t *sts;

    next = 0;
    led = HAL_LED_1;
    leds = HAL_LED_ALL;
    sts = HalLedStatusControl.HalLedControlTable;

    /* Check if sleep is active or not */
    if(!HalLedStatusControl.sleepActive)
    {
        while(leds)
        {
            if(leds & led)
            {
                if(sts->mode & HAL_LED_MODE_BLINK)
                {
                    time = TMOS_GetSystemClock();
                    if(time >= sts->next)
                    {
                        if(sts->mode & HAL_LED_MODE_ON)
                        {
                            pct = 100 - sts->onPct;             /* Percentage of cycle for off */
                            sts->mode &= ~HAL_LED_MODE_ON;      /* Say it's not on */
                            HalLedOnOff(led, HAL_LED_MODE_OFF); /* Turn it off */
                            if(!(sts->mode & HAL_LED_MODE_FLASH))
                            {
                                if(sts->todo != 0xff)
                                {
                                    sts->todo--; /* Not continuous, reduce count */
                                }
                                if(!sts->todo)
                                {
                                    sts->mode ^= HAL_LED_MODE_BLINK; /* No more blinks */
                                }
                            }
                        }
                        else
                        {
                            pct = sts->onPct;                  /* Percentage of cycle for on */
                            sts->mode |= HAL_LED_MODE_ON;      /* Say it's on */
                            HalLedOnOff(led, HAL_LED_MODE_ON); /* Turn it on */
                        }
                        if(sts->mode & HAL_LED_MODE_BLINK)
                        {
                            wait = (((uint32_t)pct * (uint32_t)sts->time) / 100);
                            sts->next = time + wait;
                        }
                        else
                        {
                            /* no more blink, no more wait */
                            wait = 0;
                            /* After blinking, set the LED back to the state before it blinks */
                            HalLedSet(led, ((preBlinkState & led) != 0) ? HAL_LED_MODE_ON : HAL_LED_MODE_OFF);
                            /* Clear the saved bit */
                            preBlinkState &= (led ^ 0xFF);
                        }
                    }
                    else
                    {
                        wait = sts->next - time; /* Time left */
                    }
                    if(!next || (wait && (wait < next)))
                    {
                        next = wait;
                    }
                }
                leds ^= led;
            }
            led <<= 1;
            sts++;
        }
        if(next)
        {
            tmos_start_task(halTaskID, LED_BLINK_EVENT, next); /* Schedule event */
        }
    }
}

/*********************************************************************
 * @fn      HalLedOnOff
 *
 * @brief   Turns specified LED ON or OFF
 *
 * @param   led         - LED bit mask
 * @param   mode        - LED_ON,LED_OFF,
 *
 * @return  none
 */
void HalLedOnOff(uint8_t leds, uint8_t mode)
{
    if(leds & HAL_LED_1)
    {
        if(mode == HAL_LED_MODE_ON)
        {
            HAL_TURN_ON_LED1();
        }
        else
        {
            HAL_TURN_OFF_LED1();
        }
    }
    if(leds & HAL_LED_2)
    {
        if(mode == HAL_LED_MODE_ON)
        {
            HAL_TURN_ON_LED2();
        }
        else
        {
            HAL_TURN_OFF_LED2();
        }
    }
    if(leds & HAL_LED_3)
    {
        if(mode == HAL_LED_MODE_ON)
        {
            HAL_TURN_ON_LED3();
        }
        else
        {
            HAL_TURN_OFF_LED3();
        }
    }
    if(leds & HAL_LED_4)
    {
        if(mode == HAL_LED_MODE_ON)
        {
            HAL_TURN_ON_LED4();
        }
        else
        {
            HAL_TURN_OFF_LED4();
        }
    }
    /* Remember current state */
    if(mode)
    {
        HalLedState |= leds;
    }
    else
    {
        HalLedState &= (leds ^ 0xFF);
    }
}

/***************************************************************************************************
 * @fn      HalGetLedState
 *
 * @brief   Dim LED2 - Dim (set level) of LED2
 *
 * @return  led state
 */
uint8_t HalLedGetState()
{
    return HalLedState;
}

/******************************** endfile @ led ******************************/
