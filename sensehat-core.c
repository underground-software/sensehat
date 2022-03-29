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
#include <linux/err.h>
#include <linux/i2c.h>
#include <linux/of_platform.h>
#include <linux/regmap.h>

static struct regmap_config sensehat_config = {
	.name = "sensehat",
	.reg_bits = 8,
	.val_bits = 8,
};

static int sensehat_probe(struct i2c_client *i2c)
{
	struct regmap *regmap =
		devm_regmap_init_i2c(i2c, &sensehat_config);

	if (IS_ERR(regmap)) {
		dev_err(&i2c->dev, "Failed to initialize sensehat regmap");
		return PTR_ERR(regmap);
	}

	devm_of_platform_populate(&i2c->dev);

	return 0;
}

static const struct of_device_id sensehat_i2c_of_match[] = {
	{ .compatible = "raspberrypi,sensehat" },
	{},
};
MODULE_DEVICE_TABLE(of, sensehat_i2c_of_match);

static struct i2c_driver sensehat_driver = {
	.probe_new = sensehat_probe,
	.driver = {
		.name = "sensehat",
		.of_match_table = sensehat_i2c_of_match,
	},
};

module_i2c_driver(sensehat_driver);

MODULE_DESCRIPTION("Raspberry Pi Sense HAT core driver");
MODULE_AUTHOR("Serge Schneider <serge@raspberrypi.org>");
MODULE_LICENSE("GPL");
