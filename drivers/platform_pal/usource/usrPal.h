#ifndef _USR_PAL_H
#define _USR_PAL_H


#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>

int pal_dma_mmap(unsigned long addr_p, unsigned int len, unsigned char **addr_v);
unsigned int pal_dma_munmap(unsigned char *addr_v, unsigned long addr_p,unsigned int len);
ssize_t pal_read(int fd,void *buf,size_t count);

#endif
