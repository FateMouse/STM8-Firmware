/**
  ******************************************************************************
  * @file    dalislave.h
  * @author  STMicroelectronics - MCD Application Team
  * @version V2.0.0
  * @date    07/04/2011
  * @brief   Dali IO pin driver implementation - header
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

#define NO_ACTION 0
#define SENDING_DATA 1
#define RECEIVING_DATA 2
#define ERR 3

#define CPU_CLK           (16000000)  //16MHz
#define TIM4_PRESCALLER   (0x03) // divide by 8
#define TIM4_DIVIDER      (CPU_CLK/(1<<TIM4_PRESCALLER)/9600) //8 x 1200 DALI baudrate
#define TICKS_PER_ONE_MS  (9600/1000)

#define US_PER_TICK       (1000000/(CPU_CLK/(1<<TIM4_PRESCALLER)/TIM4_DIVIDER))
#define US_PER_MS         (1000000/1000)


//callback function type
typedef void TDataReceivedCallback(u8 address,u8 dataByte);
typedef void TRTC_1ms_Callback(void);
typedef void TErrorCallback(u8 code);

// Receiving procedures
void receive_data(void);
void receive_tick(void);

// Common procedures
void init_DALI(GPIO_TypeDef* port_out, u8 pin_out, u8 invert_out, GPIO_TypeDef* port_in, u8 pin_in, u8 invert_in,
               TDataReceivedCallback DataReceivedFunction, TErrorCallback ErrorFunction, TRTC_1ms_Callback RTC_1ms_Function);
u8 get_flag(void);

// Sending procedures
void send_data(u8 byteToSend);
void send_tick(void);
void check_interface_failure(void);

// Timer procedures
u8 get_timer_count(void);
