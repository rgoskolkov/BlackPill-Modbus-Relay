#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H
/*
  board_config.h
   Можно переопределить через build_flags (-D...) в platformio.ini
*/

#include "main.h"
#include <stdint.h>

#ifndef MODBUS_UART_STOPBITS
#define MODBUS_UART_STOPBITS 1
#endif

/* Параметры по умолчанию (можно переопределить через -D в platformio.ini) */
#ifndef NUM_SWITCHES
#define NUM_SWITCHES 8U
#endif

#ifndef MODBUS_BAUDRATE
#define MODBUS_BAUDRATE 115200U
#endif

#ifndef MODBUS_SLAVE_ID
#define MODBUS_SLAVE_ID 2U
#endif

/* MODBUS_UART_PARITY: 0 = NONE, 1 = ODD, 2 = EVEN */
#ifndef MODBUS_UART_PARITY
#define MODBUS_UART_PARITY 0
#endif

#ifndef UART_DEBUG
#define UART_DEBUG 0
#endif

#ifndef MONITOR_TASK
#define MONITOR_TASK 0
#endif

/* --- Flash-память для хранения slaveId --- */
#ifndef FLASH_CONFIG_SECTOR_ADDR
#define FLASH_CONFIG_SECTOR_ADDR ((uint32_t)0x08020000)
#endif
#ifndef FLASH_CONFIG_SECTOR_NUM
#define FLASH_CONFIG_SECTOR_NUM  FLASH_SECTOR_5
#endif
/* ----------------------------------------- */

#ifndef DEBOUNCE_MS
#define DEBOUNCE_MS 300U
#endif

/* Relay pins (берутся напрямую из main.h) */
#define RELAY1_PIN RELAY1_Pin
#define RELAY1_PORT RELAY1_GPIO_Port
#define RELAY2_PIN RELAY2_Pin
#define RELAY2_PORT RELAY2_GPIO_Port
#define RELAY3_PIN RELAY3_Pin
#define RELAY3_PORT RELAY3_GPIO_Port
#define RELAY4_PIN RELAY4_Pin
#define RELAY4_PORT RELAY4_GPIO_Port
#define RELAY5_PIN RELAY5_Pin
#define RELAY5_PORT RELAY5_GPIO_Port
#define RELAY6_PIN RELAY6_Pin
#define RELAY6_PORT RELAY6_GPIO_Port
#define RELAY7_PIN RELAY7_Pin
#define RELAY7_PORT RELAY7_GPIO_Port
#define RELAY8_PIN RELAY8_Pin
#define RELAY8_PORT RELAY8_GPIO_Port

/* Switch pins (берутся напрямую из main.h) */
#define SWITCH1_PIN SWITCH1_Pin
#define SWITCH1_PORT SWITCH1_GPIO_Port
#define SWITCH2_PIN SWITCH2_Pin
#define SWITCH2_PORT SWITCH2_GPIO_Port
#define SWITCH3_PIN SWITCH3_Pin
#define SWITCH3_PORT SWITCH3_GPIO_Port
#define SWITCH4_PIN SWITCH4_Pin
#define SWITCH4_PORT SWITCH4_GPIO_Port
#define SWITCH5_PIN SWITCH5_Pin
#define SWITCH5_PORT SWITCH5_GPIO_Port
#define SWITCH6_PIN SWITCH6_Pin
#define SWITCH6_PORT SWITCH6_GPIO_Port
#define SWITCH7_PIN SWITCH7_Pin
#define SWITCH7_PORT SWITCH7_GPIO_Port
#define SWITCH8_PIN SWITCH8_Pin
#define SWITCH8_PORT SWITCH8_GPIO_Port


/* =================================================================================
   Hardware Peripheral Mapping
==================================================================================*/

// Declare UART handles defined in usart.c
//extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

// Map logical names to physical UART handles
#define MODBUS_UART_HANDLE huart2
//#define DEBUG_UART_HANDLE  huart1


/* =================================================================================
   Hardware Porting Layer: Pin Mapping Arrays (extern)
==================================================================================*/

extern const GPIO_TypeDef* const relay_ports[NUM_SWITCHES];
extern const uint16_t relay_pins[NUM_SWITCHES];
extern const GPIO_TypeDef* const switch_ports[NUM_SWITCHES];
extern const uint16_t switch_pins[NUM_SWITCHES];

/* =================================================================================
   Hardware Porting Layer: Function Prototypes
==================================================================================*/

// --- System Tick ---
uint32_t Board_GetTick(void);
void Board_Delay(uint32_t Delay);

// --- GPIO ---
void Board_GPIO_Write(void *gpio_port, uint16_t gpio_pin, uint8_t state);
uint8_t Board_GPIO_Read(void *gpio_port, uint16_t gpio_pin);

// --- Wrappers for specific hardware ---
void Board_LED_On(void);
void Board_LED_Off(void);
void Board_Relay_On(uint8_t relay_number);
void Board_Relay_Off(uint8_t relay_number);
uint8_t Board_Switch_Read(uint8_t switch_number);


#endif /* BOARD_CONFIG_H */