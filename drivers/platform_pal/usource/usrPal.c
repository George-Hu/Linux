#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

#include "../include/pal.h"

#define DEBUG

int pal_dma_mmap(unsigned long addr_p, unsigned int len, unsigned char **addr_v) 
{
	int fd;
	int i = 0;
	unsigned char *mem;
	unsigned int pagesize = getpagesize();
	unsigned long base_addr = addr_p & ~(pagesize - 1);
	unsigned int offset = addr_p - base_addr;

	fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (fd < 0) {
		printf("fail to open /dev/mem in mem_map\n");
		return -1;
	}

	printf("base addr:0x%lx\n", base_addr);

	mem = mmap(0, len + offset, PROT_READ | PROT_WRITE, MAP_SHARED,fd, base_addr);
	if (mem == MAP_FAILED) {
		close(fd);
		fprintf(stderr, "fail to mmap /dev/mem in 0x%lx - %d\n",
			addr_p, len);
		return -1;
	} else
		printf("mmap successful ! mem is %pf,offset is %x\n",mem,offset);	

	*addr_v = mem + offset;

	close(fd);

	return 0;
}

unsigned int pal_dma_munmap(unsigned char *addr_v, unsigned long addr_p,unsigned int len)
{
	int ret;
	unsigned int pagesize = getpagesize();
	unsigned long base_addr = addr_p & ~(pagesize - 1);
	unsigned int offset = addr_p - base_addr;

	addr_v = addr_v - offset;

	ret = munmap(addr_v, len + offset);
	if (ret < 0) {
		fprintf(stderr, "fail to munmap 0x%lx - %d\n", addr_p, len);
		return -1;
	}

	return 0;
}

/*
	pal数据阻塞读接口
*/
ssize_t pal_read(int fd,void *buf,size_t count) 
{
/*
	1、open /dev/pal0 打开设备，挂中断;
	2、read 阻塞读，读取中断的ping或者pong内存物理地址以及接收数据帧的长度;
	----------
		fd 		:	文件描述符
  		buf		:	从buf缓冲区中读入数据到文件
         		(这里读出当前pal ping/pong内存区的起始物理地址4字节，以及接收的ping/pong数据的长度4字节)
  		count	: 	读入的字节数8
  		返回值	:	成功则返回实际读到的字节数，失败则返回-1.
  	---- 
	3、pal_dma_mmap 映射ping或者pong内存的物理地址，长度为接收帧的长度
	4、读取数据
	5、pal_dma_unmap解除物理地址映射.
*/
	int ret = -1;
	int i = 0;
	unsigned char *addr_v;
	struct pal_phycisal_info physical_info;

	ret = read(fd,(unsigned char *)&physical_info,sizeof(struct pal_phycisal_info));
	if (ret > 0 ) {
		printf("pal_read OK!\n");
#ifdef DEBUG		
		printf("physical_info.pal_data_addr = 0x%x.\n",physical_info.pal_data_addr);
		printf("physical_info.pal_data_len  = 0x%x.\n",physical_info.pal_data_len);
#endif
	} else
		printf("pal_read Failed! ret = 0x%x.\n",ret);

	ret = pal_dma_mmap(physical_info.pal_data_addr,physical_info.pal_data_len,&addr_v);

	if (!ret ) {
/*		for(i=0;i<physical_info.pal_data_len;i++) {
			if ((0 == i) || ((i%16) == 0)) {
				printf("0x%016x:",i);
			}
			printf("0x%02x ",*(addr_v + i));
			if ((i+1)%16 == 0) {
				printf("\n");
			}		
		}*/
	}
	memcpy(buf,addr_v,physical_info.pal_data_len);
	ret = pal_dma_munmap(addr_v,physical_info.pal_data_addr,physical_info.pal_data_len);	
	
	if (!ret)
		ret =  physical_info.pal_data_len;

	return ret;
}

