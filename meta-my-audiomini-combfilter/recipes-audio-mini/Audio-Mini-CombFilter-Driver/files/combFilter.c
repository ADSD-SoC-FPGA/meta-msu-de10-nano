/* SPDX-License-Identifier: GPL-2.0 or MIT                               */
/* Copyright(c) 2021 Ross K.Snider. All rights reserved.                 */
/*-------------------------------------------------------------------------
 * Description:  Linux Platform Device Driver for the 
 *               combFilterProcessor_0 component
 * ------------------------------------------------------------------------
 * Authors : Ross K. Snider and Trevor Vannoy
 * Company : Montana State University
 * Create Date : November 10, 2021
 * Revision : 1.0
 * License : GPL-2.0 or MIT (opensource.org / licenses / MIT, GPL-2.0)
-------------------------------------------------------------------------*/
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/mod_devicetable.h>
#include <linux/types.h>
#include <linux/io.h>
#include <linux/mutex.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
/*#include "fp_conversions.h"*/

/*-----------------------------------------------------------------------*/
/* DEFINE STATEMENTS                                                     */
/*-----------------------------------------------------------------------*/
/* Define the Component Register Offsets*/
#define REG0_DELAYM_OFFSET 0x0
#define REG1_B0_OFFSET 0x04
#define REG2_BM_OFFSET 0x08
#define REG3_WETDRYMIX_OFFSET 0x0C

/* Memory span of all registers (used or not) in the                     */
/* component combFilterProcessor_0                                            */
#define SPAN 0x10


/*-----------------------------------------------------------------------*/
/* combFilterProcessor_0 device structure                                     */
/*-----------------------------------------------------------------------*/
/*
 * struct  combFilterProcessor_0_dev - Private combFilterProcessor_0 device struct.
 * @miscdev: miscdevice used to create a char device 
 *           for the combFilterProcessor_0 component
 * @base_addr: Base address of the combFilterProcessor_0 component
 * @lock: mutex used to prevent concurrent writes 
 *        to the combFilterProcessor_0 component
 *
 * An combFilterProcessor_0_dev struct gets created for each combFilterProcessor_0 
 * component in the system.
 */
struct combFilterProcessor_0_dev {
	struct miscdevice miscdev;
	void __iomem *base_addr;
	struct mutex lock;
};




/*-----------------------------------------------------------------------*/
/* REG0: delaym register read function show()                           */
/*-----------------------------------------------------------------------*/

static ssize_t delaym_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	u16 delaym;
	struct combFilterProcessor_0_dev *priv = dev_get_drvdata(dev);

	delaym = ioread16(priv->base_addr + REG0_DELAYM_OFFSET);

	return scnprintf(buf, PAGE_SIZE, "%u\n", delaym);
}
/*-----------------------------------------------------------------------*/
/* REG0: delaym register write function store()                         */
/*-----------------------------------------------------------------------*/

static ssize_t delaym_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	u16 delaym;
	int ret;
	struct combFilterProcessor_0_dev *priv = dev_get_drvdata(dev);

	// Parse the string we received as a u8
	// See https://elixir.bootlin.com/linux/latest/source/lib/kstrtox.c#L289 
	ret = kstrtou16(buf, 0, &delaym);
	if (ret < 0) {
		// kstrtou16 returned an error
		return ret;
	}

	iowrite16(delaym, priv->base_addr + REG0_DELAYM_OFFSET);

	// Write was succesful, so we return the number of bytes we wrote.
	return size;
}






/*-----------------------------------------------------------------------*/
/* REG1: b0 register read function show()                           */
/*-----------------------------------------------------------------------*/

static ssize_t b0_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	u16 b0;
	struct combFilterProcessor_0_dev *priv = dev_get_drvdata(dev);

	b0 = ioread16(priv->base_addr + REG1_B0_OFFSET);

	return scnprintf(buf, PAGE_SIZE, "%u\n", b0);
}
/*-----------------------------------------------------------------------*/
/* REG1: b0 register write function store()                         */
/*-----------------------------------------------------------------------*/

static ssize_t b0_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	u16 b0;
	int ret;
	struct combFilterProcessor_0_dev *priv = dev_get_drvdata(dev);

	// Parse the string we received as a u8
	// See https://elixir.bootlin.com/linux/latest/source/lib/kstrtox.c#L289 
	ret = kstrtou16(buf, 0, &b0);
	if (ret < 0) {
		// kstrtou16 returned an error
		return ret;
	}

	iowrite16(b0, priv->base_addr + REG1_B0_OFFSET);

	// Write was succesful, so we return the number of bytes we wrote.
	return size;
}






/*-----------------------------------------------------------------------*/
/* REG2: bm register read function show()                           */
/*-----------------------------------------------------------------------*/

