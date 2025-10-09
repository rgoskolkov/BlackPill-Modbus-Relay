#ifndef LED_DRIVER_H
#define LED_DRIVER_H

#include "stdio.h"
#include <stdbool.h>
#include <stdint.h>

// --- Основные функции ---
void led_signal_ack(void);
void led_signal_heartbeat(void);

// --- Функции для кастомной анимации ---
void led_do_blink(uint8_t count, uint16_t on_ms, uint16_t off_ms, void (*completion_callback)(void));
void led_stop_blink(void);
void led_acquire_lock(void);
void led_release_lock(void);

// --- Вспомогательные функции ---
void dead_hand(uint32_t on_delay, uint32_t off_delay, uint8_t count);
void led_task(void *argument);

#endif // LED_DRIVER_H