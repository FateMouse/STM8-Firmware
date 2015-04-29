/**
  ******************************************************************************
  * @file    dali.h
  * @author  STMicroelectronics - MCD Application Team
  * @version V2.0.0
  * @date    07/04/2011
  * @brief   High level functions to init and execute DALI stack - header
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

#ifndef DALI_H
#define DALI_H

extern volatile u8 dali_address;
extern volatile u8 dali_data;
extern volatile u8 dali_receive_status;
extern volatile u8 dali_error;
extern u8 dali_state;

//callback function type for light control
typedef void TDLightControlCallback(u16 lighvalue);

/*---CONSTANTS---*/
/* Constants for dali_state*/
#define DALI_IDLE		0	/* DALI sender: Idle mode */
#define DALI_SEND_START		1	/* DALI sender: send Start Condition */
#define DALI_SEND_ADDRESS	2	/* DALI sender: Send Address Bits*/
#define DALI_SEND_DATA		3	/* DALI sender: Send Data Bits */
#define DALI_SEND_STOP		4	/* DALI sender: Send Stop Bits */
#define DALI_SEND_SETTLING	5       /* DALI sender: Wait Settling Time */
#define DALI_WAIT		6	/* Waiting for Answerframe */

/* Constants for dali_receive_status */
#define DALI_READY_TO_RECEIVE	0
#define DALI_NEW_FRAME_RECEIVED	1
#define DALI_RECEIVE_OVERFLOW	2

/* Constants for dali_error */
#define DALI_NO_ERROR 0
#define DALI_INTERFACE_FAILURE_ERROR 1

#define DALI_REPETITION_WAIT 	120  /*Command repetition timeout (ms)*/

/** public functions **/
void DALI_Init(TDLightControlCallback LightControlFunction);
u8 DALI_TimerStatus(void);
u8 DALI_CheckAndExecuteTimer(void);
u8 DALI_CheckAndExecuteReceivedCommand(void);
void DALI_halt(void);
void DALI_Set_Lamp_Failure(u8 failure);

void Send_DALI_Frame(u8);
u8 Get_DALI_Random(void);

#endif


