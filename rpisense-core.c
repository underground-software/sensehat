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
 *  This driver is based on wm8350 implementation.
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include "rpisense.h"
#include <linux/slab.h>

#define RPISENSE_FB			0x00
#define RPISENSE_WAI			0xF0
#define RPISENSE_VER			0xF1
#define RPISENSE_KEYS			0xF2
#define RPISENSE_EE_WP			0xF3

#define RPISENSE_ID			's'

static struct platform_device *
rpisense_client_dev_register(struct rpisense *rpisense, const char *name);

static int rpisense_probe(struct i2c_client *i2c,
			       const struct i2c_device_id *id)
{
	int ret;

	struct rpisense *rpisense = devm_kzalloc(&i2c->dev, sizeof *rpisense, GFP_KERNEL);
	if (rpisense == NULL)
		return -ENOMEM;

	i2c_set_clientdata(i2c, rpisense);
	rpisense->dev = &i2c->dev;
	rpisense->i2c_client = i2c;


	ret = i2c_smbus_read_byte_data(rpisense->i2c_client, RPISENSE_WAI);
	if (ret < 0)
		return ret;

	if (ret != RPISENSE_ID)
			return -EINVAL;

	ret = i2c_smbus_read_byte_data(rpisense->i2c_client, RPISENSE_VER);
	if (ret < 0)
		return ret;

	dev_info(rpisense->dev,
		 "Raspberry Pi Sense HAT firmware version %i\n", ret);

	rpisense->joystick.pdev = rpisense_client_dev_register(rpisense,
							       "rpi-sense-js");

	if(IS_ERR(rpisense->joystick.pdev)) {
		dev_err(rpisense->dev, "failed to register rpisense-js");
		return PTR_ERR(rpisense->joystick.pdev);
	}

	rpisense->framebuffer.pdev = rpisense_client_dev_register(rpisense,
								  "rpi-sense-fb");

	if(IS_ERR(rpisense->framebuffer.pdev)) {
		dev_err(rpisense->dev, "failed to register rpisense-fb");
		return PTR_ERR(rpisense->framebuffer.pdev);
	}

	return 0;
}

static struct platform_device *
rpisense_client_dev_register(struct rpisense *rpisense, const char *name)
{
	long ret = -ENOMEM;
	struct platform_device *pdev = platform_device_alloc(name, -1);
	if(pdev == NULL)
		goto alloc_fail;

	pdev->dev.parent = rpisense->dev;
	platform_set_drvdata(pdev, rpisense);

	ret = platform_device_add(pdev);
	if (ret != 0)
		goto add_fail;

	ret = devm_add_action_or_reset(rpisense->dev,
		(void *)platform_device_unregister, pdev);
	if(ret != 0)
		goto alloc_fail;

	return pdev;

add_fail:
	platform_device_put(pdev);
alloc_fail:
	return ERR_PTR(ret);
}

int rpisense_get_joystick_state(struct rpisense *rpisense)
{
	int ret = i2c_smbus_read_byte_data(rpisense->i2c_client, RPISENSE_KEYS);

	return ret < 0 ? ret : ret & 0x1f;
}
EXPORT_SYMBOL_GPL(rpisense_get_joystick_state);

int rpisense_update_framebuffer(struct rpisense *rpisense)
{
	int i,j,ret;
	u8 vmem_work[193];
	u16 *mem = (u16 *)rpisense->framebuffer.vmem;
	u8 *gamma = rpisense->framebuffer.gamma;

	vmem_work[0] = RPISENSE_FB;
	for (j = 0; j < 8; j++) {
		for (i = 0; i < 8; i++) {
			vmem_work[(j * 24) + i + 1] =
				gamma[(mem[(j * 8) + i] >> 11) & 0x1F];
			vmem_work[(j * 24) + (i + 8) + 1] =
				gamma[(mem[(j * 8) + i] >> 6) & 0x1F];
			vmem_work[(j * 24) + (i + 16) + 1] =
				gamma[(mem[(j * 8) + i]) & 0x1F];
		}
	}
	ret = i2c_master_send(rpisense->i2c_client, vmem_work, 193);
	if (ret < 0)
		dev_err(rpisense->dev, "Update framebuffer failed");
	return ret;
}
EXPORT_SYMBOL_GPL(rpisense_update_framebuffer);

static const struct i2c_device_id rpisense_i2c_id[] = {
	{ "rpi-sense", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, rpisense_i2c_id);

#ifdef CONFIG_OF
static const struct of_device_id rpisense_core_id[] = {
	{ .compatible = "rpi,rpi-sense" },
	{ },
};
MODULE_DEVICE_TABLE(of, rpisense_core_id);
#endif


static struct i2c_driver rpisense_driver = {
	.driver = {
		   .name = "rpi-sense",
	},
	.probe = rpisense_probe,
	.id_table = rpisense_i2c_id,
};

module_i2c_driver(rpisense_driver);

MODULE_DESCRIPTION("Raspberry Pi Sense HAT core driver");
MODULE_AUTHOR("Serge Schneider <serge@raspberrypi.org>");
MODULE_LICENSE("GPL");
