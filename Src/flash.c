/*

  flash.c - driver code for STM32G0xx ARM processors

  Part of grblHAL

  Copyright (c) 2019-2023 Terje Io
  Copyright (c) 2026 STM32G0xx port contributors

  This code reads/writes the whole RAM-based emulated EPROM contents from/to flash

  grblHAL is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  grblHAL is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with grblHAL. If not, see <http://www.gnu.org/licenses/>.

*/

#include <string.h>

#include "main.h"
#include "grbl/hal.h"

// Last flash page(s) are reserved for settings (NVS), address is set in the linker script.
extern void *_EEPROM_Emul_Start;

bool memcpy_from_flash (uint8_t *dest)
{
    memcpy(dest, &_EEPROM_Emul_Start, hal.nvs.size);
    return true;
}

bool memcpy_to_flash (uint8_t *source)
{
    if (!memcmp(source, &_EEPROM_Emul_Start, hal.nvs.size))
        return true;

    HAL_FLASH_Unlock();

    uint32_t address = (uint32_t)&_EEPROM_Emul_Start;
    uint32_t abs_page = (address - FLASH_BASE) / FLASH_PAGE_SIZE;
    uint32_t n_pages = ((uint32_t)hal.nvs.size + FLASH_PAGE_SIZE - 1) / FLASH_PAGE_SIZE;

    FLASH_EraseInitTypeDef erase = {
        .TypeErase = FLASH_TYPEERASE_PAGES,
#if defined(FLASH_BANK_2)
        .Banks = abs_page < FLASH_PAGE_NB ? FLASH_BANK_1 : FLASH_BANK_2,
        .Page = abs_page < FLASH_PAGE_NB ? abs_page : abs_page - FLASH_PAGE_NB,
#else
        .Banks = FLASH_BANK_1,
        .Page = abs_page,
#endif
        .NbPages = n_pages
    };

    uint32_t error;
    HAL_StatusTypeDef status;

    if((status = HAL_FLASHEx_Erase(&erase, &error)) != HAL_OK)
        status = HAL_FLASHEx_Erase(&erase, &error);

    uint64_t data;
    uint8_t *src = source;
    uint32_t remaining = ((uint32_t)hal.nvs.size + 7) & ~7UL;

    while(remaining && status == HAL_OK) {
        memcpy(&data, src, sizeof(uint64_t)); // source may not be 8-byte aligned
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, data);
        src += 8;
        address += 8;
        remaining -= 8;
    }

    HAL_FLASH_Lock();

    return status == HAL_OK;
}
