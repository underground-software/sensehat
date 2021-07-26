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
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
#include "core.h"
#include <linux/slab.h>

=======
=======
>>>>>>> ed899c1 (framebuffer testfile)
=======
>>>>>>> a750644 (new char driver)
=======
>>>>>>> 067e75a (commit to merge with master)
=======
>>>>>>> ab5a08d (new display driver)
<<<<<<< HEAD
#include <linux/mfd/rpisense/core.h>
#include <linux/slab.h>

=======
=======
>>>>>>> ea10db8 (framebuffer testfile)
=======
>>>>>>> a8e899b (new char driver)
=======
>>>>>>> 3bae669 (commit to merge with master)
=======
>>>>>>> 7c4b617 (new display driver)
//#include <linux/mfd/rpisense/core.h>
=======
>>>>>>> bfd00fb (new display driver)
#include <linux/slab.h>

#include "core.h"

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> 6a892bc (rpisense-cd.c with read functionality)
=======
=======
#include <linux/mfd/rpisense/core.h>
#include <linux/slab.h>

>>>>>>> b54532f (framebuffer testfile)
<<<<<<< HEAD
>>>>>>> ea10db8 (framebuffer testfile)
=======
=======
=======
>>>>>>> 3bae669 (commit to merge with master)
//#include <linux/mfd/rpisense/core.h>

<<<<<<< HEAD
>>>>>>> 86a0552 (new char driver)
>>>>>>> a8e899b (new char driver)
=======
>>>>>>> 3bae669 (commit to merge with master)
=======
//#include <linux/mfd/rpisense/core.h>

=======
>>>>>>> bfd00fb (new display driver)
>>>>>>> 7c4b617 (new display driver)
static struct rpisense *rpisense;

>>>>>>> 639469e (rpisense-cd.c with read functionality)
static void rpisense_client_dev_register(struct rpisense *rpisense,
					 const char *name,
					 struct platform_device **pdev)
{
	int ret;

	*pdev = platform_device_alloc(name, -1);
	if (*pdev == NULL) {
		dev_err(rpisense->dev, "Failed to allocate %s\n", name);
		return;
	}

	(*pdev)->dev.parent = rpisense->dev;
	platform_set_drvdata(*pdev, rpisense);
	ret = platform_device_add(*pdev);
	if (ret != 0) {
		dev_err(rpisense->dev, "Failed to register %s: %d\n",
			name, ret);
		platform_device_put(*pdev);
		*pdev = NULL;
	}
}

static int rpisense_probe(struct i2c_client *i2c,
			       const struct i2c_device_id *id)
{
	int ret;
	struct rpisense_js *rpisense_js;

	struct rpisense *rpisense = devm_kzalloc(&i2c->dev, sizeof *rpisense, GFP_KERNEL);
	if (rpisense == NULL)
		return -ENOMEM;

	i2c_set_clientdata(i2c, rpisense);
	rpisense->dev = &i2c->dev;
	rpisense->i2c_client = i2c;

	ret = rpisense_reg_read(rpisense, RPISENSE_WAI);
	if (ret > 0) {
		if (ret != RPISENSE_ID)
			return -EINVAL;
	} else {
		return ret;
	}
	ret = rpisense_reg_read(rpisense, RPISENSE_VER);
	if (ret < 0)
		return ret;

	dev_info(rpisense->dev,
		 "Raspberry Pi Sense HAT firmware version %i\n", ret);

	rpisense_js = &rpisense->joystick;
	rpisense_js->keys_desc = devm_gpiod_get(&i2c->dev,
						"keys-int", GPIOD_IN);
	if (IS_ERR(rpisense_js->keys_desc)) {
		dev_warn(&i2c->dev, "Failed to get keys-int descriptor.\n");
		return PTR_ERR(rpisense_js->keys_desc);
	}
	rpisense_client_dev_register(rpisense, "rpi-sense-js",
				     &(rpisense->joystick.pdev));
	rpisense_client_dev_register(rpisense, "rpi-sense-fb",
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
				     &(rpisense->framebuffer.pdev));
=======
				     &(rpisense->char_dev.pdev));
