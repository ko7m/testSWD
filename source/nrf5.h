#pragma once

#include <stdbool.h>
#include <stdint.h>

bool nrf5_blank_check(uint32_t addr, uint32_t nbytes);
uint32_t nrf5_crc32(uint32_t addr, uint32_t nbytes);
void nrf5_mass_erase(void);
void nrf5_flash_init(uint32_t addr, uint32_t nbytes);
uint32_t nrf5_flash_write(const uint32_t *src, uint32_t nbytes);
void nrf5_flash_wait(void);
void nrf5_disable_uicr_approtect(void);
void nrf5_erase_uicr(void);
void nrf5_erase_all(void);
uint32_t nrf5_read_uicr_approtect(void);
void nrf5_write_uicr_approtect(uint32_t new_val);
