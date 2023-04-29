#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include "stdlib.h"
 
int main(int argc, char *argv[])
{
    int fd;
 
    int i = 0;
    printf("FMQL platform_led driver\n");
    fd = open("/dev/platform_led",0);
    if(fd < 0) {
    	printf("can't open platform_led\n");
    	return 0;
    }
 
    while(1) {
    	for(i=0;i<8;i++) {
            ioctl(fd,0,i);
            sleep(1);
            ioctl(fd,0,i);
            sleep(1);
    	}
    }
 
    close(fd);
    printf("exit platform_led\n");
 
    return 0;
}


