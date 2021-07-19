/*
 *
 *
 * Author: Mwesigwa Thomas Guma
 *
 *
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
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/fs.h>

#include "framebuffer.h"
#include "core.h"

/*
#include <linux/mfd/rpisense/framebuffer.h>
#include <linux/mfd/rpisense/core.h>
<<<<<<< HEAD


// Boolean command line argument used to reduce brightness.
static bool lowlight;

// macro function used to allow commandline arguments to be passed to the module.
module_param(lowlight, bool, 0);

// Macro used to document arguments that the module takes (lowlight).
MODULE_PARM_DESC(lowlight, "Reduce LED matrix brightness to one third");

<<<<<<< HEAD
/* static struct rpisense *rpisense contains framebuffer and joystick devices.
   Declared in linux/mfd/rpisense/core.h. */
=======
/* 
   static struct rpisense *rpisense is declared in linux/mfd/rpisense/core.h.
   contains: 
   struct device *dev;
   struct i2c_client *i2c_client;

   //  Client devices
   struct rpisense_js joystick;
   struct rpisense_fb framebuffer; 
*/
>>>>>>> ea10db8 (framebuffer testfile)
static struct rpisense *rpisense;
=======
*/
>>>>>>> a8e899b (new char driver)

/*   Function prototypes  */

static int      sense_open(struct inode *inode, struct file *file);
static int      sense_release(struct inode *inode, struct file *file);
static ssize_t  sense_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t  sense_write(struct file *filp, const char *buf, size_t len, loff_t * off);

static struct file_operations fops =
{
        .owner          = THIS_MODULE,
        .read           = sense_read,
        .write          = sense_write,
        .open           = sense_open,
        .release        = sense_release,
};


dev_t dev = 0;
static struct class *dev_class;
static struct cdev sense_cdev;

static struct rpisense *rpisense;

static int sense_open(struct inode *inode, struct file *file)
{
        pr_info("Device File Opened...!!!\n");
        return 0;
}

static int sense_release(struct inode *inode, struct file *file)
{
        pr_info("Device File Closed...!!!\n");
        return 0;
}

static ssize_t sense_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
        pr_info("Read Function\n");
        return 0;
}

static ssize_t sense_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
        /* send data to i2c device in here */

        pr_info("Write function\n");
	rpisense_block_write(rpisense, buf, 193);
        return len;
}


/* Performs early init and registers device with the kernel. */
static int rpisense_driver_probe(struct platform_device *pdev)
{
	int ret = -ENOMEM;
	struct rpisense_cd *rpisense_cd;

	rpisense = dev_get_drvdata(pdev->dev.parent);
	rpisense_cd = &rpisense->char_dev;
	
	/*Allocating Major number*/

	ret = alloc_chrdev_region(&dev, 0, 1, "sense_Dev");
        if(ret < 0){
                pr_err("Cannot allocate major number\n");
                return -1;
        }
        pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));



	/*Creating cdev structure*/
        cdev_init(&sense_cdev,&fops);



        /*Adding character device to the system*/
	ret = cdev_add(&sense_cdev,dev,1);
        if(ret < 0){
            pr_err("Cannot add the device to the system\n");
            goto r_class;
        }


	/*Creating device*/
        if((device_create(dev_class,NULL,dev,NULL,"sense_device")) == NULL){
            pr_err("Cannot create the Device 1\n");
            goto r_device;
        }

	rpisense_cd->c_dev = &sense_cdev;

        pr_info("Device Driver Insert...Done!!!\n");
        return 0;


r_device:
        class_destroy(dev_class);
r_class:
        unregister_chrdev_region(dev,1);
        return -1;
}

static int rpisense_driver_remove(struct platform_device *pdev)
{

	device_destroy(dev_class,dev);
        class_destroy(dev_class);
        cdev_del(&sense_cdev);
        unregister_chrdev_region(dev, 1);
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
MODULE_DESCRIPTION("Sense Hat char driver");
MODULE_AUTHOR("Mwesigwa Thomas Guma  <mguma@redhat.com>");
MODULE_LICENSE("GPL");

