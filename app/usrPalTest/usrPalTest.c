#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <regex.h>
#include <math.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "pal.h"
#include "usrPal.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

void usage(void)
{
    printf("\nUsage:\n");

    printf("[   ./usrPalTest.static ioctl  /dev/pal0 or /dev/asi0    	]\n");
    printf("[   ./usrPalTest.static read  /dev/pal0 or /dev/asi0 		]\n");
    printf("[   ./usrPalTest.static mmap 0x40000000 0x100        		]\n");
    printf("\n");

    exit(0);
}

/* 测试用例1: ioctl 命令测试 */
/*
#include <sys/ioctl.h>
int ioctl(int file_descriptor, int request, unsigned long *features);
参数说明：file_descriptor	: 文件描述符
		 request			: 一般传无符号整数，用于命令
		 features			: 无符号长整形，一般用于参数
*/

struct  ioctl_table_t {
	int cmd;
	char * cmdStr;
};

static const struct ioctl_table_t ioctl_table[] = {
	{
		.cmd    = PAL_GET_PAL_PING_ADDR,
		.cmdStr = "PAL_GET_PAL_PING_ADDR",
	},
	{
		.cmd    = PAL_SET_PAL_PING_ADDR,
		.cmdStr = "PAL_SET_PAL_PING_ADDR",
	},
	{
		.cmd    = PAL_GET_PAL_RX_LEN,
		.cmdStr = "PAL_GET_PAL_RX_LEN",
	},
};

char * get_cmd_str(int cmd,const struct ioctl_table_t *table) {
	int i;

	for (i = 0; i < ARRAY_SIZE(ioctl_table); i++) {
		if (cmd == table[i].cmd) {
			return table[i].cmdStr;
		}
	}
	return 0;
}

void pal_ioctl_test(const char *pathname) {
	int ret = -1;
	int fd;
	struct pal_info palinfo;
	int cmd;
	char *cmdStr;

	fd = open(pathname,O_RDWR);
	if (fd < 0) {			
		printf("open %s Failed!\n",pathname);
	}

	palinfo.pal_ping_addr  = 0;
	palinfo.pal_rx_max_len = 0;
	cmd 	= PAL_GET_PAL_RX_LEN;
	cmdStr  = get_cmd_str(cmd,ioctl_table);
	ret = ioctl( fd, cmd ,&palinfo );
	if (ret != 0) {
		printf("ioctl %s failed.\n",cmdStr);
	}
	else {
		printf("ioctl %s get palinfo.pal_rx_max_len is 0x%x.\n",cmdStr,palinfo.pal_rx_max_len);
	}

	palinfo.pal_ping_addr = 0x40000000;
	cmd = PAL_SET_PAL_PING_ADDR;
	cmdStr  = get_cmd_str(cmd,ioctl_table);
	ret = ioctl( fd, cmd,&palinfo );
	if (ret != 0) {
		printf("ioctl %s failed.\n",cmdStr);
	}

	palinfo.pal_ping_addr = 0;
	cmd = PAL_GET_PAL_PING_ADDR;
	cmdStr  = get_cmd_str(cmd,ioctl_table);
	ret = ioctl( fd, cmd,&palinfo );
	if (ret != 0) {
		printf("iocctl %s failed.\n",cmdStr);
	}
	else {
		printf("ioctl %s get palinfo.pal_ping_addr is 0x%x.\n",cmdStr,palinfo.pal_ping_addr);
	}

	palinfo.pal_ping_addr = 0x80000000;
	cmd = PAL_SET_PAL_PING_ADDR;
	cmdStr  = get_cmd_str(cmd,ioctl_table);
	ret = ioctl( fd,cmd,&palinfo );
	if (ret != 0) {
		printf("ioctl %s failed.\n",cmdStr);
	}

	palinfo.pal_ping_addr = 0;
	cmd = PAL_GET_PAL_PING_ADDR;
	cmdStr  = get_cmd_str(cmd,ioctl_table);
	ret = ioctl( fd, cmd,&palinfo );
	if (ret != 0) {
		printf("ioctl %s failed.\n",cmdStr);
	}
	else {
		printf("ioctl %s get palinfo.pal_ping_addr is 0x%x.\n",cmdStr,palinfo.pal_ping_addr);
	}

	palinfo.pal_ping_addr = 0xa0000000;
	cmd = PAL_SET_PAL_PING_ADDR;
	cmdStr  = get_cmd_str(cmd,ioctl_table);
	ret = ioctl( fd, cmd,&palinfo );
	if (ret != 0) {
		printf("ioctl %s failed.\n",cmdStr);
	}

	palinfo.pal_ping_addr = 0;
	cmd = PAL_GET_PAL_PING_ADDR;
	cmdStr  = get_cmd_str(cmd,ioctl_table);
	ret = ioctl( fd, cmd,&palinfo );
	if (ret != 0) {
		printf("ioctl %s failed.\n",cmdStr);
	}
	else {
		printf("ioctl %s get palinfo.pal_ping_addr is 0x%x.\n",cmdStr,palinfo.pal_ping_addr);
	}

	palinfo.pal_ping_addr = 0x44000000;
	cmd = PAL_SET_PAL_PING_ADDR;
	cmdStr  = get_cmd_str(cmd,ioctl_table);
	ret = ioctl( fd, cmd,&palinfo );
	if (ret != 0) {
		printf("ioctl %s failed.\n",cmdStr);
	}

	palinfo.pal_ping_addr = 0;
	cmd = PAL_GET_PAL_PING_ADDR;
	cmdStr  = get_cmd_str(cmd,ioctl_table);
	ret = ioctl( fd, cmd,&palinfo );
	if (ret != 0) {
		printf("ioctl %s failed.\n",cmdStr);
	}
	else {
		printf("ioctl %s get palinfo.pal_ping_addr is 0x%x.\n",cmdStr,palinfo.pal_ping_addr);
	}

	close(fd);
	
	return;
}

