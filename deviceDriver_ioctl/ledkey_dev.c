/*
    Device Driver using function ioctl
    Use ioctl to control LED and get KEY data of UDOO
    file : ledkey_dev.c
    device driver name : ioctldev
    Page: 294
*/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>

#include <linux/fs.h>          
#include <linux/errno.h>       
#include <linux/types.h>       
#include <linux/fcntl.h>       
#include <linux/gpio.h>
#include "ioctl_test.h"

#define   LEDKEY_DEV_NAME            "ioctldev"
#define   LEDKEY_DEV_MAJOR            240      
#define DEBUG 1
#define IMX_GPIO_NR(bank, nr)       (((bank) - 1) * 32 + (nr))

static int led[] = {
	IMX_GPIO_NR(1, 16),   //16
	IMX_GPIO_NR(1, 17),	  //17
	IMX_GPIO_NR(1, 18),   //18
	IMX_GPIO_NR(1, 19),   //19
};

static int key[] = {
	IMX_GPIO_NR(1, 20),
	IMX_GPIO_NR(1, 21),
	IMX_GPIO_NR(4, 8),
	IMX_GPIO_NR(4, 9),
  	IMX_GPIO_NR(4, 5),
  	IMX_GPIO_NR(7, 13),
  	IMX_GPIO_NR(1, 7),
 	IMX_GPIO_NR(1, 8),
};
static int led_init(void)
{
	int ret = 0;
	int i;

	for (i = 0; i < ARRAY_SIZE(led); i++) {
		ret = gpio_request(led[i], "gpio led");
		if(ret<0){
			printk("#### FAILED Request gpio %d. error : %d \n", led[i], ret);
		} 
		else
			gpio_direction_output(led[i], 0);  //0:led off
	}
	return ret;
}

static int key_init(void)
{
	int ret = 0;
	int i;

	for (i = 0; i < ARRAY_SIZE(key); i++) {
		ret = gpio_request(key[i], "gpio key");
		if(ret<0){
			printk("#### FAILED Request gpio %d. error : %d \n", key[i], ret);
		} 
		else
			gpio_direction_input(key[i]);  
	}
	return ret;
}
static void led_exit(void)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(led); i++){
		gpio_free(led[i]);
	}
}
static void key_exit(void)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(key); i++){
		gpio_free(key[i]);
	}
}

static void led_write(char data)
{
	int i;
	for(i = 0; i < ARRAY_SIZE(led); i++){
		gpio_set_value(led[i], (data >> i ) & 0x01);
	}
#if DEBUG
	printk("#### %s, data = %d\n", __FUNCTION__, data);
#endif
}

static void key_read(char * key_data)
{
	int i;
	char data=0;
//	char temp;
	for(i=0;i<ARRAY_SIZE(key);i++)
	{
		if(gpio_get_value(key[i])) // Check key
		{
			data = i+1;
			break;
		}
//		temp = gpio_get_value(key[i]) << i;
//		data |= temp;
	}
#if DEBUG
	printk("#### %s, data = %d\n", __FUNCTION__, data);
#endif
	*key_data = data;
	return;
}

static int ledkey_open (struct inode *inode, struct file *filp)
{
    int num0 = MAJOR(inode->i_rdev); 
    int num1 = MINOR(inode->i_rdev); 
    printk( "call open -> major : %d\n", num0 );
    printk( "call open -> minor : %d\n", num1 );

    return 0;
}

static ssize_t ledkey_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
	char kbuf;
	int ret;
	key_read(&kbuf);
//	put_user(kbuf,buf);
	ret = copy_to_user(buf,&kbuf,count);
    return count;
}

static ssize_t ledkey_write (struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
	char kbuf;
	int ret;
//	get_user(kbuf,buf);
	ret = copy_from_user(&kbuf,buf,count);
	led_write(kbuf);
    return count;
//	return -EFAULT;
}

