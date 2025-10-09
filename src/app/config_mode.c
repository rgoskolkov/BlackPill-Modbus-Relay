#include "config_mode.h"
#include "board_config.h"
#include <stdbool.h>
#include "led_driver.h"
#include "flash_driver.h"
#include "modbus_adapter.h"

#define CONFIG_MODE_TIMEOUT_MS 5000

extern osMessageQueueId_t userKeyEventQueueHandle;

static uint8_t current_slave_id = 0;
static uint32_t last_activity_time = 0;
static volatile bool is_blinking = false;

static void on_led_blink_complete(void) {
    last_activity_time = Board_GetTick();
    is_blinking = false;
}

static void start_blinking(uint8_t value) {
    is_blinking = true;
    led_do_blink(value, 150, 350, on_led_blink_complete);
}

static void increment_slave_id(void) {
    current_slave_id++;
    if (current_slave_id == 0 || current_slave_id > 247) {
        current_slave_id = 1;
    }
    osDelay(500);
    start_blinking(current_slave_id);
}

static void decrement_slave_id(void) {
    current_slave_id--;
    if (current_slave_id == 0 || current_slave_id > 247) {
        current_slave_id = 247;
    }
    osDelay(500);
    start_blinking(current_slave_id);
}

void config_mode_task(void *argument) {
    button_press_event_t event;
    osStatus_t status;

    // --- Enter ---
    led_acquire_lock();
    current_slave_id = flash_read_slave_id();
    led_do_blink(5, 50, 50, NULL);
    osDelay(1500);
    start_blinking(current_slave_id);

    for (;;) {
        status = osMessageQueueGet(userKeyEventQueueHandle, &event, NULL, pdMS_TO_TICKS(200));
        
        if (status == osOK) {
            switch(event) {
                case BUTTON_EVENT_SHORT_PRESS:
                    increment_slave_id();
                    break;
                case BUTTON_EVENT_MEDIUM_PRESS:
                    decrement_slave_id();
                    break;
                case BUTTON_EVENT_LONG_PRESS:
                    goto exit_loop; // Exit on long press
            }
        } else if (status == osErrorTimeout) {
            if (!is_blinking && (Board_GetTick() - last_activity_time > CONFIG_MODE_TIMEOUT_MS)) {
                goto exit_loop; // Exit on timeout
            }
        }
    }

exit_loop:
    // --- Exit ---
    led_stop_blink();
    flash_write_slave_id(current_slave_id);
    modbus_adapter_set_slave_id(current_slave_id);
    led_do_blink(2, 500, 500, NULL);
    led_release_lock();
    
    vTaskDelete(NULL);
}