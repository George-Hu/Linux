export CROSS_COMPILE = /home/share/FMQL-Linux-SDK-Prj-20220223/gcc-linaro-7.3.1-2018.05-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-

local:
	#gcc -shared usrPal.c -o libusrPal.so -fPIC
	gcc -c usrPal.c
	ar r libusrPal.a usrPal.o
	gcc usrPalTest.c -lusrPal -L. -static -o usrPalTest.static

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