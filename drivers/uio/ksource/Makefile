target := uio

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
	rm -rf  ${target}.o.ur-safe
	rm -rf *.cmd
	rm -rf .${target}.ko.cmd
	rm -rf .${target}.mod.o.cmd
	rm -rf .${target}.o.cmd
	rm -rf .tmp_versions/${target}.mod
	rm -rf .tmp_versions

install: # copy pal.ko to ../../../../rootfs/usr/lib/modules
	mkdir -p ${KO_DIR}
	cp -rf ${target}.ko ${KO_DIR}/

uninstall:
	rm -rf ${KO_DIR}/${target}.ko
