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
#include <linux/miscdevice.h>
#include <linux/platform_device.h>

#include "framebuffer.h"
#include "core.h"

/*
#include <linux/mfd/rpisense/framebuffer.h>
#include <linux/mfd/rpisense/core.h>
*/


#define MEM_SIZE 193

/*   Function prototypes  */

static int      sense_open(struct inode *inode, struct file *file);
static int      sense_release(struct inode *inode, struct file *file);
static ssize_t  sense_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t  sense_write(struct file *filp, const char *buf, size_t len, loff_t * off);
static void     user_data_to_i2c(void);

static struct file_operations sense_fops =
{
        .owner          = THIS_MODULE,
        .read           = sense_read,
        .write          = sense_write,
        .open           = sense_open,
        .release        = sense_release,
};

static struct miscdevice sense_mdev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name  = "sense_dev",
	.fops  = &sense_fops,

};


struct rpisense_param {
<<<<<<< HEAD
<<<<<<< HEAD
	char *rdata;
	//u8 *rmem;
=======
>>>>>>> b9a4fe0 (char device to replace framebuffer)
=======
	char * rdata;
>>>>>>> 1d43e44 (rpisense-cd.c with read functionality)
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

static struct rpisense *rpisense;

static int sense_open(struct inode *inode, struct file *file)
{
<<<<<<< HEAD
<<<<<<< HEAD
	if ((rpisense_param.rdata = kmalloc(MEM_SIZE, GFP_KERNEL)) == 0){
		pr_info("Failed to allocate memory...\n");
		return -1;
	}
=======
	/*
	if ((kernel_buffer = kmalloc(MEM_SIZE, GFP_KERNEL)) == 0) {
		pr_info("Failed to allocater memory....\n");
=======
	if ((rpisense_param.rdata = kmalloc(MEM_SIZE, GFP_KERNEL)) == 0) {
		pr_info("Failed to allocate memory....\n");
>>>>>>> 1d43e44 (rpisense-cd.c with read functionality)
		return -1;
	}

>>>>>>> b9a4fe0 (char device to replace framebuffer)
        pr_info("Device File Opened...!!!\n");
        return 0;
}

static int sense_release(struct inode *inode, struct file *file)
{
<<<<<<< HEAD
	kfree(rpisense_param.rdata);
=======

	kfree(rpisense_param.rdata);  

>>>>>>> b9a4fe0 (char device to replace framebuffer)
        pr_info("Device File Closed...!!!\n");
        return 0;
}

static ssize_t sense_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
<<<<<<< HEAD

<<<<<<< HEAD
	rpisense_block_read(rpisense, rpisense_param.rdata, MEM_SIZE);
	if (copy_to_user(buf, rpisense_param.rdata, MEM_SIZE)) {
                pr_err("Error: Data not read.\n");
        }
=======
	/*

	if (copy_to_user(buf, kernel_buffer, MEM_SIZE)) {
		pr_err("Error: Data not read.\n");
	}

	*/
>>>>>>> b9a4fe0 (char device to replace framebuffer)
=======
	rpisense_block_read(rpisense, rpisense_param.rdata, MEM_SIZE);
	if (copy_to_user(buf, rpisense_param.rdata, MEM_SIZE)) {
		pr_err("Error: Data not read.\n");
	}

>>>>>>> 1d43e44 (rpisense-cd.c with read functionality)

        pr_info("Read Function\n");
        return 0;
}

static ssize_t sense_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
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
	struct rpisense_cd *rpisense_cd;

	rpisense_param.vmem = vzalloc(rpisense_param.vmemsize);
	if (!rpisense_param.vmem)
		return ret;
	
	rpisense_param.vmem_work = devm_kmalloc(&pdev->dev, MEM_SIZE, GFP_KERNEL);
	if (!rpisense_param.vmem_work)
		goto error_malloc; 		

<<<<<<< HEAD
	/*
	rpisense_param.rmem = devm_kmalloc(&pdev->dev, MEM_SIZE, GFP_KERNEL);
	if (!rpisense_param.rmem)
		goto error_malloc;

	rpisense_param.rdata = (char*)rpisense_param.rmem;

	*/

=======
>>>>>>> b9a4fe0 (char device to replace framebuffer)
	rpisense = dev_get_drvdata(pdev->dev.parent);
	rpisense_cd = &rpisense->char_dev;
	
	ret = misc_register(&sense_mdev);
	if (ret != 0) {
		pr_err("could not register misc device...\n");
<<<<<<< HEAD
		goto error_malloc;
=======
>>>>>>> b9a4fe0 (char device to replace framebuffer)
	}

	pr_info("Misc minor number: %i\n", sense_mdev.minor);

	rpisense_cd->c_dev = &sense_mdev;

        pr_info("Device Driver Insert...Done!!!\n");
        return 0;

error_malloc:
        vfree(rpisense_param.vmem);
        return ret;
}

static int rpisense_driver_remove(struct platform_device *pdev)
{
	misc_deregister(&sense_mdev);
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
MODULE_DESCRIPTION("Sense Hat char driver");
MODULE_AUTHOR("Mwesigwa Thomas Guma  <mguma@redhat.com>");
MODULE_LICENSE("GPL");

