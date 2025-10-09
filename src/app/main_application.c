#include "input_driver.h"
#include "modbus_adapter.h"
#include "relay_driver.h"

void application_init(void)
{
    Input_Init();
    relay_init();
    modbus_adapter_init();
}