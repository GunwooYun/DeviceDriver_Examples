/*
   device driver proc file system
   gpio conrol with proc file system
   page : 517
*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/fs.h>          
#include <linux/errno.h>       
#include <linux/types.h>       
#include <linux/fcntl.h>       

#include <linux/gpio.h>

#include <linux/interrupt.h>
#include <linux/irq.h>

#include <linux/sched.h>
#include <linux/wait.h>

#include <linux/poll.h>

#include <asm/uaccess.h> // put_user(), get_user()

#include <linux/proc_fs.h>

#include "ioctl_test.h"

#define   CALL_DEV_NAME            "ledkey_proc"
#define   CALL_DEV_MAJOR            240      

#define DEBUG 0
#define IMX_GPIO_NR(bank, nr)       (((bank) - 1) * 32 + (nr))

DECLARE_WAIT_QUEUE_HEAD(WaitQueue_Read); // Make wait queue

static struct proc_dir_entry *keyledproc_root_fp   = NULL;
static struct proc_dir_entry *keyledproc_led_fp    = NULL;
static struct proc_dir_entry *keyledproc_key_fp    = NULL;

static int sw_irq[8] = {0}; // IRQ number
static long sw_no = 0;

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

/*
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
*/

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
	/*
	sw_irq[0] = gpio_to_irq(IMX_GPIO_NR(1, 20));
	sw_irq[1] = gpio_to_irq(IMX_GPIO_NR(1, 21));
	sw_irq[2] = gpio_to_irq(IMX_GPIO_NR(4, 8));
	sw_irq[3] = gpio_to_irq(IMX_GPIO_NR(4, 9));
	sw_irq[4] = gpio_to_irq(IMX_GPIO_NR(4, 5));
	sw_irq[5] = gpio_to_irq(IMX_GPIO_NR(7, 13));
	sw_irq[6] = gpio_to_irq(IMX_GPIO_NR(1, 7));
	sw_irq[7] = gpio_to_irq(IMX_GPIO_NR(1, 8));
	*/

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

/*
static void key_exit(void)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(key); i++){
		gpio_free(key[i]);
	}
}
*/

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

#if 0
//void led_read(unsigned long * led_data)
void key_read(char * key_data)
{
	int i;
	//unsigned long data=0;
	//unsigned long temp;
	*key_data = 0;
	for(i=0;i<8;i++)
	{
  		gpio_direction_input(key[i]); //error led all turn off
		if(gpio_get_value(key[i])){
			*key_data = i + 1;
			break;
		}

		//temp = gpio_get_value(key[i]) << i;
		//data |= temp;
		
	}
	
	/*
	for(i=0;i<8;i++){
		if((data >> i ) & 0x01)
			*key_data = i+1;
	}
	*/
			
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
#endif

int call_open (struct inode *inode, struct file *filp)
{
    int num0 = MAJOR(inode->i_rdev); 
    int num1 = MINOR(inode->i_rdev); 
    printk( "call open -> major : %d\n", num0 );
    printk( "call open -> minor : %d\n", num1 );
    return 0;
}

loff_t call_llseek (struct file *filp, loff_t off, int whence )
{
    printk( "call llseek -> off : %08X, whenec : %08X\n", (unsigned int)off, whence );
    return 0x23;
}

ssize_t call_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
	int ret;
	char kbuf;

	if(!(filp->f_flags & O_NONBLOCK)){ // Blocking mode
		if(!sw_no){
			/* Go to sleep */
			interruptible_sleep_on(&WaitQueue_Read); // if sw_no == 0 then sleep

			//curTime = get_jiffies_64();
			//diffTime = curTime - preTime;
			//preTime = curTime;
		//	printk("### interval : %d \n", diffTime);
			//wait_event_interruptible(WaitQueue_Read, sw_no); // If sw_no == 0 then sleep (No need if)
			//wait_event_interruptible_timeout(WaitQueue_Read, sw_no, 100); // 10 * 1 / HZ = 0.1sec

			//if(!sw_no) return 0;
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

	char kbuf;
	int ret;
	//get_user(kbuf, buf); // Read data from user level first
	ret = copy_from_user(&kbuf, buf, count);
	led_write(kbuf); // Write data to kernel
    return count;
}

ssize_t proc_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
	int ret, len = 0;
	char kbuf[10];


	//kbuf = (char)sw_no;

	kbuf[0] = sw_no + 0x30;
	kbuf[1] = '\n';
	kbuf[2] = 0;
	len = strlen(kbuf);
	
	ret = copy_to_user(buf, kbuf, count);
	*f_pos = -1;
	sw_no = 0;
	return len;
}

ssize_t proc_write (struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
    //printk( "call write -> buf : %08X, count : %08X \n", (unsigned int)buf, count );

	char kbuf[10];
	int ret;
	//get_user(kbuf, buf); // Read data from user level first
	printk("count %d %s\n", count, buf);
	ret = copy_from_user(kbuf, buf, count);
	led_write(simple_strtol(kbuf, NULL, 10)); // Write data to kernel
    return count;
}

//int call_ioctl (struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
static long call_ioctl (struct file *filp, unsigned int cmd, unsigned long arg)
{

	 int err, size;
	 ioctl_test_info ctrl_info = {0,0,{0}};

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
        case IOCTLTEST_INTVAL :
			//ctrl_info.time = diffTime;
			err = copy_to_user((void *)arg, (void *)&ctrl_info, sizeof(ctrl_info));
			if(err < 0)
				return -1;
            break;
        default:
            break;
    }
    return 0;
    //printk( "call ioctl -> cmd : %08X, arg : %08X \n", cmd, (unsigned int)arg );
    //return 0x53;
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
    //printk( "call release \n" );
	return 0;

}

