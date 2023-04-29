all:
	${CROSS_COMPILE}gcc uart_test.c -o uart_test

clean:
	rm -rf uart_test
	rm -rf *.o

install: # copy uart_test to ../../../../rootfs/usr/bin
	cp -f uart_test 			${APP_DIR}/

uninstall: 
	rm -rf ${APP_DIR}/uart_test


