// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Raspberry Pi Sense HAT core driver
 * http://raspberrypi.org
 *
 * Copyright (C) 2015 Raspberry Pi
 * Copyright (C) 2021 Charles Mirabile, Mwesigwa Guma, Joel Savitz
 *
 * Original Author: Serge Schneider
 * Revised for upstream Linux by: Charles Mirabile, Mwesigwa Guma, Joel Savitz
 *
 * This driver is based on wm8350 implementation and was refactored to use the
 * misc device subsystem rather than the deprecated framebuffer subsystem.
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include "sensehat.h"

#define RPISENSE_DISPLAY		0x00
#define RPISENSE_WAI			0xF0
#define RPISENSE_VER			0xF1
#define RPISENSE_KEYS			0xF2
#define RPISENSE_EE_WP			0xF3

#define RPISENSE_ID			's'

static struct platform_device *
sensehat_client_dev_register(struct sensehat *sensehat, const char *name);

static int sensehat_probe(struct i2c_client *i2c,
			       const struct i2c_device_id *id)
{
	int ret;

	struct sensehat *sensehat = devm_kzalloc(&i2c->dev, sizeof(*sensehat), GFP_KERNEL);

	if (!sensehat)
		return -ENOMEM;

	i2c_set_clientdata(i2c, sensehat);
	sensehat->dev = &i2c->dev;
	sensehat->i2c_client = i2c;


	ret = i2c_smbus_read_byte_data(sensehat->i2c_client, RPISENSE_WAI);
	if (ret < 0) {
		dev_err(sensehat->dev, "failed to read from device");
		return ret;
	}

	if (ret != RPISENSE_ID) {
		dev_err(sensehat->dev, "expected device ID %i, got %i",
			RPISENSE_ID, ret);
		return -EINVAL;
	}

	ret = i2c_smbus_read_byte_data(sensehat->i2c_client, RPISENSE_VER);
	if (ret < 0)
		return ret;

	dev_info(sensehat->dev,
		 "Raspberry Pi Sense HAT firmware version %i\n", ret);

	sensehat->joystick.pdev = sensehat_client_dev_register(sensehat,
							       "sensehat-joystick");

	if (IS_ERR(sensehat->joystick.pdev)) {
		dev_err(sensehat->dev, "failed to register sensehat-joystick");
		return PTR_ERR(sensehat->joystick.pdev);
	}

	sensehat->display.pdev = sensehat_client_dev_register(sensehat,
								  "sensehat-display");

	if (IS_ERR(sensehat->display.pdev)) {
		dev_err(sensehat->dev, "failed to register sensehat-display");
		return PTR_ERR(sensehat->display.pdev);
	}

	return 0;
}

static struct platform_device *
sensehat_client_dev_register(struct sensehat *sensehat, const char *name)
{
	long ret = -ENOMEM;
	struct platform_device *pdev = platform_device_alloc(name, -1);

	if (!pdev)
		goto alloc_fail;

	pdev->dev.parent = sensehat->dev;
	platform_set_drvdata(pdev, sensehat);

	ret = platform_device_add(pdev);
	if (ret)
		goto add_fail;

	ret = devm_add_action_or_reset(sensehat->dev,
		(void *)platform_device_unregister, pdev);
	if (ret)
		goto alloc_fail;

	return pdev;

add_fail:
	platform_device_put(pdev);
alloc_fail:
	return ERR_PTR(ret);
}

int sensehat_get_joystick_state(struct sensehat *sensehat)
{
	int ret = i2c_smbus_read_byte_data(sensehat->i2c_client, RPISENSE_KEYS);

	return ret < 0 ? ret : ret & 0x1f;
}
EXPORT_SYMBOL_GPL(sensehat_get_joystick_state);

int sensehat_update_display(struct sensehat *sensehat)
{
	int i, j, ret;
	struct sensehat_display *display = &sensehat->display;
	struct {u8 reg, pixel_data[8][3][8]; } msg;

	msg.reg = RPISENSE_DISPLAY;
	for (i = 0; i < 8; ++i) {
		for (j = 0; j < 8; ++j) {
			msg.pixel_data[i][0][j] = display->gamma[display->vmem[i][j].r];
			msg.pixel_data[i][1][j] = display->gamma[display->vmem[i][j].g];
			msg.pixel_data[i][2][j] = display->gamma[display->vmem[i][j].b];
		}
	}

	ret = i2c_master_send(sensehat->i2c_client, (u8 *)&msg, sizeof(msg));
	if (ret < 0)
		dev_err(sensehat->dev, "Update to 8x8 LED matrix display failed");
	return ret;
}
EXPORT_SYMBOL_GPL(sensehat_update_display);

static const struct i2c_device_id sensehat_i2c_id[] = {
	{ "sensehat", 0 },
	{ "rpi-sense", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, sensehat_i2c_id);

static struct i2c_driver sensehat_driver = {
	.driver = {
		   .name = "sensehat",
	},
	.probe = sensehat_probe,
	.id_table = sensehat_i2c_id,
};

module_i2c_driver(sensehat_driver);

MODULE_DESCRIPTION("Raspberry Pi Sense HAT core driver");
MODULE_AUTHOR("Serge Schneider <serge@raspberrypi.org>");
MODULE_LICENSE("GPL");
