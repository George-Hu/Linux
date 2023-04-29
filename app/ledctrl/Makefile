
all:
	${CROSS_COMPILE}gcc led_ctrl.c -o led_ctrl
	${CROSS_COMPILE}gcc led_ctrl_single.c -o led_ctrl_single

clean:
	rm -rf led_ctrl 
	rm -rf led_ctrl_single
	rm -rf *.o

install: # copy led_ctrl to ../../../../rootfs/usr/bin
	cp -f led_ctrl 					${APP_DIR}/
	cp -f led_ctrl_single 			${APP_DIR}/

uninstall: 
	rm -rf ${APP_DIR}/led_ctrl
	rm -rf ${APP_DIR}/led_ctrl_single