static ssize_t bm_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	u16 bm;
	struct combFilterProcessor_0_dev *priv = dev_get_drvdata(dev);

	bm = ioread16(priv->base_addr + REG2_BM_OFFSET);

	return scnprintf(buf, PAGE_SIZE, "%u\n", bm);
}
/*-----------------------------------------------------------------------*/
/* REG2: bm register write function store()                         */
/*-----------------------------------------------------------------------*/

static ssize_t bm_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	u16 bm;
	int ret;
	struct combFilterProcessor_0_dev *priv = dev_get_drvdata(dev);

	// Parse the string we received as a u8
	// See https://elixir.bootlin.com/linux/latest/source/lib/kstrtox.c#L289 
	ret = kstrtou16(buf, 0, &bm);
	if (ret < 0) {
		// kstrtou16 returned an error
		return ret;
	}

	iowrite16(bm, priv->base_addr + REG2_BM_OFFSET);

	// Write was succesful, so we return the number of bytes we wrote.
	return size;
}





/*-----------------------------------------------------------------------*/
/* REG3: wetdrymix register read function show()                           */
/*-----------------------------------------------------------------------*/

static ssize_t wetdrymix_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	u16 wetdrymix;
	struct combFilterProcessor_0_dev *priv = dev_get_drvdata(dev);

	wetdrymix = ioread16(priv->base_addr + REG3_WETDRYMIX_OFFSET);

	return scnprintf(buf, PAGE_SIZE, "%u\n", wetdrymix);
}
/*-----------------------------------------------------------------------*/
/* REG3: wetdrymix register write function store()                         */
/*-----------------------------------------------------------------------*/

static ssize_t wetdrymix_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	u16 wetdrymix;
	int ret;
	struct combFilterProcessor_0_dev *priv = dev_get_drvdata(dev);

	// Parse the string we received as a u8
	// See https://elixir.bootlin.com/linux/latest/source/lib/kstrtox.c#L289 
	ret = kstrtou16(buf, 0, &wetdrymix);
	if (ret < 0) {
		// kstrtou16 returned an error
		return ret;
	}

	iowrite16(wetdrymix, priv->base_addr + REG3_WETDRYMIX_OFFSET);

	// Write was succesful, so we return the number of bytes we wrote.
	return size;
}


/*-----------------------------------------------------------------------*/
/* sysfs Attributes                                                      */
/*-----------------------------------------------------------------------*/
// Define sysfs attributes
static DEVICE_ATTR_RW(delaym);    			// Attribute for REG0
static DEVICE_ATTR_RW(b0);            		// Attribute for REG1
static DEVICE_ATTR_RW(bm);            		// Attribute for REG2
static DEVICE_ATTR_RW(wetdrymix);           // Attribute for REG3

// Create an atribute group so the device core can 
// export the attributes for us.
static struct attribute *combFilterProcessor_0_attrs[] = {
	&dev_attr_delaym.attr,
	&dev_attr_b0.attr,
	&dev_attr_bm.attr,
	&dev_attr_wetdrymix.attr,
	NULL,
};
ATTRIBUTE_GROUPS(combFilterProcessor_0);


/*-----------------------------------------------------------------------*/
/* File Operations read()                                                */
/*-----------------------------------------------------------------------*/
/*
 * combFilterProcessor_0_read() - Read method for the combFilterProcessor_0 char device
 * @file: Pointer to the char device file struct.
 * @buf: User-space buffer to read the value into.
 * @count: The number of bytes being requested.
 * @offset: The byte offset in the file being read from.
 *
 * Return: On success, the number of bytes written is returned and the
 * offset @offset is advanced by this number. On error, a negative error
 * value is returned.
 */
