/*******************************************************************************
 * @file dap.h
 * @brief Low level SWD interface functions.
 * @author Silicon Labs
 * @version 1.03
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2014 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
 * obligation to support this Software. Silicon Labs is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Silicon Labs will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 ******************************************************************************/
#pragma once

//#include <kernel.h>
//#include <soc.h>
//#include "wgo2.h"
//#include "gpio.h"
#include <stdbool.h>
#include <stdint.h>
#include "pin_mux.h"
#include "fsl_gpio.h"


/* GPIO registers direct access */
#define GPIO_BSR0_ADDR	(0xC0090108)
#define GPIO_BSR1_ADDR	(0xC009010C)
#define GPIO_BRR0_ADDR	(0xC0090110)
#define GPIO_BRR1_ADDR	(0xC0090114)
#define GPIO_IDAT0_ADDR	(0xC0090118)
#define GPIO_IDAT1_ADDR	(0xC009011c)

/* Slow down + fine tune SWD signal */
#define SWDCLK_WRITE_NUM_NOPS_CLEAR	7
#define SWDCLK_WRITE_NUM_NOPS_SET	4
#define SWDCLK_CYCLE_NUM_NOPS_CLEAR	8
#define SWDCLK_CYCLE_NUM_NOPS_SET	8
#define SWDCLK_READ_NUM_NOPS_CLEAR	7
#define SWDCLK_READ_NUM_NOPS_SET	7

/* ACK responses */
#define ACK_OK     1
#define ACK_WAIT   2
#define ACK_FAULT  4

/* AP/DP register macro */
#define AP(reg)    (reg | (1 << 2))
#define DP(reg)    (reg)

/* Address of DP read registers */
#define DP_IDCODE  DP(0)
#define DP_CTRL    DP(1)
#define DP_RESEND  DP(2)
#define DP_RDBUFF  DP(3)

/* Adressses of DP write registers */
#define DP_ABORT   DP(0)
#define DP_STAT    DP(1)
#define DP_SELECT  DP(2)

/* AHB-AP registers */
#define AP_CSW     AP(0)
#define AP_TAR     AP(1)
#define AP_DRW     AP(3)
#define AP_IDR     AP(3)  /* In bank 0xf */

/* Bit fields for the CSW register */
#define AP_CSW_32BIT_TRANSFER   0x02
#define AP_CSW_AUTO_INCREMENT   0x10
#define AP_CSW_MASTERTYPE_DEBUG (1 << 29)
#define AP_CSW_HPROT            (1 << 25)
#define AP_CSW_DEFAULT (AP_CSW_MASTERTYPE_DEBUG | AP_CSW_HPROT | AP_CSW_32BIT_TRANSFER)

/* Bit fields for the Select register */
#define AP_SELECT_APSEL_AHBAP			(0<<24)
#define AP_SELECT_APSEL_CTRLAP			(1<<24) // NRF52810x
#define AP_SELECT_APBANKSEL(bank_sel)	((bank_sel & 0xF) << 4)

/* Powerup request and acknowledge bits in CTRL/STAT */
#define DP_CTRL_CDBGPWRUPREQ  (1 << 28)
#define DP_CTRL_CDBGPWRUPACK  (1 << 29)
#define DP_CTRL_CSYSPWRUPREQ  (1 << 30)
#define DP_CTRL_CSYSPWRUPACK  (1 << 31)

/* NRF52810x CTRL-AP registers */
#define CTRLAP_RESET			AP(0) /* In bank 0x0 */
#define CTRLAP_ERASEALL			AP(1)
#define CTRLAP_ERASEALLSTATUS	AP(2)
#define CTRLAP_APPPROTECTSTATUS	AP(3)
#define CTRLAP_IDR				AP(3) /* In bank 0xf */

/* Number of times to retry an SWD operation when receiving
 * a WAIT response */
#define SWD_RETRY_COUNT 200

/* Number of times to retry the connection sequence */
#define CONNECT_RETRY_COUNT 3

/* Number of times to retry reading the AHB-IDR register when connecting */
#define AHB_IDR_RETRY_COUNT 20

/* Number of times to retry reading the CTLR/STAT
 * register while waiting for powerup acknowledge */
#define PWRUP_TIMEOUT 100

/* Number of times to retry reading status registers while
 * waiting for a debug event (such as a halt of soft reset) */
#define DEBUG_EVENT_TIMEOUT 200

/* Number of times to wait for flashloader */
#define FLASHLOADER_RETRY_COUNT 1000

/* Number of times to wait for MSC operation to complete */
#define MSC_TIMEOUT 100


/* JTAG to SWD bit sequence, transmitted MSB first */
#define JTAG2SWD 0x79E7

