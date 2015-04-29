/**
  ******************************************************************************
  * @file    dalislave.c
  * @author  STMicroelectronics - MCD Application Team
  * @version V2.0.0
  * @date    07/04/2011
  * @brief   Dali IO pin driver implementation
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

#include "DALIslave.h"
#include "stm8s_it.h"

// Communication ports and pins
GPIO_TypeDef* DALIOUT_port = GPIOD; //default
u8 DALIOUT_pin = 1<<5; // default pin D5
u8 DALIOUT_invert = 0;

GPIO_TypeDef* DALIIN_port = GPIOD; //default
u8 DALIIN_pin = 1<<6; // default pin D6
u8 DALIIN_invert = 0;

//callback function
void DataReceived(u8 address, u8 dataByte);
TDataReceivedCallback *DataReceivedCallback = DataReceived;

void RTC1msFnc(void);
TRTC_1ms_Callback * RTC_1ms_Callback = RTC1msFnc;

void ErrorFnc(u8 code);
TErrorCallback *ErrorCallback = ErrorFnc;

// Data variables
u8 answer;    // data to send to controller device
u8 address;   // address byte from controller device
u8 dataByte;  // data byte from controller device

// Processing variables
u8 flag;        // status flag
u8 bit_count;   // nr of rec/send bits
u16 tick_count; // nr of ticks of the timer
u16 InterfaceFailureCounter; //nr of ticks when interface voltage is low

bool bit_value;   // value of actual bit
bool actual_val;  // bit value in this tick of timer
bool former_val;  // bit value in previous tick of timer


/***********************************************************/
/*************** R E C E I V E * P R O C E D U R E S *******/
/***********************************************************/

// edge of start bit detected
void receive_data() {

  // null variables
  address = 0;
  dataByte = 0;
  bit_count = 0;
  tick_count = 0;
  former_val = TRUE;

  // setup flag
  flag = RECEIVING_DATA;
  // disable external interrupt on DALI in port
  DALIIN_port->CR2 &= ~DALIIN_pin;
}

// gets state of the DALIIN pin
bool get_DALIIN(void) {
  if (DALIIN_invert)
  {
    if(DALIIN_port->IDR & DALIIN_pin)
      return FALSE;
    else
      return TRUE;
  }
  else
  {
    if(DALIIN_port->IDR & DALIIN_pin)
      return TRUE;
    else
      return FALSE;
  }
}

// Routine for receiving data for slave device
void receive_tick() {

  // Because of the structure of current amplifier, input has
  // to be negated
  actual_val = get_DALIIN();
  tick_count++;

  // edge detected
  if(actual_val != former_val)
  {
    switch(bit_count) {
      case 0:
        if (tick_count > 2)
        {
          tick_count = 0;
          bit_count  = 1; // start bit
        }
      break;
      case 17:      // 1st stop bit
        if(tick_count > 6) // stop bit error, no edge should exist
          flag = ERR;
      break;
      case 18:      // 2nd stop bit
        flag = ERR; // stop bit error, no edge should exist
      break;
      default:      // other bits
        if(tick_count > 6)
        {
          if(bit_count < 9) // store bit in address byte
          {
            address |= (actual_val << (8-bit_count));
          }else             // store bit in data byte
          {
            dataByte |= (actual_val << (16-bit_count));
          }
          bit_count++;
          tick_count = 0;
        }
      break;
    }
  }else // voltage level stable
  {
    switch(bit_count)
    {
      case 0:
        if(tick_count==8)  // too long start bit
            flag = ERR;
      break;
      case 17:
        // First stop bit
        if (tick_count==8)
        {
          if (actual_val==0) // wrong level of stop bit
          {
            flag = ERR;
          }
          else
          {
            bit_count++;
            tick_count = 0;
          }
        }
      break;
      case 18:
        // Second stop bit
        if (tick_count==10)
        {
          flag = NO_ACTION;
          DALIIN_port->CR2 |= DALIIN_pin;//enable EXTI
          //TIM4->CR1 &= ~TIM4_CR1_CEN;
          DataReceivedCallback(address,dataByte);
        }
      break;
      default: // normal bits
        if(tick_count==10)
        { // too long delay before edge
            flag = ERR;
        }
      break;
    }
  }
  former_val = actual_val;

  if(flag==ERR)
  {
    flag = NO_ACTION;
    DALIIN_port->CR2 |= DALIIN_pin;//enable EXTI
    //TIM4->CR1 &= ~TIM4_CR1_CEN;
  }
  return;
}

