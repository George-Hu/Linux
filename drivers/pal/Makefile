# CROSS_COMPILE = /home/share/FMQL-Linux-SDK-Prj-20220223/gcc-linaro-7.3.1-2018.05-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-

# target := pal

# obj-m = $(target).o

# KERN_DIR=/home/share/FMQL-Linux-SDK-Prj-20220223/linux-4.14.55-fmsh

# all:
# 	make ARCH=arm CROSS_COMPILE=${CROSS_COMPILE} -C $(KERN_DIR) M=${PWD} modules 

# clean:
# 	rm -rf  *.ko 
# 	rm -rf  *.o 
# 	rm -rf Module.* 
# 	rm -rf modules.order 
# 	rm -rf *mod.c 
# 	rm -rf .tmp_versions

KDIR := /lib/modules/$(shell uname -r)/build

obj-m := pal.o
all :
	$(MAKE) -C $(KDIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean
	rm -f *.ko
