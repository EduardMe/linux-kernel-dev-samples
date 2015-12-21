/*
 *  Hello World example for a simple linux kernel module.
 * 
 *  Create device file (path, char device identifier, major number, minor number):
 *  # sudo mknod /dev/mychardevice c 900 1
 *
 *  Uninstall module:
 *  # rmmod hello.ko
 *
 *  View log:
 *  # dmesg
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/semaphore.h>
#include <asm/uaccess.h>

#define DEVICE_NAME "mychardevice"

struct my_device {
	char data[100];
	struct semaphore sem;
} virtual_device;

struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = device_open,
	.release = device_close,
	.write = device_write,
	.read = device_read
};

struct cdev *mcdev;
int major_num;
int ret;

dev_t dev_num;

int device_open(struct inode *inode, struct file *filep)
{
	if(down_interruptable(&virtual_device.sem) != 0)
	{
		printk(KERN_ALERT "mychardevice: could not lock the device while openin\n");
		return -1;
	}

	printk(KERN_ALERT "mychardevice: device opened\n");
	return 0;
}

// Allocate and initialize the driver
static int driver_entry(void) 
{
	ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME); // dev_t*, minor, count, name
	
	if(ret < 0)
	{
		printk(KERN_ALERT "mychardevice: failed to allocate a major number\n");
		return ret;
	}

	major_num = MAJOR(dev_num);

	printk("mychardevice: major number is: %d\n", major_num);
	printk("\t run \"mknod /dev/%s c %d 0\" for device file\n", DEVICE_NAME, major_num);

	mcdev = cdev_alloc();
	mcdev->ops = &fops;
	mcdev->owner = THIS_MODULE;

	// add cdev to the kernel
	ret = cdev_add(mcdev, dev_num, 1);

	if(ret < 0)
	{
		printk(KERN_ALERT "mychardevice: unable to add cdev to kernel\n");
		return ret;
	}

	sema_init(&virtual_device.sem, 1); // lock the device

	return 0;
}

// Deallocate and clean up the driver
static void driver_exit(void) 
{
	cdev_del(mcdev); // delete the device
	unregister_chrdev_region(dev_num, 1);
	printk(KERN_ALERT "mychardevice: driver is unloaded\n");
}

module_init(hello_init);
module_exit(hello_exit);