static ssize_t combFilterProcessor_0_read(struct file *file, char __user *buf,
	size_t count, loff_t *offset)
{
	size_t ret;
	u32 val;

	loff_t pos = *offset;

	/*
	 * Get the device's private data from the file struct's private_data
	 * field. The private_data field is equal to the miscdev field in the
	 * combFilterProcessor_0_dev struct. container_of returns the 
     * combFilterProcessor_0_dev struct that contains the miscdev in private_data.
	 */
	struct combFilterProcessor_0_dev *priv = container_of(file->private_data,
	                            struct combFilterProcessor_0_dev, miscdev);

	// Check file offset to make sure we are reading to a valid location.
	if (pos < 0) {
		// We can't read from a negative file position.
		return -EINVAL;
	}
	if (pos >= SPAN) {
		// We can't read from a position past the end of our device.
		return 0;
	}
	if ((pos % 0x4) != 0) {
		/*
		 * Prevent unaligned access. Even though the hardware
		 * technically supports unaligned access, we want to
		 * ensure that we only access 32-bit-aligned addresses
		 * because our registers are 32-bit-aligned.
		 */
		pr_warn("combFilterProcessor_0_read: unaligned access\n");
		return -EFAULT;
	}

	// If the user didn't request any bytes, don't return any bytes :)
	if (count == 0) {
		return 0;
	}

	// Read the value at offset pos.
	val = ioread32(priv->base_addr + pos);

	ret = copy_to_user(buf, &val, sizeof(val));
	if (ret == sizeof(val)) {
		// Nothing was copied to the user.
		pr_warn("combFilterProcessor_0_read: nothing copied\n");
		return -EFAULT;
	}

	// Increment the file offset by the number of bytes we read.
	*offset = pos + sizeof(val);

	return sizeof(val);
}
/*-----------------------------------------------------------------------*/
/* File Operations write()                                               */
/*-----------------------------------------------------------------------*/
/*
 * combFilterProcessor_0_write() - Write method for the combFilterProcessor_0 char device
 * @file: Pointer to the char device file struct.
 * @buf: User-space buffer to read the value from.
 * @count: The number of bytes being written.
 * @offset: The byte offset in the file being written to.
 *
 * Return: On success, the number of bytes written is returned and the
 * offset @offset is advanced by this number. On error, a negative error
 * value is returned.
 */
static ssize_t combFilterProcessor_0_write(struct file *file, const char __user *buf,
	size_t count, loff_t *offset)
{
	size_t ret;
	u32 val;

	loff_t pos = *offset;

	/*
	 * Get the device's private data from the file struct's private_data
	 * field. The private_data field is equal to the miscdev field in the
	 * combFilterProcessor_0_dev struct. container_of returns the 
     * combFilterProcessor_0_dev struct that contains the miscdev in private_data.
	 */
	struct combFilterProcessor_0_dev *priv = container_of(file->private_data,
	                              struct combFilterProcessor_0_dev, miscdev);

	// Check file offset to make sure we are writing to a valid location.
	if (pos < 0) {
		// We can't write to a negative file position.
		return -EINVAL;
	}
	if (pos >= SPAN) {
		// We can't write to a position past the end of our device.
		return 0;
	}
	if ((pos % 0x4) != 0) {
		/*
		 * Prevent unaligned access. Even though the hardware
		 * technically supports unaligned access, we want to
		 * ensure that we only access 32-bit-aligned addresses
		 * because our registers are 32-bit-aligned.
		 */
		pr_warn("combFilterProcessor_0_write: unaligned access\n");
		return -EFAULT;
	}

	// If the user didn't request to write anything, return 0.
	if (count == 0) {
		return 0;
	}

	mutex_lock(&priv->lock);

	ret = copy_from_user(&val, buf, sizeof(val));
	if (ret == sizeof(val)) {
		// Nothing was copied from the user.
		pr_warn("combFilterProcessor_0_write: nothing copied from user space\n");
		ret = -EFAULT;
		goto unlock;
	}

	// Write the value we were given at the address offset given by pos.
	iowrite32(val, priv->base_addr + pos);

	// Increment the file offset by the number of bytes we wrote.
	*offset = pos + sizeof(val);

	// Return the number of bytes we wrote.
	ret = sizeof(val);

unlock:
	mutex_unlock(&priv->lock);
	return ret;
}


/*-----------------------------------------------------------------------*/
/* File Operations Supported                                             */
/*-----------------------------------------------------------------------*/
/*
 *  combFilterProcessor_0_fops - File operations supported by the  
 *                          combFilterProcessor_0 driver
 * @owner: The combFilterProcessor_0 driver owns the file operations; this 
 *         ensures that the driver can't be removed while the 
 *         character device is still in use.
 * @read: The read function.
 * @write: The write function.
 * @llseek: We use the kernel's default_llseek() function; this allows 
 *          users to change what position they are writing/reading to/from.
 */
static const struct file_operations  combFilterProcessor_0_fops = {
	.owner = THIS_MODULE,
	.read = combFilterProcessor_0_read,
	.write = combFilterProcessor_0_write,
	.llseek = default_llseek,
};


/*-----------------------------------------------------------------------*/
/* Platform Driver Probe (Initialization) Function                       */
/*-----------------------------------------------------------------------*/
/*
 * combFilterProcessor_0_probe() - Initialize device when a match is found
 * @pdev: Platform device structure associated with our 
 *        combFilterProcessor_0 device; pdev is automatically created by the 
 *        driver core based upon our combFilterProcessor_0 device tree node.
 *
 * When a device that is compatible with this combFilterProcessor_0 driver 
 * is found, the driver's probe function is called. This probe function 
 * gets called by the kernel when an combFilterProcessor_0 device is found 
 * in the device tree.
 */
