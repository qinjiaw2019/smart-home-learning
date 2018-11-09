#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>	
#include <linux/input.h> //输入子系统
#include <stdio.h>  

int ts_fd;//触摸文件描述符

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

int main(void)
{
	int x,y;
	ts_open();
	while(1)
	{
		get_x_y(&x,&y);
		printf("(%d,%d)\n",x,y);
		if(x<400)
		{
		  printf("左边！\n");
		}
		if(x>400)
		{
		  printf("右边！\n");
		}
	}
	ts_close();
	 
	
  return 0;
}