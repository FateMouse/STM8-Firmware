/**
  ******************************************************************************
  * @file    lite_timer_8bit.c
  * @author  STMicroelectronics - MCD Application Team
  * @version V2.0.0
  * @date    07/04/2011
  * @brief   Timer routines for DALI timing management
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

#include "stm8s.h"
#include "lite_timer_8bit.h"
#include "dali_pub.h"
#include "dali_cmd.h"

/* file global variable */
volatile u8 lite_timer_IT_state;

u16 timercounter;
u8  bigtimermins;
u16 bigtimertics;
u8  RealTimeClock_BigTimer;
u16 RealTimeClock_TimerCountDown;
u8  UserTimerActive;
u8  DAPCTimerActive;
u16 PowerOnTimerActive;
u16 BusFailureTimer;


/* Configure the Timer Lite */
void Timer_Lite_Init(void)
{
  PowerOnTimerActive = 600; //600 ms timeout after power up
  UserTimerActive = 0;
  DAPCTimerActive = 0;
}

void PowerOnTimerReset(void)
{
  PowerOnTimerActive = 0;
}

void RTC_LaunchBigTimer(u8 mins)
{
  bigtimertics = 60000; /* 60000*1ms=1mn*/
  bigtimermins = mins-1; /* Timer is launched for (mins-1)*1mn (basically 15mn, see DALI specifications)*/
  RealTimeClock_BigTimer = 1;
}

void RTC_LaunchTimer(u16 timer_value)
{
  RealTimeClock_TimerCountDown=timer_value;
}

void RTC_LaunchUserTimer(u8 TimerCount)
{
  UserTimerActive=TimerCount;
}

void RTC_DoneUserTimer(void)
{
  UserTimerActive=0;
}

void RTC_LaunchDAPCTimer(void)
{
  DAPCTimerActive=200;
}

void RTC_DoneDAPCTimer(void)
{
  DAPCTimerActive=0;
}

u8 Process_Lite_timer_IT(void)
{
  if (UserTimerActive)
  {
    if (UserTimerActive!=0xFF) UserTimerActive--;
    DALIP_TimerCallback();
    if (UserTimerActive==0)
      DALIP_SetFadeReadyFlag(0); /* fade is ready */
  }
  if (PowerOnTimerActive)
  {
    PowerOnTimerActive--;
    if (!PowerOnTimerActive)
    {
      DALIC_PowerOn();
    }
  }

  if (DAPCTimerActive)
  {
    DAPCTimerActive--;
    if (!DAPCTimerActive)
    {
      DALIP_Stop_DAPC_Sequence();
    }
  }

  lite_timer_IT_state=0;
  return (UserTimerActive + PowerOnTimerActive + DAPCTimerActive);
}


/*  for calling every 1ms - callback function */
void Lite_timer_Interrupt(void)
{
    if (bigtimertics)
    {
      bigtimertics--;
    }
    else
    {
      if (bigtimermins)
      {
        bigtimermins--;
        bigtimertics = 60000;  /* 60000*1ms=1mn*/
      }
      else
      {
        RealTimeClock_BigTimer = 0;
      }
    }
    if (RealTimeClock_TimerCountDown)
    {
      RealTimeClock_TimerCountDown--;
    }
    lite_timer_IT_state=1;
}

