/**
  ******************************************************************************
  * @file    eeprom.c
  * @author  STMicroelectronics - MCD Application Team
  * @version V2.0.0
  * @date    07/04/2011
  * @brief   EEPROM read/write routines
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
#include "eeprom.h"
#include "dali_regs.h"

#ifdef _COSMIC_
#include <iostm8s.h>
#endif

#ifdef _IAR_
__no_init EEPROM u8 eeprom_variable[125];
#else
EEPROM u8 eeprom_variable[125];
#endif


#define EEP_Wait_Finished() //while (!(FLASH->IAPSR & FLASH_IAPSR_HVOFF))

void E2_ResetEEPROM(void);
void E2_WriteSR(u8);
u8 E2_DetectMemSize(void);
u8 E2_ReadSR(void);


void EEPROM_Init(void)
{
  //unlock EEPROM
  FLASH->DUKR = 0xAE;
  FLASH->DUKR = 0x56;
  EEP_Wait_Finished();
  if ((eeprom_variable[0]=='T') && (eeprom_variable[1]=='G') && (eeprom_variable[2]==8) && (eeprom_variable[3]==0x00))
  { //test on EEPROM content validity see E2_ResetEEPROM()
    DALIR_LoadRegsFromE2();  // short_addr = E2_ReadMem(DALIREG_SHORT_ADDRESS - DALIREG_EEPROM_START);
  }
  else
  {
    DALIR_DeleteShort();
    DALIR_ResetRegs();
    E2_ResetEEPROM();
  }
  EEP_Wait_Finished();
}

void E2_ResetEEPROM(void)
{
  EEP_Wait_Finished();
  eeprom_variable[0] = 'T';   //"TG80" is written  in the EEPROM
  EEP_Wait_Finished();
  eeprom_variable[1] = 'G';   //This allows to check later that the EEPROM content is valid.
  EEP_Wait_Finished();
  eeprom_variable[2] = 8;
  EEP_Wait_Finished();
  eeprom_variable[3] = 0x00;
  EEP_Wait_Finished();
}

void E2_WriteMem(u8 addr, u8 val)
{
  EEP_Wait_Finished();
  if (eeprom_variable[addr+4] !=val)
    eeprom_variable[addr+4] = val;
  EEP_Wait_Finished();
}

void E2_WriteBurst(u8 addr, u8 times, u8 *buf)
{
  u8 address,i;
  EEP_Wait_Finished();
  address = addr + 4;
  i = 0;
  while (times--)
  {
    if (eeprom_variable[address+i] != buf[i])
      eeprom_variable[address+i] = buf[i];
    i++;
    EEP_Wait_Finished();
  }
}

u8 E2_ReadMem(u8 addr)
{
  EEP_Wait_Finished();
  return eeprom_variable[addr+4];
}
