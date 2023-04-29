target := led

obj-m = $(target).o

all:    # cross_compile
	make ARCH=arm CROSS_COMPILE=${CROSS_COMPILE} -C $(KERNEL_DIR) M=${PWD} modules

clean:
	rm -rf  *.ko 
	rm -rf  *.o 
	rm -rf Module.* 
	rm -rf modules.order 
	rm -rf *mod.c 
	rm -rf .tmp_versions
	rm -rf  led.o.ur-safe
	rm -rf *.cmd
	rm -rf .led.ko.cmd
	rm -rf .led.mod.o.cmd
	rm -rf .led.o.cmd
	rm -rf .tmp_versions/led.mod
	rm -rf .tmp_versions

install: # copy led.ko to ../../../../rootfs/usr/lib/modules
	mkdir -p ${KO_DIR}
	cp -rf led.ko ${KO_DIR}/

uninstall:
	rm -rf ${KO_DIR}/led.ko
