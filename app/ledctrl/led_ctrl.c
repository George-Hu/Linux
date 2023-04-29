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

#define	FPGA_TY_NAME_BASE	"/dev/ttyPS"
#define FPGA_NUM_TTY 		10
#define PAL0   				"/dev/pal0"
#define ASI0   				"/dev/asi0"

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
    char tyName [20];
    unsigned int ix;
    int fd[9];
    int fd_pal0,fd_asi0; 
    int select_fd;  

    fd_set rfds;

    if (FPGA_NUM_TTY > 0) {
        for (ix = 0; ix < FPGA_NUM_TTY; ix++) {
            tyName[0] = '\0';
            strcat (tyName, FPGA_TY_NAME_BASE);
            strncat (tyName, itos (ix+2), (sizeof(tyName) - 1) - strlen (tyName)); 
            
            fd[ix] = open(tyName, O_RDWR | O_NOCTTY);//阻塞式读写  //非阻塞| O_NDELAY | O_NONBLOCK
	 		if (fd[ix] < 0) {
		     	printf("open %s error\n",tyName);
		    }
            else {
                printf("open %s OK\n",tyName);
                select_fd = fd[ix];
            }
        }
    }

    fd_pal0 = open(PAL0, O_RDWR | O_NOCTTY );//阻塞式读写   //非阻塞| O_NDELAY | O_NONBLOCK
    if (fd_pal0 < 0) {
        printf("open %s error\n",PAL0);
    }
    else {
        printf("open %s OK\n",PAL0);
        select_fd = fd_pal0;
    }

    fd_asi0 = open(ASI0, O_RDWR | O_NOCTTY );//阻塞式读写   //非阻塞| O_NDELAY | O_NONBLOCK
    if (fd_asi0 < 0) {
        printf("open %s error\n",ASI0);
    }
    else {
        printf("open %s OK\n",ASI0);
        select_fd = fd_asi0;
    }    

    while(1) {
        //need add turn off all led
        FD_ZERO(&rfds);
        for (ix = 0; ix < FPGA_NUM_TTY; ix++) {
            if (fd[ix] > 0) {
                FD_SET(fd[ix],&rfds);
            }
        }

        if (fd_pal0 > 0) {
            FD_SET(fd_pal0, &rfds);
        }

        if (fd_asi0 > 0) {
            FD_SET(fd_asi0, &rfds);
        }

        ret = select(select_fd + 1, &rfds, NULL, NULL, NULL);  //block mode
        if(ret > 0) {
       		for (ix = 0; ix < FPGA_NUM_TTY; ix++) {
                if (fd[ix] > 0) {
                    if(FD_ISSET(fd[ix],&rfds)) {
                        //need add led[ix] = on;
                        printf("===>[ turn on led %d ]===>\n\n",(ix+2));
                    }
                    else {
                        //need add led[ix] = off;
                        //printf("turn off led %d\n",(ix+2));
                    }
                }
       		}

            if (fd_pal0 > 0) {
                if (FD_ISSET(fd_pal0,&rfds)) {
                    //need add led_pal0 = on;
                    printf("turn on led pal0!\n");
                }
                else {
                    //need add del_pal0 = off;
                    //printf("turn off led pal0!\n");
                }
            }

            if (fd_asi0 > 0) {
                if (FD_ISSET(fd_asi0,&rfds)) {
                    //need add led_asi0 = on;
                    //printf("turn off led asi0!\n");
                }
                else {
                    //need add led_asi0 = off;
                    //printf("turn off led asi0!\n");
                }      
            }

        } // end if(ret > 0)
    } // end while(1)
}