/***********************************************************/
/*************** C O M M O N * P R O C E D U R E S *********/
/***********************************************************/

// Setup DALIOUT port and pin
void init_DALI(GPIO_TypeDef* port_out, u8 pin_out, u8 invert_out, GPIO_TypeDef* port_in, u8 pin_in, u8 invert_in,
               TDataReceivedCallback DataReceivedFunction, TErrorCallback ErrorFunction, TRTC_1ms_Callback RTC_1ms_Function)
{
  DALIOUT_port = port_out;
  DALIOUT_pin = 1 << pin_out;
  DALIOUT_invert = invert_out;

  DALIIN_port = port_in;
  DALIIN_pin = 1 << pin_in;
  DALIIN_invert = invert_in;

  DataReceivedCallback = DataReceivedFunction;
  RTC_1ms_Callback = RTC_1ms_Function;
  ErrorCallback = ErrorFunction;

  /* Pin for data output */
  DALIOUT_port->ODR |= DALIOUT_pin; //high level
  DALIOUT_port->DDR |= DALIOUT_pin; //output mode
  DALIOUT_port->CR1 |= DALIOUT_pin; //push-pull
  DALIOUT_port->CR2 |= DALIOUT_pin; //slow slope

  /* Pin for data input */
  DALIIN_port->DDR &= ~DALIIN_pin; //input mode
  DALIIN_port->ODR &= ~DALIIN_pin; //low level
  DALIIN_port->CR1 |= DALIIN_pin; //pull-up
  DALIIN_port->CR2 |= DALIIN_pin; //interrupt enable on pin

  // External interrupts are allowed just for ports A-E,
  // application will not work properly for other ports.
  // Interrupt on Falling edge only.
  if(port_in == GPIOA)
  {
    EXTI->CR1 &= ~EXTI_CR1_PAIS;
    EXTI->CR1 |= 0x02;
  }
  else if(port_in == GPIOB)
  {
    EXTI->CR1 &= ~EXTI_CR1_PBIS;
    EXTI->CR1 |= 0x02 << 2;
  }
  else if(port_in == GPIOC)
  {
    EXTI->CR1 &= ~EXTI_CR1_PCIS;
    EXTI->CR1 |= 0x02 << 4;
  }
  else if(port_in == GPIOD)
  {
    EXTI->CR1 &= ~EXTI_CR1_PDIS;
    EXTI->CR1 |= 0x02 << 6;
  }
  else if(port_in == GPIOE)
  {
    EXTI->CR2 &= ~EXTI_CR2_PEIS;
    EXTI->CR2 |= 0x02;
  }

  //set status flaf
  flag = NO_ACTION;

  //reset 500ms interface failure counter
  InterfaceFailureCounter = 0;

  /* Configure the Fcpu to DIV1 , 16MHz*/
  CLK->CKDIVR = 0x00;

  /* Time base configuration */
  oneMScounter = 0;
  TIM4->PSCR = TIM4_PRESCALLER;
  TIM4->ARR  = TIM4_DIVIDER;
  /* Enable TIM4 Interrupt sources */
  TIM4->IER |= 0x01; //TIM4_IT_UPDATE

  // enable timer
  TIM4->CR1 |= TIM4_CR1_CEN;

  enableInterrupts();

  return;
}

