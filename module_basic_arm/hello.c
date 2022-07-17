#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static int hello_init(void){
	printk("Hello, world \n"); // kernel message
	return 0;
}

static void hello_exit(void){
	printk("Goodbye, world\n");
}

/* moudle_init, module_exit macro function */
module_init(hello_init); // execute hello_init() as soon as insmod
module_exit(hello_exit); // execute hello_exit() as soon as rmmod

MODULE_AUTHOR("GunwooYun");
MODULE_DESCRIPTION("Test module");
MODULE_LICENSE("Dual BSD/GPL");
