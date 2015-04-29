/**
  ******************************************************************************
  * @file    dali_cmd.h
  * @author  STMicroelectronics - MCD Application Team
  * @version V2.0.0
  * @date    07/04/2011
  * @brief   DALI commands implementation (according DALI spec) - header
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

#ifndef DALI_CMD_H
#define DALI_CMD_H

/** public data **/
/** public functions **/

void  DALIC_Init(void);
u8 	  DALIC_isTalkingToMe(void);
void  DALIC_ProcessCommand(void);
void  DALIC_Process_System_Failure(void);
void  DALIC_PowerOn(void);

#endif


