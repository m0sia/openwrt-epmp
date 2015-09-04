/*
 *  Cambium ePMP support
 *
 *  Copyright (C) 2015 Dmitry Moiseev <dmitry.moiseev@cambiumnetworks.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/ath9k_platform.h>

#include <asm/mach-ath79/ath79.h>
#include <asm/mach-ath79/ar71xx_regs.h>

#include "common.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-usb.h"
#include "dev-wmac.h"
#include "machtypes.h"

#define EPMP_GPIO_LED_WLAN	13
#define EPMP_GPIO_LED_QSS	15
#define EPMP_GPIO_LED_LAN	18
#define EPMP_GPIO_LED_SYSTEM	14

#define EPMP_GPIO_USB_POWER	4

#define EPMP_WMAC_CALDATA_OFFSET	0x1000

static struct flash_platform_data epmp_flash_data = {
	.type = "mx25l6405d",
};

static struct gpio_led epmp_leds_gpio[] __initdata = {
	{
		.name		= "epmp:green:qss",
		.gpio		= EPMP_GPIO_LED_QSS,
		.active_low	= 1,
	}, {
		.name		= "epmp:green:system",
		.gpio		= EPMP_GPIO_LED_SYSTEM,
		.active_low	= 1,
	}, {
		.name		= "epmp:green:lan",
		.gpio		= EPMP_GPIO_LED_LAN,
		.active_low	= 1,
	}, {
		.name		= "epmp:green:wlan",
		.gpio		= EPMP_GPIO_LED_WLAN,
		.active_low	= 1,
	},
};

static void __init epmp_setup(void)
{
	u8 *art = (u8 *)KSEG1ADDR(0x1fff0000);
	u8 mac[6];
	u8 *cal_data;

	ath79_register_m25p80(&epmp_flash_data);
	
	ath79_init_mac(mac, art, 2);

	ath79_setup_ar934x_eth_cfg(AR934X_ETH_CFG_SW_ONLY_MODE|AR934X_ETH_CFG_SW_PHY_SWAP);
	ath79_register_mdio(1, 0x0);
	ath79_init_mac(ath79_eth0_data.mac_addr, mac, 0);
	ath79_init_mac(ath79_eth1_data.mac_addr, mac, 1);
	
	ath79_switch_data.phy4_mii_en = 1;
	ath79_switch_data.phy_poll_mask = BIT(0);
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ath79_eth0_data.phy_mask = BIT(0);
	ath79_eth0_data.mii_bus_dev = &ath79_mdio1_device.dev;
	ath79_register_eth(0);

	ath79_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_GMII;
	ath79_register_eth(1);

	cal_data=kmalloc(0x1000,GFP_KERNEL);
	memcpy(cal_data, art + EPMP_WMAC_CALDATA_OFFSET, 0x1000);
	memset(cal_data+0x1C,0x64,1);
	memset(cal_data+0x1D,0x00,1);
	ath79_register_wmac(cal_data, mac);
	kfree(cal_data);

	ath79_register_leds_gpio(-1, ARRAY_SIZE(epmp_leds_gpio) - 1,
				 epmp_leds_gpio);
}

MIPS_MACHINE(ATH79_MACH_EPMP, "EPMP", "Cambium ePMP",
		epmp_setup);
