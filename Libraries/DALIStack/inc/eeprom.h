/**
  ******************************************************************************
  * @file    eeprom.h
  * @author  STMicroelectronics - MCD Application Team
  * @version V2.0.0
  * @date    07/04/2011
  * @brief   EEPROM read/write routines - header
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

#ifndef EEPROM2_H
#define EEPORM2_H

/*---FUNCTIONS---*/

void EEPROM_Init (void);

void E2_WriteMem(u8, u8);
void E2_WriteBurst(u8, u8, u8*);
u8 E2_ReadMem(u8);


/*---CONSTANTS---*/
#define E2_PHYSICAL_SIZE	256

#endif

