/*
 *  Hello World example for a simple linux kernel module.
 * 
 *  Install module:
 *  # sudo insmod hello.ko
 *
 *  Uninstall module:
 *  # sudo rmmod hello.ko
 *
 *  View log:
 *  # dmesg
 *
 *  Tested on Ubuntu 12.04
 */

#include <linux/module.h>
#include <linux/kernel.h>

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
