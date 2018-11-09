#include <stdlib.h>
#include <stdio.h> 

int main()
{
   int ret;
   ret=system("madplay 1.mp3 &");
   if(ret == -1)
   {
	   perror("system false!\n");
	   return -1;
   }
   return 0;
}