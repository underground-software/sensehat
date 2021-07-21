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

<<<<<<< HEAD
#include "joystick.h"
#include "framebuffer.h"
=======
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
#include <linux/mfd/rpisense/joystick.h>
#include <linux/mfd/rpisense/framebuffer.h>
=======
=======
>>>>>>> ea10db8 (framebuffer testfile)
=======
>>>>>>> a8e899b (new char driver)
/*
#include <linux/mfd/rpisense/joystick.h>
#include <linux/mfd/rpisense/framebuffer.h>
*/

#include "joystick.h"
#include "framebuffer.h"
<<<<<<< HEAD
>>>>>>> 6a892bc (rpisense-cd.c with read functionality)
=======
=======
#include <linux/mfd/rpisense/joystick.h>
#include <linux/mfd/rpisense/framebuffer.h>
>>>>>>> b54532f (framebuffer testfile)
<<<<<<< HEAD
>>>>>>> ea10db8 (framebuffer testfile)
<<<<<<< HEAD
>>>>>>> ed899c1 (framebuffer testfile)
=======
=======
=======
/*
#include <linux/mfd/rpisense/joystick.h>
#include <linux/mfd/rpisense/framebuffer.h>
*/

#include "joystick.h"
#include "framebuffer.h"
>>>>>>> 86a0552 (new char driver)
>>>>>>> a8e899b (new char driver)
>>>>>>> a750644 (new char driver)

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
	struct rpisense_js joystick;
<<<<<<< HEAD
	struct rpisense_cd char_dev;
=======
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
	struct rpisense_fb framebuffer;
=======
	struct rpisense_cd char_dev;
>>>>>>> 6a892bc (rpisense-cd.c with read functionality)
=======
=======
>>>>>>> a8e899b (new char driver)
	struct rpisense_cd char_dev;
=======
	struct rpisense_fb framebuffer;
>>>>>>> b54532f (framebuffer testfile)
<<<<<<< HEAD
>>>>>>> ea10db8 (framebuffer testfile)
<<<<<<< HEAD
>>>>>>> ed899c1 (framebuffer testfile)
=======
=======
=======
	struct rpisense_cd char_dev;
>>>>>>> 86a0552 (new char driver)
>>>>>>> a8e899b (new char driver)
>>>>>>> a750644 (new char driver)
};

s32 rpisense_reg_read(struct rpisense *rpisense, int reg);
int rpisense_reg_write(struct rpisense *rpisense, int reg, u16 val);
int rpisense_block_write(struct rpisense *rpisense, const char *buf, int count);
<<<<<<< HEAD
int rpisense_block_read(struct rpisense *rpisense, char *buf, int count);
=======
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
int rpisense_block_read(struct rpisense *rpisense, char *buf, int count);
>>>>>>> 6a892bc (rpisense-cd.c with read functionality)
=======
int rpisense_block_read(struct rpisense *rpisense, char *buf, int count);
=======
>>>>>>> b54532f (framebuffer testfile)
>>>>>>> ea10db8 (framebuffer testfile)
<<<<<<< HEAD
>>>>>>> ed899c1 (framebuffer testfile)
=======
=======
int rpisense_block_read(struct rpisense *rpisense, char *buf, int count);
=======
>>>>>>> b54532f (framebuffer testfile)
=======
int rpisense_block_read(struct rpisense *rpisense, char *buf, int count);
>>>>>>> 1d43e44 (rpisense-cd.c with read functionality)
>>>>>>> 421ef24 (rpisense-cd.c with read functionality)
>>>>>>> f4c8cfc (rpisense-cd.c with read functionality)

#endif
