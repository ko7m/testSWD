/*******************************************************************************
 * @file dap.c
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
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "dap.h"
#include "target.h"
#include "fsl_debug_console.h"
//#include "wgo2.h"
#define SYS_LOG_NO_NEWLINE
#ifdef SYS_LOG_DOMAIN
#undef SYS_LOG_DOMAIN
#endif
#define SYS_LOG_DOMAIN "swd"
#ifdef SYS_LOG_LEVEL
#undef SYS_LOG_LEVEL
#endif
#define SYS_LOG_LEVEL CONFIG_SYS_LOG_DEFAULT_LEVEL
#define SYS_LOG_NO_NEWLINE
//#include <logging/sys_log.h>

#define SWD_ERROR_OK                    ACK_OK
#define SWD_ERROR_WAIT                  ACK_WAIT
#define SWD_ERROR_FAULT                 ACK_FAULT
#define SWD_ERROR_PROTOCOL              0x7
#define SWD_ERROR_PARITY                8

#define RAISE(x)	PRINTF("***SWD ERROR: %d\n", x);

bool calc_parity(uint32_t data)
{
	data ^= data >> 16;
	data ^= data >> 8;
	data ^= data >> 4;
	return (0x6996 >> (data & 0xF)) & 0x1;
}

const uint8_t header_lut[16] = {
	0x81,	// DP WR A2=0 A3=0
	0x95,	// DP WR A2=1 A3=0
	0x8D,	// DP WR A2=0 A3=1
	0x99,	// DP WR A2=1 A3=1
	0xC5,	// AP WR A2=0 A3=0
	0xD1,	// AP WR A2=1 A3=0
	0xC9,	// AP WR A2=0 A3=1
	0xDD,	// AP WR A2=1 A3=1
	0xA5,	// DP RD A2=0 A3=0
	0xB1,	// DP RD A2=1 A3=0
	0xA9,	// DP RD A2=0 A3=1
	0xBD,	// DP RD A2=1 A3=1
	0xE1,	// AP RD A2=0 A3=0
	0xF5,	// AP RD A2=1 A3=0
	0xED,	// AP RD A2=0 A3=1
	0xF9,	// AP RD A2=1 A3=1
};

static void header(uint8_t reg);
static void wdata(uint32_t data);
static uint32_t rdata(void);
static uint32_t dap_read_reg(uint8_t reg, uint32_t *data);
static uint32_t dap_write_reg(uint8_t reg, uint32_t data, bool ignoreAck);
static void dap_jtag2swd(void);
//static void dap_init_gpio(void);


/**********************************************************
 * Writes header byte to SPI.
 **********************************************************/
static void header(uint8_t reg)
{
	SWDIO_SET_OUTPUT();
	SWCLK_SET_OUTPUT();

	//header_lut table is in MSB format 
	for(int i=7; i >= 0;  i--)
	{
		WRITE_BIT(0x1 & (header_lut[reg]>>i));
	}

	// turnaround
	SWCLK_CYCLE();
}

/**********************************************************
 * Writes word to SPI with reversed bit order.
 **********************************************************/
static void wdata(uint32_t data)
{
	SWDIO_SET_OUTPUT();
	SWCLK_SET_OUTPUT();

	for(int i=0; i<32; i++)
	{
		WRITE_BIT(0x1 & (data>>i));
	}

	// calculate and output parity bit
	SWDIO_OUT(calc_parity(data));

	// clock out parity bit
	SWCLK_CYCLE();
}

/**********************************************************
 * Reads word from SPI with reversed bit order.
 **********************************************************/
static uint32_t rdata(void)
{
	uint32_t b;
	uint32_t val = 0;
	SWCLK_SET_OUTPUT();
	SWDIO_SET_INPUT();

	for(int i=0; i < 32 ; i++)
	{
		READ_BIT(b);
		val |= b<<i;
	}
	return val;
}


/**********************************************************
 * Reads from an AP or DP register.
 *
 * @param ap
 *   If this parameter is true, read from AP register.
 *   If false read from DP register.
 *
 * @param reg
 *   The register number [0-3] to read from
 *
 * @param data[out]
 *   The register value is written to this parameter
 **********************************************************/
