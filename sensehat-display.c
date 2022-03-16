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

#define DISPLAY_SMB_REG 0x00

struct sensehat_display {
	struct platform_device *pdev;
	struct miscdevice mdev;
	struct mutex rw_mtx;
	struct {
		u16 b : 5, u : 1, g : 5, r : 5;
	} vmem[8][8];
	struct regmap *regmap;
};

#define VMEM_SIZE sizeof_field(struct sensehat_display, vmem)

static void sensehat_update_display(struct sensehat_display *display)
{
	int i, j, ret;
	u8 temp[8][3][8];

	for (i = 0; i < 8; ++i) {
		for (j = 0; j < 8; ++j)
			temp[i][0][j] = display->vmem[i][j].r;
		for (j = 0; j < 8; ++j)
			temp[i][1][j] = display->vmem[i][j].g;
		for (j = 0; j < 8; ++j)
			temp[i][2][j] = display->vmem[i][j].b;
	}

	ret = regmap_bulk_write(display->regmap, DISPLAY_SMB_REG, temp,
				sizeof(temp));
	if (ret < 0)
		dev_err(&display->pdev->dev,
			"Update to 8x8 LED matrix display failed");
}

static loff_t sensehat_display_llseek(struct file *filp, loff_t offset,
				      int whence)
{
	return fixed_size_llseek(filp, offset, whence, VMEM_SIZE);
}

static ssize_t sensehat_display_read(struct file *filp, char __user *buf,
				     size_t count, loff_t *f_pos)
{
	struct sensehat_display *sensehat_display =
		container_of(filp->private_data, struct sensehat_display, mdev);
	ssize_t ret = -EFAULT;

	if (*f_pos < 0 || *f_pos >= VMEM_SIZE)
		return 0;
	count = min_t(size_t, count, VMEM_SIZE - *f_pos);

	if (mutex_lock_interruptible(&sensehat_display->rw_mtx))
		return -ERESTARTSYS;
	if (copy_to_user(buf, *f_pos + (char *)sensehat_display->vmem, count))
		goto out;
	*f_pos += count;
	ret = count;
out:
	mutex_unlock(&sensehat_display->rw_mtx);
	return ret;
}

static ssize_t sensehat_display_write(struct file *filp, const char __user *buf,
				      size_t count, loff_t *f_pos)
{
	struct sensehat_display *sensehat_display =
		container_of(filp->private_data, struct sensehat_display, mdev);
	int ret = -EFAULT;

	if (*f_pos < 0 || *f_pos >= VMEM_SIZE)
		return -EFBIG;
	count = min_t(size_t, count, VMEM_SIZE - *f_pos);

	if (mutex_lock_interruptible(&sensehat_display->rw_mtx))
		return -ERESTARTSYS;
	if (copy_from_user(*f_pos + (char *)sensehat_display->vmem, buf, count))
		goto out;
	sensehat_update_display(sensehat_display);
	*f_pos += count;
	ret = count;
out:
	mutex_unlock(&sensehat_display->rw_mtx);
	return ret;
}

static const struct file_operations sensehat_display_fops = {
	.owner = THIS_MODULE,
	.llseek = sensehat_display_llseek,
	.read = sensehat_display_read,
	.write = sensehat_display_write,
};

static int sensehat_display_probe(struct platform_device *pdev)
{
	int ret;

	struct sensehat_display *sensehat_display =
		devm_kmalloc(&pdev->dev, sizeof(*sensehat_display), GFP_KERNEL);

	sensehat_display->pdev = pdev;

	dev_set_drvdata(&pdev->dev, sensehat_display);

	sensehat_display->regmap = dev_get_regmap(pdev->dev.parent, NULL);

	memset(sensehat_display->vmem, 0, VMEM_SIZE);

	mutex_init(&sensehat_display->rw_mtx);

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

static const struct of_device_id sensehat_display_device_id[] = {
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
