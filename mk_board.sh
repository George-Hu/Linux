#!/bin/bash
export DRV_BASE_DIR=${PWD}/drivers
export APP_BASE_DIR=${PWD}/apps

export PAL_DRIVER_KSRC=${DRV_BASE_DIR}/pal/ksource
export PAL_DRIVER_USRC=${DRV_BASE_DIR}/pal/usource
export LED_DRIVER_KSRC=${DRV_BASE_DIR}/led/ksource
export LED_DRIVER_USRC=${DRV_BASE_DIR}/led/usource
export UART_DRIVER_KSRC=${DRV_BASE_DIR}/uartlite/ksource
export VSER_DRIVER_KSRC=${DRV_BASE_DIR}/vser/ksource
export PLATFORM_LED_KSRC=${DRV_BASE_DIR}/platform_led/ksource
export PLATFORM_LED_USRC=${DRV_BASE_DIR}/platform_led/usource


export UART_TEST_APP=${APP_BASE_DIR}/uart
export LED_CTRL_APP=${APP_BASE_DIR}/ledctrl
export WATCHDOG_APP=${APP_BASE_DIR}/watchdog

function usage {
	echo "./mk_board.sh  all          ---- Build all"
	echo "./mk_board.sh  clean        ---- Clean all"
}

function mk_kernel_ksrc {
	echo "==========mk_kernel_ksrc=========="
	echo "make pal.ko"
	cd ${PAL_DRIVER_KSRC} && make all && make install && cd ${DRV_BASE_DIR}
	echo "make led.ko"
	cd ${LED_DRIVER_KSRC} && make all && make install && cd ${DRV_BASE_DIR}
	echo "make uartlite.ko"
	cd ${UART_DRIVER_KSRC} && make all && make install && cd ${DRV_BASE_DIR}
	echo "make vser.ko"
	cd ${VSER_DRIVER_KSRC} && make all && make install && cd ${DRV_BASE_DIR}
	echo "make led_device.ko && led_driver.ko"
	cd ${PLATFORM_LED_KSRC} && make all && make install && cd ${DRV_BASE_DIR}
}

function clean_kernel_ksrc {
	echo "==========clean_kernel_ksrc=========="
	echo "clean pal.ko"
	cd ${PAL_DRIVER_KSRC} && make clean && make uninstall && cd ${DRV_BASE_DIR}
	echo "clean led.ko"
	cd ${LED_DRIVER_KSRC} && make clean && make uninstall && cd ${DRV_BASE_DIR}
	echo "clean uartlite.ko"
	cd ${UART_DRIVER_KSRC} && make clean && make uninstall && cd ${DRV_BASE_DIR}
	echo "clean vser.ko"
	cd ${VSER_DRIVER_KSRC} && make clean && make uninstall && cd ${DRV_BASE_DIR}
	echo "clean led_device.ko && led_driver.ko"
	cd ${PLATFORM_LED_KSRC} && make clean && make uninstall && cd ${DRV_BASE_DIR}
}

function mk_kernel_usrc {
	echo "==========mk_kernel_usrc=========="
	echo "make libusrPal.a && usrPalTest.static"
	cd ${PAL_DRIVER_USRC} && make all && make install && cd ${DRV_BASE_DIR}
	echo "make usrLedTest"
	cd ${LED_DRIVER_USRC} && make all && make install && cd ${DRV_BASE_DIR}
	echo "copy led_test to /rootfs/usr/test"
	cd ${PLATFORM_LED_USRC} && make all && make install && cd ${DRV_BASE_DIR}
}

function clean_kernel_usrc {
	echo "==========clean_kernel_usrc=========="
	echo "clean libusrPal.a && usrPalTest.static"
	cd ${PAL_DRIVER_USRC} && make clean && make uninstall && cd ${DRV_BASE_DIR}
	echo "clean usrLedTest"
	cd ${LED_DRIVER_USRC} && make clean && make uninstall && cd ${DRV_BASE_DIR}
	echo "clean /rootfs/usr/test/led_test"
	cd ${PLATFORM_LED_USRC} && make all && make install && cd ${DRV_BASE_DIR}
}

function mk_apps {
	echo "==========mk_app=========="
	echo "make uart_test"
	cd ${UART_TEST_APP} && make all && make install && cd ${DRV_BASE_DIR}
	echo "make led_ctrl"
	cd ${LED_CTRL_APP} && make all && make install && cd ${DRV_BASE_DIR}
	echo "make watchdog"
	cd ${WATCHDOG_APP} && make all && make install && cd ${DRV_BASE_DIR}

}

function clean_apps {
	echo "==========clean_apps=========="
	echo "clean uart_test"
	cd ${UART_TEST_APP} && make clean && make uninstall && cd ${DRV_BASE_DIR}
	echo "clean led_ctrl"
	cd ${LED_CTRL_APP} && make clean && make uninstall && cd ${DRV_BASE_DIR}
	echo "clear watchdog"
	cd ${WATCHDOG_APP} && make clean && make uninstall && cd ${DRV_BASE_DIR}	
}

if [ $# -lt 1 ]; then
	usage
    exit 0;
fi

case $1 in
	-h | -help | --help )
		usage
	;;

	all )
		mk_kernel_ksrc
		mk_kernel_usrc
		mk_apps
	;;

	clean )
		clean_kernel_ksrc
		clean_kernel_usrc
		clean_apps
	;;

	* )
		echo "input parameter error!"
		exit -1
	;;
esac


exit 0
