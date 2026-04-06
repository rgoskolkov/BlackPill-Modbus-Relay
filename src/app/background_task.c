#include "background_task.h"
#include "cmsis_os.h"
#include "button_driver.h"
#include "system_monitor.h"
#include "board_config.h"

#define POLLING_INTERVAL_MS 50
#define MONITOR_INTERVAL_MS 10000

void background_task(void *argument)
{
    #if MONITOR_TASK == 1
    uint32_t last_monitor_tick = 0;
    #endif
    for(;;)
    {
        // High-frequency polling for button
        button_driver_poll();

        // Low-frequency call for system monitor
        #if MONITOR_TASK == 1
        uint32_t current_tick = osKernelGetTickCount();
        if (current_tick - last_monitor_tick >= pdMS_TO_TICKS(MONITOR_INTERVAL_MS)) {
            system_monitor();
            last_monitor_tick = current_tick;
        }
        #endif

        osDelay(pdMS_TO_TICKS(POLLING_INTERVAL_MS));
    }
}