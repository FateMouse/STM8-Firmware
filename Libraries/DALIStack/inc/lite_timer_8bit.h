/**
  ******************************************************************************
  * @file    lite_timer_8bit.h
  * @author  STMicroelectronics - MCD Application Team
  * @version V2.0.0
  * @date    07/04/2011
  * @brief   Timer routines for DALI timing management - header
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

#ifndef LITE_TIM2_H
#define LITE_TIM2_H

extern volatile u8 lite_timer_IT_state;

/*---FUNCTIONS---*/

void Timer_Lite_Init(void);
void PowerOnTimerReset(void);
void RTC_LaunchBigTimer(u8);
void RTC_LaunchTimer(u16);
void RTC_LaunchUserTimer(u8);
void RTC_DoneUserTimer(void);
void RTC_LaunchDAPCTimer(void);
void RTC_DoneDAPCTimer(void);
u8 Process_Lite_timer_IT(void); //SESE
void Lite_timer_Interrupt(void);


/*---Variables---*/

extern u8  RealTimeClock_BigTimer;
extern u16 RealTimeClock_TimerCountDown;

#endif


