#include "system_monitor.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Modbus.h"
#include <string.h>
#include <stdio.h>
#include "cmsis_os.h"
#include "ModbusConfig.h"
#include "button_driver.h"

// Символы из linker-скрипта для анализа памяти
extern uint32_t _sdata; // Начало .data
extern uint32_t _edata; // Конец .data
extern uint32_t _sbss;  // Начало .bss
extern uint32_t _ebss;  // Конец .bss
extern uint32_t _estack; // Конец RAM

// Эти переменные должны быть доступны из других модулей
extern modbusHandler_t mHandler;

// Доступ к атрибутам задач для получения размеров стека
extern const osThreadAttr_t ledTask_attributes;
extern const osThreadAttr_t syncTask_attributes;
extern const osThreadAttr_t backgroundTask_attributes;
extern const osThreadAttr_t myTaskModbusA_attributes;

// Вспомогательная функция для получения общего размера стека задачи по ее имени
uint32_t get_task_stack_size(const char* task_name) {
    if (strcmp(task_name, ledTask_attributes.name) == 0) {
        return ledTask_attributes.stack_size;
    }
    if (strcmp(task_name, syncTask_attributes.name) == 0) {
        return syncTask_attributes.stack_size;
    }
    if (strcmp(task_name, backgroundTask_attributes.name) == 0) {
        return backgroundTask_attributes.stack_size;
    }
    if (strcmp(task_name, myTaskModbusA_attributes.name) == 0) {
        return myTaskModbusA_attributes.stack_size;
    }
    return 0; // Задача не найдена
}

const char* taskStateToString(eTaskState state) {
    switch(state) {
        case eRunning:   return "Run";
        case eReady:     return "Ready";
        case eBlocked:   return "Blc";
        case eSuspended: return "Spd";
        case eDeleted:   return "Del";
        case eInvalid:   return "Inv";
        default:         return "Unk";
    }
}


void print_fault_details(void) {
  // Включаем тактирование PWR и разрешаем доступ к бэкап-домену
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWR_EnableBkUpAccess();

  printf("\r\n--- APPLICATION STARTUP ---\r\n");

  // Проверка HardFault
  if (RTC->BKP0R == 0xDEADBEEF) {
      uint32_t fault_addr = RTC->BKP1R;
      RTC->BKP0R = 0; // Очищаем магическое число

      printf("\r\n--- PREVIOUS SESSION CRASHED ---\r\n");
      printf("HardFault at PC: 0x%08lX\r\n", fault_addr);
      printf("--------------------------------\r\n");
  }

  // Проверка MemManage Fault
  if (RTC->BKP2R == 0xDEADBEEF) {
      uint32_t fault_addr = RTC->BKP3R;
      RTC->BKP2R = 0; // Очищаем

      printf("\r\n--- PREVIOUS SESSION FAILED ---\r\n");
      printf("MemManage fault at: 0x%08lX\r\n", fault_addr);
      printf("--------------------------------\r\n");
  }

  // Проверка configASSERT
  if (RTC->BKP4R == 0xDEADBEEF) {
      uint32_t fault_addr = RTC->BKP5R;
      RTC->BKP4R = 0; // Очищаем

      printf("\r\n--- PREVIOUS SESSION FAILED ---\r\n");
      printf("configASSERT at: 0x%08lX\r\n", fault_addr);
      printf("--------------------------------\r\n");
  }

  // Проверка Stack Overflow
  if (RTC->BKP6R == 0xDEADBEEF) {
      uint32_t task_hash = RTC->BKP7R;
      RTC->BKP6R = 0; // Очищаем

      printf("\r\n--- PREVIOUS SESSION FAILED ---\r\n");
      printf("Stack Overflow in task with hash: 0x%08lX\r\n", task_hash);
      printf("--------------------------------\r\n");
  }

  // Проверка Malloc Failed
  if (RTC->BKP8R == 0xDEADBEEF) {
      uint32_t fail_addr = RTC->BKP9R;
      RTC->BKP8R = 0; // Очищаем

      printf("\r\n--- PREVIOUS SESSION FAILED ---\r\n");
      printf("Malloc Failed near: 0x%08lX\r\n", fail_addr);
      printf("--------------------------------\r\n");
  }

  // Отключаем доступ к бэкап-домену
  HAL_PWR_DisableBkUpAccess();
}

// Статический буфер для сборки строки, вынесен из функции, чтобы не занимать стек
static char monitor_buffer[1024];

void system_monitor(void) {
    char *p = monitor_buffer;
    int len = sizeof(monitor_buffer);
    int written;

    UBaseType_t task_count = uxTaskGetNumberOfTasks();
    
    // Очищаем буфер
    memset(monitor_buffer, 0, sizeof(monitor_buffer));

    // Заголовок
    written = snprintf(p, len, "--- System Monitor ---\r\n");
    if (written > 0) { p += written; len -= written; }

    // Ограничим максимальное количество задач для избежания переполнения
    if (task_count > 0 && task_count < 10) {
        unsigned long _total_runtime;
        TaskStatus_t _task_status_array[task_count];
        task_count = uxTaskGetSystemState(_task_status_array, task_count, &_total_runtime);

        written = snprintf(p, len, "Name\tState\tPrio\tStackLeft\tTotal\tUsed%%\r\n");
        if (written > 0 && written < len) { p += written; len -= written; }
        written = snprintf(p, len, "--------------------------------------------------------------------------\r\n");
        if (written > 0 && written < len) { p += written; len -= written; }

        for (int i = 0; i < task_count; i++) {
            uint32_t total_stack = get_task_stack_size(_task_status_array[i].pcTaskName);
            uint32_t hwm_bytes = _task_status_array[i].usStackHighWaterMark * sizeof(StackType_t);
            uint32_t used_percent = 0;
            if (total_stack > 0) {
                used_percent = ((total_stack - hwm_bytes) * 100) / total_stack;
            }

            written = snprintf(p, len, "%-10s\t%-9s\t%lu\t%lu\t\t%lu\t%lu%%\r\n",
                               _task_status_array[i].pcTaskName,
                               taskStateToString(_task_status_array[i].eCurrentState),
                               _task_status_array[i].uxCurrentPriority,
                               hwm_bytes,
                               total_stack,
                               used_percent);

            if (written > 0 && written < len) {
                p += written;
                len -= written;
            } else {
                break; // Буфер закончился
            }
        }
    }

    // Информация о куче
    written = snprintf(p, len, "[HEAP] Total: %u, Current Free: %u, Minimal Ever Free: %u\r\n",
                       (unsigned int)configTOTAL_HEAP_SIZE,
                       (unsigned int)xPortGetFreeHeapSize(),
                       (unsigned int)xPortGetMinimumEverFreeHeapSize());
    if (written > 0 && written < len) {
        p += written;
        len -= written;
    }
    
    // Общая информация о RAM
    uint32_t total_ram = (uint32_t)&_estack - 0x20000000;
    uint32_t static_plus_bss = (uint32_t)&_ebss - (uint32_t)&_sdata;
    uint32_t heap_size = (unsigned int)configTOTAL_HEAP_SIZE;
    uint32_t static_only = static_plus_bss - heap_size;
    uint32_t free_for_stack = total_ram - static_plus_bss;

    written = snprintf(p, len, "[RAM] Total: %lu | Static: %lu | Heap: %lu | Free for Stacks: %lu\r\n",
                       total_ram,
                       static_only,
                       heap_size,
                       free_for_stack);
    if (written > 0 && written < len) {
        p += written;
        len -= written;
    }

    printf("%s", monitor_buffer);
}
