
#include "usrPal.h"

void main(void) {
	int i = 0;
	unsigned char *addr_v;

	dma_mmap(0x00090000,0x100,&addr_v);
	for(i=0;i<0x100;i++) {
		if ((0 == i) || ((i%16) == 0)) {
			printf("0x%016x:",i);
		}
		printf("0x%02x ",*(addr_v + i));
		if ((i+1)%16 == 0) {
			printf("\n");
		}		
	}
	dma_munmap(addr_v,0x00090000,0x100);


}

