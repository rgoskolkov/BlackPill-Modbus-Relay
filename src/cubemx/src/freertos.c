/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "led_driver.h"
#include "modbus_adapter.h"
#include "system_monitor.h"
#include "board_config.h"
#include <string.h>
#include "input_driver.h"
#include "config_mode.h"
#include "background_task.h"
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
osMessageQueueId_t switchEventQueueHandle;
osMessageQueueId_t userKeyEventQueueHandle;
osMutexId_t printfMutexHandle;

// --- Таймер для опроса кнопок ---
osTimerId_t inputPollingTimerHandle;
const osTimerAttr_t inputPollingTimer_attributes = {
  .name = "inputPollingTimer"
};

// --- For dynamic config task ---
osThreadId_t configModeTaskHandle;
const osThreadAttr_t configModeTask_attributes = {
  .name = "configModeTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
osThreadId_t inputTaskHandle;
const osThreadAttr_t inputTask_attributes = {
  .name = "inputTask",
  .stack_size = configMINIMAL_STACK_SIZE * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* USER CODE END Variables */
const osMutexAttr_t printfMutex_attributes = {
  .name = "printfMutex"
};
/* Definitions for ledTask */
osThreadId_t ledTaskHandle;
const osThreadAttr_t ledTask_attributes = {
  .name = "ledTask",
  .stack_size = configMINIMAL_STACK_SIZE * 6,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for syncTask */
osThreadId_t syncTaskHandle;
const osThreadAttr_t syncTask_attributes = {
  .name = "syncTask",
  .stack_size = configMINIMAL_STACK_SIZE * 6,
  .priority = (osPriority_t) osPriorityLow5,
};
/* Definitions for backgroundTask */
osThreadId_t backgroundTaskHandle;
const osThreadAttr_t backgroundTask_attributes = {
  .name = "backgroundTask",
  .stack_size = configMINIMAL_STACK_SIZE * 10,
  .priority = (osPriority_t) osPriorityLow1,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);
void vApplicationMallocFailedHook(void);

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

/* USER CODE BEGIN 4 */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
    // Простая хэш-функция для имени задачи
    uint32_t task_hash = 0;
    const char *p = (const char *)pcTaskName;
    while (*p) {
        task_hash = (task_hash << 5) + *p++;
    }

    // Enable PWR clock and allow access to Backup domain
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR |= PWR_CR_DBP;

    RTC->BKP6R = 0xDEADBEEF; // Magic number
    RTC->BKP7R = task_hash;  // Store task hash

    PWR->CR &= ~PWR_CR_DBP;

    dead_hand(100000, 100000, 7); // 7 short blinks
}
/* USER CODE END 4 */

/* USER CODE BEGIN 5 */
void vApplicationMallocFailedHook(void)
{
    uint32_t malloc_fail_addr = 0;
    __asm volatile("mov %0, lr" : "=r"(malloc_fail_addr));

    // Enable PWR clock and allow access to Backup domain
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR |= PWR_CR_DBP;

    RTC->BKP8R = 0xDEADBEEF; // Magic number
    RTC->BKP9R = malloc_fail_addr;

    PWR->CR &= ~PWR_CR_DBP;

    dead_hand(200000, 200000, 8); // 8 medium blinks
}
/* USER CODE END 5 */
/* USER CODE BEGIN 6 */
void configASSERT_failed(const char *file, int line) {
    // Получаем адрес возврата, который примерно указывает на место сбоя
    uint32_t assert_addr = 0;
    __asm volatile("mov %0, lr" : "=r"(assert_addr));

    // Enable PWR clock and allow access to Backup domain
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR |= PWR_CR_DBP;

    RTC->BKP4R = 0xDEADBEEF; // Magic number for assert
    RTC->BKP5R = assert_addr; // Store the address

    // Disable access to Backup domain
    PWR->CR &= ~PWR_CR_DBP;

    // 6 short blinks
    dead_hand(100000, 100000, 6);
}
/* USER CODE END 6 */

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
  printfMutexHandle = osMutexNew(&printfMutex_attributes);
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  inputPollingTimerHandle = osTimerNew((osTimerFunc_t)Input_PollSwitches, osTimerPeriodic, NULL, &inputPollingTimer_attributes);
  if (inputPollingTimerHandle != NULL) {
      osTimerStart(inputPollingTimerHandle, 20); // Используем 20 мс напрямую
  }
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  const osMessageQueueAttr_t switchEventQueue_attributes = {
    .name = "switchEventQueue"
  };
  switchEventQueueHandle = osMessageQueueNew(16, sizeof(uint8_t), &switchEventQueue_attributes);

  const osMessageQueueAttr_t userKeyEventQueue_attributes = {
    .name = "userKeyEventQueue"
  };
  userKeyEventQueueHandle = osMessageQueueNew(8, sizeof(button_press_event_t), &userKeyEventQueue_attributes);
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of ledTask */
  ledTaskHandle = osThreadNew(led_task, NULL, &ledTask_attributes);

  inputTaskHandle = osThreadNew(input_task, NULL, &inputTask_attributes);
  /* creation of syncTask */
  syncTaskHandle = osThreadNew(sync_task, NULL, &syncTask_attributes);
  
  /* creation of backgroundTask */
  backgroundTaskHandle = osThreadNew(background_task, NULL, &backgroundTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/* USER CODE END Application */
