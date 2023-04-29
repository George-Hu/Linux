all:
	${CROSS_COMPILE}gcc watchdog.c -o watchdog-mbch20
	${CROSS_COMPILE}gcc watchdog-test.c -o watchdog-test

clean:
	rm -rf watchdog-mbch20
	rm -rf watchdog-test
	rm -rf *.o

install: # copy watchdog to ../../../../rootfs/usr/bin
	cp -f watchdog-mbch20	${APP_DIR}/
	cp -f watchdog-test     ${APP_DIR}/

uninstall: 
	rm -rf ${APP_DIR}/watchdog-mbch20
	rm -rf ${APP_DIR}/watchdog-test