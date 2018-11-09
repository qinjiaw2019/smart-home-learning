#include "font.h"
#include <stdio.h>

int main(int argc, char const *argv[])
{
	Init_Font();

	Clean_Area(0,  //x坐标起始点
               0,  //y坐标起始点
			   800, //绘制的宽度
			   480,//绘制的高度
			   0x0); //往屏幕指定区域填充颜色

	Display_characterX(0,          //x坐标起始点
					   0,          //y坐标起始点
					   "测试看看！！！",   //GB2312 中文字符串
					    0xFF0000,     //字体颜色值
						 2);	

	UnInit_Font();		

	return 0;
}
