#include <stdbool.h>
#include "dap.h"
#include "target.h"
#include "fsl_debug_console.h"
//#include "wgo2.h"

#include "target.h"
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


#ifdef __cplusplus
  #define   __I     volatile             /*!< Defines 'read only' permissions */
#else
  #define   __I     volatile const       /*!< Defines 'read only' permissions */
#endif
#define     __O     volatile             /*!< Defines 'write only' permissions */
#define     __IO    volatile             /*!< Defines 'read / write' permissions */

/* following defines should be used for structure members */
#define     __IM     volatile const      /*! Defines 'read only' structure member permissions */
#define     __OM     volatile            /*! Defines 'write only' structure member permissions */
#define     __IOM    volatile            /*! Defines 'read / write' structure member permissions */

typedef struct
{
  __IOM uint32_t DHCSR;                  /*!< Offset: 0x000 (R/W)  Debug Halting Control and Status Register */
  __OM  uint32_t DCRSR;                  /*!< Offset: 0x004 ( /W)  Debug Core Register Selector Register */
  __IOM uint32_t DCRDR;                  /*!< Offset: 0x008 (R/W)  Debug Core Register Data Register */
  __IOM uint32_t DEMCR;                  /*!< Offset: 0x00C (R/W)  Debug Exception and Monitor Control Register */
} Target_CoreDebug_Type;


typedef struct
{
  __IM  uint32_t CPUID;                  /*!< Offset: 0x000 (R/ )  CPUID Base Register */
  __IOM uint32_t ICSR;                   /*!< Offset: 0x004 (R/W)  Interrupt Control and State Register */
  __IOM uint32_t VTOR;                   /*!< Offset: 0x008 (R/W)  Vector Table Offset Register */
  __IOM uint32_t AIRCR;                  /*!< Offset: 0x00C (R/W)  Application Interrupt and Reset Control Register */
  __IOM uint32_t SCR;                    /*!< Offset: 0x010 (R/W)  System Control Register */
  __IOM uint32_t CCR;                    /*!< Offset: 0x014 (R/W)  Configuration Control Register */
  __IOM uint8_t  SHP[12U];               /*!< Offset: 0x018 (R/W)  System Handlers Priority Registers (4-7, 8-11, 12-15) */
  __IOM uint32_t SHCSR;                  /*!< Offset: 0x024 (R/W)  System Handler Control and State Register */
  __IOM uint32_t CFSR;                   /*!< Offset: 0x028 (R/W)  Configurable Fault Status Register */
  __IOM uint32_t HFSR;                   /*!< Offset: 0x02C (R/W)  HardFault Status Register */
  __IOM uint32_t DFSR;                   /*!< Offset: 0x030 (R/W)  Debug Fault Status Register */
  __IOM uint32_t MMFAR;                  /*!< Offset: 0x034 (R/W)  MemManage Fault Address Register */
  __IOM uint32_t BFAR;                   /*!< Offset: 0x038 (R/W)  BusFault Address Register */
  __IOM uint32_t AFSR;                   /*!< Offset: 0x03C (R/W)  Auxiliary Fault Status Register */
  __IM  uint32_t PFR[2U];                /*!< Offset: 0x040 (R/ )  Processor Feature Register */
  __IM  uint32_t DFR;                    /*!< Offset: 0x048 (R/ )  Debug Feature Register */
  __IM  uint32_t ADR;                    /*!< Offset: 0x04C (R/ )  Auxiliary Feature Register */
  __IM  uint32_t MMFR[4U];               /*!< Offset: 0x050 (R/ )  Memory Model Feature Register */
  __IM  uint32_t ISAR[5U];               /*!< Offset: 0x060 (R/ )  Instruction Set Attributes Register */
        uint32_t RESERVED0[5U];
  __IOM uint32_t CPACR;                  /*!< Offset: 0x088 (R/W)  Coprocessor Access Control Register */
} Target_SCB_Type;

#define CoreDebug_BASE      (0xE000EDF0UL)                               /*!< Core Debug Base Address */
#define CoreDebug_Target    ((Target_CoreDebug_Type *) CoreDebug_BASE)   /*!< Core Debug configuration struct */



#define SCB_BASE            (SCS_BASE + 0x0D00UL)                        /*!< System Control Block Base Address */

#define SCS_BASE            (0xE000E000UL)                               /*!< System Control Space Base Address */
#define SCB_Target          ((Target_SCB_Type       *) SCB_BASE)         /*!< SCB configuration struct */

static uint32_t target_poll(void);
static void target_wait_reg_ready(void);


static uint32_t target_poll(void) {
	return target_read_word((uint32_t)&(CoreDebug_Target->DHCSR));
}

/**********************************************************
 * Performs the initialization sequence on the SW-DP.
 * After this completes the debug interface can be used.
 * Raises an exception on any error during connection.
 **********************************************************/
