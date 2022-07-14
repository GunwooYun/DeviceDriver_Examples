#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/fs.h>          
#include <linux/errno.h>       
#include <linux/types.h>       
#include <linux/fcntl.h>       
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <linux/ioport.h>

#include <linux/gpio.h>

#include <linux/interrupt.h>
#include <linux/irq.h>


#include <linux/sched.h>
#include <linux/wait.h>

#include <linux/poll.h>

#include <asm/uaccess.h> // put_user(), get_user()

#include "ioctl_test.h"

#define   CALL_DEV_NAME            "ledkey_ncs"
#define   CALL_DEV_MAJOR            240      

#define DEBUG 0
#define IMX_GPIO_NR(bank, nr)       (((bank) - 1) * 32 + (nr))

//#define TIME_STEP timeval

DECLARE_WAIT_QUEUE_HEAD(WaitQueue_Read); // Make wait queue

typedef struct
{
    struct			timer_list timer;
    unsigned long	led;
	int				time_val;
} __attribute__ ((packed)) KERNEL_TIMER_MANAGER;

static int sw_irq[8] = {0}; // IRQ number
static long sw_no = 0;

//static char timeval = 0;

void kerneltimer_timeover(unsigned long arg);

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


/* Interrupt service function */
irqreturn_t sw_isr(int irq, void * unused)
{
	int i;
	/* Check which switch pushed */
	for (i = 0; i < ARRAY_SIZE(key); i++) {
		if(irq == sw_irq[i]){
			sw_no = i + 1;
			break;
		}
	}
	printk("IRQ : %d, %ld \n", irq, sw_no);
	wake_up_interruptible(&WaitQueue_Read);
	return IRQ_HANDLED;
}