void DataReceived(u8 address, u8 dataByte)
{
  // Data has been received from master device
  // Received data were stored in bytes address (1st byte)
  // and dataByte (2nd byte)
}

void RTC1msFnc(void)
{
  //here is called routines every 1ms
}

void ErrorFnc(u8 code_val)
{
  //here is error management
}

u8 get_flag(void)
{
  return flag;
}

//returns timer counter
u8 get_timer_count(void)
{
  return (TIM4->CNTR);
}

/*************** S E N D * P R O C E D U R E S *************/
/***********************************************************/

// Set value to the DALIOUT pin
void set_DALIOUT(bool pin_value)
{
  if (DALIOUT_invert)
  {
    if(pin_value)
      DALIOUT_port->ODR &= ~DALIOUT_pin;
    else
      DALIOUT_port->ODR |= DALIOUT_pin;
  }
  else
  {
    if(pin_value)
      DALIOUT_port->ODR |= DALIOUT_pin;
    else
      DALIOUT_port->ODR &= ~DALIOUT_pin;
  }
}

// gets state of the DALIOUT pin
bool get_DALIOUT(void)
{
  if (DALIOUT_invert)
  {
    if(DALIOUT_port->IDR & DALIOUT_pin)
      return FALSE;
    else
      return TRUE;
  }
  else
  {
    if(DALIOUT_port->IDR & DALIOUT_pin)
      return TRUE;
    else
      return FALSE;
  }
}

// Send answer to the controller device
void send_data(u8 byteToSend)
{
  answer = byteToSend;
  bit_count = 0;
  tick_count = 0;

  // disable external interrupt - no incoming data now
  DALIIN_port->CR2 &= ~DALIIN_pin;

  flag = SENDING_DATA;
  //TIM4->CR1 |= TIM4_CR1_CEN;
}


// DALI protocol physical layer for slave device
void send_tick(void)
{
  //access to the routine just every 4 ticks = every half bit
  if((tick_count & 0x03)==0)
  {
    if(tick_count < 104)
    {
      // settling time between forward and backward frame
      if(tick_count < 32)
      {
        tick_count++;
        return;
      }

      // start of the start bit
      // 32 ticks = 8*Te time = delay between forward and backward message frame (1*Te time must be added as half of stop bit)
      if(tick_count == 32)
      {
        set_DALIOUT(FALSE);
        tick_count++;
        return;
      }

      // edge of the start bit
      if(tick_count == 36)
      {
        set_DALIOUT(TRUE);
        tick_count++;
        return;
      }

      // bit value (edge) selection
      bit_value = (bool)( (answer >> (7-bit_count)) & 0x01);

      // Every half bit -> Manchester coding
      if( !( (tick_count-32) & 0x0007) )
      { // div by 8
        if(get_DALIOUT() == bit_value ) // former value of bit = new value of bit
          set_DALIOUT((bool)(1-bit_value));
      }

      // Generate edge for actual bit
      if( !( (tick_count - 36) & 0x0007) )
      {
        set_DALIOUT(bit_value);
        bit_count++;
      }
    }else
    { // end of data byte, start of stop bits
      if(tick_count == 104)
      {
        set_DALIOUT(TRUE); // start of stop bit
      }

      // end of stop bits, no settling time
      if(tick_count == 120)
      {
        flag = NO_ACTION;
        //TIM4->CR1 &= ~TIM4_CR1_CEN;
        DALIIN_port->CR2 |= DALIIN_pin;//enable EXTI
      }
    }
  }
  tick_count++;

  return;
}

/* checking if DALI bus is in the error state for long time */
void check_interface_failure(void)
{
  if (get_DALIIN())
  {
    InterfaceFailureCounter = 0;
    return;
  }

  InterfaceFailureCounter++;
  if (InterfaceFailureCounter > ((1000l * 500)/US_PER_TICK) )  //check 500ms timeout
  {
    ErrorCallback(1);
    InterfaceFailureCounter = 0;
  }
}

