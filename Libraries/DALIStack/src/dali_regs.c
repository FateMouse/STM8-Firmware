/**
  ******************************************************************************
  * @file    dali_regs.c
  * @author  STMicroelectronics - MCD Application Team
  * @version V2.0.0
  * @date    07/04/2011
  * @brief   Dali Registers management
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
#include "dali_cmd.h"
#include "dali_regs.h"
#include "eeprom.h"
#include "dali_config.h"

uint8_t RAMRegs[DALIREG_RAM_END - DALIREG_RAM_START];
uint8_t short_addr;
uint8_t randbuf[2];

extern const uint8_t ROMRegs[2]; /* Declared in DALI_PUB.C */
extern const uint8_t DaliRegDefaults[];

#define DALIR_IsRAMReg(a) (a<DALIREG_RAM_END)
#define DALIR_IsEEPROMReg(a) ((a>=DALIREG_EEPROM_START) && (a<DALIREG_EEPROM_END))
#define DALIR_IsROMReg(a)  ((a>=DALIREG_ROM_START) && (a<DALIREG_ROM_END))
#define DALIR_IsValid(a) (a < DALI_NUMBER_REGS)

#if (DEVICE_TYPE == 6)          /* LED type device */
extern u8 DALIP_FastFade;
extern u8 DALIP_CurveType;
#endif

void DALIR_Init(void)
{
  uint8_t i;
  for (i = 0; i<5; i++) RAMRegs[i]=0;
}

void DALIR_WriteEEPROMReg(uint8_t idx, uint8_t val)
{

  if (idx == DALIREG_SHORT_ADDRESS - DALIREG_EEPROM_START) short_addr = val;
  E2_WriteMem(idx,val);
}

uint8_t DALIR_ReadEEPROMReg(uint8_t idx)
{
  if (idx == DALIREG_SHORT_ADDRESS - DALIREG_EEPROM_START) return short_addr;
  return E2_ReadMem(idx);
}

uint8_t DALIR_ReadReg(uint8_t idx)
{
  if (!DALIR_IsValid(idx)) return 0;
  if (DALIR_IsRAMReg(idx)) { return RAMRegs[idx - DALIREG_RAM_START];}
  if (DALIR_IsROMReg(idx)) { return ROMRegs[idx - DALIREG_ROM_START];}
  return DALIR_ReadEEPROMReg(idx - DALIREG_EEPROM_START);
}

void DALIR_WriteReg(uint8_t idx, uint8_t newval)
{
    uint8_t i;
    if (!DALIR_IsValid(idx)) return;
    if (DALIR_IsROMReg(idx)) return;
    if (DALIR_IsRAMReg(idx))
    {
        RAMRegs[idx - DALIREG_RAM_START] = newval;
    }
    else
    {
        DALIR_WriteEEPROMReg(idx - DALIREG_EEPROM_START, newval);
    }

    if (DALIR_ReadStatusBit(DALIREG_STATUS_FADE_READY)==0)
    {

        DALIR_WriteStatusBit(DALIREG_STATUS_RESET_STATE,1);   /*set reset State*/

        for (i=0; i<DALI_NUMBER_REGS; i++)
        {           //to refresh "reset state" bit
            switch (i)
            {
                case DALIREG_SHORT_ADDRESS:
                    break;
                case DALIREG_STATUS_INFORMATION:
                    break;
                case DALIREG_MIN_LEVEL:
                    if (DALIR_ReadReg(DALIREG_MIN_LEVEL)!= DALIR_ReadReg(DALIREG_PHYS_MIN_LEVEL))
                    DALIR_WriteStatusBit(DALIREG_STATUS_RESET_STATE,0); /*clear reset State*/;
                    break;
                default:
                    if(DALIR_IsEEPROMReg(i) || DALIR_IsRAMReg(i))
                    if (DALIR_ReadReg(i) != DaliRegDefaults[i])
                    DALIR_WriteStatusBit(DALIREG_STATUS_RESET_STATE,0); /*clear reset State*/
                    break;
            }
        }
    }
}

void DALIR_WriteStatusBit(uint8_t bit_nbr,uint8_t val)
{
    if (val == 0)
    {
        ClrBit(RAMRegs[DALIREG_STATUS_INFORMATION - DALIREG_RAM_START],bit_nbr);
    }
    else
    {
        SetBit(RAMRegs[DALIREG_STATUS_INFORMATION - DALIREG_RAM_START],bit_nbr);
    }
}

uint8_t DALIR_ReadStatusBit(uint8_t bit_nbr)
{
    return ValBit(RAMRegs[DALIREG_STATUS_INFORMATION - DALIREG_RAM_START],bit_nbr);
}

void DALIR_ResetRegs(void)
{
    uint8_t i;

    E2_WriteBurst(0,(u8)(DALIREG_EEPROM_END-DALIREG_EEPROM_START),(u8*)(&(DaliRegDefaults[DALIREG_EEPROM_START])));
    E2_WriteMem(DALIREG_SHORT_ADDRESS - DALIREG_EEPROM_START,short_addr);
    for (i=0; i<DALI_NUMBER_REGS; i++)
    {
        switch (i)
        {
            case DALIREG_SHORT_ADDRESS:
                break;
            case DALIREG_STATUS_INFORMATION:
                break;
            case DALIREG_MIN_LEVEL:
                DALIR_WriteReg(DALIREG_MIN_LEVEL,DALIR_ReadReg(DALIREG_PHYS_MIN_LEVEL));
                break;
            default:
                if(DALIR_IsEEPROMReg(i) || DALIR_IsRAMReg(i))
                {
                    DALIR_WriteReg(i,DaliRegDefaults[i]);
                }
                break;
        }
    }
    i=DALIR_ReadReg(DALIREG_STATUS_INFORMATION);
    i&=0x47;
    DALIR_WriteReg(DALIREG_STATUS_INFORMATION, i);
    DALIR_WriteStatusBit(DALIREG_STATUS_RESET_STATE,1); /*Set reset State*/
  #if (DEVICE_TYPE == 6)          /* LED type device */
    DALIP_FastFade = 0;
    DALIP_CurveType = 0;
  #endif
}

void DALIR_LoadRegsFromE2(void)
{
    short_addr = E2_ReadMem(DALIREG_SHORT_ADDRESS - DALIREG_EEPROM_START);
}

void DALIR_DeleteShort(void)
{
    short_addr = 0xFF;
    E2_WriteMem(DALIREG_SHORT_ADDRESS - DALIREG_EEPROM_START,0xFF);
    DALIR_WriteStatusBit(DALIREG_STATUS_MISSING_SHORT,1);
}