void target_connect(void) {
	dap_init_dp();
	
	/* Read chip protection. Erase unlock through ctrl-ap if needed. */
	/*For noridc e or later version, this also need to be addressed to be nordic fw 21/02/2023*/	
	if (dap_nrf5810x_ctrlap_unlock() == false)
	{
		PRINTF("Failed to unlock nrf52810x chip through ctrl-ap\n");
	}	

	/* Set up parameters for AHB-AP. This must be done before accessing
	 * internal memory. */
	dap_init_ap();

	/* Reset and halt target */
	target_halt();
	target_reset(true);

	/*
	 * Ensure xPSR.T is set to avoid trying to run things in arm
	 * (non-thumb) mode, which armv7m does not support.
	 *
	 * We do this by setting the entirety of xPSR, which should
	 * remove all the unknowns about xPSR state.
	 *
	 * Because xPSR.T is populated on reset from the vector table,
	 * it might be 0 if the vector table has "bad" data in it.
	 */
	target_write_reg(ARMV7M_xPSR, 0x01000000);

	/* Disable UICR.APPROTECT if needed so that the chip 
		doesn't lock itself on next reset. */
	//mask nordic ap protection on 21/02/2023
	//nrf5_disable_uicr_approtect();
}


/**********************************************************
 * Resets the target CPU by using the AIRCR register.
 * Does not reset the debug interface
 **********************************************************/
void target_reset(bool halt) {
	if (halt) {
		/* Halt target first. This is necessary before setting
		* the VECTRESET bit */
		target_halt();
	}

	/* Set/Clear halt-on-reset bit */
	target_write_word((uint32_t)&(CoreDebug_Target->DEMCR), halt ? VC_CORERESET : 0);

	/* Do a dummy read of sticky bit to make sure it is set/cleared */
	target_poll();

	/* Reset CPU */
	target_write_word((uint32_t)&(SCB_Target->AIRCR), AIRCR_VECTKEY | AIRCR_SYSRESETREQ | AIRCR_VECTCLRACTIVE);

	/* Wait for reset to complete */

	/* First wait until sticky bit is set. This means we are
	 * or have been in reset */
	while (!(S_RESET_ST & target_poll()));

	/* Wait for sticky bit to be cleared. When bit is cleared are we out of reset */
	while (S_RESET_ST & target_poll());
}


/**********************************************************
 * Lets the target CPU run freely (stops halting)
 **********************************************************/
void target_run(void) {
	target_write_word((uint32_t)&(CoreDebug_Target->DHCSR), DBGKEY | C_DEBUGEN);
}


/**********************************************************
 * Halts the target CPU
 **********************************************************/
void target_halt(void) {
	target_write_word((uint32_t)&(CoreDebug_Target->DHCSR), DBGKEY | C_DEBUGEN | C_HALT);
	target_wait_halted();
}


/**********************************************************
 * Wait for target CPU halted
 **********************************************************/
void target_wait_halted(void) {
	while (!(S_HALT & target_poll()));
}


/**********************************************************
 * Waits for the REGRDY bit in DCRSR. This bit indicates
 * that the DCRDR/DCRSR registers are ready to accept
 * new data.
 **********************************************************/
static void target_wait_reg_ready(void) {
	while (!(S_REGRDY & target_poll()));
}


/**********************************************************
 * Reads a value from a CPU register in the target.
 **********************************************************/
uint32_t target_read_reg(int reg) {
	/* Wait until debug register is ready to accept new data */
	target_wait_reg_ready();

	/* Write register number ot Selector Register.
	 * This will update the DCRD register */
	target_write_word((uint32_t)&(CoreDebug_Target->DCRSR), reg);

	/* Read Data Register */
	return target_read_word((uint32_t)&(CoreDebug_Target->DCRDR));
}


/**********************************************************
 * Writes a value to a CPU register in the target.
 **********************************************************/
void target_write_reg(int reg, uint32_t value) {
	/* Wait until debug register is ready to accept new data */
	target_wait_reg_ready();

	/* Write value to Data Register */
	target_write_word((uint32_t)&(CoreDebug_Target->DCRDR), value);

	/* Write register number ot Selector Register.
	 * This will update the CPU register */
	target_write_word((uint32_t)&(CoreDebug_Target->DCRSR), DCRSR_WnR | reg);
}


/**********************************************************
 * Reads one word from internal memory
 **********************************************************/
uint32_t target_read_word(uint32_t addr) {
	uint32_t ret;

	target_read_mem(addr, &ret, 1);

	return ret;
}


/**********************************************************
 * Writes one word to internal memory
 **********************************************************/
void target_write_word(uint32_t addr, uint32_t data) {
	target_write_mem(addr, &data, 1);
}


void target_read_mem(uint32_t addr, uint32_t *buf, uint32_t count)
{
	bool auto_inc = false;

	if (count > 1) {
		auto_inc = true;
		dap_write_ap(AP_CSW, AP_CSW_DEFAULT | AP_CSW_AUTO_INCREMENT);
	}

	dap_write_ap(AP_TAR, addr);

	// initial dummy read
	dap_read_ap(AP_DRW, buf);
	while(--count) {
		dap_read_ap(AP_DRW, buf++);
	}
	dap_read_dp(DP_RDBUFF, buf);

	if (auto_inc) {
		// disable TAR auto-increment
		dap_init_ap();
	}
}


// TAR auto-increments in 4k address range - bottom 12-bits of TAR
bool target_write_mem(uint32_t addr, const uint32_t *buf, uint32_t count)
{
	bool auto_inc = false;
	bool isOk = false;
	if (count > 1) {
		auto_inc = true;
		isOk = dap_write_ap(AP_CSW, AP_CSW_DEFAULT | AP_CSW_AUTO_INCREMENT);
	}

	isOk = dap_write_ap(AP_TAR, addr);

	while(count--) {
		isOk = dap_write_ap(AP_DRW, *buf++);
		if (isOk == false)
			return false;
	}

	if (auto_inc) {
		// disable TAR auto-increment
		dap_init_ap();
	}

	return true;
}
