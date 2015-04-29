/** @page STM8S_DALI_AN stm8s DALI library: STM8S DALI application note


  @par Example description

DALI is standardized interface to control "intelligent" light sources.
This AN describes DALI slave library and simple user application using this library.
The main purpose of this firmware package is to provide resources to build
light control application which uses DALI as communication interface.

DALI slave library comes with simple application example (DALI slave device). It was
designed (and tested) for use with this evaluation hardware: STM8S Discovery board (see
UM0817 - STM8S Discovery User manual on www.st.com/mcu) together with DALI
transceiver board. Application example controls the luminance of the on-board LED diode.
Light intensity is controlled by PWM method using built-in Timer. As DALI master device
can be used ST7DALI evaluation kit (board and PC software - see www.st.com/mcu) or some
commercial DALI controller (e.g. from Osram).

  @par Directory contents

  See AN for detailed description.

  + libraries
    + DALIStack
        + inc
            - dali.h                            - high level DALI stack functions header file
            - dali_cmd.h                        - DALI commands implementation header file
            - dali_config.h                     - configuration of DALI slave device header file
            - dali_pub.h                        - public functions header file
            - dali_regs.h                       - DALI register management header file
            - eeprom.h                          - read/write management for EEPROM data header file
            - lite_timer_8bit.h                 - DALI timing management header file
        + src
            - dali.c                            - high level DALI stack functions source file
            - dali_cmd.c                        - DALI commands implementation source file
            - dali_config.c                     - configuration of DALI slave device source file
            - dali_pub.c                        - public functions source file
            - dali_regs.c                       - DALI register management source file
            - eeprom.c                          - read/write management for EEPROM data source file
            - lite_timer_8bit.c                 - DALI timing management source file
    + STM8S_StdPeriph_Driver (only used files are listed here)
        + inc
            - stm8s.h                           - HW registers definitions and memory mapping
            - stm8s_flash.h                     - flash driver header file (needed only with for IAR)
        + src
            - stm8s_flash.c                     - flash driver source file (needed only with IAR)
    + Project
        + inc
          - DALIslave.h                         - low level DALI pin driver header file
          - stm8_it.h                           - interrupt service routines header file
          - stm8s_conf.h                        - STM8S peripheral modules configuration header file
        + src
          - DALIslave.c                         - low level DALI pin driver source file
          - stm8_interrupt_vector.c             - interrupt vector table content source file
          - stm8_it.c                           - interrupt service routines source file
          - eeprom_itf.c                        - utility code for automatic EEPROM memory access (needed only with IAR)
          - main.c                              - Main file containing the "main" function
        + STVD
            + raisonance
                - STM8DALI.stw                  - Contains the STVD workspace for cosmic compiler
                - stm8dalislave.stp             - Contains the STVD project for cosmic compiler
            + cosmic
                - STM8DALI.stw                  - Contains the STVD workspace for raisonance compiler
                - stm8dalislave.stp             - Contains the STVD project for raisonance compiler
        + EWSTM8
            - STM8DALI.eww                      - Contains the EWSTM8 workspace for IAR compiler
            - stm8dalislave.ewp                 - Contains the EWSTM8 project for IAR compiler

  @par Hardware environment

 - Plug DALI transceiver board to STM8S Discovery board. Load firmware into the STM8S105 on
   STM8S Discovery board using USB and STVD or EWSTM8 software. Connect DALI transceiver board to
   DALI master device with 2 wires (DALI bus). As DALI master device can be used
   ST7DALI evaluation kit or some commercial DALI controller.


  @par How to use it with STVD ?

  - Open the STVD workspace
  - Rebuild all files: Project->Rebuild all
  - Load project image: Debug->Start/Stop Debug Session
  - Run program: Debug->Run (F5)

  @par How to use it with EWSTM8 ?

  - Open the EWSTM8 workspace
  - Rebuild all files: Project->Rebuild all
  - Load project image: Project->Download and Debug
  - Run program: Debug->Go (F5)

  */

/******************* (C) COPYRIGHT 2012 STMicroelectronics *****END OF FILE****/