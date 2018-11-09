#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>//内存映射

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
  //读之前，先跳过前面54字节
  lseek(bmp_fd,54,SEEK_SET);
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


int main(int argc,const char *argv[])
{
  show_bmp(100,100,100,100,argv[1]);
  return 0;
}