>>>>>>> 6a892bc (rpisense-cd.c with read functionality)
=======
=======
>>>>>>> a8e899b (new char driver)
=======
>>>>>>> 2615cec (accessing plaltform device through misdevice.parent)
				     &(rpisense->char_dev.pdev));
=======
				     &(rpisense->framebuffer.pdev));
>>>>>>> b54532f (framebuffer testfile)
<<<<<<< HEAD
>>>>>>> ea10db8 (framebuffer testfile)
=======
=======
				     &(rpisense->char_dev.pdev));
>>>>>>> 86a0552 (new char driver)
<<<<<<< HEAD
>>>>>>> a8e899b (new char driver)
=======
=======
				     &(rpisense->display.pdev));
>>>>>>> 107da44 (accessing plaltform device through misdevice.parent)
>>>>>>> 2615cec (accessing plaltform device through misdevice.parent)
=======
				     &(rpisense->display.pdev));
>>>>>>> 3bae669 (commit to merge with master)

	return 0;
}

static int rpisense_remove(struct i2c_client *i2c)
{
	struct rpisense *rpisense = i2c_get_clientdata(i2c);

	platform_device_unregister(rpisense->joystick.pdev);
	return 0;
}


s32 rpisense_reg_read(struct rpisense *rpisense, int reg)
{
	int ret = i2c_smbus_read_byte_data(rpisense->i2c_client, reg);

	if (ret < 0)
		dev_err(rpisense->dev, "Read from reg %d failed\n", reg);
	/* Due to the BCM283x I2C clock stretching bug, some values
	 * may have MSB set. Clear it to avoid incorrect values.
	 * */
	return ret & 0x7F;
}
EXPORT_SYMBOL_GPL(rpisense_reg_read);

int rpisense_block_write(struct rpisense *rpisense, const char *buf, int count)
{
	int ret = i2c_master_send(rpisense->i2c_client, buf, count);

	if (ret < 0)
		dev_err(rpisense->dev, "Block write failed\n");
	return ret;
}
EXPORT_SYMBOL_GPL(rpisense_block_write);

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
=======
>>>>>>> ea10db8 (framebuffer testfile)
=======
>>>>>>> 421ef24 (rpisense-cd.c with read functionality)
int rpisense_block_read(struct rpisense *rpisense, char *buf, int count)
{
	int ret = i2c_master_recv(rpisense->i2c_client, buf, count);

	if (ret < 0)
		dev_err(rpisense->dev, "Block read failed\n");
	return ret;

}
EXPORT_SYMBOL_GPL(rpisense_block_read);

<<<<<<< HEAD
>>>>>>> 6a892bc (rpisense-cd.c with read functionality)
=======
=======
>>>>>>> b54532f (framebuffer testfile)
<<<<<<< HEAD
>>>>>>> ea10db8 (framebuffer testfile)
=======
=======
int rpisense_block_read(struct rpisense *rpisense, char *buf, int count)
{
        int ret = i2c_master_recv(rpisense->i2c_client, buf, count);

        if (ret < 0)
                dev_err(rpisense->dev, "Block read failed\n");
        return ret;
}
EXPORT_SYMBOL_GPL(rpisense_block_read);

<<<<<<< HEAD
>>>>>>> 1d43e44 (rpisense-cd.c with read functionality)
<<<<<<< HEAD
>>>>>>> 421ef24 (rpisense-cd.c with read functionality)
=======
=======

>>>>>>> 107da44 (accessing plaltform device through misdevice.parent)
>>>>>>> 2615cec (accessing plaltform device through misdevice.parent)
=======
>>>>>>> 3bae669 (commit to merge with master)
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
	.remove = rpisense_remove,
	.id_table = rpisense_i2c_id,
};

module_i2c_driver(rpisense_driver);

MODULE_DESCRIPTION("Raspberry Pi Sense HAT core driver");
MODULE_AUTHOR("Serge Schneider <serge@raspberrypi.org>");
MODULE_LICENSE("GPL");

