#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>    //标准输入输出
#include <sys/ioctl.h>//监测头文件
#include <unistd.h>   //延时


#define TEST_MAGIC 'x'                    //定义幻数

//定义下LED灯
#define LED1 _IO(TEST_MAGIC,0)

//定义开和关
#define LED_ON   0    //低电平  亮灯
#define LED_OFF  1    //高电平  灭灯

int main()
{
   int led_fd;//led灯的文件描述符
   //打开led灯的驱动
   led_fd=open("/dev/Led",O_RDWR);
   if(led_fd == -1)
   {
	   perror("led_fd open false!\n");
	   return -1;
   }
   
   //监测led的状态
   while(1)
   {
	   /*****监测led是否亮******/
	   ioctl(led_fd,LED1,LED_ON);
	   sleep(1);
	   /*****监测led是否灭*****/
	   ioctl(led_fd,LED1,LED_OFF);
	   sleep(1);
   }
  return 0;
}