#define SWDIO_SET_INPUT() {\
	  gpio_pin_config_t PINSWDIO_config = {\
		  .direction = kGPIO_DigitalInput,\
		  .outputLogic = 0U,\
		  .interruptMode = kGPIO_NoIntmode\
	  };\
	  GPIO_PinInit(BOARD_INITPINS_PINSWDIO_PERIPHERAL, BOARD_INITPINS_PINSWDIO_CHANNEL, &PINSWDIO_config);\
	}	

#define SWDIO_SET_OUTPUT() {\
	  gpio_pin_config_t PINSWDIO_config = {\
		  .direction = kGPIO_DigitalOutput,\
		  .outputLogic = 0U,\
		  .interruptMode = kGPIO_NoIntmode\
	  };\
	  GPIO_PinInit(BOARD_INITPINS_PINSWDIO_PERIPHERAL, BOARD_INITPINS_PINSWDIO_CHANNEL, &PINSWDIO_config);\
	}

#define SWCLK_SET_INPUT() {\
	  gpio_pin_config_t PINSWDCLK_config = {\
		  .direction = kGPIO_DigitalInput,\
		  .outputLogic = 0U,\
		  .interruptMode = kGPIO_NoIntmode\
	  };\
	  GPIO_PinInit(BOARD_INITPINS_PINSWDCLK_PERIPHERAL, BOARD_INITPINS_PINSWDCLK_CHANNEL, &PINSWDCLK_config);\
	}

#define SWCLK_SET_OUTPUT() {\
	  gpio_pin_config_t PINSWDCLK_config = {\
		  .direction = kGPIO_DigitalOutput,\
		  .outputLogic = 0U,\
		  .interruptMode = kGPIO_NoIntmode\
	  };\
	  GPIO_PinInit(BOARD_INITPINS_PINSWDCLK_PERIPHERAL, BOARD_INITPINS_PINSWDCLK_CHANNEL, &PINSWDCLK_config);\
	  }

#define SWDIO_SET() GPIO_PortSet(BOARD_INITPINS_PINSWDIO_PERIPHERAL, 1U << BOARD_INITPINS_PINSWDIO_CHANNEL);
#define SWDIO_CLR() GPIO_PortClear(BOARD_INITPINS_PINSWDIO_PERIPHERAL, 1U << BOARD_INITPINS_PINSWDIO_CHANNEL);
#define SWCLK_SET() GPIO_PortSet(BOARD_INITPINS_PINSWDCLK_PERIPHERAL, 1U << BOARD_INITPINS_PINSWDCLK_CHANNEL);
#define SWCLK_CLR() GPIO_PortClear(BOARD_INITPINS_PINSWDCLK_PERIPHERAL, 1U << BOARD_INITPINS_PINSWDCLK_CHANNEL);

#define SWDIO_OUT(bit) { if (bit) { SWDIO_SET(); } else { SWDIO_CLR(); }}

#define SWDIO_IN() GPIO_PinRead(BOARD_INITPINS_PINSWDIO_PERIPHERAL, BOARD_INITPINS_PINSWDIO_CHANNEL)

#define SWCLK_CYCLE()   \
	SWCLK_CLR();          \
	for (uint32_t i=0; i<SWDCLK_CYCLE_NUM_NOPS_CLEAR; i++) { \
		__asm__ __volatile__ ("nop"); \
	} \
	SWCLK_SET(); \
	for (uint32_t i=0; i<SWDCLK_CYCLE_NUM_NOPS_SET; i++) { \
		__asm__ __volatile__ ("nop"); \
	}

#define WRITE_BIT(bit) \
	SWDIO_OUT(bit) \
	SWCLK_CLR(); \
	for (uint32_t i=0; i<SWDCLK_WRITE_NUM_NOPS_CLEAR; i++) { \
		__asm__ __volatile__ ("nop"); \
	} \
	SWCLK_SET(); \
	for (uint32_t i=0; i<SWDCLK_WRITE_NUM_NOPS_SET; i++) { \
		__asm__ __volatile__ ("nop"); \
	}

#define READ_BIT(bit) \
	SWCLK_CLR(); \
	for (uint32_t i=0; i<SWDCLK_READ_NUM_NOPS_CLEAR; i++) { \
		__asm__ __volatile__ ("nop"); \
	} \
	bit = SWDIO_IN(); \
	SWCLK_SET(); \
	for (uint32_t i=0; i<SWDCLK_READ_NUM_NOPS_SET; i++) { \
		__asm__ __volatile__ ("nop"); \
	}

bool dap_write_ap(int reg, uint32_t data);
void dap_read_ap(int reg, uint32_t *data);
uint8_t dap_read_dp(int reg, uint32_t *data);
uint32_t dap_init_dp(void);
void dap_init_ap(void);
bool dap_nrf5810x_ctrlap_unlock(void);

