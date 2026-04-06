#include "board_config.h"
#include <stdint.h>

/* =================================================================================
   Switch-to-Relay Mapping Configuration
   Размер = NUM_SWITCHES, каждое значение = индекс реле (0..NUM_RELAYS-1)

   Формат в platformio.ini:
     -D SWITCH_1_RELAY=0  (выключатель 1 → реле 0)
     -D SWITCH_8_RELAY=1  (выключатель 8 → реле 1)

   Выключатели 1..NUM_RELAYS: дефолтный маппинг 1:1 (можно переопределить)
   Выключатели NUM_RELAYS+1..NUM_SWITCHES: ДОЛЖНЫ быть заданы явно в platformio.ini
   иначе будет ошибка сборки.
==================================================================================*/

/* --- Дефолтный маппинг 1:1 для основных выключателей (1-based номера реле) --- */
#ifndef SWITCH_1_RELAY
#define SWITCH_1_RELAY  1
#endif
#ifndef SWITCH_2_RELAY
#define SWITCH_2_RELAY  2
#endif
#ifndef SWITCH_3_RELAY
#define SWITCH_3_RELAY  3
#endif
#ifndef SWITCH_4_RELAY
#define SWITCH_4_RELAY  4
#endif
#ifndef SWITCH_5_RELAY
#define SWITCH_5_RELAY  5
#endif
#ifndef SWITCH_6_RELAY
#define SWITCH_6_RELAY  6
#endif
#ifndef SWITCH_7_RELAY
#define SWITCH_7_RELAY  7
#endif
#ifndef SWITCH_8_RELAY
#define SWITCH_8_RELAY  8
#endif

/* --- Дополнительные выключатели: НЕТ дефолтного значения! --- */
#if NUM_SWITCHES > 8
#ifndef SWITCH_9_RELAY
#define SWITCH_9_RELAY  0xFF
#endif
#endif
#if NUM_SWITCHES > 9
#ifndef SWITCH_10_RELAY
#define SWITCH_10_RELAY 0xFF
#endif
#endif
#if NUM_SWITCHES > 10
#ifndef SWITCH_11_RELAY
#define SWITCH_11_RELAY 0xFF
#endif
#endif
#if NUM_SWITCHES > 11
#ifndef SWITCH_12_RELAY
#define SWITCH_12_RELAY 0xFF
#endif
#endif

/* Конвертер: 1-based (из config) → 0-based (для кода) */
#define _M(r) ((r) - 1)

/* =================================================================================
   Массив маппинга (индекс 0 = выключатель 1, ...; значение = 0-based relay index)
==================================================================================*/

const uint8_t switch_to_relay_map[NUM_SWITCHES] = {
    _M(SWITCH_1_RELAY), _M(SWITCH_2_RELAY), _M(SWITCH_3_RELAY), _M(SWITCH_4_RELAY),
    _M(SWITCH_5_RELAY), _M(SWITCH_6_RELAY), _M(SWITCH_7_RELAY), _M(SWITCH_8_RELAY)
#if NUM_SWITCHES > 8
    , _M(SWITCH_9_RELAY)
#endif
#if NUM_SWITCHES > 9
    , _M(SWITCH_10_RELAY)
#endif
#if NUM_SWITCHES > 10
    , _M(SWITCH_11_RELAY)
#endif
#if NUM_SWITCHES > 11
    , _M(SWITCH_12_RELAY)
#endif
};

/* =================================================================================
   Compile-time assertions
==================================================================================*/

#define STATIC_ASSERT(cond, msg) _Static_assert(cond, #msg)

