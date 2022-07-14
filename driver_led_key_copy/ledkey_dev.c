#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/fs.h>          
#include <linux/errno.h>       
#include <linux/types.h>       
#include <linux/fcntl.h>       

#include <linux/gpio.h>

#include <asm/uaccess.h> // put_user(), get_user()

#define   CALL_DEV_NAME            "ledkey"
#define   CALL_DEV_MAJOR            240      

#define DEBUG 0
#define IMX_GPIO_NR(bank, nr)       (((bank) - 1) * 32 + (nr))

static int key[] = {
	IMX_GPIO_NR(1, 20), // SW1
	IMX_GPIO_NR(1, 21),
	IMX_GPIO_NR(4, 8),
	IMX_GPIO_NR(4, 9),
  	IMX_GPIO_NR(4, 5),
  	IMX_GPIO_NR(7, 13),
  	IMX_GPIO_NR(1, 7),
 	IMX_GPIO_NR(1, 8), // SW8
};

static int led[] = {
	IMX_GPIO_NR(1, 16),   //16
	IMX_GPIO_NR(1, 17),	  //17
	IMX_GPIO_NR(1, 18),   //18
	IMX_GPIO_NR(1, 19),   //19
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

void led_write(unsigned long data)
{
	int i;
	for(i = 0; i < ARRAY_SIZE(led); i++){
		gpio_direction_output(led[i], (data >> i ) & 0x01);
//		gpio_set_value(led[i], (data >> i ) & 0x01);
	}
#if DEBUG
	printk("#### %s, data = %ld\n", __FUNCTION__, data);
#endif
}
//void led_read(unsigned long * led_data)
void key_read(char * key_data)
{
	int i, j = 0;
	unsigned long data=0;
	unsigned long temp;
	*key_data = 0;
	for(i=0;i<8;i++)
	{
  		gpio_direction_input(key[i]); //error led all turn off
		//if(gpio_get_value(key[i]))
	//		j = i;
		temp = gpio_get_value(key[i]) << i;
		data |= temp;
		
	}
	
	for(i=0;i<8;i++){
		if((data >> i ) & 0x01)
			*key_data = i+1;
	}
			
/*	
	for(i=3;i>=0;i--)
	{
  		gpio_direction_input(led[i]); //error led all turn off
		temp = gpio_get_value(led[i]);
		data |= temp;
		if(i==0)
			break;
		data <<= 1;  //data <<= 1;
	}
*/
#if DEBUG
	printk("#### %s, data = %ld\n", __FUNCTION__, data);
#endif
	//*key_data = (char)data;
	//*key_data = j;
  	///led_write(data);
	return;
}
int call_open (struct inode *inode, struct file *filp)
{
    int num0 = MAJOR(inode->i_rdev); 
    int num1 = MINOR(inode->i_rdev); 
    printk( "call open -> major : %d\n", num0 );
    printk( "call open -> minor : %d\n", num1 );
	led_init();
	key_init();
    return 0;
}

loff_t call_llseek (struct file *filp, loff_t off, int whence )
{
    printk( "call llseek -> off : %08X, whenec : %08X\n", (unsigned int)off, whence );
    return 0x23;
}

ssize_t call_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
    //printk( "call read -> buf : %08X, count : %08X \n", (unsigned int)buf, count );
	//key_read(buf);
	int ret;
	char kbuf;
	key_read(&kbuf);
	//put_user(kbuf, buf); // put_user(value, ptr)
	ret = copy_to_user(buf, &kbuf, count);

	/* If need to write array */
	/*
	   char kbuf[10] = {0};
	   for(int i = 0; i < 10; i++)
			put_user(kbuf[i], buf++);
	*/
    return count;
}

ssize_t call_write (struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
    //printk( "call write -> buf : %08X, count : %08X \n", (unsigned int)buf, count );

	char kbuf;
	int ret;
	//get_user(kbuf, buf); // Read data from user level first
	ret = copy_from_user(&kbuf, buf, count);
	led_write(kbuf); // Write data to kernel
    return count;
}

//int call_ioctl (struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
static long call_ioctl (struct file *filp, unsigned int cmd, unsigned long arg)
{

    printk( "call ioctl -> cmd : %08X, arg : %08X \n", cmd, (unsigned int)arg );
    return 0x53;
}

int call_release (struct inode *inode, struct file *filp)
{
    printk( "call release \n" );
	led_exit();
	key_exit();
    return 0;
}

struct file_operations call_fops =
{
    .owner    = THIS_MODULE,
    .llseek   = call_llseek,   
    .read     = call_read,     
    .write    = call_write,    
//    .ioctl    = call_ioctl,    
	.unlocked_ioctl = call_ioctl,
    .open     = call_open,     
    .release  = call_release,  
};

int call_init(void)
{
    int result;

    printk( "call call_init \n" );    

    result = register_chrdev( CALL_DEV_MAJOR, CALL_DEV_NAME, &call_fops);
    if (result < 0) return result;

    return 0;
}

void call_exit(void)
{
    printk( "call call_exit \n" );    
    unregister_chrdev( CALL_DEV_MAJOR, CALL_DEV_NAME );
}

module_init(call_init);
module_exit(call_exit);

MODULE_LICENSE("Dual BSD/GPL");
