#ifndef INPUT_DRIVER_H
#define INPUT_DRIVER_H
#include <stdint.h>
#include "cmsis_os.h"

extern osMessageQueueId_t switchEventQueueHandle;

void Input_Init(void);
void input_task(void *argument); //функция для задачи обработки событий изменения статуса кнопок
void process_switch_event(uint8_t i);
void Input_PollSwitches(void);   // Новая функция для вызова из прерывания таймера

#endif