// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Raspberry Pi Sense HAT 8x8 LED matrix display driver
 * http://raspberrypi.org
 *
 * Copyright (C) 2015 Raspberry Pi
 * Copyright (C) 2021 Charles Mirabile, Mwesigwa Guma, Joel Savitz
 *
 * Original Author: Serge Schneider
 * Revised for upstream Linux by: Charles Mirabile, Mwesigwa Guma, Joel Savitz
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/mod_devicetable.h>
#include <linux/miscdevice.h>
#include <linux/regmap.h>
#include <linux/property.h>
#include "sensehat.h"

#define GAMMA_SIZE 32
#define VMEM_SIZE 192

struct sensehat_display {
	struct platform_device *pdev;
	struct miscdevice mdev;
	struct mutex rw_mtx;
	u8 gamma[GAMMA_SIZE];
	u8 vmem[VMEM_SIZE];
	u32 display_register;
	struct regmap *regmap;
};

static bool lowlight;
module_param(lowlight, bool, 0);
MODULE_PARM_DESC(lowlight, "Reduce LED matrix brightness to one third");

static const u8 gamma_presets[][GAMMA_SIZE] = {
	[GAMMA_DEFAULT] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
		0x02, 0x02, 0x03, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0E, 0x0F, 0x11,
		0x12, 0x14, 0x15, 0x17, 0x19, 0x1B, 0x1D, 0x1F,
	},
	[GAMMA_LOWLIGHT] = {
		0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
		0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02,
		0x03, 0x03, 0x03, 0x04, 0x04, 0x05, 0x05, 0x06,
		0x06, 0x07, 0x07, 0x08, 0x08, 0x09, 0x0A, 0x0A,
	},
};

static void sensehat_update_display(struct sensehat_display *display)
{
	int i, ret;
	u8 temp[VMEM_SIZE];

	for(i = 0; i < VMEM_SIZE; ++i)
		temp[i] = display->gamma[display->vmem[i] & 0x1f];

	ret = regmap_bulk_write(display->regmap, display->display_register, temp,
				VMEM_SIZE);
	if (ret < 0)
		dev_err(&display->pdev->dev,
			"Update to 8x8 LED matrix display failed");
}

static loff_t sensehat_display_llseek(struct file *filp, loff_t offset, int whence)
{
	return fixed_size_llseek(filp, offset, whence, VMEM_SIZE);
}

static ssize_t sensehat_display_read(struct file *filp, char __user *buf,
				     size_t count, loff_t *f_pos)
{
	struct sensehat_display *sensehat_display =
		container_of(filp->private_data, struct sensehat_display, mdev);
	ssize_t retval = -EFAULT;

	if (*f_pos >= VMEM_SIZE)
		return 0;
	if (*f_pos + count > VMEM_SIZE)
		count = VMEM_SIZE - *f_pos;
	if (mutex_lock_interruptible(&sensehat_display->rw_mtx))
		return -ERESTARTSYS;
	if (copy_to_user(buf, sensehat_display->vmem + *f_pos, count))
		goto out;
	*f_pos += count;
	retval = count;
out:
	mutex_unlock(&sensehat_display->rw_mtx);
	return retval;
}

static ssize_t sensehat_display_write(struct file *filp, const char __user *buf,
				      size_t count, loff_t *f_pos)
{
	struct sensehat_display *sensehat_display =
		container_of(filp->private_data, struct sensehat_display, mdev);
	int ret = count;

	if (*f_pos >= VMEM_SIZE)
		return -EFBIG;
	if (*f_pos + count > VMEM_SIZE)
		count = VMEM_SIZE - *f_pos;
	if (mutex_lock_interruptible(&sensehat_display->rw_mtx))
		return -ERESTARTSYS;
	if (copy_from_user(sensehat_display->vmem + *f_pos, buf, count))
	{
		ret = -EFAULT;
		goto out;
	}
	sensehat_update_display(sensehat_display);
	*f_pos += count;
out:
	mutex_unlock(&sensehat_display->rw_mtx);
	return ret;
}

