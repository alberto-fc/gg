/*
 * Copyright (C) 2011 Freescale Semiconductor, Inc. All Rights Reserved.
 */

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <linux/init.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/smsc911x.h>

#include <mach/common.h>
#include <mach/hardware.h>
#include <mach/imx-uart.h>
#include <mach/iomux-mx53.h>

#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/time.h>

#include "crm_regs.h"
#include "devices-imx53.h"

#define ARD_SD1_WP IMX_GPIO_NR(1, 9)
#define ARD_SD2_WP IMX_GPIO_NR(1, 2)
#define ARD_ETHERNET_INT_B IMX_GPIO_NR(2, 31)

static iomux_v3_cfg_t mx53_ard_pads[] = {
	/* UART */
	MX53_PAD_PATA_DIOW__UART1_TXD_MUX,
	MX53_PAD_PATA_DMACK__UART1_RXD_MUX,
	MX53_PAD_PATA_BUFFER_EN__UART2_RXD_MUX,
	MX53_PAD_PATA_DMARQ__UART2_TXD_MUX,
	MX53_PAD_PATA_DIOR__UART2_RTS,
	MX53_PAD_PATA_INTRQ__UART2_CTS,
	MX53_PAD_PATA_CS_0__UART3_TXD_MUX,
	MX53_PAD_PATA_CS_1__UART3_RXD_MUX,
	MX53_PAD_PATA_DA_1__UART3_CTS,
	MX53_PAD_PATA_DA_2__UART3_RTS,

	/* SDHC1 */
	MX53_PAD_SD1_CMD__ESDHC1_CMD,
	MX53_PAD_SD1_CLK__ESDHC1_CLK,
	MX53_PAD_SD1_DATA0__ESDHC1_DAT0,
	MX53_PAD_SD1_DATA1__ESDHC1_DAT1,
	MX53_PAD_SD1_DATA2__ESDHC1_DAT2,
	MX53_PAD_SD1_DATA3__ESDHC1_DAT3,
	MX53_PAD_PATA_DATA8__ESDHC1_DAT4,
	MX53_PAD_PATA_DATA9__ESDHC1_DAT5,
	MX53_PAD_PATA_DATA10__ESDHC1_DAT6,
	MX53_PAD_PATA_DATA11__ESDHC1_DAT7,
	MX53_PAD_GPIO_1__GPIO1_1,
	MX53_PAD_GPIO_9__GPIO1_9,

	/* SDHC2 */
	MX53_PAD_SD2_CLK__ESDHC2_CLK,
	MX53_PAD_SD2_CMD__ESDHC2_CMD,
	MX53_PAD_SD2_DATA0__ESDHC2_DAT0,
	MX53_PAD_SD2_DATA1__ESDHC2_DAT1,
	MX53_PAD_SD2_DATA2__ESDHC2_DAT2,
	MX53_PAD_SD2_DATA3__ESDHC2_DAT3,
	MX53_PAD_PATA_DATA12__ESDHC2_DAT4,
	MX53_PAD_PATA_DATA13__ESDHC2_DAT5,
	MX53_PAD_PATA_DATA14__ESDHC2_DAT6,
	MX53_PAD_PATA_DATA15__ESDHC2_DAT7,
	MX53_PAD_GPIO_4__GPIO1_4,
	MX53_PAD_GPIO_2__GPIO1_2,

	/* WEIM for CS1 */
	/* ETHERNET_INT_B */
	MX53_PAD_EIM_EB3__GPIO2_31,
	MX53_PAD_EIM_D16__EMI_WEIM_D_16,
	MX53_PAD_EIM_D17__EMI_WEIM_D_17,
	MX53_PAD_EIM_D18__EMI_WEIM_D_18,
	MX53_PAD_EIM_D19__EMI_WEIM_D_19,
	MX53_PAD_EIM_D20__EMI_WEIM_D_20,
	MX53_PAD_EIM_D21__EMI_WEIM_D_21,
	MX53_PAD_EIM_D22__EMI_WEIM_D_22,
	MX53_PAD_EIM_D23__EMI_WEIM_D_23,
	MX53_PAD_EIM_D24__EMI_WEIM_D_24,
	MX53_PAD_EIM_D25__EMI_WEIM_D_25,
	MX53_PAD_EIM_D26__EMI_WEIM_D_26,
	MX53_PAD_EIM_D27__EMI_WEIM_D_27,
	MX53_PAD_EIM_D28__EMI_WEIM_D_28,
	MX53_PAD_EIM_D29__EMI_WEIM_D_29,
	MX53_PAD_EIM_D30__EMI_WEIM_D_30,
	MX53_PAD_EIM_D31__EMI_WEIM_D_31,
	MX53_PAD_EIM_DA0__EMI_NAND_WEIM_DA_0,
	MX53_PAD_EIM_DA1__EMI_NAND_WEIM_DA_1,
	MX53_PAD_EIM_DA2__EMI_NAND_WEIM_DA_2,
	MX53_PAD_EIM_DA3__EMI_NAND_WEIM_DA_3,
	MX53_PAD_EIM_DA4__EMI_NAND_WEIM_DA_4,
	MX53_PAD_EIM_DA5__EMI_NAND_WEIM_DA_5,
	MX53_PAD_EIM_DA6__EMI_NAND_WEIM_DA_6,
	MX53_PAD_EIM_OE__EMI_WEIM_OE,
	MX53_PAD_EIM_RW__EMI_WEIM_RW,
	MX53_PAD_EIM_CS1__EMI_WEIM_CS_1,
};

