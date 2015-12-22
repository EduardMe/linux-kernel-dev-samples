/*
 *  Hello World example for a simple linux kernel module.
 * 
 *  Install module as root:  
 *  # insmod char-device.ko
 *
 *  Check dmesg for the next instruction
 *  # dmesg
 *
 *  Create device file as root (path, char device identifier, major number, minor number).
 #  You find the correct major number in the instructions from dmesg obtained in the last step.
 *  The following step is just an example, make sure you check `dmesg`.
 *  # mknod /dev/mychardevice c 900 1
 *
 *  Uninstall module as root:
 *  # rmmod hello.ko
 *
 *  View log:
 *  # dmesg
 *
 *  Tested on Ubuntu 12.04
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/semaphore.h>
#include <asm/uaccess.h>

#define DEVICE_NAME "mychardevice"

// Stores data for the driver
struct my_device {
	char data[100];
	struct semaphore sem;
} virtual_device;

struct cdev *mcdev;
int major_num;
int ret;

dev_t dev_num;

/*
 * Called when the device is opened (/dev/mychardevice)
 * inode = reference to file
 * filep = abstract open file containing file operations structure (fops)
 */
int device_open(struct inode* inode, struct file* filep)
{
	// Lock the file with a mutual exclusive lock mutex
	if(down_interruptible(&virtual_device.sem) != 0)
	{
		printk(KERN_ALERT "mychardevice: could not lock the device while opening\n");
		return -1;
	}

	printk(KERN_ALERT "mychardevice: device opened\n");
	return 0;
}

/*
 * Called when user is reading from the device to get information.
 * Copy data from kernel (device) to user space (process).
 */
ssize_t device_read(struct file* filep, char* data, size_t len, loff_t* offset)
{
	printk(KERN_ALERT "mychardevice: Reading from device.\n");
	ret = copy_to_user(data, virtual_device.data, len); // destination, source, length

	return ret;
}

/*
 * Called when user is writing information to the device.
 * Copy data from user (process) to kernel space (device).
 */
ssize_t device_write(struct file* filep, const char* data, size_t len, loff_t* offset)
{
	printk(KERN_ALERT "mychardevice: Writing to device.\n");
	ret = copy_from_user(virtual_device.data, data, len); // destination, source, length

	return ret;
}

/*
 * Called when the device is closed (/dev/mychardevice)
 * inode = reference to file
 * filep = abstract open file containing file operations structure (fops)
 */
int device_close(struct inode* inode, struct file* filep)
{
	// Release lock from device
	up(&virtual_device.sem);
	printk(KERN_ALERT "mychardevice: Close device.\n");

	return 0;
}

// Map file operation on the device file to functions
struct file_operations fops = {
	.owner 		= THIS_MODULE,
	.open 		= device_open,
	.release 	= device_close,
	.write 		= device_write,
	.read 		= device_read
};

// Allocate and initialize the driver
static int driver_init(void) 
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

module_init(driver_init);
module_exit(driver_exit);