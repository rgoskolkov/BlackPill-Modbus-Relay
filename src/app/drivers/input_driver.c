#include "input_driver.h"
#include "board_config.h"
#include <string.h>
#include "led_driver.h"
#include "stdbool.h"
#include "relay_driver.h"

#define LONG_PRESS_TIME_MS 1000
#define DEBOUNCE_POLL_PERIOD_MS 20
#define DEBOUNCE_TICKS (DEBOUNCE_MS / DEBOUNCE_POLL_PERIOD_MS)

static uint8_t prev_states[NUM_SWITCHES];
static uint32_t press_start_ts[NUM_SWITCHES];
static bool switch_state[NUM_SWITCHES];

// --- Переменные для нового алгоритма debounce ---
static uint8_t debounce_counter[NUM_SWITCHES];
static uint8_t stable_state[NUM_SWITCHES];
// ---------------------------------------------

void Input_Init(void)
{
    memset(press_start_ts, 1, sizeof(press_start_ts));
    memset(switch_state, 0, sizeof(switch_state));
    memset(debounce_counter, 0, sizeof(debounce_counter));

    // Инициализируем стабильное состояние (1 - отпущено)
    for(int i=0; i<NUM_SWITCHES; i++)
    {
        stable_state[i] = 0;
        prev_states[i] = 0;
    }
}

void process_switch_event(uint8_t i)
{
    if (i >= NUM_SWITCHES) return;

    // Получаем текущее стабильное состояние, определенное в Input_PollSwitches
    uint8_t current_state = stable_state[i];
    uint32_t now = Board_GetTick();

    // Проверяем, было ли изменение по сравнению с последним обработанным состоянием
    // if(current_state == prev_states[i])
    // {
    //     printf("no changes, sorry\r\n");
    //     return; // Нет изменений, выходим
    // }

    if (current_state == 1) // Кнопка нажата (уровень LOW)
    {
        printf("change low\r\n");
        press_start_ts[i] = now;
        // Мгновенное переключение при нажатии
        switch_state[i] = !switch_state[i];
        led_signal_ack();
        switch_state[i] ? relay_on(i) : relay_off(i);
    }
    else // Кнопка отпущена (уровень HIGH)
    {
        if (press_start_ts[i] == 0) {
            printf("ignoring no press\r\n");
            return; // Игнорируем отпускание, если не было нажатия
        }
        uint32_t press_duration = now - press_start_ts[i];
        
        if (press_duration >= LONG_PRESS_TIME_MS) {
            // Это был фиксируемый выключатель, который "отпустили".
            // Возвращаем состояние обратно.
            printf("release long pressed\r\n");
            switch_state[i] = !switch_state[i];
            led_signal_ack();
            switch_state[i] ? relay_on(i) : relay_off(i);
        } else {
            printf("short no moving\r\n");
        }
        // Если это была кнопка (короткое нажатие), ничего не делаем.
        
        press_start_ts[i] = 0; // Сбрасываем в любом случае
    }
    
    prev_states[i] = current_state; // Обновляем последнее обработанное состояние
}

void input_task(void *argument)
{
    uint8_t switch_index;
    osStatus_t status;

    for(;;)
    {
        status = osMessageQueueGet(switchEventQueueHandle, &switch_index, NULL, osWaitForever);
        if (status == osOK)
        {
            process_switch_event(switch_index);
        }
    }
}

// Функция, вызываемая из прерывания таймера для опроса всех входов
void Input_PollSwitches(void)
{
    for (uint8_t i = 0; i < NUM_SWITCHES; i++)
    {
        uint8_t raw_state = Board_Switch_Read(i);

        if (raw_state == 1)
        {
            if (debounce_counter[i] < DEBOUNCE_TICKS)
            {
                debounce_counter[i]++;
            }
        }
        else // Пин в высоком состоянии (кнопка отпущена)
        {
            if (debounce_counter[i] > 0)
            {
                debounce_counter[i]--;
            }
        }

        // Проверяем, достигли ли счетчики стабильного состояния
        if (debounce_counter[i] == DEBOUNCE_TICKS) // Стабильно нажат
        {
            if (stable_state[i] == 0) // Если предыдущее стабильное состояние было "отпущено"
            {
                printf("press detected state: %u!!!\r\n", Board_Switch_Read(i));
                stable_state[i] = 1; // Новое стабильное состояние - "нажато"
                osMessageQueuePut(switchEventQueueHandle, &i, 0U, 0U);
            }
        }
        else if (debounce_counter[i] == 0) // Стабильно отпущена
        {
            if (stable_state[i] == 1) // Если предыдущее стабильное состояние было "нажато"
            {
                printf("release detected state: %u!!!\r\n", Board_Switch_Read(i));
                stable_state[i] = 0; // Новое стабильное состояние - "отпущено"
                osMessageQueuePut(switchEventQueueHandle, &i, 0U, 0U);
            }
        }
    }
}
