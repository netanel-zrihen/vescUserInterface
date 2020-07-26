/*
 Copyright 2015 Benjamin Vedder	benjamin@vedder.se

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * comm_uart.c
 *
 *  Created on: 17 aug 2015
 *      Author: benjamin
 */

#include "comm_uart.h"
#include "cmsis_os2.h"
#include "bldc_interface_uart.h"
#include <stm32f4xx_hal.h>

#include <string.h>

extern UART_HandleTypeDef huart3;

#define SERIAL_RX_BUFFER_SIZE	1024

// Private functions
static void send_packet(unsigned char *data, unsigned int len);

static volatile osEventFlagsId_t event_id;

// Variables
static uint8_t serial_rx_buffer[SERIAL_RX_BUFFER_SIZE];
static int serial_rx_read_pos = 0;
static int serial_rx_write_pos = 0;

/*
 * This callback is invoked when a transmission buffer has been completely
 * read by the driver.
 */
//static void txend1(UART_HandleTypeDef *uartp)
//{
//  (void) uartp;
//}

/*
 * This callback is invoked when a transmission has physically completed.
 */
//static void txend2(UART_HandleTypeDef *uartp)
//{
//  (void) uartp;
//}

/*
 * This callback is invoked on a receive error, the errors mask is passed
 * as parameter.
 */
//static void rxerr(UART_HandleTypeDef *uartp, uartflags_t e)
//{
//  (void) uartp;
//  (void) e;
//}

/*
 * This callback is invoked when a character is received but the application
 * was not ready to receive it, the character is passed as parameter.
 */
void rxchar(UART_HandleTypeDef *uartp, uint16_t c)
{
  UNUSED(uartp);
  /*
   * Put the character in a buffer and notify a thread that there is data
   * available. An alternative way is to use
   *
   * packet_process_byte(c);
   *
   * here directly and skip the thread. However, this could drop bytes if
   * processing packets takes a long time.
   */

  serial_rx_buffer[serial_rx_write_pos++] = c;

  if (serial_rx_write_pos == SERIAL_RX_BUFFER_SIZE)
  {
    serial_rx_write_pos = 0;
  }

  osEventFlagsSet(event_id, 1);
}

/*
 * This callback is invoked when a receive buffer has been completely written.
 */
//static void rxend(UART_HandleTypeDef *uartp)
//{
//  (void) uartp;
//}


void uart_packet_process(void *argument)
{
  (void)argument;

  for (;;)
  {
    osEventFlagsWait(event_id, 1, osFlagsWaitAll, osWaitForever);

    /*
     * Wait for data to become available and process it as long as there is data.
     */

    while (serial_rx_read_pos != serial_rx_write_pos)
    {
      bldc_interface_uart_process_byte(serial_rx_buffer[serial_rx_read_pos++]);

      if (serial_rx_read_pos == SERIAL_RX_BUFFER_SIZE)
      {
        serial_rx_read_pos = 0;
      }
    }
  }
}

/**
 * Callback that the packet handler uses to send an assembled packet.
 *
 * @param data
 * Data array pointer
 * @param len
 * Data array length
 */
static void send_packet(unsigned char *data, unsigned int len)
{
  if (len > (PACKET_MAX_PL_LEN + 5))
  {
    return;
  }

  // Wait for the previous transmission to finish.
  while (huart3.gState == HAL_UART_STATE_BUSY_TX)
  {
    osDelay(1);
  }

  // Copy this data to a new buffer in case the provided one is re-used
  // after this function returns.
  static uint8_t buffer[PACKET_MAX_PL_LEN + 5];
  memcpy(buffer, data, len);

  // Send the data over UART
  HAL_UART_Transmit_IT(&huart3, buffer, len);
}

/**
 * This thread is only for calling the timer function once
 * per millisecond. Can also be implementer using interrupts
 * if no RTOS is available.
 */
void uart_timer(void *argument)
{
  (void)argument;

  for (;;)
  {
    bldc_interface_uart_run_timer();
    osDelay(1);
  }
}

void comm_uart_init(void)
{
  // Initialize the bldc interface and provide a send function
  bldc_interface_uart_init(send_packet);
  event_id = osEventFlagsNew(event_id);
}
