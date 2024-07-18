/*
 * Copyright 2016-2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file    testswd.cpp
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MIMXRT1021.h"
#include "fsl_debug_console.h"
#include "dap.h"

//#include "firmware.h"

typedef struct {
    uint32_t address;
    uint8_t length;
    uint8_t data[16];
 } DataRecord;

DataRecord records[] = {
    {0x0000, 16, {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}},
 };

extern "C" {
extern void swd_connect_read_test(void);
#include "nrf5.h"
}

/*
 * @brief   Application entry point.
 */
int main(void) {
    /* Init board hardware. */
    BOARD_ConfigMPU();
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif

    PRINTF("\nSWD tests...\n\n");
    PRINTF("SRAM read and write test\n");
    swd_connect_read_test();

    // Check the current flash blank status
    uint32_t val = nrf5_blank_check(0UL, 256UL);
    PRINTF("\nFlash is currently %sblank\n", val ? "" : "non-");

    PRINTF("Erasing flash\n");
    nrf5_mass_erase();

    val = nrf5_blank_check(0UL, 256UL);
    PRINTF("Flash is currently %sblank\n", val ? "" : "non-");

    PRINTF("Writing the first hex record to flash\n");
    nrf5_flash_init(records[0].address, (uint32_t) records[0].length);
    val = nrf5_flash_write((const uint32_t *) records[0].address, records[0].length);
    nrf5_flash_wait();
    PRINTF("Done\n");

    // Release the SWD GPIO pins so I can access part with J-Link
    SWDIO_SET_INPUT();
    SWCLK_SET_INPUT();

    /* Force the counter to be placed into memory. */
    volatile static int i = 0 ;
    /* Enter an infinite loop, just incrementing a counter. */
    while(1) {
        i++ ;
        /* 'Dummy' NOP to allow source level single stepping of
            tight while() loop */
        __asm volatile ("nop");
    }
    return 0 ;
}
