#include  <stdio.h>
#include  "lcd.h"//显示画面
#include  "api_v4l2.h"//摄像头

int main()
{
	//初始化lcd屏幕
	lcd_open();
	//映射内存
	mmap_lcd();
	
	//获取数据
	FrameBuffer video_buf;
	
	//初始化摄像头
	 linux_v4l2_device_init("/dev/video7");

	 //开启摄像头捕捉
	 linux_v4l2_start_capturing();
     
	 while(1)
	 {
         //获取摄像头的画面
	      linux_v4l2_get_fream(&video_buf);

         //摄像头显示画面
	     show_video_data(0,0,video_buf.buf,video_buf.length);
     }
	 //摄像头暂停捕捉
	 linux_v4l2_stop_capturing();

	 //解除摄像头的初始化 
     linux_v4l2_device_uinit();
	 
	 //关闭设备
	 lcd_close();
	return 0;
}