/* Config CS1 settings for ethernet controller */
static void weim_cs_config(void)
{
	u32 reg;
	void __iomem *weim_base, *iomuxc_base;
	weim_base = ioremap(MX53_WEIM_BASE_ADDR, SZ_4K);
	iomuxc_base = ioremap(MX53_IOMUXC_BASE_ADDR, SZ_4K);

	/* CS1 timings for LAN9220 */
	writel(0x20001, (weim_base + 0x18));
	writel(0x0, (weim_base + 0x1C));
	writel(0x16000202, (weim_base + 0x20));
	writel(0x00000002, (weim_base + 0x24));
	writel(0x16002082, (weim_base + 0x28));
	writel(0x00000000, (weim_base + 0x2C));
	writel(0x00000000, (weim_base + 0x90));

	/* specify 64 MB on CS1 and CS0 on GPR1 */
	reg = readl(iomuxc_base + 0x4);
	reg &= ~0x3F;
	reg |= 0x1B;
	writel(reg, (iomuxc_base + 0x4));
	iounmap(iomuxc_base);
	iounmap(weim_base);
}

static struct resource ard_smsc911x_resources[] = {
	{
	 .start = MX53_CS1_BASE_ADDR,
	 .end = MX53_CS1_BASE_ADDR + SZ_4K - 1,
	 .flags = IORESOURCE_MEM,
	},
	{
	 .start =  gpio_to_irq(ARD_ETHERNET_INT_B),
	 .end =  gpio_to_irq(ARD_ETHERNET_INT_B),
	 .flags = IORESOURCE_IRQ,
	},
};


struct smsc911x_platform_config ard_smsc911x_config = {
	.irq_polarity = SMSC911X_IRQ_POLARITY_ACTIVE_LOW,
	.irq_type = SMSC911X_IRQ_TYPE_PUSH_PULL,
	.flags = SMSC911X_USE_32BIT,
};

static struct platform_device ard_smsc_lan9220_device = {
	.name = "smsc911x",
	.id = 0,
	.num_resources = ARRAY_SIZE(ard_smsc911x_resources),
	.resource = ard_smsc911x_resources,
};


static const struct imxuart_platform_data mx53_ard_uart_data __initconst = {
	.flags = IMXUART_HAVE_RTSCTS,
};

static const struct esdhc_platform_data mx53_ard_sd0_data __initconst = {
	.wp_gpio = ARD_SD1_WP,
};

static const struct esdhc_platform_data mx53_ard_sd1_data __initconst = {
	.wp_gpio = ARD_SD2_WP,
};

static inline void mx53_ard_init_uart(void)
{
	imx53_add_imx_uart(0, NULL);
	imx53_add_imx_uart(1, &mx53_ard_uart_data);
	imx53_add_imx_uart(2, &mx53_ard_uart_data);
}

static void __init mx53_ard_io_init(void)
{
	/* MX53 ARD board */
	pr_info("MX53 ARD board \n");
	gpio_request(ARD_ETHERNET_INT_B, "eth-int-b");
	gpio_direction_input(ARD_ETHERNET_INT_B);
}
static void __init mx53_ard_board_init(void)
{
	mxc_iomux_v3_setup_multiple_pads(mx53_ard_pads,
					ARRAY_SIZE(mx53_ard_pads));
	mx53_ard_init_uart();
	imx53_add_imx2_wdt(0, NULL);
	imx53_add_sdhci_esdhc_imx(0, &mx53_ard_sd0_data);
	imx53_add_sdhci_esdhc_imx(1, &mx53_ard_sd1_data);

	weim_cs_config();
	mx53_ard_io_init();
	mxc_register_device(&ard_smsc_lan9220_device, &ard_smsc911x_config);
}

static void __init mx53_ard_timer_init(void)
{
	mx53_clocks_init(32768, 24000000, 22579200, 0);
}

static struct sys_timer mx53_ard_timer = {
	.init	= mx53_ard_timer_init,
};

MACHINE_START(MX53_ARD, "Freescale MX53 ARD Board")
	.map_io = mx53_map_io,
	.init_early = imx53_init_early,
	.init_irq = mx53_init_irq,
	.timer = &mx53_ard_timer,
	.init_machine = mx53_ard_board_init,
MACHINE_END