/*           ret = ioctl(int fd, int requeset, char *argp)  */
/*                                   |              |       */ 
                      /* unsgined int cmd, unsigned long arg */
static long ledkey_ioctl (struct file *filp, unsigned int cmd, unsigned long arg)
{

	/* 1 : Validate command
	   2 : Validate memory
	*/

	ioctl_test_info ctrl_info = {0,{0}};
	int err, size;
	// _IOC_TYPE() : Read magic number field
	// _IOC_NR() : Read number field
	// EINVAL : invalid argument
	if( _IOC_TYPE( cmd ) != IOCTLTEST_MAGIC ) return -EINVAL; // IOCTLTEST_MAIGC : 't'
	if( _IOC_NR( cmd ) >= IOCTLTEST_MAXNR ) return -EINVAL; // IOCTLTEEST_MAXNR : 4

	/* IOCTLTEST_KEYLEDINIT : cmd = {'t', 0} */ 
	/* IOCTLTEST_LEDON : cmd = {'t', 1} */ 
	/* IOCTLTEST_LEDOFF : cmd = {'t', 2} */ 
	/* IOCTLTEST_KEYLEDFREE : cmd = {'t', 3} */ 

	/* Validate memory */
	// _IOC_SIZE() : Read size of data field
	size = _IOC_SIZE( cmd );
	if( size )
	{
		err = 0;
		/* _IOC_DIR : Read attribute of r/w */
		if( _IOC_DIR( cmd ) & _IOC_READ ) /* if cmd == read ? */
			/* access_ok() : Check the address of arg in user space */
			/* #define access_ok(type,addr,size)   (__range_ok(addr,size) == 0) */
			/* #define __range_ok(addr,size)   ((void)(addr),0) */
			err = access_ok( VERIFY_WRITE, (void *) arg, size ); // VERIFY_WRITE : 1
		else if( _IOC_DIR( cmd ) & _IOC_WRITE ) /* if cmd == write ? */
			err = access_ok( VERIFY_READ , (void *) arg, size ); // VERIFY_READ : 0
		if( !err ) return err;
	}
	switch( cmd )
	{
        case IOCTLTEST_KEYLEDINIT :
            key_init();
            led_init();
            break;
        case IOCTLTEST_KEYLEDFREE :
            key_exit();
            led_exit();
            break;
		case IOCTLTEST_LEDOFF :
			led_write(0);
			break;
		case IOCTLTEST_LEDON :
			led_write(15);
			break;
		case IOCTLTEST_LEDWRITE :
			/* type cast : pointer -> unsgined long -> pointer*/
			/* unknown size, so first argument is casting to (void *) */
			err = copy_from_user((void *)&ctrl_info, (void *)arg, sizeof(ctrl_info));
			led_write(ctrl_info.buff[0]);
			break;

		default:
			break;
	}	
	return 0;
}

static int ledkey_release (struct inode *inode, struct file *filp)
{
    printk( "call release \n" );
    return 0;
}

struct file_operations ledkey_fops =
{
    .owner    = THIS_MODULE,
    .read     = ledkey_read,     
    .write    = ledkey_write,    
	.unlocked_ioctl = ledkey_ioctl,
    .open     = ledkey_open,     
    .release  = ledkey_release,  
};

static int ledkey_init(void)
{
    int result;

    printk( "call ledkey_init \n" );    

    result = register_chrdev( LEDKEY_DEV_MAJOR, LEDKEY_DEV_NAME, &ledkey_fops);
    if (result < 0) return result;

//	led_init();
//	key_init();
    return 0;
}

static void ledkey_exit(void)
{
    printk( "call ledkey_exit \n" );    
    unregister_chrdev( LEDKEY_DEV_MAJOR, LEDKEY_DEV_NAME );
//	led_exit();
//	key_exit();
}

module_init(ledkey_init);
module_exit(ledkey_exit);

MODULE_LICENSE("Dual BSD/GPL");
