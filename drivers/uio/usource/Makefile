target := uio_test

all:
	${CROSS_COMPILE}gcc ${target}.c -o ${target}

clean:
	rm -f *.out	
	rm -rf *.o
	rm -rf ${target}

install: # copy uio_test to ../../../../rootfs/usr/bin
	cp -f ${target} 					${APP_DIR}/

uninstall: 
	rm -rf ${APP_DIR}/${target}	

