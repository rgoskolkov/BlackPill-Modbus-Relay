#include "input_driver.h"
#include "board_config.h"
#include <string.h>
#include "led_driver.h"
#include "stdbool.h"
#include "relay_driver.h"

#define DEBOUNCE_POLL_PERIOD_MS 20
#define DEBOUNCE_TICKS (DEBOUNCE_MS / DEBOUNCE_POLL_PERIOD_MS)

static uint8_t debounce_counter[NUM_SWITCHES];
static uint8_t stable_state[NUM_SWITCHES];

void Input_Init(void)
{
    memset(debounce_counter, 0, sizeof(debounce_counter));
    for(int i=0; i<NUM_SWITCHES; i++)
    {
        stable_state[i] = Board_Switch_Read(i);
        if (stable_state[i]) {
            debounce_counter[i] = DEBOUNCE_TICKS;
        }
    }
}

void process_switch_event(uint8_t i)
{
    if (i >= NUM_SWITCHES) return;
    printf("change state\r\n");
    led_signal_ack();

    // Определяем, к какому реле привязан этот выключатель
    uint8_t relay_idx = switch_to_relay_map[i];
    relay_toggle(relay_idx);
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
