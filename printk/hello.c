#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static int hello_init(void){
	printk(KERN_INFO "Hello, world \n");
	printk(KERN_DEBUG "Hello, world \n");
	return 0;
}

static void hello_exit(void){
	printk("Goodbye, world\n");
}

/* moudle_init, module_exit macro function */
module_init(hello_init);
module_exit(hello_exit);

MODULE_AUTHOR("GunwooYun");
MODULE_DESCRIPTION("Test module");
MODULE_LICENSE("Dual BSD/GPL");