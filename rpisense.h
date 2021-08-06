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

#ifndef __LINUX_MFD_RPISENSE_H_
#define __LINUX_MFD_RPISENSE_H_
#include <linux/miscdevice.h>

#define SENSEFB_FBIO_IOC_MAGIC 0xF1

#define SENSEFB_FBIOGET_GAMMA _IO(SENSEFB_FBIO_IOC_MAGIC, 0)
#define SENSEFB_FBIOSET_GAMMA _IO(SENSEFB_FBIO_IOC_MAGIC, 1)
#define SENSEFB_FBIORESET_GAMMA _IO(SENSEFB_FBIO_IOC_MAGIC, 2)

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
		struct miscdevice mdev;
		struct mutex rw_mtx;
		u8 *gamma;
		u8 *vmem;
	} framebuffer;
};

int rpisense_get_joystick_state(struct rpisense *rpisense);
int rpisense_update_framebuffer(struct rpisense *rpisense);

#endif
