
#include "usrPal.h"

void main(void) {
	int i = 0;
	unsigned char *addr_v;
	//#define BASE_ADDR = 0xbf000000;

	//dma_mmap(0xbff00000,0x100,addr_v);

	dma_mmap(0x00090000,0x100,&addr_v);
	for(i=0;i<0x100;i++) {
		if ((0 == i) || ((i%16) == 0)) {
			printf("0x%016x:",i);
		}
		printf("0x%02x ",*(addr_v + i));
		if ((i+1)%16 == 0) {
			printf("\n");
		}
		//printf("*(addr_v + %d) is: 0x%02x.\n\n",i,*(addr_v + i));
	}
	dma_munmap(addr_v,0x00090000,0x100);

	//printf("v_addr : %08x\n",v_addr);
	//dma_mmap(0x40000000,0x100,&addr_v);
/*	for(i=0;i<0x100;i++) {
		printf("*(addr_v + %d) is: 0x%x.\n\n",i,*(addr_v + i));
	}*/
	//dma_munmap(addr_v,0x40000000,0x100);

}

