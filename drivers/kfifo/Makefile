KDIR := /lib/modules/$(shell uname -r)/build

obj-m := test02.o
all :
	$(MAKE) -C $(KDIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean
	rm -f *.ko