/* Проверка: все значения маппинга в диапазоне 1..NUM_RELAYS */
STATIC_ASSERT(SWITCH_1_RELAY >= 1 && SWITCH_1_RELAY <= NUM_RELAYS, switch_1_relay_invalid);
STATIC_ASSERT(SWITCH_2_RELAY >= 1 && SWITCH_2_RELAY <= NUM_RELAYS, switch_2_relay_invalid);
STATIC_ASSERT(SWITCH_3_RELAY >= 1 && SWITCH_3_RELAY <= NUM_RELAYS, switch_3_relay_invalid);
STATIC_ASSERT(SWITCH_4_RELAY >= 1 && SWITCH_4_RELAY <= NUM_RELAYS, switch_4_relay_invalid);
STATIC_ASSERT(SWITCH_5_RELAY >= 1 && SWITCH_5_RELAY <= NUM_RELAYS, switch_5_relay_invalid);
STATIC_ASSERT(SWITCH_6_RELAY >= 1 && SWITCH_6_RELAY <= NUM_RELAYS, switch_6_relay_invalid);
STATIC_ASSERT(SWITCH_7_RELAY >= 1 && SWITCH_7_RELAY <= NUM_RELAYS, switch_7_relay_invalid);
STATIC_ASSERT(SWITCH_8_RELAY >= 1 && SWITCH_8_RELAY <= NUM_RELAYS, switch_8_relay_invalid);
#if NUM_SWITCHES > 8
STATIC_ASSERT(SWITCH_9_RELAY >= 1 && SWITCH_9_RELAY <= NUM_RELAYS, switch_9_relay_MUST_be_set_in_platformio_ini);
#endif
#if NUM_SWITCHES > 9
STATIC_ASSERT(SWITCH_10_RELAY >= 1 && SWITCH_10_RELAY <= NUM_RELAYS, switch_10_relay_MUST_be_set_in_platformio_ini);
#endif
#if NUM_SWITCHES > 10
STATIC_ASSERT(SWITCH_11_RELAY >= 1 && SWITCH_11_RELAY <= NUM_RELAYS, switch_11_relay_MUST_be_set_in_platformio_ini);
#endif
#if NUM_SWITCHES > 11
STATIC_ASSERT(SWITCH_12_RELAY >= 1 && SWITCH_12_RELAY <= NUM_RELAYS, switch_12_relay_MUST_be_set_in_platformio_ini);
#endif

/* Проверка: каждое реле (1-based) имеет хотя бы один выключатель */
#if NUM_SWITCHES <= 8
#define _RC(r) \
    (SWITCH_1_RELAY==(r) || SWITCH_2_RELAY==(r) || SWITCH_3_RELAY==(r) || SWITCH_4_RELAY==(r) || \
     SWITCH_5_RELAY==(r) || SWITCH_6_RELAY==(r) || SWITCH_7_RELAY==(r) || SWITCH_8_RELAY==(r))
#elif NUM_SWITCHES <= 9
#define _RC(r) \
    (SWITCH_1_RELAY==(r) || SWITCH_2_RELAY==(r) || SWITCH_3_RELAY==(r) || SWITCH_4_RELAY==(r) || \
     SWITCH_5_RELAY==(r) || SWITCH_6_RELAY==(r) || SWITCH_7_RELAY==(r) || SWITCH_8_RELAY==(r) || \
     SWITCH_9_RELAY==(r))
#elif NUM_SWITCHES <= 10
#define _RC(r) \
    (SWITCH_1_RELAY==(r) || SWITCH_2_RELAY==(r) || SWITCH_3_RELAY==(r) || SWITCH_4_RELAY==(r) || \
     SWITCH_5_RELAY==(r) || SWITCH_6_RELAY==(r) || SWITCH_7_RELAY==(r) || SWITCH_8_RELAY==(r) || \
     SWITCH_9_RELAY==(r) || SWITCH_10_RELAY==(r))
#elif NUM_SWITCHES <= 11
#define _RC(r) \
    (SWITCH_1_RELAY==(r) || SWITCH_2_RELAY==(r) || SWITCH_3_RELAY==(r) || SWITCH_4_RELAY==(r) || \
     SWITCH_5_RELAY==(r) || SWITCH_6_RELAY==(r) || SWITCH_7_RELAY==(r) || SWITCH_8_RELAY==(r) || \
     SWITCH_9_RELAY==(r) || SWITCH_10_RELAY==(r) || SWITCH_11_RELAY==(r))
#else
#define _RC(r) \
    (SWITCH_1_RELAY==(r) || SWITCH_2_RELAY==(r) || SWITCH_3_RELAY==(r) || SWITCH_4_RELAY==(r) || \
     SWITCH_5_RELAY==(r) || SWITCH_6_RELAY==(r) || SWITCH_7_RELAY==(r) || SWITCH_8_RELAY==(r) || \
     SWITCH_9_RELAY==(r) || SWITCH_10_RELAY==(r) || SWITCH_11_RELAY==(r) || SWITCH_12_RELAY==(r))
#endif

STATIC_ASSERT(_RC(1), relay_1_not_assigned);
#if NUM_RELAYS > 1
STATIC_ASSERT(_RC(2), relay_2_not_assigned);
#endif
#if NUM_RELAYS > 2
STATIC_ASSERT(_RC(3), relay_3_not_assigned);
#endif
#if NUM_RELAYS > 3
STATIC_ASSERT(_RC(4), relay_4_not_assigned);
#endif
#if NUM_RELAYS > 4
STATIC_ASSERT(_RC(5), relay_5_not_assigned);
#endif
#if NUM_RELAYS > 5
STATIC_ASSERT(_RC(6), relay_6_not_assigned);
#endif
#if NUM_RELAYS > 6
STATIC_ASSERT(_RC(7), relay_7_not_assigned);
#endif
#if NUM_RELAYS > 7
STATIC_ASSERT(_RC(8), relay_8_not_assigned);
#endif

