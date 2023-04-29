target1 := led_device
target2 := led_driver

obj-m := $(target1).o $(target2).o

all:    # cross_compile
	make ARCH=arm CROSS_COMPILE=${CROSS_COMPILE} -C $(KERNEL_DIR) M=${PWD} modules

clean:
	rm -rf  *.ko 
	rm -rf  *.o 
	rm -rf Module.* 
	rm -rf modules.order 
	rm -rf *mod.c 
	rm -rf .tmp_versions
	rm -rf *.cmd
	rm -rf  ${target1}.o.ur-safe	
	rm -rf .${target1}.ko.cmd
	rm -rf .${target1}.mod.o.cmd
	rm -rf .${target1}.o.cmd
	rm -rf  ${target2}.o.ur-safe	
	rm -rf .${target2}.ko.cmd
	rm -rf .${target2}.mod.o.cmd
	rm -rf .${target2}.o.cmd
	

install: # copy led.ko to ../../../../rootfs/usr/lib/modules
	mkdir -p ${KO_DIR}
	cp -rf ${target1}.ko ${KO_DIR}/
	cp -rf ${target2}.ko ${KO_DIR}/

uninstall:
	rm -rf ${KO_DIR}/${target1}.ko
	rm -rf ${KO_DIR}/${target2}.ko
