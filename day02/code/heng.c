#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main(void)
{
  int lcd_fd;//lcd文件描述符
  int ret;
  int i;
  int buf[800*480];
  //打开屏幕
  lcd_fd=open("/dev/fb0",O_RDWR);
  if(lcd_fd == -1)
  {
	  printf("open lcd_fd fails!\n");
	  return -1;
  }
  //准备颜色
  for(i=0;i<800*160;i++)//黑色
  {
     buf[i]=0x00000000;
  }
  for(;i<800*320;i++)//红色
  {
	  buf[i]=0x00ff0000;
  }
  for(;i<800*480;i++)//黄色
  {
	  buf[i]=0x00ffff00;
  }
  //写入颜色
  ret=write(lcd_fd,buf,800*480*4);
  if(ret == -1)
  {
      printf("write lcd_fd fails!\n");
	  return -1;
  }
  //关闭文件
  close(lcd_fd);
  return 0;
}