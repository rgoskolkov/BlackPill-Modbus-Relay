#ifndef FLASH_DRIVER_H
#define FLASH_DRIVER_H

#include <stdint.h>

/**
 * @brief Writes the Modbus slave ID to the internal Flash memory.
 *
 * @param slave_id The slave ID to be written.
 * @return 0 on success, -1 on failure.
 */
int8_t flash_write_slave_id(uint8_t slave_id);

/**
 * @brief Reads the Modbus slave ID from the internal Flash memory.
 *
 * @return The stored slave ID, or a default value if the Flash is empty or corrupted.
 */
uint8_t flash_read_slave_id(void);

#endif /* FLASH_DRIVER_H */