static uint32_t dap_read_reg(uint8_t reg, uint32_t *data)
{
	__attribute__((unused)) uint32_t parity;
	uint32_t b;
	uint32_t ack = 0;

	// write header incl. turnaround
	header(reg + (sizeof(header_lut) / 2));

	SWCLK_SET_OUTPUT();
	SWDIO_SET_INPUT();

	/* Read acknowledge */
	READ_BIT(b);
	ack |= b;
	READ_BIT(b);
	ack |= b << 1;
	READ_BIT(b);
	ack |= b << 2;

	/* Verify that ACK is OK */
	if ( ack == ACK_OK ) {

		// read a word using SPI
		*data = rdata();
	
		/* Read parity bit */
		READ_BIT(parity);

		/* Verify parity */
		// if ( cb == parity )
		// {
			// ret = SWD_ERROR_OK;
		// } else {
		// 	ack = SWD_ERROR_PARITY;
		// }
	}

	/* Turnaround */
	SWCLK_CYCLE();

	// return bus to idle state
	SWCLK_CLR();
	SWDIO_SET_OUTPUT();
	SWDIO_SET();
	for (uint32_t i=0; i<5; i++) {
		__asm__ __volatile__ ("nop");
	}

	return ack;
}


/**********************************************************
 * Writes to a DP or AP register.
 *
 * @param ap
 *   If this parameter is true, write to AP register.
 *   If false write to DP register.
 *
 * @param reg
 *   The register number [0-3] to write to
 *
 * @param data
 *   The value to write to the register
 **********************************************************/
static uint32_t dap_write_reg(uint8_t reg, uint32_t data, bool ignoreAck)
{
	uint32_t ack = 0;
	uint32_t b;

	// write header incl. turnaround
	header(reg);

	SWCLK_SET_OUTPUT();
	SWDIO_SET_INPUT();

	/* Read acknowledge */
	READ_BIT(b);
	ack |= b;
	READ_BIT(b);
	ack |= b << 1;
	READ_BIT(b);
	ack |= b << 2;

	/* Turnaround */
	SWCLK_CYCLE();

	if ( ack == ACK_OK || ignoreAck )
	{
		// write data incl. parity bit
		wdata(data);
	} else {
		//gpio_config(SWDIO_PORT, SWDIO_PIN, PORT, OUT, PP);
		SWDIO_SET_OUTPUT();
	}

	// return bus to idle state
	SWCLK_CLR();
	SWDIO_SET();
	for (uint32_t i=0; i<5; i++) {
		__asm__ __volatile__ ("nop");
	}

	return ack;
	
}


/**********************************************************
 * Sends the JTAG-to-SWD sequence. This must be performed
 * at the very beginning of every debug session and
 * again in case of a protocol error.
 **********************************************************/
static void dap_jtag2swd(void)
{	
	uint16_t val = JTAG2SWD;

	SWDIO_SET_OUTPUT();
	SWCLK_SET_OUTPUT();


	// line reset (64 bits w/ SWDIO high)
	for (int i = 64; i--; ) {
		WRITE_BIT(1);
	}	

	//JTAG-to-SWD Switching sequence, JTAG2SWD is in MSB format
	for (int i = 15; i >= 0 ; i--) {
		WRITE_BIT(0x1 & (val>>i));
	}

	// line reset (64 bits w/ SWDIO high)
	for (int i = 64; i--; ) {
		WRITE_BIT(1);
	}

	// idle period (16 bits w/ SWD low)
	for (int i = 16; i--; ) {
		WRITE_BIT(0);
	}

	// return bus to idle state
	SWDIO_SET();
	SWCLK_CLR();
	for (uint32_t i=0; i<5; i++) {
		__asm__ __volatile__ ("nop");
	}
}


/**********************************************************
 * Writes to one of the four AP registers in the currently
 * selected AP bank.
 *
 * @param reg[in]
 *    The register number [0-3] to write to
 *
 * @param data[in]
 *    Value to write to the register
 *
 **********************************************************/
bool dap_write_ap(int reg, uint32_t data)
{
	uint32_t swdStatus;
	uint32_t retry = SWD_RETRY_COUNT;

	do {
		swdStatus = dap_write_reg(reg, data, false);
		retry--;
	} while ( swdStatus == SWD_ERROR_WAIT && retry > 0 );

	if ( swdStatus != SWD_ERROR_OK )
	{
		RAISE(swdStatus);
		return false;
	}

	return true;
}


/**********************************************************
 * Writes to one of the four DP registers.
 *
 * @param reg[in]
 *    The register number [0-3] to write to
 *
 * @param data[in]
 *    Value to write to the register
 * 
 * @return Success status
 *
 **********************************************************/
uint8_t dap_write_dp(int reg, uint32_t data)
{
	uint32_t swdStatus;
	uint32_t retry = SWD_RETRY_COUNT;

	do {
		swdStatus = dap_write_reg(reg, data, false);
		retry--;
	} while ( swdStatus == SWD_ERROR_WAIT && retry > 0 );

	if ( swdStatus != SWD_ERROR_OK )
	{
		RAISE(swdStatus);
	}
	return swdStatus;
}


/**********************************************************
 * Reads one of the four AP registers in the currently
 * selected AP bank.
 *
 * @param reg[in]
 *    The register number [0-3] to read
 *
 * @param data[out]
 *    Value of register is written to this parameter
 *
 **********************************************************/
