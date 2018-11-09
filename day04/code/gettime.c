#include <time.h>
#include <stdio.h>
#include <unistd.h>

int main(void)
{
	//定义数组
	char time_array[25]={0};
	//时间格式空间
	time_t It;
	//获取时间
	while(1)
	{
		It=time(NULL);//清空
		//拼接时间
		sprintf(time_array,"%s",ctime(&It));
		printf("time is %s\n",time_array);
		//延时
		sleep(1);//秒级延时
		//usleep();//毫秒级
	}
	return 0;
}