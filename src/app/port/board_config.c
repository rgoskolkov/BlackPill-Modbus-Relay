#include "board_config.h"

// =================================================================================
// Hardware Pinout Arrays
// =================================================================================

const GPIO_TypeDef* const relay_ports[NUM_SWITCHES] = {
    RELAY1_PORT, RELAY2_PORT, RELAY3_PORT, RELAY4_PORT,
    RELAY5_PORT, RELAY6_PORT, RELAY7_PORT, RELAY8_PORT
};

const uint16_t relay_pins[NUM_SWITCHES] = {
    RELAY1_PIN, RELAY2_PIN, RELAY3_PIN, RELAY4_PIN,
    RELAY5_PIN, RELAY6_PIN, RELAY7_PIN, RELAY8_PIN
};

const GPIO_TypeDef* const switch_ports[NUM_SWITCHES] = {
    SWITCH1_PORT, SWITCH2_PORT, SWITCH3_PORT, SWITCH4_PORT,
    SWITCH5_PORT, SWITCH6_PORT, SWITCH7_PORT, SWITCH8_PORT
};

const uint16_t switch_pins[NUM_SWITCHES] = {
    SWITCH1_PIN, SWITCH2_PIN, SWITCH3_PIN, SWITCH4_PIN,
    SWITCH5_PIN, SWITCH6_PIN, SWITCH7_PIN, SWITCH8_PIN
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
    if (relay_number >= NUM_SWITCHES) return;
    HAL_GPIO_WritePin((GPIO_TypeDef*)relay_ports[relay_number], relay_pins[relay_number], GPIO_PIN_RESET);
}

void Board_Relay_Off(uint8_t relay_number)
{
    if (relay_number >= NUM_SWITCHES) return;
    HAL_GPIO_WritePin((GPIO_TypeDef*)relay_ports[relay_number], relay_pins[relay_number], GPIO_PIN_SET);
}

uint8_t Board_Switch_Read(uint8_t switch_number)
{
    if (switch_number >= NUM_SWITCHES) return 0;
    return (HAL_GPIO_ReadPin((GPIO_TypeDef*)switch_ports[switch_number], switch_pins[switch_number]) == GPIO_PIN_RESET) ? 1 : 0;
}
