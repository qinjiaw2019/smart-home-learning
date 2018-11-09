#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>	
#include <linux/input.h> //输入子系统
#include <stdio.h> 
#include <string.h>//strlen头文件 

int ts_fd;//触摸文件描述符
int fifo_fd;//管道的文件描述符

//打开触摸屏
int ts_open()
{
	//打开触摸屏的驱动
	ts_fd=open("/dev/input/event0",O_RDWR);
	if(ts_fd == -1)
	{
		perror("open ts_fd false!\n");
		return -1;//异常退出
	}
	return 0;
}
//获取坐标
int get_x_y(int *x,int *y)
{
	struct input_event tsevent;//初始化结构体
	int count=0;//计数
	while(1)
	{
		//读取坐标值
		read(ts_fd,&tsevent,sizeof(tsevent));
		if(tsevent.type==EV_ABS)
		{
			if(tsevent.code==ABS_X)
			{
				*x=tsevent.value;
				count++;
			}
			if(tsevent.code == ABS_Y)
			{
				*y=tsevent.value;
				count++;
			}
			if(count == 2)
			{
				break;
			}
		}
	}
}

//关闭触摸屏
int ts_close()
{
	close(ts_fd);
	return 0;
}

//管道文件
int init_my_fifo()
{
	int ret;
	
	//判断文件是否存在
	if(access("/tmp/fifo",F_OK))
	{
		//创建管道文件，在开发板/tmp下面创建一个fifo的管道文件
		ret=mkfifo("/tmp/fifo",0777);
		if(ret < 0)
		{
			perror("mkfifo /tmp/fifo false!\n");
			return -1;
		}
	}
	//打开管道文件
	fifo_fd=open("/tmp/fifo",O_RDWR);
	if(fifo_fd == -1)
	{
		perror("open error!\n");
		return -1;
	}
	
	return 0;
}

//发送命令
int write_cmd(char *cmd)
{
   write(fifo_fd,cmd,strlen(cmd));
   return 0;   
}

int main()//伪代码思路
{
	int x,y;
	//1.初始化管道文件，创建并打开   /tmp/fifo
	init_my_fifo();
	
	//2.播放视频 后台播放
	system("mplayer -slave -quiet -input file=/tmp/fifo -geometry 0:0 -zoom -x 800 -y 480 1.avi &");
	
	//3.触摸屏
	ts_open();
	
	//4.具体区域去做具体的事
	while(1)
	{
	   get_x_y(&x,&y);
	   printf("(%d,%d)\n",x,y);
	   if(x<400)
	   {
		   write_cmd("volume  1000\n");
		   printf("++++！\n");
	   }
	   if(x>400)
	   {
	       printf("快进！\n");
		   write_cmd("seek 10\n");
	   }
	}
	
	
	ts_close();
	return 0;
}