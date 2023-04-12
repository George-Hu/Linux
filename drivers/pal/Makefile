CROSS_COMPILE = /home/share/FMQL-Linux-SDK-Prj-20220223/gcc-linaro-7.3.1-2018.05-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-
KERN_DIR=/home/share/FMQL-Linux-SDK-Prj-20220223/linux-4.14.55-fmsh

LOCAL_KERN_DIR := /lib/modules/$(shell uname -r)/build

target := pal

obj-m = $(target).o


local:  # x86_64 ubuntu
	$(MAKE)  -C $(LOCAL_KERN_DIR) M=$(PWD) modules

all:    # cross_compile
	make ARCH=arm CROSS_COMPILE=${CROSS_COMPILE} -C $(KERN_DIR) M=${PWD} modules 

clean:
	rm -rf  *.ko 
	rm -rf  *.o 
	rm -rf Module.* 
	rm -rf modules.order 
	rm -rf *mod.c 
	rm -rf .tmp_versions
	rm -rf  pal.o.ur-safe

