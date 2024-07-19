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

#include "firmware.h"

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

    PRINTF("Writing binary to flash flash\n");
    for (int i = 0; i < sizeof(records) / sizeof(records[0]); i++) {
    	PRINTF("%3d ", i);
		nrf5_flash_init(records[i].address, (uint32_t) records[i].length);
		val = nrf5_flash_write((const uint32_t *) records[i].data, records[i].length);
		if (val != records[i].length) {
			PRINTF("Expected to write %d, actually wrote %d\n", records[i].length, val);
		}
	}
    PRINTF("\n");
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
