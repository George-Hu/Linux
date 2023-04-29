
all:
	${CROSS_COMPILE}gcc -c  usrPal.c
	${CROSS_COMPILE}ar r libusrPal.a usrPal.o
	${CROSS_COMPILE}gcc usrPalTest.c -lusrPal -L. -static -o usrPalTest.static

clean:
	rm -f *.out
	rm -rf lib*
	rm -rf usrPalTest
	rm -rf usrPalTest.static
	rm -rf *.o

install: # copy usrPalTest.static and usrPalTest to ../../../../rootfs/usr/lib/modules
	cp -f usrPalTest.static 			${APP_DIR}/
	cp -f libusrPal.a					${USERLIB_DIR}/

uninstall: 
	rm -rf ${APP_DIR}/usrPalTest.static
	rm -rf ${USERLIB_DIR}/libusrPal.a


# x86_64
local:
	#gcc -shared usrPal.c -o libusrPal.so -fPIC
	gcc -c usrPal.c
	ar r libusrPal.a usrPal.o
	gcc usrPalTest.c -lusrPal -L. -static -o usrPalTest.static