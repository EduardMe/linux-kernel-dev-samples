/*
 *  Hello World example for a simple linux kernel module.
 * 
 *  Install module:
 *  # insmod hello.ko
 *
 *  Uninstall module:
 *  # rmmod hello.ko
 *
 *  View log:
 *  # dmesg
 */


#include <linux/module.h>
#include <linux/kernel.h>

#define DRIVER_AUTHOR "Eduard Metzger"
#define DRIVER_DESC   "A sample driver"

// Allocate and initialize the module
static int __init hello_init(void) 
{
	printk(KERN_ALERT "Hello World!\n");
	return 0;
}

// Deallocate and clean up the module
static void __exit hello_exit(void) 
{
	printk(KERN_ALERT "Good byte world!\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);	
MODULE_DESCRIPTION(DRIVER_DESC);
