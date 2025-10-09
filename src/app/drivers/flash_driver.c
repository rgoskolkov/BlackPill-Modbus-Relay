#include "flash_driver.h"
#include "main.h"
#include "board_config.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_flash.h"
#include "stm32f4xx_hal_flash_ex.h"

int8_t flash_write_slave_id(uint8_t slave_id)
{
    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef EraseInitStruct;
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
    EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
    EraseInitStruct.Sector = FLASH_CONFIG_SECTOR_NUM;
    EraseInitStruct.NbSectors = 1;

    uint32_t SectorError = 0;
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
    {
        HAL_FLASH_Lock();
        return -1;
    }

    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, FLASH_CONFIG_SECTOR_ADDR, slave_id) != HAL_OK)
    {
        HAL_FLASH_Lock();
        return -1;
    }

    HAL_FLASH_Lock();
    return 0;
}

uint8_t flash_read_slave_id(void)
{
    uint8_t slave_id = *(__IO uint8_t*)FLASH_CONFIG_SECTOR_ADDR;

    if (slave_id == 0xFF) // Flash erased
    {
        return MODBUS_SLAVE_ID; // Return default
    }

    return slave_id;
}