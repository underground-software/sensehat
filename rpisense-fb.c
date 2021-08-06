/*
 * Raspberry Pi Sense HAT framebuffer driver
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
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/platform_device.h>

#include "rpisense.h"

static bool lowlight;
module_param(lowlight, bool, 0);
MODULE_PARM_DESC(lowlight, "Reduce LED matrix brightness to one third");

static const u8 gamma_presets[][32] =
{
	/* default gamma */
	{
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
		0x02, 0x02, 0x03, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0E, 0x0F, 0x11,
		0x12, 0x14, 0x15, 0x17, 0x19, 0x1B, 0x1D, 0x1F,
	},
	/* lowlight gamma */
	{
		0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
		0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02,
		0x03, 0x03, 0x03, 0x04, 0x04, 0x05, 0x05, 0x06,
		0x06, 0x07, 0x07, 0x08, 0x08, 0x09, 0x0A, 0x0A,
	},
};

static struct file_operations rpisense_fb_fops;

static int rpisense_fb_probe(struct platform_device *pdev)
{
	int ret;

	struct rpisense *rpisense = dev_get_drvdata(&pdev->dev);
	struct rpisense_fb *rpisense_fb = &rpisense->framebuffer;

	rpisense_fb->gamma = devm_kmalloc(&pdev->dev, 32, GFP_KERNEL);
	if(!rpisense_fb->gamma)
		return -ENOMEM;
	memcpy(rpisense_fb->gamma, gamma_presets[lowlight], 32);

	rpisense_fb->vmem = (void *)devm_get_free_pages(&pdev->dev, GFP_KERNEL, 0);
	if (!rpisense_fb->vmem)
		return -ENOMEM;
	memset(rpisense_fb->vmem, 0, PAGE_SIZE);

	mutex_init(&rpisense_fb->rw_mtx);

	rpisense_fb->mdev = (struct miscdevice) {
		.minor	= MISC_DYNAMIC_MINOR,
		.name	= "sense-hat",
		.mode	= 0666,
		.fops	= &rpisense_fb_fops,
	};

	ret = misc_register(&rpisense_fb->mdev);
	if (ret < 0) {
		dev_err(&pdev->dev, "Could not register framebuffer.\n");
		return ret;
	}

	dev_info(&pdev->dev, "framebuffer registered with minor number %i", rpisense_fb->mdev.minor);

	rpisense_update_framebuffer(rpisense);
	return 0;
}

static int rpisense_fb_remove(struct platform_device *pdev)
{
	struct rpisense *rpisense = dev_get_drvdata(&pdev->dev);
	struct rpisense_fb *rpisense_fb = &rpisense->framebuffer;
	misc_deregister(&rpisense_fb->mdev);
	return 0;
}

static loff_t rpisense_fb_llseek(struct file *filp, loff_t pos, int whence)
{
	loff_t base;
	switch(whence)
	{
	case SEEK_SET:
		base = 0;
		break;
	case SEEK_CUR:
		base = filp->f_pos;
		break;
	case SEEK_END:
		base = 128;
		break;
	default:
		return -EINVAL;
	}
	base += pos;
	if(base < 0 || base >= 128)
		return -EINVAL;
	filp->f_pos = base;
	return base;
}

static ssize_t rpisense_fb_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	struct rpisense *rpisense = container_of(filp->private_data, struct rpisense, framebuffer.mdev);
	struct rpisense_fb *rpisense_fb = &rpisense->framebuffer;
	ssize_t retval = -EFAULT;
	if(*f_pos >= 128)
		return 0;
	if(*f_pos + count > 128)
		count = 128 - *f_pos;
	if(mutex_lock_interruptible(&rpisense_fb->rw_mtx))
		return -ERESTARTSYS;
	if(copy_to_user(buf, rpisense_fb->vmem + *f_pos, count))
		goto out;
	*f_pos += count;
	retval = count;
out:
	mutex_unlock(&rpisense_fb->rw_mtx);
	return retval;
}

static ssize_t rpisense_fb_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	struct rpisense *rpisense = container_of(filp->private_data, struct rpisense, framebuffer.mdev);
	struct rpisense_fb *rpisense_fb = &rpisense->framebuffer;
	u8 temp[128];
	if(*f_pos >= 128)
		return -EFBIG;
	if(*f_pos + count > 128)
		count = 128 - *f_pos;
	if(copy_from_user(temp, buf, count))
		return -EFAULT;
	if(mutex_lock_interruptible(&rpisense_fb->rw_mtx))
		return -ERESTARTSYS;
	memcpy(rpisense_fb->vmem + *f_pos, temp, count);
	rpisense_update_framebuffer(rpisense);
	*f_pos += count;
	mutex_unlock(&rpisense_fb->rw_mtx);
	return count;
}

static long rpisense_fb_ioctl(struct file *filp, unsigned int cmd,
			     unsigned long arg)
{
	struct rpisense *rpisense = container_of(filp->private_data, struct rpisense, framebuffer.mdev);
	struct rpisense_fb *rpisense_fb = &rpisense->framebuffer;
	void __user *user_ptr = (void __user *)arg;
	u8 temp[32];
	int ret;

	if (mutex_lock_interruptible(&rpisense_fb->rw_mtx))
		return -ERESTARTSYS;
	switch (cmd) {
	case SENSEFB_FBIOGET_GAMMA:
		if (copy_to_user(user_ptr, rpisense_fb->gamma, 32)) {
			ret = -EFAULT;
			goto out_unlock;
		}
		ret = 0;
		goto out_unlock;
	case SENSEFB_FBIOSET_GAMMA:
		if (copy_from_user(temp, user_ptr, 32)) {
			ret = -EFAULT;
			goto out_unlock;
		}
		ret = 0;
		goto out_update;
	case SENSEFB_FBIORESET_GAMMA:
		if(arg >= 2) {
			ret = -EINVAL;
			goto out_unlock;
		}
		memcpy(temp, gamma_presets[arg], 32);
		ret = 0;
		goto out_update;
	default:
		ret = -EINVAL;
		goto out_unlock;
	}
out_update:
	memcpy(rpisense_fb->gamma, temp, 32);
	rpisense_update_framebuffer(rpisense);
out_unlock:
	mutex_unlock(&rpisense_fb->rw_mtx);
	return ret;
}

static struct file_operations rpisense_fb_fops =
{
	.owner		= THIS_MODULE,
	.llseek		= rpisense_fb_llseek,
	.read		= rpisense_fb_read,
	.write		= rpisense_fb_write,
	.unlocked_ioctl	= rpisense_fb_ioctl,
};

#ifdef CONFIG_OF
static const struct of_device_id rpisense_fb_id[] = {
	{ .compatible = "rpi,rpi-sense-fb" },
	{ },
};
MODULE_DEVICE_TABLE(of, rpisense_fb_id);
#endif

static struct platform_device_id rpisense_fb_device_id[] = {
	{ .name = "rpi-sense-fb" },
	{ },
};
MODULE_DEVICE_TABLE(platform, rpisense_fb_device_id);

static struct platform_driver rpisense_fb_driver = {
	.probe = rpisense_fb_probe,
	.remove = rpisense_fb_remove,
	.driver = {
		.name = "rpi-sense-fb",
	},
};

module_platform_driver(rpisense_fb_driver);

MODULE_DESCRIPTION("Raspberry Pi Sense HAT framebuffer driver");
MODULE_AUTHOR("Serge Schneider <serge@raspberrypi.org>");
MODULE_LICENSE("GPL");

