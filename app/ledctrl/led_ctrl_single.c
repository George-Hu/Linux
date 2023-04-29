#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define TEST_UART       "/dev/ttyPS4"

/*
* itos - given an integer, return a string representing it.
*/

static char * itos (unsigned int val) {
    static char str [20];
    char * pChar;

    if (val == 0) {
	   return "0";
    }

    pChar = &str[19];
    *pChar = '\0';

    while (val != 0) {
		*--pChar = '0' + (val % 10);
		val /= 10;
	}

    return pChar;
}

void main(int argc, char **argv) {
	int ret;
    char buf[255] = {0};
    int rxlen = 0;
    int i; 
    int fd_test;
    fd_set rfds;


    fd_test = open(TEST_UART, O_RDWR | O_NOCTTY);//阻塞式读写             非阻塞| O_NDELAY | O_NONBLOCK
    if (fd_test < 0) {
        printf("open %s error\n",TEST_UART);
        return;
    }
    else {
        printf("open %s OK\n",TEST_UART);
    }


    while(1) {
        FD_ZERO(&rfds);
        if (fd_test > 0) {
            FD_SET(fd_test, &rfds);
        }
        else {
            printf("Can't open %s , this test exit!\n",TEST_UART);
            return;
        }
        
        printf("start to select.\n");
        printf("turn off led!\n");
        ret = select(fd_test + 1, &rfds, NULL, NULL, NULL);  //block mode
        printf("select ret is 0x%x.\n",ret);

        if(ret > 0) {
            if (fd_test > 0) {
                if(FD_ISSET(fd_test,&rfds)) {
                    //led[ix] = on;
                    printf("turn on led!\n");
                    rxlen = read(fd_test, buf, 255);
                    if(rxlen > 0) {
                        printf("%s recv data len:%d\n",TEST_UART,rxlen);
                        printf("%s recv data:\n",TEST_UART);
                        for (i=0; i<rxlen; i++) {                            
                            if ((0 == i) || ((i%16) == 0)) {
                                printf("0x%016x:",i);
                            }
                            printf("0x%02x ",buf[i]);
                            if ((i+1)%16 == 0) {
                                printf("\n");
                            }
                        }
                        //printf("%s rx:%s\n\r",,TEST_UART,buf);
                        //write(fd1, buf, rxlen);
                    }
                }
                else {
                    //led[ix] = off;
                    printf("turn off led!\n");
                }
            }
            else {
                return;
            }
        }
    }
}


