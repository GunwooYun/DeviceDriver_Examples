#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

/* static keyword makes this funciton used only this file */
static int hello_init(void){
	printk("Hello, world \n"); // Kernel message
	return 0;
}

static void hello_exit(void){
	printk("Goodbye, world\n");
}

/* moudle_init, module_exit macro function */
module_init(hello_init); // insmod
module_exit(hello_exit); // rmmod

MODULE_AUTHOR("GunwooYun");
MODULE_DESCRIPTION("Test module");
MODULE_LICENSE("Dual BSD/GPL"); // license should be written
