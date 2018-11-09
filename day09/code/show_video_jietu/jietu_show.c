#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include  "lcd.h"//显示画面
#include  "api_v4l2.h"//摄像头
#include "jpeglib.h"


int main()
{
	//初始化lcd屏幕
	lcd_open("dev/fb0");
	
	//获取数据
	FrameBuffer f_buf;
	
	//初始化摄像头
	 linux_v4l2_device_init("/dev/video7");

	 //开启摄像头捕捉
	 linux_v4l2_start_capturing();
     
	 //获取摄像头的画面
	 linux_v4l2_get_fream(&f_buf);

     lcd_draw_jpg(0,0,NULL,f_buf.buf,f_buf.length,0);
     int fd=open("1.jpg",O_RDWR|O_CREAT| O_TRUNC,0777);	 
	 write(fd,f_buf.buf,f_buf.length); 
    
	 //摄像头暂停捕捉
	 linux_v4l2_stop_capturing();

	 //解除摄像头的初始化 
     linux_v4l2_device_uinit();
	 

	return 0;
}













