#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>	
#include <linux/input.h> //输入子系统
#include <stdio.h>  

int main(void)
{
	struct input_event tsevent;//初始化结构体
	int ts_fd;
	//打开触摸屏的驱动
	ts_fd=open("/dev/input/event0",O_RDWR);
	if(ts_fd == -1)
	{
		perror("open ts_fd false!\n");
		return -1;//异常退出
	}
	//读取坐标值
	while(1)
	{
		read(ts_fd,&tsevent,sizeof(tsevent));
		if(tsevent.type==EV_ABS)
		{
			if(tsevent.code==ABS_X)
			{
				printf("%d\n",tsevent.value);
			}
		}
	}
	 
	
  return 0;
}