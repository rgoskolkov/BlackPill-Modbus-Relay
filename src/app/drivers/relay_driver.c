#include "relay_driver.h"
#include "board_config.h"
#include <string.h>
#include "modbus_adapter.h"

static uint8_t relay_states_local[NUM_RELAYS];

static void Relay_Init(void)
{
    memset(relay_states_local, 0, sizeof(relay_states_local));
    for (uint8_t i = 0; i < NUM_RELAYS; ++i)
    {
        Board_Relay_Off(i);
        relay_states_local[i] = 0;
    }
}

static void Relay_SetState(uint8_t relay_number, uint8_t state)
{
    if (relay_number >= NUM_RELAYS)
        return;
    relay_states_local[relay_number] = state ? 1 : 0;
    state ? Board_Relay_On(relay_number) : Board_Relay_Off(relay_number);
    if (syncTaskHandle != NULL)
    {
        osThreadFlagsSet(syncTaskHandle, FLAG_SYNC_FROM_RELAY);
    }
}

uint8_t Relay_GetState(uint8_t relay_number)
{
    if (relay_number >= NUM_RELAYS)
        return 0;
    return relay_states_local[relay_number];
}

void relay_init(void) { Relay_Init(); }
void relay_on(uint8_t relay_number) { Relay_SetState(relay_number, 1); }
void relay_off(uint8_t relay_number) { Relay_SetState(relay_number, 0); }
void relay_toggle(uint8_t relay_number) { Relay_SetState(relay_number, !Relay_GetState(relay_number)); }