static int combFilterProcessor_0_probe(struct platform_device *pdev)
{
	struct combFilterProcessor_0_dev *priv;
	int ret;

	/*
	 * Allocate kernel memory for the combFilterProcessor_0 device and set it to 0.
	 * GFP_KERNEL specifies that we are allocating normal kernel RAM;
	 * see the kmalloc documentation for more info. The allocated memory
	 * is automatically freed when the device is removed.
	 */
	priv = devm_kzalloc(&pdev->dev, sizeof(struct combFilterProcessor_0_dev), GFP_KERNEL);
	if (!priv) {
		pr_err("Failed to allocate kernel memory for hps_led_pattern\n");
		return -ENOMEM;
	}

	/*
	 * Request and remap the device's memory region. Requesting the region
	 * make sure nobody else can use that memory. The memory is remapped
	 * into the kernel's virtual address space becuase we don't have access
	 * to physical memory locations.
	 */
	priv->base_addr = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(priv->base_addr)) {
		pr_err("Failed to request/remap platform device resource (combFilterProcessor_0)\n");
		return PTR_ERR(priv->base_addr);
	}

	// Initialize the misc device parameters
	priv->miscdev.minor = MISC_DYNAMIC_MINOR;
	priv->miscdev.name = "combFilterProcessor_0";
	priv->miscdev.fops = &combFilterProcessor_0_fops;
	priv->miscdev.parent = &pdev->dev;
	priv->miscdev.groups = combFilterProcessor_0_groups;

	// Register the misc device; this creates a char dev at 
    // /dev/combFilterProcessor_0
	ret = misc_register(&priv->miscdev);
	if (ret) {
		pr_err("Failed to register misc device for combFilterProcessor_0\n");
		return ret;
	}

	// Attach the combFilterProcessor_0' private data to the 
    // platform device's struct.
	platform_set_drvdata(pdev, priv);

	pr_info("combFilterProcessor_0_probe successful\n");

	return 0;
}

/*-----------------------------------------------------------------------*/
/* Platform Driver Remove Function                                       */
/*-----------------------------------------------------------------------*/
/*
 * combFilterProcessor_0_remove() - Remove an combFilterProcessor_0 device.
 * @pdev: Platform device structure associated with our combFilterProcessor_0 device.
 *
 * This function is called when an combFilterProcessor_0 devicee is removed or
 * the driver is removed.
 */
static int combFilterProcessor_0_remove(struct platform_device *pdev)
{
	// Get thecombFilterProcessor_0' private data from the platform device.
	struct combFilterProcessor_0_dev *priv = platform_get_drvdata(pdev);

	// Deregister the misc device and remove the /dev/combFilterProcessor_0 file.
	misc_deregister(&priv->miscdev);

	pr_info("combFilterProcessor_0_remove successful\n");

	return 0;
}

/*-----------------------------------------------------------------------*/
/* Compatible Match String                                               */
/*-----------------------------------------------------------------------*/
/*
 * Define the compatible property used for matching devices to this driver,
 * then add our device id structure to the kernel's device table. For a 
 * device to be matched with this driver, its device tree node must use the 
 * same compatible string as defined here.
 */
static const struct of_device_id combFilterProcessor_0_of_match[] = {
    // ****Note:**** This .compatible string must be identical to the 
    // .compatible string in the Device Tree Node for combFilterProcessor_0
	{ .compatible = "zab,combFilterProcessor_0", },  
	{ }
};
MODULE_DEVICE_TABLE(of, combFilterProcessor_0_of_match);

/*-----------------------------------------------------------------------*/
/* Platform Driver Structure                                             */
/*-----------------------------------------------------------------------*/
/*
 * struct combFilterProcessor_0_driver - Platform driver struct for the 
 *                                  combFilterProcessor_0 driver
 * @probe: Function that's called when a device is found
 * @remove: Function that's called when a device is removed
 * @driver.owner: Which module owns this driver
 * @driver.name: Name of the combFilterProcessor_0 driver
 * @driver.of_match_table: Device tree match table
 * @driver.dev_groups: combFilterProcessor_0 sysfs attribute group; this
 *                     allows the driver core to create the
 *                     attribute(s) without race conditions.
 */
static struct platform_driver combFilterProcessor_0_driver = {
	.probe = combFilterProcessor_0_probe,
	.remove = combFilterProcessor_0_remove,
	.driver = {
		.owner = THIS_MODULE,
		.name = "combFilterProcessor_0",
		.of_match_table = combFilterProcessor_0_of_match,
		.dev_groups = combFilterProcessor_0_groups,
	},
};

/*
 * We don't need to do anything special in module init/exit.
 * This macro automatically handles module init/exit.
 */
module_platform_driver(combFilterProcessor_0_driver);

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Zackery Backman");  // Adapted from Trevor Vannoy's Echo Driver
MODULE_DESCRIPTION("combFilterProcessor_0 driver");
MODULE_VERSION("1.0");
