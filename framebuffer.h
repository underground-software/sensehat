/*
 * Raspberry Pi Sense HAT framebuffer driver
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

#ifndef __LINUX_RPISENSE_FB_H_
#define __LINUX_RPISENSE_FB_H_

#define SENSEFB_FBIO_IOC_MAGIC 0xF1

#define SENSEFB_FBIOGET_GAMMA _IO(SENSEFB_FBIO_IOC_MAGIC, 0)
#define SENSEFB_FBIOSET_GAMMA _IO(SENSEFB_FBIO_IOC_MAGIC, 1)
#define SENSEFB_FBIORESET_GAMMA _IO(SENSEFB_FBIO_IOC_MAGIC, 2)

struct rpisense;

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
struct rpisense_fb {
	struct platform_device *pdev;
	struct fb_info *info;
=======
struct rpisense_cd {
	struct platform_device *pdev;
	struct miscdevice *c_dev;
>>>>>>> 6a892bc (rpisense-cd.c with read functionality)
=======
=======
>>>>>>> a8e899b (new char driver)
struct rpisense_cd {
	struct platform_device *pdev;
	struct miscdevice *c_dev;
=======
struct rpisense_fb {
	struct platform_device *pdev;
	struct fb_info *info;
>>>>>>> b54532f (framebuffer testfile)
<<<<<<< HEAD
>>>>>>> ea10db8 (framebuffer testfile)
=======
=======
struct rpisense_cd {
	struct platform_device *pdev;
<<<<<<< HEAD
	struct cdev *c_dev;
>>>>>>> 86a0552 (new char driver)
<<<<<<< HEAD
>>>>>>> a8e899b (new char driver)
=======
=======
	struct miscdevice *c_dev;
>>>>>>> b9a4fe0 (char device to replace framebuffer)
>>>>>>> cc74e0b (char device to replace framebuffer)
};

#endif
