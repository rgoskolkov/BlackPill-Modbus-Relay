#include "button_driver.h"
#include "board_config.h"
#include "config_mode.h"
#include <stdbool.h>
#include <stdio.h>

#define SHORT_PRESS_THRESHOLD_MS 1000
#define LONG_PRESS_THRESHOLD_MS 4000

extern osThreadId_t configModeTaskHandle;
extern const osThreadAttr_t configModeTask_attributes;
extern osMessageQueueId_t userKeyEventQueueHandle;

static bool last_state_is_pressed = false;
static uint32_t press_start_time = 0;

void button_driver_poll(void) {
    uint32_t now = Board_GetTick();
    bool current_state_is_pressed = (Board_GPIO_Read(BOARD_BUTTON_GPIO_Port, BOARD_BUTTON_Pin) == GPIO_PIN_RESET);

    if (current_state_is_pressed != last_state_is_pressed) {
        last_state_is_pressed = current_state_is_pressed;
        
        if (current_state_is_pressed) { // PRESSED
            press_start_time = now;
        } else { // RELEASED
            if (press_start_time > 0) {
                uint32_t press_duration = now - press_start_time;
                
                if (configModeTaskHandle != NULL && osThreadGetState(configModeTaskHandle) != osThreadTerminated) {
                    // Config task is running, send event to it
                    button_press_event_t event;
                    if (press_duration >= LONG_PRESS_THRESHOLD_MS) {
                        event = BUTTON_EVENT_LONG_PRESS;
                    } else if (press_duration >= SHORT_PRESS_THRESHOLD_MS) {
                        event = BUTTON_EVENT_MEDIUM_PRESS;
                    } else {
                        event = BUTTON_EVENT_SHORT_PRESS;
                    }
                    osMessageQueuePut(userKeyEventQueueHandle, &event, 0, 0);
                } else {
                    // Config task is NOT running, only react to a long press
                    if (press_duration >= LONG_PRESS_THRESHOLD_MS) {
                        configModeTaskHandle = osThreadNew(config_mode_task, NULL, &configModeTask_attributes);
                    }
                }
            }
            press_start_time = 0;
        }
    }
}