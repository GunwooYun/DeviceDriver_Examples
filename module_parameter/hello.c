/*
   Module Paramter / p.119
*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h> // To use module parameter

static int onevalue = 1; // Init 1
static char * twostring = NULL; // Init NULL

/* module_param(variable, data type, access attribute) */
module_param(onevalue, int , 0);
module_param(twostring ,charp, 0);

static int hello_init(void)
{
	printk("Hello, world [onevalue=%d:twostring=%s]\n",onevalue,twostring); // Kernel message
	return 0;
}
static void hello_exit(void)
{
	printk("Goodbye, world\n");
}
module_init(hello_init); // insmod
module_exit(hello_exit); // rmmod

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("GunwooYun");
MODULE_DESCRIPTION("module hello");
