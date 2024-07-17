#pragma once

#include <stdbool.h>
#include <stdint.h>

#define DCRSR_WnR	(1 << 16)

/* DCB_DHCSR bit and field definitions */
#define DBGKEY		(0xA05F << 16)
#define C_DEBUGEN	(1 << 0)
#define C_HALT		(1 << 1)
#define C_STEP		(1 << 2)
#define C_MASKINTS	(1 << 3)
#define S_REGRDY	(1 << 16)
#define S_HALT		(1 << 17)
#define S_SLEEP		(1 << 18)
#define S_LOCKUP	(1 << 19)
#define S_RETIRE_ST	(1 << 24)
#define S_RESET_ST	(1 << 25)

/* DCB_DEMCR bit and field definitions */
#define TRCENA			(1 << 24)
#define VC_HARDERR		(1 << 10)
#define VC_INTERR		(1 << 9)
#define VC_BUSERR		(1 << 8)
#define VC_STATERR		(1 << 7)
#define VC_CHKERR		(1 << 6)
#define VC_NOCPERR		(1 << 5)
#define VC_MMERR		(1 << 4)
#define VC_CORERESET	(1 << 0)

/* NVIC_AIRCR bits */
#define AIRCR_VECTKEY		(0x5FA << 16)
#define AIRCR_SYSRESETREQ	(1 << 2)
#define AIRCR_VECTCLRACTIVE	(1 << 1)
#define AIRCR_VECTRESET		(1 << 0)
/* NVIC_SHCSR bits */
#define SHCSR_BUSFAULTENA	(1 << 17)
/* NVIC_DFSR bits */
#define DFSR_HALTED			1
#define DFSR_BKPT			2
#define DFSR_DWTTRAP		4
#define DFSR_VCATCH			8

enum {
	/* for convenience, the first set of indices match
	 * the Cortex-M3/-M4 DCRSR selectors
	 */
	ARMV7M_R0,
	ARMV7M_R1,
	ARMV7M_R2,
	ARMV7M_R3,

	ARMV7M_R4,
	ARMV7M_R5,
	ARMV7M_R6,
	ARMV7M_R7,

	ARMV7M_R8,
	ARMV7M_R9,
	ARMV7M_R10,
	ARMV7M_R11,

	ARMV7M_R12,
	ARMV7M_R13,
	ARMV7M_R14,
	ARMV7M_PC = 15,

	ARMV7M_xPSR = 16,
	ARMV7M_MSP,
	ARMV7M_PSP
};

void target_connect(void);
void target_reset(bool halt);
void target_run(void);
void target_halt(void);
void target_wait_halted(void);

uint32_t target_read_reg(int reg);
void target_write_reg(int reg, uint32_t value);

uint32_t target_read_word(uint32_t addr);
void target_write_word(uint32_t addr, uint32_t data);

void target_read_mem(uint32_t addr, uint32_t *buf, uint32_t count);
bool target_write_mem(uint32_t addr, const uint32_t *buf, uint32_t count);