static int key_irq_init(void)
{
	int ret;
	int i;
	char* irq_name[8] = {"irq sw1","irq sw2","irq sw3","irq sw4","irq sw5","irq sw6","irq sw7","irq sw8"};

	/* GPIO ports initialize */
	for (i = 0; i < ARRAY_SIZE(key); i++) {
		sw_irq[i] = gpio_to_irq(key[i]); // Convert from gpio number to irq
	}

	for (i = 0; i < ARRAY_SIZE(key); i++) {
		/* Register interrupt service function */
		ret = request_irq(sw_irq[i], sw_isr, IRQF_TRIGGER_RISING, irq_name[i], NULL);
		if(ret){
			//perror("request_irq()");
			printk("### FAILED Request IRQ %d. error : %d \n", sw_irq[i], ret);
			return -1;
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

static void key_irq_exit(void)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(key); i++){
		free_irq(sw_irq[i], NULL);
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

void kerneltimer_registertimer(KERNEL_TIMER_MANAGER *pdata, unsigned long timeover)
{
    init_timer( &(pdata->timer) );
    pdata->timer.expires = get_jiffies_64() + timeover;  //10ms *100 = 1sec ( 1 * HZ * 100)
    pdata->timer.data    = (unsigned long)pdata ;
    pdata->timer.function = kerneltimer_timeover; // handler function
    add_timer( &(pdata->timer) );
}

void kerneltimer_timeover(unsigned long arg)
{
    KERNEL_TIMER_MANAGER* pdata = NULL;
    if( arg )
    {
        pdata = ( KERNEL_TIMER_MANAGER *)arg;
        led_write(pdata->led);
#if DEBUG
        printk("led : %#04x, key : %#04x \n",(unsigned int)(pdata->led & 0x0000000f),(unsigned int)pdata->key);
#endif
        pdata->led = ~(pdata->led);
        kerneltimer_registertimer( pdata, pdata->time_val);
    }
}

int call_open (struct inode *inode, struct file *filp)
{
    int num0 = MAJOR(inode->i_rdev); 
    int num1 = MINOR(inode->i_rdev); 
	KERNEL_TIMER_MANAGER* ptrmng = NULL;
    printk( "call open -> major : %d\n", num0 );
    printk( "call open -> minor : %d\n", num1 );

    //int ret;
    //ret = kerneltimer_init();
    //if(ret < 0) return -1;

    //ptrmng = (KERNEL_TIMER_MANAGER *)kmalloc( sizeof(KERNEL_TIMER_MANAGER ), GFP_KERNEL); //32byte
    /* private_data <-- type : void * */
    ptrmng = (KERNEL_TIMER_MANAGER *)kmalloc( sizeof(KERNEL_TIMER_MANAGER ), GFP_KERNEL); //32byte
    if(ptrmng == NULL) return -ENOMEM;
    memset( ptrmng, 0, sizeof( KERNEL_TIMER_MANAGER));
    //ptrmng->led = ledval;
    //kerneltimer_registertimer( ptrmng, TIME_STEP);
    filp->private_data = ptrmng;
    return 0;
}

ssize_t call_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
	int ret;
	char kbuf;

	if(!(filp->f_flags & O_NONBLOCK)){ // Blocking mode
		if(!sw_no){
			/* Go to sleep */
			interruptible_sleep_on(&WaitQueue_Read); // if sw_no == 0 then sleep
		}
	}
	kbuf = (char)sw_no;
	ret = copy_to_user(buf, &kbuf, count);
	sw_no = 0;
	return count;
}

ssize_t call_write (struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
    //printk( "call write -> buf : %08X, count : %08X \n", (unsigned int)buf, count );
	KERNEL_TIMER_MANAGER* ptrmng = (KERNEL_TIMER_MANAGER *)filp->private_data;

	char kbuf;
	int ret;
	ret = copy_from_user(&kbuf, buf, count);
	ptrmng->led = (unsigned long)kbuf;
    return count;
}

static long call_ioctl (struct file *filp, unsigned int cmd, unsigned long arg)
{
	 int err, size;
	 keyled_data ctrl_info = {0};
	 KERNEL_TIMER_MANAGER* ptrmng =(KERNEL_TIMER_MANAGER *)filp->private_data;

    if( _IOC_TYPE( cmd ) != IOCTLTEST_MAGIC ) return -EINVAL; // IOCTLTEST_MAIGC : 't'
    if( _IOC_NR( cmd ) >= IOCTLTEST_MAXNR ) return -EINVAL; // IOCTLTEEST_MAXNR : 4

    size = _IOC_SIZE( cmd );
    if( size )
    {
        err = 0;
        if( _IOC_DIR( cmd ) & _IOC_READ ) /* if cmd == read ? */
            err = access_ok( VERIFY_WRITE, (void *) arg, size ); // VERIFY_WRITE : 1
        else if( _IOC_DIR( cmd ) & _IOC_WRITE ) /* if cmd == write ? */
            err = access_ok( VERIFY_READ , (void *) arg, size ); // VERIFY_READ : 0
        if( !err ) return err;
		 }
    switch( cmd )
    {
		case TIMER_START :
			if(!(timer_pending(&(ptrmng->timer))))
				kerneltimer_registertimer( ptrmng, ptrmng->time_val);
			break;
		case TIMER_STOP :
			if(timer_pending(&(ptrmng->timer)))
				del_timer(&(ptrmng->timer));
			break;
		case TIMER_VALUE :
			// err = copy_from_user((void *)&ctrl_info, (void *)arg, size);
			err = copy_from_user((void *)&ctrl_info, (void *)arg, sizeof(ctrl_info));
			ptrmng->time_val = (int)ctrl_info.timer_val;
			break;
        default:
            break;
    }
    return 0;
}


static unsigned int ledkey_poll(struct file *filp, struct poll_table_struct *wait)
{
	int mask = 0;
	poll_wait(filp, &WaitQueue_Read, wait); // sleep app:poll(Events, 2, 2000)
	if(sw_no > 0)
		mask = POLLIN;
	return mask;
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
    printk( "call release \n" );
	return 0;

}

struct file_operations call_fops =
{
    .owner    = THIS_MODULE,
    .read     = call_read,     
    .write    =	call_write,    
	.unlocked_ioctl = call_ioctl,
	.poll	  =	ledkey_poll,
    .open     = call_open,     
    .release  = call_release,  
};

int kerneltimer_init(void)
{
    int result;
    led_init(); // led init
    //key_init();
	key_irq_init(); // key interrupt initialize
    result = register_chrdev( CALL_DEV_MAJOR, CALL_DEV_NAME, &call_fops); // fops initialize
    if(result < 0) return -1;

    //printk("timeval : %d , sec : %d size : %d\n",timeval,timeval/HZ,sizeof(KERNEL_TIMER_MANAGER) );

    return 0;
}

void kerneltimer_exit(void)
{
    led_exit();
    //key_exit();
	key_irq_exit();
    unregister_chrdev( CALL_DEV_MAJOR, CALL_DEV_NAME );
}

module_init(kerneltimer_init);
module_exit(kerneltimer_exit);

MODULE_LICENSE("Dual BSD/GPL");
