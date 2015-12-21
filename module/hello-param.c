/*
 *  Hello World example for a simple linux kernel module with parameters.
 * 
 *  Install module:
 *  # insmod hello-param.ko param_var=42 param_array=40,41,42
 *
 *  Uninstall module:
 *  # rmmod hello-param.ko
 *
 *  View log:
 *  # dmesg
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

 int param_var = 0;
 int param_array[3] = { 0, 0, 0 };

 // Register variable with read and write permissions.
 // S_IRUSR = read, user
 // S_IWUSR = write, user
 // S_IXUSR = execute, user
 // S_IRGRP = read, group
 // S_IWGRP = write, group
 module_param(param_var, int, S_IRUSR | S_IWUSR);

 // Register an array
 module_param_array(param_array, int, NULL, S_IRUSR | S_IWUSR);

void display(void)
{
	printk(KERN_ALERT "param variable = %d\n", param_var);
	printk(KERN_ALERT "param array[0] = %d\n", param_array[0]);
	printk(KERN_ALERT "param array[1] = %d\n", param_array[1]);
	printk(KERN_ALERT "param array[2] = %d\n", param_array[2]);
}

// Allocate and initialize the module
static int hello_init(void) 
{
	printk(KERN_ALERT "Hello World!\n");
	display();
	return 0;
}

// Deallocate and clean up the module
static void hello_exit(void) 
{
	printk(KERN_ALERT "Good byte world!\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
