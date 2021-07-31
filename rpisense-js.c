/*
 * Raspberry Pi Sense HAT joystick driver
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

#include <linux/module.h>

#include <linux/mfd/rpisense/joystick.h>
#include <linux/mfd/rpisense/core.h>


// NOTE: jstest utility for testing

// static rpisense struct. Ugly!
static struct rpisense *rpisense;
// input event codes from include/uapi/linux/input-event-codes.h via <linux/input.h>
static unsigned char keymap[5] = {KEY_DOWN, KEY_RIGHT, KEY_UP, KEY_ENTER, KEY_LEFT,};

// does this not need mutual exclusion? nah because the interupt is masked right?
static void keys_work_fn(struct work_struct *work)
{
	int i;
	static s32 prev_keys;
	struct rpisense_js *rpisense_js = &rpisense->joystick;
	// magically get the new state
	s32 keys = rpisense_reg_read(rpisense, RPISENSE_KEYS);
	// rising or falling only
	s32 changes = keys ^ prev_keys;

	// save for next time
	prev_keys = keys;
	for (i = 0; i < 5; i++) {
		// go through bit by bit,
		// if there is a change at a place, report it's state
		if (changes & 1) {
			// report for a EV_KEY
			input_report_key(rpisense_js->keys_dev,
					 keymap[i], keys & 1);
		}
		changes >>= 1;
		keys >>= 1;
	}
	// input report is completed.
	input_sync(rpisense_js->keys_dev);
}

// deprecated, will delete
static irqreturn_t keys_irq_handler(int irq, void *pdev)
{
	struct rpisense_js *rpisense_js = &rpisense->joystick;

	schedule_work(&rpisense_js->keys_work_s);
	return IRQ_HANDLED;
}

static int rpisense_js_probe(struct platform_device *pdev)
{
	int ret;
	int i;
	struct rpisense_js *rpisense_js;

	// get static instance of device
	rpisense = rpisense_get_dev();
	// connect to static instance's joystick
	rpisense_js = &rpisense->joystick;

	// put in that work
	// FIXME is this outdated? replaced by threaded IRQ
	INIT_WORK(&rpisense_js->keys_work_s, keys_work_fn);

	// I guess this could be devm_*
	rpisense_js->keys_dev = input_allocate_device();
	if (!rpisense_js->keys_dev) {
		dev_err(&pdev->dev, "Could not allocate input device.\n");
		return -ENOMEM;
		// self explanatory
	}

	// duplicate? yeah obviously we accept key preses
	rpisense_js->keys_dev->evbit[0] = BIT_MASK(EV_KEY);
	// set each bit in keybitmap to keys designated in keymap array
	for (i = 0; i < ARRAY_SIZE(keymap); i++) {
		set_bit(keymap[i],
			rpisense_js->keys_dev->keybit);
	}

	// sensible name
	rpisense_js->keys_dev->name = "Raspberry Pi Sense HAT Joystick";
	// I guess this makes sense too
	rpisense_js->keys_dev->phys = "rpi-sense-joy/input0";
	// yeah this is the right bus, no other fields needed?
	// TODO: what happens if you remove this
	rpisense_js->keys_dev->id.bustype = BUS_I2C;
	// this is just a repeat of above? Why
	rpisense_js->keys_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REP);
	// seems repetitive with keybit
	rpisense_js->keys_dev->keycode = keymap;
	// makes sense
	rpisense_js->keys_dev->keycodesize = sizeof(unsigned char);
	// also makes sense
	rpisense_js->keys_dev->keycodemax = ARRAY_SIZE(keymap);

	// pretty self explanatory
	ret = input_register_device(rpisense_js->keys_dev);
	if (ret) {
		dev_err(&pdev->dev, "Could not register input device.\n");
		goto err_keys_alloc;
	}

	// kind of important, but why not do all of the gpio stuff here?
	ret = gpiod_direction_input(rpisense_js->keys_desc);
	if (ret) {
		dev_err(&pdev->dev, "Could not set keys-int direction.\n");
		goto err_keys_reg;
	}

	// it all comes does down to an integer here
	rpisense_js->keys_irq = gpiod_to_irq(rpisense_js->keys_desc);
	if (rpisense_js->keys_irq < 0) {
		dev_err(&pdev->dev, "Could not determine keys-int IRQ.\n");
		ret = rpisense_js->keys_irq;
		goto err_keys_reg;
	}

	// new method uses threaded_irq, no need for work
	// ONESHOT patch: http://lkml.iu.edu/hypermail/linux/kernel/0908.1/02114.html
	// commit: b25c340c195447afb1860da580fe2a85a6b652c5
	// that bothered me so: https://lkml.org/lkml/2021/7/31/15
	ret = devm_request_irq(&pdev->dev, rpisense_js->keys_irq,
			       keys_irq_handler, IRQF_TRIGGER_RISING,
			       "keys", &pdev->dev);
	if (ret) {
		dev_err(&pdev->dev, "IRQ request failed.\n");
		goto err_keys_reg;
	}
	return 0;
err_keys_reg:
	input_unregister_device(rpisense_js->keys_dev);
err_keys_alloc:
	input_free_device(rpisense_js->keys_dev);
	return ret;
}

static int rpisense_js_remove(struct platform_device *pdev)
{
	struct rpisense_js *rpisense_js = &rpisense->joystick;

	// straightforward, all else is devm'ed
	input_unregister_device(rpisense_js->keys_dev);
	input_free_device(rpisense_js->keys_dev);
	return 0;


	// but tbh is this bloat?
}

// What is this actually used for? It doesn't correspond to anything in the DT
#ifdef CONFIG_OF
static const struct of_device_id rpisense_js_id[] = {
	{ .compatible = "rpi,rpi-sense-js" },
	{ },
};
MODULE_DEVICE_TABLE(of, rpisense_js_id);
#endif

// why is this a platform device?
static struct platform_device_id rpisense_js_device_id[] = {
	{ .name = "rpi-sense-js" },
	{ },
};
MODULE_DEVICE_TABLE(platform, rpisense_js_device_id);

// bloat?
static struct platform_driver rpisense_js_driver = {
	.probe = rpisense_js_probe,
	.remove = rpisense_js_remove,
	.driver = {
		.name = "rpi-sense-js",
		.owner = THIS_MODULE,
	},
};

// tidy but who cares?
module_platform_driver(rpisense_js_driver);

MODULE_DESCRIPTION("Raspberry Pi Sense HAT joystick driver");
MODULE_AUTHOR("Serge Schneider <serge@raspberrypi.org>");
MODULE_LICENSE("GPL");
