/*
 * Raspberry Pi Sense HAT core driver
 * http://raspberrypi.org
 *
 * Copyright (C) 2015 Raspberry Pi
 *
 * Author: Serge Schneider & Mwesigwa Thomas Guma
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 *  This driver is based on wm8350 implementation.
 */



#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>

#include "display.h"
#include "core.h"

/*
#include <linux/mfd/rpisense/framebuffer.h>
#include <linux/mfd/rpisense/core.h>
*/


#define MEM_SIZE 193

/*   Function prototypes  */

static int      rpisense_open(struct inode *inode, struct file *file);
static int      rpisense_release(struct inode *inode, struct file *file);
static ssize_t  rpisense_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t  rpisense_write(struct file *filp, const char *buf, size_t len, loff_t * off);
static void     user_data_to_i2c(void);

static struct file_operations rpisense_fops =
{
        .owner          = THIS_MODULE,
        .read           = rpisense_read,
        .write          = rpisense_write,
        .open           = rpisense_open,
        .release        = rpisense_release,
};

static struct miscdevice mdev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name  = "sense-hat",
	.fops  = &rpisense_fops,

};


struct rpisense_param {
	char __iomem *vmem;
	u8 *vmem_work;
	u32 vmemsize;
	u8 *gamma;
};


static u8 gamma_default[32] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
			       0x02, 0x02, 0x03, 0x03, 0x04, 0x05, 0x06, 0x07,
			       0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0E, 0x0F, 0x11,
			       0x12, 0x14, 0x15, 0x17, 0x19, 0x1B, 0x1D, 0x1F,};

static struct rpisense_param rpisense_param = {
	.vmem = NULL,
	.vmemsize = 128,
	.gamma = gamma_default,
};

static int rpisense_open(struct inode *inode, struct file *file)
{
        pr_info("Device File Opened...!!!\n");
        return 0;
}

static int rpisense_release(struct inode *inode, struct file *file)
{
        pr_info("Device File Closed...!!!\n");
        return 0;
}

static ssize_t rpisense_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
        pr_info("Read Function\n");
        return 0;
}

static ssize_t rpisense_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
        /* send data to i2c device in here */
	
	if (copy_from_user(rpisense_param.vmem, buf, rpisense_param.vmemsize)) {
		pr_err("Error: Data not written.\n");
	}
	
	user_data_to_i2c();
        pr_info("Write function\n");
        return len;
}

static void user_data_to_i2c(){
	
	int i;
	int j;
	u8 *vmem_work = rpisense_param.vmem_work;
	u16 *mem = (u16 *)rpisense_param.vmem;
	u8 *gamma = rpisense_param.gamma;

	struct rpisense* rpisense = dev_get_drvdata(mdev.parent);

	vmem_work[0] = 0;
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

	rpisense_block_write(rpisense, vmem_work, MEM_SIZE);
}


/* Performs early init and registers device with the kernel. */
static int rpisense_driver_probe(struct platform_device *pdev)
{
	int ret = -ENOMEM;
	struct rpisense *rpisense;
	struct rpisense_disp *rpisense_disp;

	rpisense_param.vmem = vzalloc(rpisense_param.vmemsize);
	if (!rpisense_param.vmem)
		return ret;
	
	rpisense_param.vmem_work = devm_kmalloc(&pdev->dev, MEM_SIZE, GFP_KERNEL);
	if (!rpisense_param.vmem_work)
		goto error_malloc; 		

	rpisense = dev_get_drvdata(pdev->dev.parent);
	rpisense_disp = &rpisense->display;

	mdev.parent = &pdev->dev;
	
	ret = misc_register(&mdev);
	if (ret != 0) {
		pr_err("could not register misc device...\n");
	}

	pr_info("Misc minor number: %i\n", mdev.minor);

	rpisense_disp->sense_mdev = &mdev;

        pr_info("Device Driver Insert...Done!!!\n");
        return 0;

error_malloc:
        vfree(rpisense_param.vmem);
        return ret;
}

static int rpisense_driver_remove(struct platform_device *pdev)
{
	misc_deregister(&mdev);
	vfree(rpisense_param.vmem);

        pr_info("Device Driver Remove...Done!!!\n");

	return 0;
}

#ifdef CONFIG_OF // CONFIG_OF is derived by the kernel from .config file 

/* structure used to find the framebuffer driver described in the device tree */
static const struct of_device_id rpisense_fb_id[] = {
	{ .compatible = "rpi,rpi-sense-fb" },
	{ },
};

/* 
   Macro is necessary to allow user-space tools to figure out what devices 
   the framebuffer driver can control.
 */ 
MODULE_DEVICE_TABLE(of, rpisense_fb_id); 
#endif

/* Used by bus code to bind actual device to driver */
static struct platform_device_id rpisense_fb_device_id[] = {
	{ .name = "rpi-sense-fb" },
	{ },
};
MODULE_DEVICE_TABLE(platform, rpisense_fb_device_id);


/* Structure used to register platform device (framebuffer device) */
static struct platform_driver rpisense_fb_driver = {
	.probe = rpisense_driver_probe,
	.remove = rpisense_driver_remove,
	.driver = {
		.name = "rpi-sense-fb",
		.owner = THIS_MODULE,
	},
};

/* used in place of module_init and module_exit which are called at moudule insertion and exit time */
module_platform_driver(rpisense_fb_driver);

/* Module description and licensing */ 
MODULE_DESCRIPTION("Sense Hat display driver");
MODULE_AUTHOR("Serge Schneider <serge@raspberrypi.org> Mwesigwa Thomas Guma  <gumathomas097@gmail.com");
MODULE_LICENSE("GPL");

