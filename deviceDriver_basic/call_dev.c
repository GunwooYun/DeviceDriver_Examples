/*
	Device Driver basic
	file : call_dev.c
	module name : calldev
	Page: 170
*/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fcntl.h>



#define CALL_DEV_NAME "calldev" // Loaded module name
#define CALL_DEV_MAJOR 240 // Major num

static int call_open(struct inode *inode, struct file *filp)
{
	/* MINOR, MAJOR <- macro function, masking bit */
	int num = MINOR(inode->i_rdev);
	printk("call open -> minor : %d\n", num);
	num = MAJOR(inode->i_rdev);
	printk("call open -> major : %d\n", num);
	return 0;
}

/* Force to move file pointer of device driver */
static loff_t call_llseek(struct file *filp, loff_t off, int whence)
{
	printk("call llseek -> off : %08X, whence : %08X\n", (unsigned int)off, whence);
	return 0x23;
}

/* Read to buffer from Device Driver as many as count */
/* Return number of read data */
static ssize_t call_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
	printk("call read -> buf : %08X, count : %08X\n", (unsigned int)buf, count);
	return 0x33;
}

/* Write data from buffer to Device Driver as many as count */
/* Return number of written data */
static ssize_t call_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
	printk("call write -> buf : %08X, count : %08X\n", (unsigned int)buf, count);
	return 0x43;
}

/* Call function with cmd, swtich() case */
/*           ret = ioctl(int fd, int requeset, char *argp)  */
/*                                   |              |       */
                      /* unsgined int cmd, unsigned long arg */
static long call_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	printk("call ioctl -> cmd : %08X, arg : %08X\n", cmd, (unsigned int)arg);
	return 0x53;
}

/* Call this function when close file in app */
static int call_release(struct inode *inode, struct file *filp)
{
	printk("call release \n");
	return 0;
}

struct file_operations call_fops =
{
	/* function pointer : mapping between 'low level open, close' and 'system call func' */
	.owner = THIS_MODULE,
	.llseek = call_llseek,
	.read = call_read,
	.write = call_write,
	.unlocked_ioctl = call_ioctl,
	.open = call_open,
	.release = call_release,
};

static int call_init(void)
{
	int result;
	printk("call call_init \n");
	/* Register file_operation struct */
	result = register_chrdev(CALL_DEV_MAJOR, CALL_DEV_NAME, &call_fops); // Register file_operation to character devices[MAX_PROBE_HASH] with MAJOR num(index) & name
	if(result < 0) return result;
	return 0;
}

static void call_exit(void)
{
	printk("call call_exit \n");
	unregister_chrdev(CALL_DEV_MAJOR, CALL_DEV_NAME); // Unregister file_operation
}

module_init(call_init);
module_exit(call_exit);
MODULE_LICENSE("Dual BSD/GPL");
