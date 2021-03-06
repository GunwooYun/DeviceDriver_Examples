/*
	Module Paramter LED example on UDOO
	Usage : ./app 0xf
*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/moduleparam.h>

#define DEBUG 1
/* Macro function of returning port number */
#define IMX_GPIO_NR(bank, nr)       (((bank) - 1) * 32 + (nr))

static unsigned int ledVal = 0; // value of led port of UDOO
static char * twostring = NULL;

module_param(ledVal, uint , 0);
module_param(twostring ,charp, 0);

/* Return port number */
static int led[] = {
	IMX_GPIO_NR(1, 16),   //16
	IMX_GPIO_NR(1, 17),   //17
	IMX_GPIO_NR(1, 18),   //18
	IMX_GPIO_NR(1, 19),   //19
};

/* Print kernel message that which LED ON or NOT */
void led_status(unsigned long data){
	int i = 0;
	while(i < 4){
		if((data >> i) & 0x01)
			printk("O");
		else
			printk("X");
		if(i != 3)
			printk(":");
		else
			printk("\n");
		i++;
	}
}

static int led_init(void)
{
	int ret = 0;
	int i;

	/* gpio_request() :  If available, return 0*/
	for (i = 0; i < ARRAY_SIZE(led); i++) {
		ret = gpio_request(led[i], "gpio led"); // reserve with string "gpio led"
		if(ret<0){
			printk("#### FAILED Request gpio %d. error : %d \n", led[i], ret);
		}
		gpio_direction_output(led[i], 1);// port, value
	}
	return ret;
}

static void led_exit(void)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(led); i++){
		gpio_free(led[i]); // If not exec, next call function return value< 0
	}
}

void led_write(unsigned long data)
{
	int i;
	for(i = 0; i < ARRAY_SIZE(led); i++){
		/* gpio_direction_output(led[i], (data >> i ) & 0x01);// port, value */
		gpio_set_value(led[i], (data >> i ) & 0x01);
	}
#if DEBUG
	printk("#### %s, data = %ld\n", __FUNCTION__, data);
#endif
}

static int led_on(void){
	led_init(); // Reserve GPIO ports and set ports OUTPUT
	printk("Hello, world [ledvalue=%d:twostring=%s]\n",ledVal,twostring);
	led_write(ledVal); // LED ON
	led_status(ledVal); // Print LED status

	return 0;
}

static void led_off(void){
	led_write(0x00); // LED OFF
	led_status(0x00); // Print status 0
	led_exit(); // Set ports free
}

/* moudle_init, module_exit macro function */
module_init(led_on);
module_exit(led_off);

MODULE_AUTHOR("GunwooYun");
MODULE_DESCRIPTION("Test module");
MODULE_LICENSE("Dual BSD/GPL");
