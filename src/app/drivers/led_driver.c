#include "led_driver.h"
#include "FreeRTOS.h"
#include "task.h"
#include <limits.h> // Для ULONG_MAX
#include "board_config.h"
#include <stdbool.h>

// --- Определения для управления светодиодом ---
#define LED_ON() Board_LED_On()
#define LED_OFF() Board_LED_Off()

// --- Определения событий для уведомлений задачи ---
#define LED_EVENT_ACK       (1 << 0) // Событие: подтверждение команды
#define LED_EVENT_HEARTBEAT (1 << 1) // Событие: сигнал о работе Modbus
#define LED_EVENT_DO_BLINK  (1 << 2)

// --- Статические переменные ---
static TaskHandle_t led_task_handle = NULL;
static volatile TickType_t g_last_heartbeat_tick = 0;
static bool led_locked = false;
static volatile bool stop_blink_flag = false;

// Blink sequence parameters
static uint8_t g_blink_count = 0;
static uint16_t g_on_ms = 0;
static uint16_t g_off_ms = 0;
static void (*completion_cb)(void) = NULL;
const TickType_t HEARTBEAT_TIMEOUT_TICKS = pdMS_TO_TICKS(15000); // 15 секунд

// --- Публичные функции ---
void dead_hand(uint32_t on_delay, uint32_t off_delay, uint8_t count) {
    // Вечный цикл с миганием, использующий 'busy-wait' циклы,
    // так как HAL_Delay и RTOS функции могут быть недоступны.
    for (;;) {
        for (uint8_t i = 0; i < count; i++) {
            LED_ON();
            for (volatile uint32_t d = 0; d < on_delay; d++);
            LED_OFF();
            for (volatile uint32_t d = 0; d < off_delay; d++);
        }
        // Длинная пауза перед повторением последовательности
        for (volatile uint32_t d = 0; d < 1000000; d++);
    }
}


void led_signal_ack(void) {
    if (led_task_handle != NULL && !led_locked) {
        xTaskNotify(led_task_handle, LED_EVENT_ACK, eSetBits);
    }
}

void led_signal_heartbeat(void) {
    if (led_locked) return;
    g_last_heartbeat_tick = xTaskGetTickCount();
    if (led_task_handle != NULL) {
        // Это уведомление просто разбудит задачу для проверки, если она спит
        xTaskNotify(led_task_handle, LED_EVENT_HEARTBEAT, eSetBits);
    }
}

void led_do_blink(uint8_t count, uint16_t on_ms, uint16_t off_ms, void (*callback)(void)) {
    stop_blink_flag = false;
    g_blink_count = count;
    g_on_ms = on_ms;
    g_off_ms = off_ms;
    completion_cb = callback;
    if (led_task_handle != NULL) {
        xTaskNotify(led_task_handle, LED_EVENT_DO_BLINK, eSetBits);
    }
}

void led_stop_blink(void) {
    stop_blink_flag = true;
}

void led_acquire_lock(void) {
    led_locked = true;
}

void led_release_lock(void) {
    led_locked = false;
}

// --- Основная задача ---

void led_task(void *argument) {
    led_task_handle = xTaskGetCurrentTaskHandle();
    uint32_t notified_value;
    TickType_t next_heartbeat_display_time = 0;

    // Сигнал о старте системы
    led_signal_ack();

    for (;;) {
        // Ожидаем уведомления. Выходим по таймауту, чтобы проверить heartbeat.
        // Таймаут равен времени до следующего отображения heartbeat.
        TickType_t now = xTaskGetTickCount();
        TickType_t timeout;

        if (next_heartbeat_display_time > now) {
            timeout = next_heartbeat_display_time - now;
        } else {
            // Если время уже прошло, ждем следующего события бесконечно.
            // Это не даст задаче зациклиться в состоянии Ready.
            // Проверка heartbeat ниже все равно выполнится.
            timeout = portMAX_DELAY;
        }
        
        BaseType_t result = xTaskNotifyWait(0x00,          // Не сбрасывать биты при входе
                                          ULONG_MAX,     // Сбрасывать все биты при выходе
                                          &notified_value, // Переменная для хранения уведомления
                                          timeout);      // Таймаут

        now = xTaskGetTickCount(); // Обновляем время после пробуждения

        if (result == pdPASS) { // Если проснулись по уведомлению
            // 1. Приоритет: Сигнал ACK
            if (notified_value & LED_EVENT_DO_BLINK) {
                for (uint8_t i = 0; i < g_blink_count; i++) {
                    if (stop_blink_flag) break;
                    LED_ON(); vTaskDelay(pdMS_TO_TICKS(g_on_ms));
                    if (stop_blink_flag) { LED_OFF(); break; }
                    LED_OFF(); vTaskDelay(pdMS_TO_TICKS(g_off_ms));
                }
                if (!stop_blink_flag && completion_cb != NULL) {
                    completion_cb();
                }
                // Reset for next operation
                stop_blink_flag = false;
                completion_cb = NULL;
                continue;
            }
            if (notified_value & LED_EVENT_ACK && !led_locked) {
                for (uint8_t i = 0; i < 3; i++) {
                    LED_ON();
                    vTaskDelay(pdMS_TO_TICKS(100));
                    LED_OFF();
                    vTaskDelay(pdMS_TO_TICKS(100));
                }
                // После ACK лучше пересчитать время следующего heartbeat,
                // чтобы избежать слишком частого мигания
                next_heartbeat_display_time = now + HEARTBEAT_TIMEOUT_TICKS;
                continue; // Начинаем цикл ожидания заново
            }
        }
        
        if (led_locked) {
            // В режиме конфигурации не мигаем heartbeat
            timeout = portMAX_DELAY;
            continue;
        }

        if (now >= next_heartbeat_display_time) {
            // Если с момента последнего сигнала по Modbus прошло не много времени
            if ((now - g_last_heartbeat_tick) < HEARTBEAT_TIMEOUT_TICKS) {
                LED_ON();
                vTaskDelay(pdMS_TO_TICKS(500)); // Среднее мигание
                LED_OFF();
            }
            // Планируем следующий heartbeat
            next_heartbeat_display_time = now + HEARTBEAT_TIMEOUT_TICKS;
        }
    }
}