/**
  ******************************************************************************
  * @file    dali_config.h
  * @author  STMicroelectronics - MCD Application Team
  * @version V2.0.0
  * @date    07/04/2011
  * @brief   User configuration of DALI hardware and stack - header
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

#ifndef DALI_CONFIG_H
#define DALI_CONFIG_H

#include "stm8s.h"

//callback function type for light control
typedef void TLightControlCallback(u16 lighvalue);

/* defines if device is physically selected */
extern volatile u8 Physically_Selected;

/* --- HARDWARE definitions --- */
/* IO pins for DALI in and DALI out signals */
#define OUT_DALI_PORT      GPIOB //PB1 = TX
#define OUT_DALI_PIN       1
#define INVERT_OUT_DALI    0

#define IN_DALI_PORT       GPIOB //PB0 = RX
#define IN_DALI_PIN        0
#define INVERT_IN_DALI     0

/* pushbutton for device physical selection */
#define DALI_BUTTON_PORT   GPIOB //PB4 = button to GND
#define DALI_BUTTON_PIN    4

/* pull-up Vdd control for energy saving in halt */
#define DALI_PULLUP_PORT   GPIOB //PB5 = Vdd for pull-up for LED in optocoupler
#define DALI_PULLUP_PIN    5





/* --- Ballast properties definitions --- */

/*  ------------------------ Arc table ------------------------ */
#define USE_ARC_TABLE
/*  ------------------------ ROM registers values ------------------------ */
/**************************************************************************
 * These are the two Regs which are read only and therefore stored in ROM *
 * The first is                               *
 * - Version number   and the second          *
 * - Physical minimum Level                   *
 * You might want to adjust these.            *
 **************************************************************************/
#define DALI_VERSION_NUMBER_ROM        1    //DALI version 1
#define PHYSICAL_MIN_LEVEL_ROM         5

/*  ------------------------ another device definitions ------------------------ */
#define DEVICE_TYPE                    6    // LED module
        /*
        0          Fluorescent lamps IEC 62386-201
        1          Self-contained emergency lighting IEC 62386-202
        2          Discharge lamps (excluding fluorescent lamps) IEC 62386-203
        3          Low voltage halogen lamps IEC 62386-204
        4          Supply Voltage controller for incandescent lamps IEC 62386-205
        5          Conversion from digital into D.C. voltage IEC 62386-206
        6          LED modules IEC 62386-207
        7          Switching function IEC 62386-208
        8          Colour control IEC 62386-209
        9          Sequencer IEC 62386-210
        10         Optical control IEC 62386-211
        11  - 127  Not yet defined
        128 - 254  Reserved for control devices
        255        Control gear supports more than one device type
        */

#endif
