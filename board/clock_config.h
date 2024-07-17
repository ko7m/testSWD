#ifndef _CLOCK_CONFIG_H_
#define _CLOCK_CONFIG_H_

#include "fsl_common.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define BOARD_XTAL0_CLK_HZ                         24000000U  /*!< Board xtal0 frequency in Hz */

#define BOARD_XTAL32K_CLK_HZ                          32768U  /*!< Board xtal32k frequency in Hz */
/*******************************************************************************
 ************************ BOARD_InitBootClocks function ************************
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus*/

/*!
 * @brief This function executes default configuration of clocks.
 *
 */
void BOARD_InitBootClocks(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus*/

/*******************************************************************************
 ********************** Configuration BOARD_BootClockRUN ***********************
 ******************************************************************************/
/*******************************************************************************
 * Definitions for BOARD_BootClockRUN configuration
 ******************************************************************************/
#define BOARD_BOOTCLOCKRUN_CORE_CLOCK             500000000U  /*!< Core clock frequency: 500000000Hz */

/* Clock outputs (values are in Hz): */
#define BOARD_BOOTCLOCKRUN_AHB_CLK_ROOT               500000000UL    /* Clock consumers of AHB_CLK_ROOT output : AIPSTZ1, AIPSTZ2, AIPSTZ3, AIPSTZ4, ARM, FLEXSPI */
#define BOARD_BOOTCLOCKRUN_CAN_CLK_ROOT               40000000UL     /* Clock consumers of CAN_CLK_ROOT output : CAN1, CAN2 */
#define BOARD_BOOTCLOCKRUN_CKIL_SYNC_CLK_ROOT         32768UL        /* Clock consumers of CKIL_SYNC_CLK_ROOT output : CSU, EWM, GPT1, GPT2, KPP, PIT, RTWDOG, SNVS, SPDIF, TEMPMON, USB, WDOG1, WDOG2 */
#define BOARD_BOOTCLOCKRUN_CLKO1_CLK                  0UL            /* Clock consumers of CLKO1_CLK output : N/A */
#define BOARD_BOOTCLOCKRUN_CLKO2_CLK                  0UL            /* Clock consumers of CLKO2_CLK output : N/A */
#define BOARD_BOOTCLOCKRUN_CLK_1M                     1000000UL      /* Clock consumers of CLK_1M output : EWM, RTWDOG */
#define BOARD_BOOTCLOCKRUN_CLK_24M                    24000000UL     /* Clock consumers of CLK_24M output : GPT1, GPT2 */
#define BOARD_BOOTCLOCKRUN_ENET_125M_CLK              0UL            /* Clock consumers of ENET_125M_CLK output : N/A */
#define BOARD_BOOTCLOCKRUN_ENET_25M_REF_CLK           0UL            /* Clock consumers of ENET_25M_REF_CLK output : ENET */
#define BOARD_BOOTCLOCKRUN_ENET_500M_REF_CLK          500000000UL    /* Clock consumers of ENET_500M_REF_CLK output : N/A */
#define BOARD_BOOTCLOCKRUN_ENET_REF_CLK               0UL            /* Clock consumers of ENET_REF_CLK output : ENET */
#define BOARD_BOOTCLOCKRUN_ENET_TX_CLK                0UL            /* Clock consumers of ENET_TX_CLK output : ENET */
#define BOARD_BOOTCLOCKRUN_FLEXIO1_CLK_ROOT           30000000UL     /* Clock consumers of FLEXIO1_CLK_ROOT output : FLEXIO1 */
#define BOARD_BOOTCLOCKRUN_FLEXSPI_CLK_ROOT           132000000UL    /* Clock consumers of FLEXSPI_CLK_ROOT output : FLEXSPI */
#define BOARD_BOOTCLOCKRUN_GPT1_IPG_CLK_HIGHFREQ      62500000UL     /* Clock consumers of GPT1_ipg_clk_highfreq output : GPT1 */
#define BOARD_BOOTCLOCKRUN_GPT2_IPG_CLK_HIGHFREQ      62500000UL     /* Clock consumers of GPT2_ipg_clk_highfreq output : GPT2 */
#define BOARD_BOOTCLOCKRUN_IPG_CLK_ROOT               125000000UL    /* Clock consumers of IPG_CLK_ROOT output : ADC1, ADC2, ADC_ETC, AOI, ARM, BEE, CAN1, CAN2, CCM, CMP1, CMP2, CMP3, CMP4, CSU, DCDC, DCP, DMA0, DMAMUX, ENC1, ENC2, ENET, EWM, FLEXIO1, FLEXRAM, FLEXSPI, GPC, GPIO1, GPIO2, GPIO3, GPIO5, IOMUXC, KPP, LPI2C1, LPI2C2, LPI2C3, LPI2C4, LPSPI1, LPSPI2, LPSPI3, LPSPI4, LPUART1, LPUART2, LPUART3, LPUART4, LPUART5, LPUART6, LPUART7, LPUART8, NVIC, OCOTP, PWM1, PWM2, RTWDOG, SAI1, SAI2, SAI3, SNVS, SPDIF, SRC, TEMPMON, TMR1, TMR2, TRNG, USB, USDHC1, USDHC2, WDOG1, WDOG2, XBARA, XBARB */
#define BOARD_BOOTCLOCKRUN_LPI2C_CLK_ROOT             60000000UL     /* Clock consumers of LPI2C_CLK_ROOT output : LPI2C1, LPI2C2, LPI2C3, LPI2C4 */
#define BOARD_BOOTCLOCKRUN_LPSPI_CLK_ROOT             105600000UL    /* Clock consumers of LPSPI_CLK_ROOT output : LPSPI1, LPSPI2, LPSPI3, LPSPI4 */
#define BOARD_BOOTCLOCKRUN_MQS_MCLK                   63529411UL     /* Clock consumers of MQS_MCLK output : N/A */
#define BOARD_BOOTCLOCKRUN_PERCLK_CLK_ROOT            62500000UL     /* Clock consumers of PERCLK_CLK_ROOT output : GPT1, GPT2, PIT */
#define BOARD_BOOTCLOCKRUN_SAI1_CLK_ROOT              63529411UL     /* Clock consumers of SAI1_CLK_ROOT output : N/A */
#define BOARD_BOOTCLOCKRUN_SAI1_MCLK1                 63529411UL     /* Clock consumers of SAI1_MCLK1 output : SAI1 */
#define BOARD_BOOTCLOCKRUN_SAI1_MCLK2                 63529411UL     /* Clock consumers of SAI1_MCLK2 output : SAI1 */
#define BOARD_BOOTCLOCKRUN_SAI1_MCLK3                 30000000UL     /* Clock consumers of SAI1_MCLK3 output : SAI1 */
#define BOARD_BOOTCLOCKRUN_SAI2_CLK_ROOT              63529411UL     /* Clock consumers of SAI2_CLK_ROOT output : N/A */
#define BOARD_BOOTCLOCKRUN_SAI2_MCLK1                 63529411UL     /* Clock consumers of SAI2_MCLK1 output : SAI2 */
#define BOARD_BOOTCLOCKRUN_SAI2_MCLK2                 0UL            /* Clock consumers of SAI2_MCLK2 output : SAI2 */
#define BOARD_BOOTCLOCKRUN_SAI2_MCLK3                 30000000UL     /* Clock consumers of SAI2_MCLK3 output : SAI2 */
#define BOARD_BOOTCLOCKRUN_SAI3_CLK_ROOT              63529411UL     /* Clock consumers of SAI3_CLK_ROOT output : N/A */
#define BOARD_BOOTCLOCKRUN_SAI3_MCLK1                 63529411UL     /* Clock consumers of SAI3_MCLK1 output : SAI3 */
#define BOARD_BOOTCLOCKRUN_SAI3_MCLK2                 0UL            /* Clock consumers of SAI3_MCLK2 output : SAI3 */
#define BOARD_BOOTCLOCKRUN_SAI3_MCLK3                 30000000UL     /* Clock consumers of SAI3_MCLK3 output : SAI3 */
#define BOARD_BOOTCLOCKRUN_SEMC_CLK_ROOT              62500000UL     /* Clock consumers of SEMC_CLK_ROOT output : SEMC */
#define BOARD_BOOTCLOCKRUN_SPDIF0_CLK_ROOT            30000000UL     /* Clock consumers of SPDIF0_CLK_ROOT output : SPDIF */
#define BOARD_BOOTCLOCKRUN_SPDIF0_EXTCLK_OUT          0UL            /* Clock consumers of SPDIF0_EXTCLK_OUT output : SPDIF */
#define BOARD_BOOTCLOCKRUN_TRACE_CLK_ROOT             132000000UL    /* Clock consumers of TRACE_CLK_ROOT output : ARM */
#define BOARD_BOOTCLOCKRUN_UART_CLK_ROOT              80000000UL     /* Clock consumers of UART_CLK_ROOT output : LPUART1, LPUART2, LPUART3, LPUART4, LPUART5, LPUART6, LPUART7, LPUART8 */
#define BOARD_BOOTCLOCKRUN_USBPHY1_CLK                0UL            /* Clock consumers of USBPHY1_CLK output : TEMPMON, USB */
#define BOARD_BOOTCLOCKRUN_USDHC1_CLK_ROOT            176000000UL    /* Clock consumers of USDHC1_CLK_ROOT output : USDHC1 */
#define BOARD_BOOTCLOCKRUN_USDHC2_CLK_ROOT            176000000UL    /* Clock consumers of USDHC2_CLK_ROOT output : USDHC2 */

/*! @brief Usb1 PLL set for BOARD_BootClockRUN configuration.
 */
extern const clock_usb_pll_config_t usb1PllConfig_BOARD_BootClockRUN;
/*! @brief Sys PLL for BOARD_BootClockRUN configuration.
 */
extern const clock_sys_pll_config_t sysPllConfig_BOARD_BootClockRUN;
/*! @brief Enet PLL set for BOARD_BootClockRUN configuration.
 */
extern const clock_enet_pll_config_t enetPllConfig_BOARD_BootClockRUN;

/*******************************************************************************
 * API for BOARD_BootClockRUN configuration
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus*/

/*!
 * @brief This function executes configuration of clocks.
 *
 */
void BOARD_BootClockRUN(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus*/

/*******************************************************************************
 ******************* Configuration BOARD_BootClockRUN_400M *********************
 ******************************************************************************/
/*******************************************************************************
 * Definitions for BOARD_BootClockRUN_400M configuration
 ******************************************************************************/
#define BOARD_BOOTCLOCKRUN_400M_CORE_CLOCK        396000000U  /*!< Core clock frequency: 396000000Hz */

/* Clock outputs (values are in Hz): */
#define BOARD_BOOTCLOCKRUN_400M_AHB_CLK_ROOT          396000000UL    /* Clock consumers of AHB_CLK_ROOT output : AIPSTZ1, AIPSTZ2, AIPSTZ3, AIPSTZ4, ARM, FLEXSPI */
#define BOARD_BOOTCLOCKRUN_400M_CAN_CLK_ROOT          40000000UL     /* Clock consumers of CAN_CLK_ROOT output : CAN1, CAN2 */
#define BOARD_BOOTCLOCKRUN_400M_CKIL_SYNC_CLK_ROOT    32768UL        /* Clock consumers of CKIL_SYNC_CLK_ROOT output : CSU, EWM, GPT1, GPT2, KPP, PIT, RTWDOG, SNVS, SPDIF, TEMPMON, USB, WDOG1, WDOG2 */
#define BOARD_BOOTCLOCKRUN_400M_CLKO1_CLK             0UL            /* Clock consumers of CLKO1_CLK output : N/A */
#define BOARD_BOOTCLOCKRUN_400M_CLKO2_CLK             0UL            /* Clock consumers of CLKO2_CLK output : N/A */
#define BOARD_BOOTCLOCKRUN_400M_CLK_1M                1000000UL      /* Clock consumers of CLK_1M output : EWM, RTWDOG */
#define BOARD_BOOTCLOCKRUN_400M_CLK_24M               24000000UL     /* Clock consumers of CLK_24M output : GPT1, GPT2 */
#define BOARD_BOOTCLOCKRUN_400M_ENET_125M_CLK         0UL            /* Clock consumers of ENET_125M_CLK output : N/A */
#define BOARD_BOOTCLOCKRUN_400M_ENET_25M_REF_CLK      0UL            /* Clock consumers of ENET_25M_REF_CLK output : ENET */
#define BOARD_BOOTCLOCKRUN_400M_ENET_500M_REF_CLK     500000000UL    /* Clock consumers of ENET_500M_REF_CLK output : N/A */
#define BOARD_BOOTCLOCKRUN_400M_ENET_REF_CLK          0UL            /* Clock consumers of ENET_REF_CLK output : ENET */
#define BOARD_BOOTCLOCKRUN_400M_ENET_TX_CLK           0UL            /* Clock consumers of ENET_TX_CLK output : ENET */
#define BOARD_BOOTCLOCKRUN_400M_FLEXIO1_CLK_ROOT      30000000UL     /* Clock consumers of FLEXIO1_CLK_ROOT output : FLEXIO1 */
#define BOARD_BOOTCLOCKRUN_400M_FLEXSPI_CLK_ROOT      132000000UL    /* Clock consumers of FLEXSPI_CLK_ROOT output : FLEXSPI */
#define BOARD_BOOTCLOCKRUN_400M_GPT1_IPG_CLK_HIGHFREQ 49500000UL     /* Clock consumers of GPT1_ipg_clk_highfreq output : GPT1 */
#define BOARD_BOOTCLOCKRUN_400M_GPT2_IPG_CLK_HIGHFREQ 49500000UL     /* Clock consumers of GPT2_ipg_clk_highfreq output : GPT2 */
#define BOARD_BOOTCLOCKRUN_400M_IPG_CLK_ROOT          99000000UL     /* Clock consumers of IPG_CLK_ROOT output : ADC1, ADC2, ADC_ETC, AOI, ARM, BEE, CAN1, CAN2, CCM, CMP1, CMP2, CMP3, CMP4, CSU, DCDC, DCP, DMA0, DMAMUX, ENC1, ENC2, ENET, EWM, FLEXIO1, FLEXRAM, FLEXSPI, GPC, GPIO1, GPIO2, GPIO3, GPIO5, IOMUXC, KPP, LPI2C1, LPI2C2, LPI2C3, LPI2C4, LPSPI1, LPSPI2, LPSPI3, LPSPI4, LPUART1, LPUART2, LPUART3, LPUART4, LPUART5, LPUART6, LPUART7, LPUART8, NVIC, OCOTP, PWM1, PWM2, RTWDOG, SAI1, SAI2, SAI3, SNVS, SPDIF, SRC, TEMPMON, TMR1, TMR2, TRNG, USB, USDHC1, USDHC2, WDOG1, WDOG2, XBARA, XBARB */
#define BOARD_BOOTCLOCKRUN_400M_LPI2C_CLK_ROOT        60000000UL     /* Clock consumers of LPI2C_CLK_ROOT output : LPI2C1, LPI2C2, LPI2C3, LPI2C4 */
#define BOARD_BOOTCLOCKRUN_400M_LPSPI_CLK_ROOT        105600000UL    /* Clock consumers of LPSPI_CLK_ROOT output : LPSPI1, LPSPI2, LPSPI3, LPSPI4 */
#define BOARD_BOOTCLOCKRUN_400M_MQS_MCLK              63529411UL     /* Clock consumers of MQS_MCLK output : N/A */
#define BOARD_BOOTCLOCKRUN_400M_PERCLK_CLK_ROOT       49500000UL     /* Clock consumers of PERCLK_CLK_ROOT output : GPT1, GPT2, PIT */
#define BOARD_BOOTCLOCKRUN_400M_SAI1_CLK_ROOT         63529411UL     /* Clock consumers of SAI1_CLK_ROOT output : N/A */
#define BOARD_BOOTCLOCKRUN_400M_SAI1_MCLK1            63529411UL     /* Clock consumers of SAI1_MCLK1 output : SAI1 */
#define BOARD_BOOTCLOCKRUN_400M_SAI1_MCLK2            63529411UL     /* Clock consumers of SAI1_MCLK2 output : SAI1 */
#define BOARD_BOOTCLOCKRUN_400M_SAI1_MCLK3            30000000UL     /* Clock consumers of SAI1_MCLK3 output : SAI1 */
#define BOARD_BOOTCLOCKRUN_400M_SAI2_CLK_ROOT         63529411UL     /* Clock consumers of SAI2_CLK_ROOT output : N/A */
#define BOARD_BOOTCLOCKRUN_400M_SAI2_MCLK1            63529411UL     /* Clock consumers of SAI2_MCLK1 output : SAI2 */
#define BOARD_BOOTCLOCKRUN_400M_SAI2_MCLK2            0UL            /* Clock consumers of SAI2_MCLK2 output : SAI2 */
#define BOARD_BOOTCLOCKRUN_400M_SAI2_MCLK3            30000000UL     /* Clock consumers of SAI2_MCLK3 output : SAI2 */
#define BOARD_BOOTCLOCKRUN_400M_SAI3_CLK_ROOT         63529411UL     /* Clock consumers of SAI3_CLK_ROOT output : N/A */
#define BOARD_BOOTCLOCKRUN_400M_SAI3_MCLK1            63529411UL     /* Clock consumers of SAI3_MCLK1 output : SAI3 */
#define BOARD_BOOTCLOCKRUN_400M_SAI3_MCLK2            0UL            /* Clock consumers of SAI3_MCLK2 output : SAI3 */
#define BOARD_BOOTCLOCKRUN_400M_SAI3_MCLK3            30000000UL     /* Clock consumers of SAI3_MCLK3 output : SAI3 */
#define BOARD_BOOTCLOCKRUN_400M_SEMC_CLK_ROOT         49500000UL     /* Clock consumers of SEMC_CLK_ROOT output : SEMC */
#define BOARD_BOOTCLOCKRUN_400M_SPDIF0_CLK_ROOT       30000000UL     /* Clock consumers of SPDIF0_CLK_ROOT output : SPDIF */
#define BOARD_BOOTCLOCKRUN_400M_SPDIF0_EXTCLK_OUT     0UL            /* Clock consumers of SPDIF0_EXTCLK_OUT output : SPDIF */
#define BOARD_BOOTCLOCKRUN_400M_TRACE_CLK_ROOT        132000000UL    /* Clock consumers of TRACE_CLK_ROOT output : ARM */
#define BOARD_BOOTCLOCKRUN_400M_UART_CLK_ROOT         80000000UL     /* Clock consumers of UART_CLK_ROOT output : LPUART1, LPUART2, LPUART3, LPUART4, LPUART5, LPUART6, LPUART7, LPUART8 */
#define BOARD_BOOTCLOCKRUN_400M_USBPHY1_CLK           0UL            /* Clock consumers of USBPHY1_CLK output : TEMPMON, USB */
#define BOARD_BOOTCLOCKRUN_400M_USDHC1_CLK_ROOT       176000000UL    /* Clock consumers of USDHC1_CLK_ROOT output : USDHC1 */
#define BOARD_BOOTCLOCKRUN_400M_USDHC2_CLK_ROOT       176000000UL    /* Clock consumers of USDHC2_CLK_ROOT output : USDHC2 */

/*! @brief Usb1 PLL set for BOARD_BootClockRUN_400M configuration.
 */
extern const clock_usb_pll_config_t usb1PllConfig_BOARD_BootClockRUN_400M;
/*! @brief Sys PLL for BOARD_BootClockRUN_400M configuration.
 */
extern const clock_sys_pll_config_t sysPllConfig_BOARD_BootClockRUN_400M;
/*! @brief Enet PLL set for BOARD_BootClockRUN_400M configuration.
 */
extern const clock_enet_pll_config_t enetPllConfig_BOARD_BootClockRUN_400M;

/*******************************************************************************
 * API for BOARD_BootClockRUN_400M configuration
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus*/

/*!
 * @brief This function executes configuration of clocks.
 *
 */
void BOARD_BootClockRUN_400M(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus*/

#endif /* _CLOCK_CONFIG_H_ */

