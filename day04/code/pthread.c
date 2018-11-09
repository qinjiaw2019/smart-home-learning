#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

void *pthread_time(void *arg)//子线程
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
	//退出子线程
	pthread_exit(NULL);
}

int main(void)
{
	int ret;
	printf("你好世界！\n");
	pthread_t threadid;//创建线程号
	//创建线程
	ret=pthread_create(&threadid,NULL,pthread_time,NULL);
	if(ret != 0)
	{
		perror("pthread_create false!\n");
		return -1;//异常推出
	}
	printf("你好宇宙！\n");
	//回收线程
	pthread_join(threadid,NULL);
	return 0;
}