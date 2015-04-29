/**************************************************
 *
 * Utility code for automatic EEPROM memory access.
 *
 * Copyright 2010 IAR Systems AB.
 *
 * $Revision: 2051 $
 *
 **************************************************/
#include "stm8s_flash.h"
#pragma language=extended

/*
 * The user must implement the three extern-declared functions below
 * in order for the compiler to be able to automatically write to the
 * EEPROM memory when __eeprom variables are assigned to.
 */

/*
 * Write one byte to the data EEPROM memory.
 */

void __eeprom_program_byte(unsigned char __near * dst, unsigned char v)
{
  FLASH_ProgramByte((u32)dst, (u8)v);
}

/*
 * Write one 4-byte long word to the data EEPROM memory.  The address
 * must be 4-byte aligned.
 */

void __eeprom_program_long(unsigned char __near * dst, unsigned long v)
{
  FLASH_ProgramWord((u32)dst, (u32)v);
}

/*
 * Wait for the last data EEPROM operation to finish.  Return 0 if the
 * operation failed, otherwise non-zero.  You may want to handle
 * errors here, since the utility functions below simply ignore
 * errors, aborting multi-write operations early.
 */

int __eeprom_wait_for_last_operation(void)
{
  FLASH_Status_TypeDef status = FLASH_WaitForLastOperation(FLASH_MEMTYPE_DATA);
  return !!(status & (  FLASH_STATUS_SUCCESSFUL_OPERATION
                      | FLASH_STATUS_END_HIGH_VOLTAGE));
}

