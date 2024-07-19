#include <stdbool.h>
//#include "DA14195.h"
//#include "system.h"
//#include "wgo2.h"
#include "swd_app.h"
#include "target.h"
#include "nrf5.h"
//#include "wgo2.h"
//#include <logging/sys_log.h>
#include "fsl_debug_console.h"


#define NRF52_SRAM_BASE		0x20000000
#define NRF52_RAM0S0_BASE	NRF52_SRAM_BASE
#define NRF52_RAM0S1_BASE	(NRF52_RAM0S0_BASE + 0x1000)
#define NRF52_RAM1S0_BASE	(NRF52_RAM0S1_BASE + 0x1000)
#define NRF52_RAM1S1_BASE	(NRF52_RAM1S0_BASE + 0x1000)

#define ALGO_CODE_BASE		NRF52_SRAM_BASE

#define ALGO_FIFO_WP_ADDR	WORKAREA_BASE
#define ALGO_FIFO_RP_ADDR	(WORKAREA_BASE + 4)
#define ALGO_FIFO_BASE		NRF52_RAM1S1_BASE
#define ALGO_FIFO_SIZE		0x1000

#define	WORKAREA_BASE		(ALGO_FIFO_BASE - 8)	// place first two words below NRF52_RAM1S1_BASE to exploit TAR wraparound
#define WORKAREA_SIZE		(ALGO_FIFO_SIZE + 8)

/* Non-Volatile Memory Controller Registers */
enum nrf5_nvmc_registers {
	NRF5_NVMC_BASE = 0x4001E000, 
#define NRF5_NVMC_REG(offset) (NRF5_NVMC_BASE + offset)
	NRF5_NVMC_READY		= NRF5_NVMC_REG(0x400),
	NRF5_NVMC_CONFIG	= NRF5_NVMC_REG(0x504),
	NRF5_NVMC_ERASEPAGE	= NRF5_NVMC_REG(0x508),
	NRF5_NVMC_ERASEALL	= NRF5_NVMC_REG(0x50C),
	NRF5_NVMC_ERASEUICR	= NRF5_NVMC_REG(0x514),
};

enum nrf5_nvmc_config_bits {
	NRF5_NVMC_CONFIG_REN = 0x00,
	NRF5_NVMC_CONFIG_WEN = 0x01,
	NRF5_NVMC_CONFIG_EEN = 0x02,
};

/* UICR non-volatile registers */
enum nrf5_uicr_registers {
	NRF5_UICR_BASE = 0x10001000, 
#define NRF5_UICR_REG(offset) (NRF5_UICR_BASE + offset)
	NRF5_UICR_APPROTECT	= NRF5_UICR_REG(0x208),
};

#define NRF52810X_UICR_APPROTECT_HW_DISABLED	(0xFFFFFF00 | 0x5A)

static const __aligned(4) uint8_t algo_word_copy[] = {
	#include "algorithm/cm0-word_copy.inc"
};

static const __aligned(4) uint8_t algo_blank_check[] = {
	#include "algorithm/cm0-blank_check.inc"
};

static const __aligned(4) uint8_t algo_crc32[] = {
	#include "algorithm/cm0-crc32.inc"
};


static void waitForNvmcReady(void);
static void nvmcConfig(enum nrf5_nvmc_config_bits enable);


bool nrf5_blank_check(uint32_t addr, uint32_t nbytes) {
	struct {
		union {
			uint32_t nwords;
			bool result;
		};
		uint32_t addr;
	} blocks[] = {
		// { .nwords = (nbytes + 3 + (addr % 4)) / 4, .addr = addr & 0x3 },
		{ .nwords = nbytes / 4, .addr = addr },
		{ .nwords = 0 }	// terminates algorithm
	};

	target_write_mem(ALGO_CODE_BASE, (const uint32_t *)algo_blank_check, sizeof(algo_blank_check) / sizeof(uint32_t));

	target_write_reg(ARMV7M_R0, ALGO_FIFO_BASE);		// pointer to struct { uint32_t size_in_result_out, uint32_t addr }
	target_write_reg(ARMV7M_R1, ~0);					// value to check
	target_write_reg(ARMV7M_PC, ALGO_CODE_BASE);		// PC to algorithm entry

	target_write_mem(ALGO_FIFO_BASE, (const uint32_t *)blocks, 3);

	target_run();

	target_wait_halted();

	return target_read_word(ALGO_FIFO_BASE);
}

uint32_t nrf5_crc32(uint32_t addr, uint32_t nbytes) {
	// target_write_mem(ALGO_FIFO_BASE, (const uint32_t *)"123456789", 3);
	// addr = ALGO_FIFO_BASE;
	// nbytes = 9;

	target_write_mem(ALGO_CODE_BASE, (const uint32_t *)algo_crc32, sizeof(algo_crc32) / sizeof(uint32_t));

	target_write_reg(ARMV7M_R0, addr);				// start address
	target_write_reg(ARMV7M_R1, nbytes);				// byte count
	target_write_reg(ARMV7M_PC, ALGO_CODE_BASE);		// PC to algorithm entry

	target_run();

	target_wait_halted();

	return target_read_reg(ARMV7M_R0);
}

static void waitForNvmcReady(void) {
	while (!target_read_word(NRF5_NVMC_READY));
}

