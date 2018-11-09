/*
项目名称：电子相册
项目环境：VMware12   Ubuntu12.04   CRT串口调试
项目时间：1 day
项目负责人：hu
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>//标准输入输入
#include <unistd.h>
#include <sys/mman.h>//内存映射
#include <linux/input.h> //输入子系统
#include <time.h>//时间
#include <pthread.h>//线程

int ts_fd;//触摸文件描述符

char str[][50]={"/images/1.bmp","/images/2.bmp","/images/3.bmp","/images/4.bmp",
                "/images/login.bmp","/images/log.bmp"};
char img[][50]={"/images/s0.bmp","/images/s1.bmp","/images/s2.bmp","/images/s3.bmp",
                "/images/s4.bmp","/images/s5.bmp","/images/s6.bmp","/images/s7.bmp",				
                "/images/s8.bmp","/images/s9.bmp","/images/time_sdd.bmp"};

/********显示bmp图**********/
//自定义函数，显示任意位置任意大小
/*
  坐标原点：（x，y）
  真实的高宽： （w，h）
  路径 ：const char *pathname
*/
int show_bmp(int x,int y,int w,int h,const char *pathname)
{
    int lcd_fd;
    int bmp_fd;//图片文件描述符
	char bmpbuf[w*h*3];
	int lcdbuf[w*h];
	int i,j;
	int *lcdmmap;//内存映射的指针
	
  //准备lcd屏幕
  lcd_fd=open("/dev/fb0",O_RDWR);
  if(lcd_fd == -1)
  {
     perror("open lcd_fd false!\n");
	 return -1;
  }
  //准备图片 1.bmp
  bmp_fd=open(pathname,O_RDWR);
  if(bmp_fd == -1)
  {
	  perror("open bmp_fd false!\n");
	  return -1;
  }
  //将lcd屏幕的地址映射到用户空间
  lcdmmap=mmap(NULL,800*480*4,PROT_READ|PROT_WRITE,MAP_SHARED,lcd_fd,0);
    if(lcdmmap == NULL)
	{
	   perror("lcdmmap mmap false!\n");
	   return -1;
	}
  //读之前，先跳过前面54字节
  lseek(bmp_fd,54,SEEK_SET);
  //读取图片颜色值
  read(bmp_fd,bmpbuf,w*h*3);
  
  //将读取到的图片颜色3字节转化为4字节
  for(i=0;i<w*h;i++)
  {
	  //           B          G               R
     lcdbuf[i]=bmpbuf[3*i] | bmpbuf[3*i+1]<<8 | bmpbuf[3*i+2]<<16 | 0x00 <<24;
  }
  
  //将图片翻转 （x，y） （x，479-y）
  for(i=0;i<w;i++)
  {
    for(j=0;j<h;j++)
	{
		//lcdmmap[(479-y)*800+x]=lcdbuf[y*800+x];//最后一行存第一行的数据
		//*(lcdmmap+(h-1-j)*w+i)=lcdbuf[(y+j)*800+x+i];
		*(lcdmmap+(y+j)*800+x+i)=lcdbuf[(h-1-j)*w+i];
	}
  }
 
  //关闭文件
  close(lcd_fd);
  close(bmp_fd);
  //解除映射
  munmap(lcdmmap,800*480*4);
}

/********触摸屏使用********/
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

/********子线程调用*******/
//struct tm *localtime(const time_t *timep);
void *pthread_time(void *arg)
{
	int h,m,s;//时分秒
	int h1,h2,m1,m2,s1,s2;
	//获取时间
	while(1)
	{
		struct tm  *t;
		//时间格式空间
	    time_t It;
		//获取时间
		time(&It);
		//获取完整格式
		t=localtime(&It);
		
		h = t->tm_hour;//时
		m = t->tm_min;//分
		s = t->tm_sec;//秒
		
		s1=s/10;//时的十位
		s2=s%10;//时的个位
		
		show_bmp(640,10,40,80,img[s1]);
		show_bmp(690,10,40,80,img[s2]);
		show_bmp(740,10,40,80,img[10]);
	}
	//退出子线程
	pthread_exit(NULL); 
}

/********自动播放*********/
void auto_play()
{
	int i,x,y;
	for(i=0;i<4;i++)//图片显示
	{
		show_bmp(0,0,800,480,str[i]);
		sleep(3);
	}
	
	while(1)
	{
		get_x_y(&x,&y);
		printf("(%d,%d)\n",x,y);
		if((x>600&&x<800)&&(y>380&&y<480))
		{
			printf("返回主界面！\n");
			//主界面
	        show_bmp(0,0,800,480,str[5]);
			break;
		}
	}
}

/********手动播放*********/
void manl_play()
{
	int x,y;
	int i=0;
	while(1)
	{
		get_x_y(&x,&y);
		printf("(%d,%d)\n",x,y);
		//上一张
		if((x>0&&x<200)&&(y>0&&y<480))
		{
			if(i == 0)
			{
				i=3;
			}
			i--;
			printf("现在显示的编号%d\n",i);
			show_bmp(0,0,800,480,str[i]);
		}
		//下一张
		if((x>600&&x<800)&&(y>0&&y<480))
		{
			if(i ==3)
			{
				i=0;
			}
			i++;
			printf("现在显示的编号%d\n",i);
			show_bmp(0,0,800,480,str[i]);
		}
		if((x>200&&x<600)&&(y>0&&y<480))
		{
			printf("返回主界面！\n");
			//主界面
	        show_bmp(0,0,800,480,str[5]);
			break;
		}
	}
}

/*********主界面**********/
void login()
{
	int x;
	int y;
	while(1)
	{
		get_x_y(&x,&y);
		printf("(%d,%d)",x,y);
		if((x>130&&x<210)&&(y>155&&y<325))
		{
			//自动
			auto_play();
		}
		if((x>550&&x<632)&&(y>155&&y<325))
		{
			//手动
			manl_play();
		}
	}
}


int main(int argc,const char *argv[])
{
	int ret;
	//打开触摸屏
    ts_open();
	//定义线程号
	pthread_t threadid;
	//创建线程
	ret=pthread_create(&threadid,NULL,pthread_time,NULL);
	if(ret != 0)
	{
		perror("pthread_create false!\n");
		return -1;//异常推出
	}
	//欢迎界面
	show_bmp(0,0,800,480,str[4]);
	//延时5秒
	sleep(5);
	//主界面
	show_bmp(0,0,800,480,str[5]);
	login();
	
	//关闭触摸屏
    ts_close();
	//回收线程
	pthread_join(threadid,NULL);
	
  return 0;
}