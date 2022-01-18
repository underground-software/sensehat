// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Raspberry Pi Sense HAT joystick driver
 * http://raspberrypi.org
 *
 * Copyright (C) 2015 Raspberry Pi
 * Copyright (C) 2021 Charles Mirabile, Mwesigwa Guma, Joel Savitz
 *
 * Original Author: Serge Schneider
 * Revised for upstream Linux by: Charles Mirabile, Mwesigwa Guma, Joel Savitz
 */

#include <linux/module.h>
#include <linux/input.h>
#include <linux/i2c.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include "sensehat.h"

#define SENSEHAT_KEYS 0xF2

static int sensehat_get_joystick_state(struct sensehat *sensehat);

static const unsigned char keymap[] = {
	KEY_DOWN, KEY_RIGHT, KEY_UP, KEY_ENTER, KEY_LEFT,
};

static irqreturn_t sensehat_joystick_report(int n, void *cookie)
{
	int i;
	static s32 prev_keys;
	struct sensehat *sensehat = cookie;
	struct sensehat_joystick *sensehat_joystick = &sensehat->joystick;
	s32 keys = sensehat_get_joystick_state(sensehat);
	s32 changes = keys ^ prev_keys;

	prev_keys = keys;
	for (i = 0; i < ARRAY_SIZE(keymap); ++i) {
		if (changes & (1 << i)) {
			input_report_key(sensehat_joystick->keys_dev, keymap[i],
					 keys & (1 << i));
		}
	}
	input_sync(sensehat_joystick->keys_dev);
	return IRQ_HANDLED;
}

static int sensehat_joystick_probe(struct platform_device *pdev)
{
	int error, i;
	struct sensehat *sensehat = dev_get_drvdata(&pdev->dev);
	struct sensehat_joystick *sensehat_joystick = &sensehat->joystick;

	sensehat_joystick->keys_dev = devm_input_allocate_device(&pdev->dev);
	if (!sensehat_joystick->keys_dev) {
		dev_err(&pdev->dev, "Could not allocate input device.\n");
		return -ENOMEM;
	}

	for (i = 0; i < ARRAY_SIZE(keymap); i++) {
		set_bit(keymap[i], sensehat_joystick->keys_dev->keybit);
	}

	sensehat_joystick->keys_dev->name = "Raspberry Pi Sense HAT Joystick";
	sensehat_joystick->keys_dev->phys = "sensehat-joystick/input0";
	sensehat_joystick->keys_dev->id.bustype = BUS_I2C;
	sensehat_joystick->keys_dev->evbit[0] =
		BIT_MASK(EV_KEY) | BIT_MASK(EV_REP);

	error = input_register_device(sensehat_joystick->keys_dev);
	if (error) {
		dev_err(&pdev->dev, "Could not register input device.\n");
		return error;
	}

	error = devm_request_threaded_irq(&pdev->dev, sensehat->i2c_client->irq,
					NULL, sensehat_joystick_report,
					IRQF_ONESHOT, "keys", sensehat);

	if (error) {
		dev_err(&pdev->dev, "IRQ request failed.\n");
		return error;
	}
	return 0;
}

int sensehat_get_joystick_state(struct sensehat *sensehat)
{
	unsigned int reg;
	int ret = regmap_read(sensehat->regmap, SENSEHAT_KEYS, &reg);

	return ret < 0 ? ret : reg;
}

static struct platform_device_id sensehat_joystick_device_id[] = {
	{ .name = "sensehat-joystick" },
	{},
};
MODULE_DEVICE_TABLE(platform, sensehat_joystick_device_id);

static struct platform_driver sensehat_joystick_driver = {
	.probe = sensehat_joystick_probe,
	.driver = {
		.name = "sensehat-joystick",
	},
};

module_platform_driver(sensehat_joystick_driver);

MODULE_DESCRIPTION("Raspberry Pi Sense HAT joystick driver");
MODULE_AUTHOR("Serge Schneider <serge@raspberrypi.org>");
MODULE_LICENSE("GPL");
