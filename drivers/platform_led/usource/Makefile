
all:
	${CROSS_COMPILE}gcc led_test.c -o led_test

clean:
	rm -f *.out
	rm -rf led_test
	rm -rf *.o

install: # copy usrLedTest to ../../../../rootfs/usr/bin
	cp -f led_test 					${APP_DIR}/
	cp -rf ../test/led.sh 			${USER_TEST_SH_DIR}

uninstall: 
	rm -rf ${APP_DIR}/led_test
	rm -rf ${USER_TEST_SH_DIR}/led.sh