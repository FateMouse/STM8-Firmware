/**
  ******************************************************************************
  * @file    dali.c
  * @author  STMicroelectronics - MCD Application Team
  * @version V2.0.0
  * @date    07/04/2011
  * @brief   High level functions to init and execute DALI stack
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
#include "dali.h"
#include "DALIslave.h"
#include "dali_config.h"
#include "lite_timer_8bit.h"
#include "dali_cmd.h"
#include "dali_pub.h"
#include "dali_regs.h"
#include "eeprom.h"


volatile u8 dali_address;
volatile u8 dali_data;
volatile u8 dali_receive_status;
volatile u8 dali_error;

u8 dali_state;


/*-----------------------------------------------------------------------------
ROUTINE NAME : DALI_Interrupt
INPUT/OUTPUT : None
DESCRIPTION  : DALI receiving callback
COMMENTS     :
-----------------------------------------------------------------------------*/
void DALI_Interrupt(u8 address,u8 dataByte)
{
  dali_address = address; 		// read DALI forward address
  dali_data = dataByte;		// read DALI forward data;
  dali_receive_status = DALI_NEW_FRAME_RECEIVED;
}

/*-----------------------------------------------------------------------------
ROUTINE NAME : DALI_Error
INPUT/OUTPUT : None
DESCRIPTION  : DALI Error callback
COMMENTS     :
-----------------------------------------------------------------------------*/
void DALI_Error(u8 code_val)
{
  switch (code_val)
  {
    case 1:  dali_error = DALI_INTERFACE_FAILURE_ERROR; break;
    default: dali_error = DALI_NO_ERROR; break;
  }
}

/*-----------------------------------------------------------------------------
ROUTINE NAME : DALI_Init
INPUT/OUTPUT : None
DESCRIPTION  : Configure the DALI Module
COMMENTS     :
-----------------------------------------------------------------------------*/
void DALI_Init(TDLightControlCallback LightControlFunction)
{
  /* Pull-up Vdd pin for data output */
  DALI_PULLUP_PORT->ODR |= (1<<DALI_PULLUP_PIN); //high level
  DALI_PULLUP_PORT->DDR |= (1<<DALI_PULLUP_PIN); //output mode
  DALI_PULLUP_PORT->CR1 |= (1<<DALI_PULLUP_PIN); //push-pull
  DALI_PULLUP_PORT->CR2 |= (1<<DALI_PULLUP_PIN); //slow slope

  /* button pin for data input */
  DALI_BUTTON_PORT->DDR &= ~(1<<DALI_BUTTON_PIN); //input mode
  DALI_BUTTON_PORT->ODR &= ~(1<<DALI_BUTTON_PIN); //low level
  DALI_BUTTON_PORT->CR1 |=  (1<<DALI_BUTTON_PIN); //pull-up
  DALI_BUTTON_PORT->CR2 &= ~(1<<DALI_BUTTON_PIN); //interrupt disable on pin

  /* dali flags init */
  dali_state = DALI_IDLE;
  dali_receive_status = DALI_READY_TO_RECEIVE;

  /* Initialisation of DALI stack modules*/
  Timer_Lite_Init();
  EEPROM_Init();
  DALIR_Init();
  DALIP_Init(LightControlFunction);
  DALIC_Init();

  /* Initialisation of DALI IO driver */
  init_DALI(OUT_DALI_PORT, OUT_DALI_PIN, INVERT_OUT_DALI, IN_DALI_PORT, IN_DALI_PIN, INVERT_IN_DALI, DALI_Interrupt, DALI_Error, Lite_timer_Interrupt);
}

/*-----------------------------------------------------------------------------
ROUTINE NAME : DALI_TimerStatus
INPUT/OUTPUT : returns if 1ms timer elapses
DESCRIPTION  : checks if 1ms timer is elapsed
COMMENTS     :
-----------------------------------------------------------------------------*/
u8 DALI_TimerStatus(void)
{
  return lite_timer_IT_state;
}

