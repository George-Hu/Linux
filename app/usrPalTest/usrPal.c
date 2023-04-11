#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

int dma_mmap(unsigned long addr_p, unsigned int len, unsigned char **addr_v)
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

unsigned int dma_munmap(unsigned char *addr_v, unsigned long addr_p,unsigned int len)
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


