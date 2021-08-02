/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Raspberry Pi Sense HAT core driver
 * http://raspberrypi.org
 *
 * Copyright (C) 2015 Raspberry Pi
 *
 * Author: Serge Schneider
 * Adapted for mainline Linux by: Joel Savitz
 */

#ifndef __LINUX_MFD_RPISENSE_CORE_H_
#define __LINUX_MFD_RPISENSE_CORE_H_

#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/gpio/consumer.h>
#include <linux/platform_device.h>

/*
 * Register values.
 */
#define RPISENSE_FB			0x00
#define RPISENSE_WAI			0xF0
#define RPISENSE_VER			0xF1
#define RPISENSE_KEYS			0xF2
#define RPISENSE_EE_WP			0xF3

#define RPISENSE_ID			's'

#define SENSEFB_FBIO_IOC_MAGIC 0xF1

#define SENSEFB_FBIOGET_GAMMA _IO(SENSEFB_FBIO_IOC_MAGIC, 0)
#define SENSEFB_FBIOSET_GAMMA _IO(SENSEFB_FBIO_IOC_MAGIC, 1)
#define SENSEFB_FBIORESET_GAMMA _IO(SENSEFB_FBIO_IOC_MAGIC, 2)

struct rpisense_fb {
	struct platform_device *pdev;
	struct fb_info *info;
};


struct rpisense_js {
	struct platform_device *pdev;
	struct input_dev *keys_dev;
	struct gpio_desc *keys_desc;
	struct work_struct keys_work_s;
	int keys_irq;
};


struct rpisense {
	struct device *dev;
	struct i2c_client *i2c_client;

	/* Client devices */
	struct rpisense_js joystick;
	struct rpisense_fb framebuffer;
};

struct rpisense *rpisense_get_dev(void);
s32 rpisense_reg_read(struct rpisense *rpisense, int reg);
int rpisense_reg_write(struct rpisense *rpisense, int reg, u16 val);
int rpisense_block_write(struct rpisense *rpisense, const char *buf, int count);

#endif