/* =================================================================================
   Hardware Pinout Arrays (размер зависит от NUM_RELAYS / NUM_SWITCHES)
==================================================================================*/

const GPIO_TypeDef* const relay_ports[NUM_RELAYS] = {
    RELAY1_PORT, RELAY2_PORT, RELAY3_PORT, RELAY4_PORT
#if NUM_RELAYS > 4
    , RELAY5_PORT
#endif
#if NUM_RELAYS > 5
    , RELAY6_PORT
#endif
#if NUM_RELAYS > 6
    , RELAY7_PORT
#endif
#if NUM_RELAYS > 7
    , RELAY8_PORT
#endif
};

const uint16_t relay_pins[NUM_RELAYS] = {
    RELAY1_PIN, RELAY2_PIN, RELAY3_PIN, RELAY4_PIN
#if NUM_RELAYS > 4
    , RELAY5_PIN
#endif
#if NUM_RELAYS > 5
    , RELAY6_PIN
#endif
#if NUM_RELAYS > 6
    , RELAY7_PIN
#endif
#if NUM_RELAYS > 7
    , RELAY8_PIN
#endif
};

const GPIO_TypeDef* const switch_ports[NUM_SWITCHES] = {
    SWITCH1_PORT, SWITCH2_PORT, SWITCH3_PORT, SWITCH4_PORT,
    SWITCH5_PORT, SWITCH6_PORT, SWITCH7_PORT, SWITCH8_PORT
#if NUM_SWITCHES > 8
    , SWITCH9_PORT
#endif
#if NUM_SWITCHES > 9
    , SWITCH10_PORT
#endif
#if NUM_SWITCHES > 10
    , SWITCH11_PORT
#endif
#if NUM_SWITCHES > 11
    , SWITCH12_PORT
#endif
};

const uint16_t switch_pins[NUM_SWITCHES] = {
    SWITCH1_PIN, SWITCH2_PIN, SWITCH3_PIN, SWITCH4_PIN,
    SWITCH5_PIN, SWITCH6_PIN, SWITCH7_PIN, SWITCH8_PIN
#if NUM_SWITCHES > 8
    , SWITCH9_PIN
#endif
#if NUM_SWITCHES > 9
    , SWITCH10_PIN
#endif
#if NUM_SWITCHES > 10
    , SWITCH11_PIN
#endif
#if NUM_SWITCHES > 11
    , SWITCH12_PIN
#endif
};


// =================================================================================
// Hardware Porting Layer: Function Implementations
// =================================================================================

// --- System Tick ---

uint32_t Board_GetTick(void)
{
    return HAL_GetTick();
}

void Board_Delay(uint32_t Delay)
{
    HAL_Delay(Delay);
}

// --- GPIO ---

void Board_GPIO_Write(void *gpio_port, uint16_t gpio_pin, uint8_t state)
{
    HAL_GPIO_WritePin((GPIO_TypeDef*)gpio_port, gpio_pin, (GPIO_PinState)state);
}

uint8_t Board_GPIO_Read(void *gpio_port, uint16_t gpio_pin)
{
    return (uint8_t)HAL_GPIO_ReadPin((GPIO_TypeDef*)gpio_port, gpio_pin);
}

// --- Wrappers for specific hardware ---

void Board_LED_On(void)
{
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
}

void Board_LED_Off(void)
{
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
}

void Board_Relay_On(uint8_t relay_number)
{
    if (relay_number >= NUM_RELAYS) return;
    HAL_GPIO_WritePin((GPIO_TypeDef*)relay_ports[relay_number], relay_pins[relay_number], GPIO_PIN_RESET);
}

void Board_Relay_Off(uint8_t relay_number)
{
    if (relay_number >= NUM_RELAYS) return;
    HAL_GPIO_WritePin((GPIO_TypeDef*)relay_ports[relay_number], relay_pins[relay_number], GPIO_PIN_SET);
}

uint8_t Board_Switch_Read(uint8_t switch_number)
{
    if (switch_number >= NUM_SWITCHES) return 0;
    return (HAL_GPIO_ReadPin((GPIO_TypeDef*)switch_ports[switch_number], switch_pins[switch_number]) == GPIO_PIN_RESET) ? 1 : 0;
}
