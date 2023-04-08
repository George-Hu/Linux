KDIR := /lib/modules/$(shell uname -r)/build

obj-m := procfs.o
all :
	$(MAKE) -C $(KDIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean
	rm -f *.ko
