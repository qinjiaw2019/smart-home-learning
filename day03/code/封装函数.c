#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int showbmp(const char *pathname)
{
	int lcd_fd;
	int bmp_fd;//图片文件描述符
	char bmpbuf[800*480*3];
	int lcdbuf[800*480];
	int tempbuf[800*480];
	int i;
	int x,y;
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
  //读取图片颜色值
  read(bmp_fd,bmpbuf,800*480*3);
  
  //将读取到的图片颜色3字节转化为4字节
  for(i=0;i<800*480;i++)
  {
	  //           B          G               R
     lcdbuf[i]=bmpbuf[3*i] | bmpbuf[3*i+1]<<8 | bmpbuf[3*i+2]<<16 | 0x00 <<24;
  }
  
  //将图片翻转 （x，y） （x，479-y）
  for(x=0;x<800;x++)
  {
    for(y=0;y<480;y++)
	{
		tempbuf[(479-y)*800+x]=lcdbuf[y*800+x];//最后一行存第一行的数据
	}
  }
  //写入到屏幕
  write(lcd_fd,tempbuf,800*480*4);
  //关闭文件
  close(lcd_fd);
  close(bmp_fd);
}


int main(int argc,const char *argv[])
{
	showbmp("/1.bmp");
  return 0;
}