void dap_read_ap(int reg, uint32_t *data)
{	
	uint32_t swdStatus;
	uint32_t retry = SWD_RETRY_COUNT;

	do {
		swdStatus = dap_read_reg(reg, data);
		if (SWD_ERROR_FAULT & swdStatus) {
			dap_write_dp(DP_ABORT, 0x1E);	// TODO: decode and define
			// dap_write_dp(DP_SELECT, 0);
		}
		retry--;
	} while ( swdStatus == SWD_ERROR_WAIT && retry > 0 );

	if ( swdStatus != SWD_ERROR_OK )
	{
		RAISE(swdStatus);
	}
}


/**********************************************************
 * Reads one of the four DP registers.
 *
 * @param reg[in]
 *    The register number [0-3] to read
 *
 * @param data[out]
 *    Value of register is written to this parameter
 * 
 * @return
 *    Success status
 *
 **********************************************************/
uint8_t dap_read_dp(int reg, uint32_t *data)
{
	uint32_t swdStatus;
	uint32_t retry = SWD_RETRY_COUNT;

	do {
		swdStatus = dap_read_reg(reg, data);
		if (SWD_ERROR_FAULT & swdStatus) {
			dap_write_dp(DP_ABORT, 0x1E);	// TODO: decode and define
			// dap_write_dp(DP_SELECT, 0);
		}
		retry--;
	} while ( swdStatus == SWD_ERROR_WAIT && retry > 0 );

	if ( swdStatus != SWD_ERROR_OK ) {
		RAISE(swdStatus);
	}
	return swdStatus;
}


static void dap_init_gpio(void)
{
//	struct device *gpio_dev = device_get_binding(CONFIG_GPIO_ACTS_DEV_NAME);
//	struct device *gpio_dev = NULL;
//	if (gpio_dev == NULL)
//	{
//       PRINTF("cannot find dap gpio \n");
//	   return;
//  }
	SWDIO_SET_OUTPUT();
	SWDIO_SET_INPUT();

	PRINTF("dap_init_gpio\n");
}


/**********************************************************
 * Initalized the SW-DP. This function performs first
 * sends the JTAG-to-SWD sequence and then reads
 * the IDCODE register.
 *
 * @returns
 *    The value of the IDCODE register
 **********************************************************/
uint32_t dap_init_dp(void)
{
	uint32_t dpId, retry;
	uint8_t initStatus;

	dap_init_gpio();

	retry = 100;
	while ( retry > 0 )
	{
		/* If connecting to a Multidrop capable SW-DP (SWD version 2), this
		* function must be run first. */
		//exitDormantState();

		/* Send the JTAG-to-SWD switching sequence */
		dap_jtag2swd();

		/* Read IDCODE to get the DAP out of reset state */ 
		initStatus = dap_read_dp(DP_IDCODE, &dpId);
		if (initStatus == SWD_ERROR_OK) 
		{
			initStatus = dap_write_dp(DP_ABORT, 0x1E);
		}

		/* Debug power up request */
		if (initStatus == SWD_ERROR_OK) 
		{
			initStatus = dap_write_dp(DP_CTRL, DP_CTRL_CSYSPWRUPREQ | DP_CTRL_CDBGPWRUPREQ);
		}

		/* Retry on fail */
		if (initStatus == SWD_ERROR_OK) 
		{
			break;
		}
		else 
		{
			PRINTF("SWD comm init fail\n");
			//k_sleep(K_MSEC(1));
			retry--;
			continue;
		}
	}
	if (initStatus != SWD_ERROR_OK)
	{
		PRINTF("Couldn't init the SWD comm! \n");
	}

	/* Wait until we receive powerup ACK */
	uint32_t rbStatus;
	retry = PWRUP_TIMEOUT + 200;
	while ( retry > 0 )
	{
		dap_read_dp(DP_CTRL, &rbStatus);
		if ( (rbStatus & (DP_CTRL_CDBGPWRUPACK | DP_CTRL_CSYSPWRUPACK))
							 == (DP_CTRL_CDBGPWRUPACK | DP_CTRL_CSYSPWRUPACK) )
		{
			break;
		}
		retry--;
	}

	/* Throw error if we failed to power up the debug interface */
	if ( (rbStatus & (DP_CTRL_CDBGPWRUPACK | DP_CTRL_CSYSPWRUPACK))
							!= (DP_CTRL_CDBGPWRUPACK | DP_CTRL_CSYSPWRUPACK) )
	{
		RAISE(rbStatus);
	}

	/* Select ahb-ap Bank 0x0 */
	dap_write_dp(DP_SELECT, AP_SELECT_APSEL_AHBAP | AP_SELECT_APBANKSEL(0x0));
	PRINTF("Select ahb-ap bank 0x0: %x\n", AP_SELECT_APSEL_AHBAP | AP_SELECT_APBANKSEL(0x0));

	return dpId;
}