static long sensehat_display_ioctl(struct file *filp, unsigned int cmd,
				   unsigned long arg)
{
	struct sensehat_display *sensehat_display =
		container_of(filp->private_data, struct sensehat_display, mdev);
	void __user *user_ptr = (void __user *)arg;
	int i, ret = 0;

	if (mutex_lock_interruptible(&sensehat_display->rw_mtx))
		return -ERESTARTSYS;

	switch (cmd) {
	case SENSEDISP_IOGET_GAMMA:
		if (copy_to_user(user_ptr, sensehat_display->gamma,
				 GAMMA_SIZE))
			ret = -EFAULT;
		goto no_update;
	case SENSEDISP_IOSET_GAMMA:
		if (copy_from_user(sensehat_display->gamma, user_ptr,
				GAMMA_SIZE))
			ret = -EFAULT;
		break;
	case SENSEDISP_IORESET_GAMMA:
		if (arg >= GAMMA_PRESET_COUNT)
		{
			ret = -EINVAL;
			goto no_update;
		}
		memcpy(sensehat_display->gamma, gamma_presets[arg],
			GAMMA_SIZE);
		goto no_check;
	default:
		ret = -EINVAL;
		break;
	}

	for(i = 0; i < GAMMA_SIZE; ++i)
		sensehat_display->gamma[i] &= 0x1f;
no_check:
	sensehat_update_display(sensehat_display);
no_update:
	mutex_unlock(&sensehat_display->rw_mtx);
	return ret;
}

static const struct file_operations sensehat_display_fops = {
	.owner = THIS_MODULE,
	.llseek = sensehat_display_llseek,
	.read = sensehat_display_read,
	.write = sensehat_display_write,
	.unlocked_ioctl = sensehat_display_ioctl,
};

static int sensehat_display_probe(struct platform_device *pdev)
{
	int ret;

	struct sensehat_display *sensehat_display = devm_kmalloc(&pdev->dev,
		sizeof(*sensehat_display), GFP_KERNEL);

	sensehat_display->pdev = pdev;

	dev_set_drvdata(&pdev->dev, sensehat_display);

	sensehat_display->regmap = dev_get_regmap(pdev->dev.parent, NULL);

	memcpy(sensehat_display->gamma, gamma_presets[lowlight], GAMMA_SIZE);

	memset(sensehat_display->vmem, 0, VMEM_SIZE);

	mutex_init(&sensehat_display->rw_mtx);

	ret = device_property_read_u32(&pdev->dev, "reg",
		&sensehat_display->display_register);
	if (ret) {
		dev_err(&pdev->dev, "Could not read register propery.\n");
		return ret;
	}

	sensehat_update_display(sensehat_display);

	sensehat_display->mdev = (struct miscdevice){
		.minor = MISC_DYNAMIC_MINOR,
		.name = "sense-hat",
		.mode = 0666,
		.fops = &sensehat_display_fops,
	};

	ret = misc_register(&sensehat_display->mdev);
	if (ret < 0) {
		dev_err(&pdev->dev,
			"Could not register 8x8 LED matrix display.\n");
		return ret;
	}

	dev_info(&pdev->dev,
		 "8x8 LED matrix display registered with minor number %i",
		 sensehat_display->mdev.minor);

	return 0;
}

static int sensehat_display_remove(struct platform_device *pdev)
{
	struct sensehat_display *sensehat_display = dev_get_drvdata(&pdev->dev);

	misc_deregister(&sensehat_display->mdev);
	return 0;
}

static struct of_device_id sensehat_display_device_id[] = {
	{ .compatible = "raspberrypi,sensehat-display" },
	{},
};
MODULE_DEVICE_TABLE(of, sensehat_display_device_id);

static struct platform_driver sensehat_display_driver = {
	.probe = sensehat_display_probe,
	.remove = sensehat_display_remove,
	.driver = {
		.name = "sensehat-display",
		.of_match_table = sensehat_display_device_id,
	},
};

module_platform_driver(sensehat_display_driver);

MODULE_DESCRIPTION("Raspberry Pi Sense HAT 8x8 LED matrix display driver");
MODULE_AUTHOR("Serge Schneider <serge@raspberrypi.org>");
MODULE_LICENSE("GPL");
