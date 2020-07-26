/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     
#include <comm_uart.h>
#include <datatypes.h>
#include <bldc_interface.h>
#include <string.h>
#include <stdio.h>
#include <userInterface.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern UART_HandleTypeDef huart3;
uint8_t _received_char = 0;
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for UART_TIMER */
osThreadId_t UART_TIMERHandle;
const osThreadAttr_t UART_TIMER_attributes = {
  .name = "UART_TIMER",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 128 * 4
};
/* Definitions for UART_PKT_PROC */
osThreadId_t UART_PKT_PROCHandle;
const osThreadAttr_t UART_PKT_PROC_attributes = {
  .name = "UART_PKT_PROC",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 1024 * 4
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
extern void rxchar(UART_HandleTypeDef *uartp, uint16_t c);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
extern void uart_timer(void *argument);
extern void uart_packet_process(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);

/* USER CODE BEGIN 1 */
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
__weak void configureTimerForRunTimeStats(void)
{

}

__weak unsigned long getRunTimeCounterValue(void)
{
return 0;
}
/* USER CODE END 1 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
       
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of UART_TIMER */
  UART_TIMERHandle = osThreadNew(uart_timer, NULL, &UART_TIMER_attributes);

  /* creation of UART_PKT_PROC */
  UART_PKT_PROCHandle = osThreadNew(uart_packet_process, NULL, &UART_PKT_PROC_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */

void bldc_val_received(mc_values *val)
{
  printf("\r\n");
//  printf("Input voltage: %.2f V\r\n", val->v_in);
//  printf("Temp:          %.2f degC\r\n", val->temp_mos);
//  printf("Current motor: %.2f A\r\n", val->current_motor);
//  printf("Current in:    %.2f A\r\n", val->current_in);
//  printf("RPM:           %.1f RPM\r\n", val->rpm);
//  printf("Duty cycle:    %.1f %%\r\n", val->duty_now * 100.0);
//  printf("Ah Drawn:      %.4f Ah\r\n", val->amp_hours);
//  printf("Ah Regen:      %.4f Ah\r\n", val->amp_hours_charged);
//  printf("Wh Drawn:      %.4f Wh\r\n", val->watt_hours);
//  printf("Wh Regen:      %.4f Wh\r\n", val->watt_hours_charged);
  printf("Tacho:         %i counts\r\n", val->tachometer);
  printf("Tacho ABS:     %i counts\r\n", val->tachometer_abs);
  printf("Fault Code:    %s\r\n", bldc_interface_fault_to_string(val->fault_code));
}

/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  comm_uart_init();

  // Give bldc_interface a function to call when values are received.
  bldc_interface_set_rx_value_func(ui_print_esc_values);

  HAL_UART_Receive_IT(&huart3, &_received_char, 1);

  ui_initialize();


  for(;;)
  {
    bldc_interface_get_values();

    osDelay(1000);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  rxchar(&huart3, _received_char);
  HAL_UART_Receive_IT(&huart3, &_received_char, 1);
}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
