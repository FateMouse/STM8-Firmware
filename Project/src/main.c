/**
  ******************************************************************************
  * @file    main.c
  * @author  STMicroelectronics - MCD Application Team
  * @version V2.0.0
  * @date    07/04/2011
  * @brief   Main Routine for DALI library usage on STM8S discovery board
  ******************************************************************************
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */


/* Include files */
#include "stm8s.h"
#include "dali_config.h"
#include "dali.h"
#include "lite_timer_8bit.h"
#include "dali_cmd.h"
#include "dali_pub.h"
#include "dali_regs.h"
#include "eeprom.h"


/* ------------------------- Code header section ------------------------- */
/*ST is writing some data at the beginning of the code, in order to give some
information about it. A typical appearance is:
"DALIbySTM8DALI T"
"Version V1.0"
"(c)2010 by ST  D"
*/

/* -- Arc table option --
If the arc table mode is enable, a T is to be displayed.
For more information about he arc table mode, please read the documentation */
#ifdef USE_ARC_TABLE
  #define ARC_TYPE "T"
#else
  #define ARC_TYPE "D"
#endif

/* -- Debug option --
If the debuging mode is enable, a D is to be displayed.
For more information about he arc table mode, please read the documentation */
#ifdef DEBUG
  #define DEBUGSTRING "D"
#else
  #define DEBUGSTRING " "
#endif

/* -- Version number -- */
#define _VERSION_ "Version V1.0"


CONST uint8_t version[3][20]={
  "DALIbySTM8DALI " ARC_TYPE,
  _VERSION_,
  "(c)2010 by ST  " DEBUGSTRING
};
/* -----------------------End of code header section ----------------------*/


/* global variables */
#define LOW_POWER_TIMEOUT      2000  // 2 seconds to go to sleep/halt
volatile u8 LEDlight;                // current light level
u16 HALTtimer;                       // timeout counter for low power mode

/* control of light level callback function - must be type TLightControlCallback - see dali.h */
/* PWM for LED light control on STM8S discovery board */
void PWM_LED(u16 lightlevel)
{
  /* PD0 pin - TIM3_CH2 */
  #define LED_PORT GPIOD
  #define LED_PIN  0

  // PWM uses timer 3 on Discovery board
  LED_PORT->DDR |= 1<<LED_PIN; // output mode
  LED_PORT->CR1 |= 1<<LED_PIN; // push-pull

  TIM3->PSCR  = 0;             // prescaller to 1
  TIM3->CCMR2 = 0x60;          // PWM mode1
  TIM3->CCER1 = 0x30;          // enable OC2 output pin (TIM3_CH2), polarity active low (LED from Vdd)
  TIM3->ARRH  = 0xFF;          // PWM period to 0xFFFF cycles  - #ifdef USE_ARC_TABLE is enabled in "dali_config.h"
  TIM3->ARRL  = 0xFF;
  TIM3->CCR2H = lightlevel >>   8;          // duty cycle is light level - #ifdef USE_ARC_TABLE is enabled in "dali_config.h"
  TIM3->CCR2L = lightlevel & 0xFF;
  TIM3->CR1  |= TIM3_CR1_CEN;  // enable PWM counter

  LEDlight = lightlevel;       // store current light level to global variable (for entering into halt if zero)
}


/* main program loop */
void main(void)
{
  /* Dummy access to avoid removal by compiler optimisation */
  __IO u8 t;
  u8 s = 0;
  s=version[s][s];
  t=s;

  /* Initialisation of DALI */
  DALI_Init(PWM_LED);
  /* End of initialisation */

  /* sleep/halt coudown counter */
  HALTtimer = LOW_POWER_TIMEOUT;
  LEDlight = 0;


  /* main program loop */
  while(1)
  {
    /* -------------------------------------------------------------------------------- */
    if (DALI_TimerStatus()) // must be checked at least each 1ms
    {
      if (HALTtimer) // countdown timeout if no activity in timer
        HALTtimer--;
      if (DALI_CheckAndExecuteTimer())  // need to call this function under 1ms interval periodically (fading function)
        HALTtimer = LOW_POWER_TIMEOUT;  // restart 10seconds timeout if some activity in timer
    }
    /* -------------------------------------------------------------------------------- */
    if (DALI_CheckAndExecuteReceivedCommand()) //need to call this function periodically (receive and process DALI command)
    {
      HALTtimer = LOW_POWER_TIMEOUT;    // restart 10seconds timeout if received and executed command
      Physically_Selected = !(DALI_BUTTON_PORT->IDR & (1<<DALI_BUTTON_PIN));   // physical selection = pushbutton in GND
    }
    /* -------------------------------------------------------------------------------- */
    if (!HALTtimer) // go to power save state (WFI or HALT)
    {
      if (LEDlight) // go to sleep or halt according light level (level "0" = power off = halt)
      {
        wfi();       // enable sleep only: PWM function requires continuous run and/or interrupts
      }
      else
      {
        DALI_halt();     // enable halt: PWM function is off - not requires continuous run and/or not uses interrupts
        HALTtimer = 600; // wake-up = DALI bus changed - command is receiving, 600ms to receive command and check bus errors
      }
    }
    /* -------------------------------------------------------------------------------- */
    if (!(TIM3->CR1 & TIM3_CR1_CEN))   // if PWM counter is not running (hardware error)
      DALI_Set_Lamp_Failure(1);        // set Lamp failure
    else
      DALI_Set_Lamp_Failure(0);        // reset Lamp failure
    /* -------------------------------------------------------------------------------- */
  } /* while(1) loop */
  
} /* main program loop */

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif