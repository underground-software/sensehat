/*
 * Raspberry Pi Sense HAT core driver
 * http://raspberrypi.org
 *
 * Copyright (C) 2015 Raspberry Pi
 *
 * Author: Serge Schneider
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 */

#ifndef __LINUX_MFD_RPISENSE_CORE_H_
#define __LINUX_MFD_RPISENSE_CORE_H_

#define SENSEFB_FBIO_IOC_MAGIC 0xF1

#define SENSEFB_FBIOGET_GAMMA _IO(SENSEFB_FBIO_IOC_MAGIC, 0)
#define SENSEFB_FBIOSET_GAMMA _IO(SENSEFB_FBIO_IOC_MAGIC, 1)
#define SENSEFB_FBIORESET_GAMMA _IO(SENSEFB_FBIO_IOC_MAGIC, 2)


/*
 * Register values.
 */
#define RPISENSE_FB			0x00
#define RPISENSE_WAI			0xF0
#define RPISENSE_VER			0xF1
#define RPISENSE_KEYS			0xF2
#define RPISENSE_EE_WP			0xF3

#define RPISENSE_ID			's'

struct rpisense {
	struct device *dev;
	struct i2c_client *i2c_client;

	/* Client devices */
	struct rpisense_js {
		struct platform_device *pdev;
		struct input_dev *keys_dev;
		struct gpio_desc *keys_desc;
		int keys_irq;
	} joystick;
	struct rpisense_fb {
		struct platform_device *pdev;
		struct fb_info *info;
	} framebuffer;
};

s32 rpisense_reg_read(struct rpisense *rpisense, int reg);
int rpisense_reg_write(struct rpisense *rpisense, int reg, u16 val);
int rpisense_block_write(struct rpisense *rpisense, const char *buf, int count);

#endif