/* 测试用例2: pal_read 阻塞读DMA ping/pong数据测试  */
void pal_read_test(const char *pathname) {
	int ret = -1;
	int fd;
	unsigned char buf[0x20000];
	int  count = 0x1000;
	int i;
	
	struct pal_phycisal_info physical_info;

	while(1) {
		fd = open(pathname,O_RDWR);
		if (fd < 0) {			
			sleep(1);
			continue;
		}

		ret = pal_read(fd,(unsigned char*)&buf[0],count);
		printf("=================main=================\n");
		for(i=0;i<0x100;i++) {
			if ((0 == i) || ((i%16) == 0)) {
				printf("0x%016x:",i);
			}
			printf("0x%02x ",buf[i]);
			if ((i+1)%16 == 0) {
				printf("\n");
			}		
		}
	}
	return;
}

/* 测试用例3: 测试获取/dev/pal0的ping地址和ping内存数据长度，并做mmap，取数据。 */
void usr_pal_mmap_test(unsigned long addr_p, unsigned int len) {
	int i = 0;
	unsigned char *addr_v;
	int ret = -1;

	ret = pal_dma_mmap(addr_p,len,&addr_v);
	if(!ret) {
		for(i=0;i<len;i++) {
			if ((0 == i) || ((i%16) == 0)) {
				printf("0x%016x:",i);
			}
			printf("0x%02x ",*(addr_v + i));
			if ((i+1)%16 == 0) {
				printf("\n");
			}		
		}
	}
	printf("pal_dma_mmap test OK!\n");

	ret = pal_dma_munmap(addr_v,addr_p,len);
	if (!ret) {
		printf("pal_dma_munmap test OK!\n");
	}
	return;
}

int main(int argc,char **argv)
{    

    if((argc == 1) || (strcmp(argv[1],"-h") == 0) || (strcmp(argv[1],"-help") == 0) )
    {
        usage();
    }

    if(strcmp(argv[1],"ioctl") == 0) {
    	pal_ioctl_test(argv[2]);
    }
    else if(strcmp(argv[1],"read") == 0) {
    	pal_read_test(argv[2]);
    }
    else if(strcmp(argv[1],"mmap") == 0) {
    	usr_pal_mmap_test(strtol(argv[2], NULL, 16),strtol(argv[3], NULL, 16));
    }

    return 0;
}