/*-----------------------------------------------------------------------------
ROUTINE NAME : DALI_CheckAndExecuteTimer
INPUT/OUTPUT : returns if user timer is active
DESCRIPTION  : checks if user timer is active and performs timer action if necessary
COMMENTS     :
-----------------------------------------------------------------------------*/
u8 DALI_CheckAndExecuteTimer(void)
{
  static u8 TimerActive = 0;

  if(lite_timer_IT_state==1) //set every 1 ms by timer
  {
    TimerActive = Process_Lite_timer_IT(); //manage fade effects each 1ms (fade time and fade rate)
  }
  return TimerActive;
}

/*-----------------------------------------------------------------------------
ROUTINE NAME : DALI_CheckAndExecuteReceivedCommand
INPUT/OUTPUT : returns if some commands was active and executed
DESCRIPTION  : checks if some received command is pending and if then execute it
COMMENTS     :
-----------------------------------------------------------------------------*/
u8 DALI_CheckAndExecuteReceivedCommand(void)
{
  //check received data
  if(dali_receive_status == DALI_NEW_FRAME_RECEIVED)
  {
    if (DALIC_isTalkingToMe())
    {
      DALIC_ProcessCommand();
      dali_receive_status = DALI_READY_TO_RECEIVE;
      return 1;
    }
    dali_receive_status = DALI_READY_TO_RECEIVE;
  }

  //check error
  if(dali_error == DALI_INTERFACE_FAILURE_ERROR)
  {
    //error management
    DALIC_Process_System_Failure();
    dali_error = DALI_NO_ERROR;
    return 2;
  }

  return 0;
}

/*-----------------------------------------------------------------------------
ROUTINE NAME : DALI_halt
INPUT/OUTPUT : none
DESCRIPTION  : checks if DALI packed receiving/sending is not in progress and go to halt if not
COMMENTS     :
-----------------------------------------------------------------------------*/
void DALI_halt(void)
{
  sim(); //disable interrupts (to not start receiving)
  if ((dali_receive_status != DALI_NEW_FRAME_RECEIVED) && (get_flag() == NO_ACTION) )  //if DALI frame receiving in progress
  {
    halt();
  }
  rim(); //enable interrupts
}


/*-----------------------------------------------------------------------------
ROUTINE NAME : Send_DALI_Frame
INPUT/OUTPUT : None
DESCRIPTION  :
COMMENTS     :
 ************************************************
 * Initializes Sending but returns immediately!	 *
 * Sending continues asynchronously.             *
 ************************************************
-----------------------------------------------------------------------------*/
void Send_DALI_Frame(u8 data_val)
{
  send_data(data_val);
  dali_state = DALI_SEND_START;
}

/*-----------------------------------------------------------------------------
ROUTINE NAME : Get_DALI_Random
INPUT/OUTPUT : None
DESCRIPTION  :
COMMENTS     :
 ************************************************
 * Returns random number	 *
 ************************************************
-----------------------------------------------------------------------------*/
u8 Get_DALI_Random(void)
{
  static u8 RandomCounter;
  u8 TimerCount;

  RandomCounter ++;
  TimerCount = get_timer_count();
  switch (RandomCounter & 0x03)
  {
    case 0:  return (TimerCount); break;
    case 1:  return (~TimerCount); break;
    case 2:  return ((TimerCount >> 1)^(TimerCount   )); break;
    default: return ((TimerCount     )^(RandomCounter)); break;
  }
}

/*-----------------------------------------------------------------------------
ROUTINE NAME : DALI_Set_Lamp_Failure
INPUT/OUTPUT : is error
DESCRIPTION  : Notification that Lamp has hardware error
COMMENTS     :
 ************************************************
 * Returns random number	 *
 ************************************************
-----------------------------------------------------------------------------*/
void DALI_Set_Lamp_Failure(u8 failure)
{
  DALIP_SetLampFailureFlag(failure);
}