static void nvmcConfig(enum nrf5_nvmc_config_bits enable) {
	target_write_word(NRF5_NVMC_CONFIG, enable);
	waitForNvmcReady();
}

void nrf5_mass_erase(void) {
	nvmcConfig(NRF5_NVMC_CONFIG_EEN);

	target_write_word(NRF5_NVMC_ERASEALL, 1);
	waitForNvmcReady();

	nvmcConfig(NRF5_NVMC_CONFIG_REN);
}

void nrf5_flash_init(uint32_t addr, uint32_t nbytes) {
	//PRINTF("nrf5_flash_init\n");
	target_write_mem(ALGO_CODE_BASE, (const uint32_t *)algo_word_copy, sizeof(algo_word_copy) / sizeof(uint32_t));

	target_write_reg(ARMV7M_R0, nbytes);							// nbytes to write
	target_write_reg(ARMV7M_R1, WORKAREA_BASE);					// workarea addr
	target_write_reg(ARMV7M_R2, WORKAREA_BASE + WORKAREA_SIZE);	// workarea end
	target_write_reg(ARMV7M_R3, addr);							// dest addr
	target_write_reg(ARMV7M_PC, ALGO_CODE_BASE);					// PC to algorithm entry

	target_write_word(ALGO_FIFO_WP_ADDR, ALGO_FIFO_BASE);			// write pointer
	target_write_word(ALGO_FIFO_RP_ADDR, ALGO_FIFO_BASE);			// read pointer

	nvmcConfig(NRF5_NVMC_CONFIG_WEN);

	target_run();
}

uint32_t nrf5_flash_write(const uint32_t *src, uint32_t nbytes) {
	struct {
		uint32_t wp;
		uint32_t rp;
	} ring_buf;
	uint32_t nfree;

	//PRINTF("nrf5_flash_write\n");

	target_read_mem(WORKAREA_BASE, (uint32_t *)&ring_buf, sizeof(ring_buf) / sizeof(uint32_t));

	nfree = ring_buf.rp - ring_buf.wp;
	if (ring_buf.rp <= ring_buf.wp) {
		nfree += ALGO_FIFO_SIZE;
	}
	if (nfree) {
		nfree -= 4;		// leave a slack of one word, don't allow wp to match rp
	}
	if (nbytes > nfree) {
		nbytes = nfree;
	}

	if (nbytes) {
		target_write_mem(ring_buf.wp, src, nbytes / 4);

		ring_buf.wp += nbytes;
		if (ring_buf.wp >= (ALGO_FIFO_BASE + ALGO_FIFO_SIZE)) {
			ring_buf.wp -= ALGO_FIFO_SIZE;
		}
		target_write_word(WORKAREA_BASE, ring_buf.wp);
	}

	return nbytes;
}

void nrf5_flash_wait(void) {
	target_wait_halted();

	nvmcConfig(NRF5_NVMC_CONFIG_REN);
}

void nrf5_disable_uicr_approtect(void)
{
	uint32_t readback;

	/* Read current "Hardware lock" state in the UICR registers */
	nvmcConfig(NRF5_NVMC_CONFIG_REN);
	readback = target_read_word(NRF5_UICR_APPROTECT);
	PRINTF("UICR.APPROTECT: %x\n", readback);

	/* Disable it if needed */
	if (readback != NRF52810X_UICR_APPROTECT_HW_DISABLED)
	{
		nvmcConfig(NRF5_NVMC_CONFIG_WEN);
		target_write_word(NRF5_UICR_APPROTECT, NRF52810X_UICR_APPROTECT_HW_DISABLED);
		PRINTF("Disabled UICR.APPROTECT\n");
		// Reset NVMC config to read only
		nvmcConfig(NRF5_NVMC_CONFIG_REN);
	}
	else
	{
		PRINTF("UICR.APPROTECT is already disabled\n");
	}
}

void nrf5_erase_uicr(void)
{
	nvmcConfig(NRF5_NVMC_CONFIG_EEN);
	target_write_word(NRF5_NVMC_ERASEUICR, 0x1);
	nvmcConfig(NRF5_NVMC_CONFIG_REN);
	PRINTF("Erased UICR\n");
}

void nrf5_erase_all(void)
{
	nvmcConfig(NRF5_NVMC_CONFIG_EEN);
	target_write_word(NRF5_NVMC_ERASEALL, 0x1);
	nvmcConfig(NRF5_NVMC_CONFIG_REN);
	PRINTF("Erased ALL\n");
}

uint32_t nrf5_read_uicr_approtect(void)
{
	uint32_t readback;
	nvmcConfig(NRF5_NVMC_CONFIG_REN);
	readback = target_read_word(NRF5_UICR_APPROTECT);
	PRINTF("UICR.APPROTECT readback: %x\n", readback);
	return readback;
}

void nrf5_write_uicr_approtect(uint32_t new_val)
{
	target_halt();
	nvmcConfig(NRF5_NVMC_CONFIG_WEN);
	target_write_word(NRF5_UICR_APPROTECT, new_val);
	PRINTF("Wrote: %x to UICR approtect\n", new_val);
	nvmcConfig(NRF5_NVMC_CONFIG_REN);
}
