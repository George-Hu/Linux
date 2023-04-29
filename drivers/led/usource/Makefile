
all:
	${CROSS_COMPILE}gcc usrLedTest.c -o usrLedTest

clean:
	rm -f *.out
	rm -rf usrLedTest
	rm -rf *.o

install: # copy usrLedTest to ../../../../rootfs/usr/bin
	cp -f usrLedTest 					${APP_DIR}/

uninstall: 
	rm -rf ${APP_DIR}/usrLedTest