/**********************************************************
 * Initalize the AHB-AP. The transfer size must be set
 * to 32-bit before trying to access any internal
 * memory.
 **********************************************************/
void dap_init_ap(void)
{
	/* Set transfer size to 32 bit */
	dap_write_ap(AP_CSW, AP_CSW_DEFAULT);
}

/**********************************************************
 * Check chip protection state through the CTRL-AP.
 * If chip is protected, runs the Erase All sequence.
 * This will erase all chip memories and unlock the AHB-AP
 * until the next reset.
 *  * @returns
 *    Operation success
 **********************************************************/
bool dap_nrf5810x_ctrlap_unlock(void)
{
	uint32_t readback, retry;

	/* Select ctrl-ap Bank 0xF */
	dap_write_dp(DP_SELECT, AP_SELECT_APSEL_CTRLAP | AP_SELECT_APBANKSEL(0xF));
	PRINTF("Select ctrl-ap bank 0xF: %x\n", AP_SELECT_APSEL_CTRLAP | AP_SELECT_APBANKSEL(0xF));

	/* Read ctrl-ap IDR (0xFC) */
	dap_read_ap(CTRLAP_IDR, &readback); // initial dummy read to set the read offset
	dap_read_dp(DP_RDBUFF, &readback);
	PRINTF("Read CTRL-AP IDR: %x\n", readback);

	/* Select ctrl-ap Bank 0x0 */
	dap_write_dp(DP_SELECT, AP_SELECT_APSEL_CTRLAP | AP_SELECT_APBANKSEL(0x0));
	PRINTF("Select ctrl-ap bank 0x0: %x\n", AP_SELECT_APSEL_CTRLAP | AP_SELECT_APBANKSEL(0x0));

	/* Read protection status */
	dap_read_ap(CTRLAP_APPPROTECTSTATUS, &readback); // initial dummy read to set the read offset
	dap_read_dp(DP_RDBUFF, &readback);
	PRINTF("APPPROTECTSTATUS: %x\n", readback);

	/* Un-protect if needed */
	if (!(readback & 0x1))
	{
		PRINTF("NRF5 protection is detected. Running unlock sequence.\n");
		/* Hold the chip in reset. Readback until reads 1 (as seen on jlink)*/
		PRINTF("Resetting chip through ctrl-ap\n");
		dap_write_ap(CTRLAP_RESET, 0x1);
		retry = 100;
		while ( retry > 0 )
		{
			dap_read_ap(CTRLAP_RESET, &readback);
			dap_read_dp(DP_RDBUFF, &readback);
			if (readback & 0x1)
			{
				break;
			}
			retry--;
		}
		if (!retry)
		{
			PRINTF("Couldn't reset the nrf chip from ctrl-ap\n");
			return false;
		}
		dap_write_ap(CTRLAP_RESET, 0x0);

		/* Disable the reset. Readback until reads 0 (as seen on jlink)*/
		dap_write_ap(CTRLAP_RESET, 0x0);
		retry = 100;
		while ( retry > 0 )
		{
			dap_read_ap(CTRLAP_RESET, &readback);
			dap_read_dp(DP_RDBUFF, &readback);
			if (!(readback & 0x1))
			{
				break;
			}
			retry--;
		}
		if (!retry)
		{
			PRINTF("Couldn't disable nrf reset from ctrl-ap\n");
			return false;
		}
		PRINTF("Reset done\n");

		/* Write erase all */
		PRINTF("Setting EraseAll to unlock chip\n");
		dap_write_ap(CTRLAP_ERASEALL, 0x1);

		/* Wait until erase status is ready */
		retry = 1000;
		while ( retry > 0 )
		{
			dap_read_ap(CTRLAP_ERASEALLSTATUS, &readback);
			dap_read_dp(DP_RDBUFF, &readback);
			if (!(readback & 0x1))
			{
				PRINTF("EraseAll Done\n");
				break;
			}
			retry--;
			//k_sleep(K_MSEC(1));
		}
		if (!retry)
		{
			PRINTF("Coulnd't reset the nrf chip from ctrl-ap\n");
			return false;
		}
		PRINTF("NRF5 chip is successfully unlocked\n");
	}
	else
	{
		PRINTF("NRF5 chip is already unlocked\n");
	}

	/* Re-select ahb-ap Bank 0x0 for the rest of the regular session */
	dap_write_dp(DP_SELECT, AP_SELECT_APSEL_AHBAP | AP_SELECT_APBANKSEL(0x0));
	PRINTF("Select ahb-ap bank 0x0: %x\n", AP_SELECT_APSEL_AHBAP | AP_SELECT_APBANKSEL(0x0));

	return true;
}
