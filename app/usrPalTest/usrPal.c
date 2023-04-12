#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

#include "pal.h"

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
	}
	else {
		printf("mmap successful ! mem is %pf,offset is %x\n",mem,offset);
	}

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
	pal�����������ӿ�
*/
ssize_t pal_read(int fd,void *buf,size_t count) {
/*
	1��open /dev/pal0 ���豸�����ж�;
	2��read ����������ȡ�жϵ�ping����pong�ڴ������ַ�Լ���������֡�ĳ���;
	----------
		fd 		:	�ļ�������
  		buf		:	��buf�������ж������ݵ��ļ�
         		(���������ǰpal ping/pong�ڴ�������ʼ�����ַ4�ֽڣ��Լ����յ�ping/pong���ݵĳ���4�ֽ�)
  		count	: 	������ֽ���8
  		����ֵ	:	�ɹ��򷵻�ʵ�ʶ������ֽ�����ʧ���򷵻�-1.
  	---- 
	3��pal_dma_mmap ӳ��ping����pong�ڴ�������ַ������Ϊ����֡�ĳ���
	4����ȡ����
	5��pal_dma_unmap��������ַӳ��.
*/
	int ret = -1;
	int i = 0;
	unsigned char *addr_v;
	struct pal_phycisal_info physical_info;

	ret = read(fd,(unsigned char *)&physical_info,sizeof(struct pal_phycisal_info));
	if (ret > 0 )
	{
		printf("pal_read OK!\n");
#ifdef DEBUG		
		printf("physical_info.pal_data_addr = 0x%x.\n",physical_info.pal_data_addr);
		printf("physical_info.pal_data_len  = 0x%x.\n",physical_info.pal_data_len);
#endif
	}
	else {
		printf("pal_read Failed! ret = 0x%x.\n",ret);
	}

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
	
	if (!ret) {
		ret = physical_info.pal_data_len;
	}
	return ret;
}

