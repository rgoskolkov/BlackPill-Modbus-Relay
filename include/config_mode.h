#ifndef CONFIG_MODE_H
#define CONFIG_MODE_H

#include "cmsis_os.h"

typedef enum {
    BUTTON_EVENT_SHORT_PRESS,
    BUTTON_EVENT_MEDIUM_PRESS,
    BUTTON_EVENT_LONG_PRESS
} button_press_event_t;

// Task function
void config_mode_task(void *argument);

#endif /* CONFIG_MODE_H */