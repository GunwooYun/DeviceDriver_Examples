#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <linux/ioport.h>
#include <linux/gpio.h>
#include <linux/moduleparam.h>
#define TIME_STEP	timeval  //KERNEL HZ=100
#define DEBUG 1
#define IMX_GPIO_NR(bank, nr)       (((bank) - 1) * 32 + (nr))

#define   CALL_DEV_NAME            "kerneltimer"
#define   CALL_DEV_MAJOR            240


static int timeval = 0;	//f=100HZ, T=1/100 = 10ms, 100*10ms = 1Sec
module_param(timeval,int ,0);
static int ledval = 0;
module_param(ledval,int ,0);
typedef struct
{
	struct timer_list timer;
	unsigned long 	  key;
	unsigned long 	  led;
} __attribute__ ((packed)) KERNEL_TIMER_MANAGER;

void kerneltimer_timeover(unsigned long arg);

int key[] = {
    IMX_GPIO_NR(1, 20),
    IMX_GPIO_NR(1, 21),
    IMX_GPIO_NR(4, 8),
    IMX_GPIO_NR(4, 9),
    IMX_GPIO_NR(4, 5),
    IMX_GPIO_NR(7, 13),
    IMX_GPIO_NR(1, 7),
    IMX_GPIO_NR(1, 8),
};

int led[] = {
    IMX_GPIO_NR(1, 16),
    IMX_GPIO_NR(1, 17),
    IMX_GPIO_NR(1, 18),
    IMX_GPIO_NR(1, 19),
};

static int led_init(void)
{
	int ret = 0;
	int i;

	for (i = 0; i < ARRAY_SIZE(led); i++) {
		ret = gpio_request(led[i], "gpio led");
		if(ret){
			printk("#### FAILED Request gpio %d. error : %d \n", led[i], ret);
		} 
		else {
			gpio_direction_output(led[i], 0);
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
	printk("key_init()\n");
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

void led_write(char data)
{
    int i;
    for(i = 0; i < ARRAY_SIZE(led); i++){
        gpio_set_value(led[i], ((data >> i) & 0x01));
#if 0
        printk("#### %s, data = %d(%d)\n", __FUNCTION__, data,(data>>i));
#endif
    }
}

void kerneltimer_registertimer(KERNEL_TIMER_MANAGER *pdata, unsigned long timeover)
{
	init_timer( &(pdata->timer) );
	pdata->timer.expires = get_jiffies_64() + timeover;  //10ms *100 = 1sec ( 1 * HZ * 100)
	pdata->timer.data	 = (unsigned long)pdata ;
	pdata->timer.function = kerneltimer_timeover; // handler function
	add_timer( &(pdata->timer) );
}

void kerneltimer_timeover(unsigned long arg)
{
	KERNEL_TIMER_MANAGER* pdata = NULL;
	if( arg )
	{
		pdata = ( KERNEL_TIMER_MANAGER *)arg;
		led_write(pdata->led & 0x0f);
#if DEBUG
		printk("led : %#04x, key : %#04x \n",(unsigned int)(pdata->led & 0x0000000f),(unsigned int)pdata->key);
#endif
		pdata->led = ~(pdata->led);
		kerneltimer_registertimer( pdata, TIME_STEP);
		printk("registertimer func end \n");
	}
}


static void key_read(char * key_data)
{
    int i;
    char data=0;
    for(i=0;i<ARRAY_SIZE(key);i++)
    {
        if(gpio_get_value(key[i]))
        {
            data = i+1;
            break;
        }
    }
#if 0
    printk("#### %s, data = %d\n", __FUNCTION__, data);
#endif
    *key_data = data;
    return;
}

int call_open (struct inode *inode, struct file *filp)
{
	KERNEL_TIMER_MANAGER* ptrmng = NULL;
	//int ret;
	//ret = kerneltimer_init();
	//if(ret < 0) return -1;

	//ptrmng = (KERNEL_TIMER_MANAGER *)kmalloc( sizeof(KERNEL_TIMER_MANAGER ), GFP_KERNEL); //32byte
	/* private_data <-- type : void * */
	ptrmng = (KERNEL_TIMER_MANAGER *)kmalloc( sizeof(KERNEL_TIMER_MANAGER ), GFP_KERNEL); //32byte
	if(ptrmng == NULL) return -ENOMEM;
	memset( ptrmng, 0, sizeof( KERNEL_TIMER_MANAGER));
	ptrmng->led = ledval;
	kerneltimer_registertimer( ptrmng, TIME_STEP);
	filp->private_data = ptrmng;
    return 0;
}

ssize_t call_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
	char kbuf;
	int ret = 0;
	KERNEL_TIMER_MANAGER* ptrmng = NULL;
	ptrmng = filp->private_data;
    key_read(&kbuf);
	if(kbuf){
	ptrmng->key = kbuf;
	ret = copy_to_user(buf, &kbuf, count);
	}
    return ret;
}

ssize_t call_write (struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
	KERNEL_TIMER_MANAGER* ptrmng = (KERNEL_TIMER_MANAGER *)filp->private_data;
	ptrmng->led = *buf;
    return count;
}

int call_release (struct inode *inode, struct file *filp)
{
	KERNEL_TIMER_MANAGER* ptrmng = (KERNEL_TIMER_MANAGER *)filp->private_data;
	/* Check timer exists */
	if(timer_pending(&(ptrmng->timer)))
		del_timer(&(ptrmng->timer));
	if(ptrmng != NULL)
	{
		kfree(ptrmng);
	}
	led_write(0);
    return 0;
}

struct file_operations call_fops =
{
    .owner    = THIS_MODULE,
    .read     = call_read,
    .write    = call_write,
    .open     = call_open,
    .release  = call_release,
};

int kerneltimer_init(void)
{
	int result;
	led_init();
	key_init();
    result = register_chrdev( CALL_DEV_MAJOR, CALL_DEV_NAME, &call_fops);
	if(result < 0) return -1;

	//printk("timeval : %d , sec : %d size : %d\n",timeval,timeval/HZ,sizeof(KERNEL_TIMER_MANAGER) );

	return 0;
}

void kerneltimer_exit(void)
{
	led_exit();
	key_exit();
    unregister_chrdev( CALL_DEV_MAJOR, CALL_DEV_NAME );
}


module_init(kerneltimer_init);
module_exit(kerneltimer_exit);
MODULE_LICENSE("Dual BSD/GPL");
