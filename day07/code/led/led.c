/*---------------------------------------
*功能描述:  实现LED的驱动
*创建者：   粤嵌技术部
*创建时间： 2015,01,01
---------------------------------------
*修改日志：
*修改内容：
*修改人：
*修改时间：
----------------------------------------*/

/*************************************************
*头文件
*************************************************/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <cfg_type.h>

#define DEVICE_NAME "Led"                    //定义设备名字

//定义Led的结构体，包括管脚和名字
struct led {
	int gpio;
	char *name;	
};

//定义Led的管脚和名字                                          
static struct led led_gpios[] = {
	{PAD_GPIO_B+26,"led1"},
    {PAD_GPIO_C+11,"led2"},
    {PAD_GPIO_C+7,"led3"},
	{PAD_GPIO_C+12,"led4"},
};

#define LED_NUM		4                     //ARRAY_SIZE(led_gpios)
#define TEST_MAX_NR 4 		              //定义命令的最大序数
#define TEST_MAGIC 'x'                    //定义幻数

/*************************************************
*led_open函数
*************************************************/
static int led_open(struct inode *inode, struct file *filp)
{
	printk(DEVICE_NAME ":open\n");
	return 0;				     
}

/*************************************************
*LED控制函数
*************************************************/
static long gec5260_leds_ioctl(struct file *filp, unsigned int cmd,
		unsigned long arg)
{
#if 1
	printk("led_num = %d \n", LED_NUM);
	if(_IOC_TYPE(cmd) != TEST_MAGIC) return - EINVAL;
	if(_IOC_NR(cmd) > TEST_MAX_NR) return - EINVAL;	
	
	gpio_set_value(led_gpios[_IOC_NR(cmd)].gpio, arg);
	printk(DEVICE_NAME": %d %lu\n", _IOC_NR(cmd), arg);

#endif
	//printk("xxxx %lu, %d xxxxx \n", cmd, arg);
	return 0;
}

/*************************************************
*文件操作集
*************************************************/
static struct file_operations gec5260_led_dev_fops = {
	.owner			= THIS_MODULE,
	.open                   = led_open,
	.unlocked_ioctl	= gec5260_leds_ioctl,
};

/*************************************************
*杂项设备
*************************************************/
static struct miscdevice gec5260_led_dev = {
	.minor			= MISC_DYNAMIC_MINOR,
	.name			= DEVICE_NAME,
	.fops			= &gec5260_led_dev_fops,
};

/********************************************************************
*驱动的初始化函数--->从内核中申请资源（内核、中断、设备号、锁....）
********************************************************************/
static int __init gec5260_led_dev_init(void) 
{
	int ret, i;
	gpio_free(PAD_GPIO_B+26);
	gpio_free(PAD_GPIO_C+11);
	gpio_free(PAD_GPIO_C+7);
	gpio_free(PAD_GPIO_C+12);

	for (i = 0; i < LED_NUM; i++)
	{
		ret = gpio_request(led_gpios[i].gpio, led_gpios[i].name);            //io申请
		if (ret) 															 //失败则打印出那个管教申请失败
		{
			printk("%s: request GPIO %d for LED failed, ret = %d\n", led_gpios[i].name, led_gpios[i].gpio, ret);
			return ret;
		}
		gpio_direction_output(led_gpios[i].gpio,0);                 //设置io为输出管脚              
	}
	
	ret = misc_register(&gec5260_led_dev);                                   //杂项设备申请
	printk(DEVICE_NAME"\tinitialized\n");									 //显示申请成功
	return ret;
}

/*****************************************************************
*驱动退出函数 --->将申请的资源还给内核
*****************************************************************/
static void __exit gec5260_led_dev_exit(void) 
{
	int i;
	//释放管教
	for (i = 0; i < LED_NUM; i++) 
	{
		gpio_free(led_gpios[i].gpio);
	}
	//移除杂项设备
	misc_deregister(&gec5260_led_dev);
}

module_init(gec5260_led_dev_init);                                          //驱动的入口函数会调用一个用户的初始化函数
module_exit(gec5260_led_dev_exit);                                          //驱动的出口函数会调用一个用户的退出函数

/***************************************************************
*驱动的描述信息： #modinfo  *.ko , 驱动的描述信息并不是必需的。
***************************************************************/
MODULE_AUTHOR("ZOROE@GEC");                                                //驱动的作者
MODULE_DESCRIPTION("the LED of driver");                                   //驱动的描述
MODULE_LICENSE("GPL");                                                     //遵循的协议

