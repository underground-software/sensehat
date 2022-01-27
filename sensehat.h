/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Raspberry Pi Sense HAT core driver
 * http://raspberrypi.org
 *
 * Copyright (C) 2015 Raspberry Pi
 * Copyright (C) 2021 Charles Mirabile, Mwesigwa Guma, Joel Savitz
 *
 * Original Author: Serge Schneider
 * Revised for upstream Linux by: Charles Mirabile, Mwesigwa Guma, Joel Savitz
 */

#ifndef __LINUX_MFD_SENSEHAT_H_
#define __LINUX_MFD_SENSEHAT_H_

#define SENSEDISP_IOC_MAGIC 0xF1

#define SENSEDISP_IOGET_GAMMA _IO(SENSEDISP_IOC_MAGIC, 0)
#define SENSEDISP_IOSET_GAMMA _IO(SENSEDISP_IOC_MAGIC, 1)
#define SENSEDISP_IORESET_GAMMA _IO(SENSEDISP_IOC_MAGIC, 2)

enum gamma_preset {
	GAMMA_DEFAULT = 0,
	GAMMA_LOWLIGHT,
	GAMMA_PRESET_COUNT,
};

#endif
