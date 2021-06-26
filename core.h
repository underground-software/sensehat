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

#include "joystick.h"
#include "framebuffer.h"

/*
 * Register values.
 */
//framebuffer starts at address 0 (unused atm but maybe could be used as part of framebuffer block write?)
#define RPISENSE_FB			0x00
//register holding id byte
#define RPISENSE_WAI			0xF0
//register holding firmware version
#define RPISENSE_VER			0xF1
//register that holds the state of the joystick
#define RPISENSE_KEYS			0xF2
//unused, I believe this is for the write protected on the eeprom but I am not sure
#define RPISENSE_EE_WP			0xF3

//this is never used, although it probably should be used in probe instead of a literal 's'
#define RPISENSE_ID			's'

//represents the data associated with one sensehat device
struct rpisense {
	//pointer to the linux device struct associated with this sensehat
	struct device *dev;
	//pointer to the i2c client for the sensehat on the i2c bus
	struct i2c_client *i2c_client;

	/* Client devices */
	struct rpisense_js joystick;
	struct rpisense_fb framebuffer;
};

//returns pointer to the sensehat struct
struct rpisense *rpisense_get_dev(void);
//read/write to device (these basically just call linux i2c funcs on your behalf)
s32 rpisense_reg_read(struct rpisense *rpisense, int reg);
//this function is actually not defined... not sure why it is included
int rpisense_reg_write(struct rpisense *rpisense, int reg, u16 val);
int rpisense_block_write(struct rpisense *rpisense, const char *buf, int count);

#endif