struct file_operations call_fops =
{
    .owner    = THIS_MODULE,
    .llseek   = call_llseek,   
    .read     = call_read,     
    .write    =	call_write,    
	.unlocked_ioctl = call_ioctl,
	.poll	  =	ledkey_poll,
    .open     = call_open,     
    .release  = call_release,  
};

/* led file_operation */
struct file_operations proc_led_fops =
{
    .owner    = THIS_MODULE,
    .write    =	proc_write,    
};

/* key file_operation */
struct file_operations proc_key_fops =
{
    .owner    = THIS_MODULE,
    .read     = proc_read,     
};

static void mkproc(void)
{
	/* Make directory */
	keyledproc_root_fp  = proc_mkdir( "ledkey", 0 ); // /proc/ledkey 0 : root

	/* Create file with options */
	keyledproc_led_fp   = proc_create_data( "led", S_IFREG | S_IRWXU, keyledproc_root_fp, &proc_led_fops, NULL );
	keyledproc_key_fp   = proc_create_data( "key", S_IFREG | S_IRWXU, keyledproc_root_fp, &proc_key_fops, NULL );
	/*
	if( keyledproc_ledkey_fp )
	{
		keyledproc_ledkey_fp->data       = &proc_led_no;
		keyledproc_ledkey_fp->read_proc  = read_ledproc_val;
		keyledproc_ledkey_fp->write_proc = write_ledproc_val;
	}
	*/
}
static void rmproc(void)
{
	remove_proc_entry( "led"  , keyledproc_root_fp );
	remove_proc_entry( "key"  , keyledproc_root_fp );
	remove_proc_entry( "ledkey" , 0 );
}

int call_init(void)
{
    int result;

    printk( "call call_init \n" );    

    result = register_chrdev( CALL_DEV_MAJOR, CALL_DEV_NAME, &call_fops);
    if (result < 0) return result;

	led_init();
	//key_init();
	key_irq_init(); // key interrupt initialize

	mkproc();

    return 0;
}

void call_exit(void)
{
    printk( "call call_exit \n" );    
    unregister_chrdev( CALL_DEV_MAJOR, CALL_DEV_NAME );
	led_exit();
	//key_exit();
	key_irq_exit();
	rmproc();
}

module_init(call_init);
module_exit(call_exit);

MODULE_LICENSE("Dual BSD/GPL");
