#!/bin/bash
export KO_DIR=`dirname "${PWD}"`/ksource
export USR_DIR=`dirname "${PWD}"`/usource
export TEST_DIR=${PWD}
export TEST_DRV=pal_driver
export TEST_DEV=pal_device


function usage {
	echo "./test_local.sh  all          ---- Test all"
	echo "./test_local.sh  usr          ---- Test usr interfaces"
	echo "./test_local.sh  ko           ---- Test insmod & rmmod $TEST_DEV.ko & $TEST_DRV.ko"
}

function test_usr {
	echo "==========Test usr interfaces=========="
	rmmod $TEST_DRV
	rmmod $TEST_DEV

	cd ${KO_DIR} && make clean && make local && cd ${TEST_DIR}
	check_results=`lsmod | grep $TEST_DEV | awk '{print \$1}'`
	if [[ $check_results =~ $TEST_DEV ]]
	then
		echo "have $TEST_DEV"
	else
		insmod ${KO_DIR}/$TEST_DEV.ko
	fi

	check_results=`lsmod | grep $TEST_DRV | awk '{print \$1}'`
	if [[ $check_results =~ $TEST_DRV ]]
	then
		echo "have $TEST_DRV"
	else
		insmod ${KO_DIR}/$TEST_DRV.ko
	fi

	${USR_DIR}/usrPalTest.static open /dev/pal0
	${USR_DIR}/usrPalTest.static open /dev/asi0
	${USR_DIR}/usrPalTest.static close /dev/pal0
	${USR_DIR}/usrPalTest.static close /dev/asi0
}

function test_ko_mode1 {
	echo "==========Test insmod & rmmod $TEST_DEV.ko & $TEST_DRV.ko mode 1=========="
	echo ${KO_DIR}
	cd ${KO_DIR}
	make clean
	make local	
	sleep 1
	dmesg -c
	insmod $TEST_DEV.ko
	insmod $TEST_DRV.ko
	sleep 1
	${USR_DIR}/usrPalTest.static open /dev/pal0
	${USR_DIR}/usrPalTest.static open /dev/asi0
	${USR_DIR}/usrPalTest.static close /dev/pal0
	${USR_DIR}/usrPalTest.static close /dev/asi0
	sleep 1
	rmmod $TEST_DRV
	rmmod $TEST_DEV
	dmesg -c
	cd ${TEST_DIR}
}

function test_ko_mode2 {
	echo "==========Test insmod & rmmod $TEST_DEV.ko & $TEST_DRV.ko mode 2=========="
	echo ${KO_DIR}
	cd ${KO_DIR}
	make clean
	make local
	sleep 1
	dmesg -c
	insmod $TEST_DEV.ko
	insmod $TEST_DRV.ko
	sleep 1
	${USR_DIR}/usrPalTest.static open /dev/pal0
	${USR_DIR}/usrPalTest.static open /dev/asi0
	${USR_DIR}/usrPalTest.static close /dev/pal0
	${USR_DIR}/usrPalTest.static close /dev/asi0
	sleep 1
	rmmod $TEST_DEV
	rmmod $TEST_DRV
	dmesg -c
	cd ${TEST_DIR}
}

function test_ko_mode3 {
	echo "==========Test insmod & rmmod $TEST_DEV.ko & $TEST_DRV.ko mode 3=========="
	echo ${KO_DIR}
	cd ${KO_DIR}
	make clean
	make local
	sleep 1
	dmesg -c
	insmod $TEST_DRV.ko
	insmod $TEST_DEV.ko
	sleep 1
	${USR_DIR}/usrPalTest.static open /dev/pal0
	${USR_DIR}/usrPalTest.static open /dev/asi0
	${USR_DIR}/usrPalTest.static close /dev/pal0
	${USR_DIR}/usrPalTest.static close /dev/asi0
	sleep 1
	rmmod $TEST_DEV
	rmmod $TEST_DRV
	dmesg -c
	cd ${TEST_DIR}
}

function test_ko_mode4 {
	echo "==========Test insmod & rmmod $TEST_DEV.ko & $TEST_DRV.ko mode 4=========="
	echo ${KO_DIR}
	cd ${KO_DIR}
	make clean
	make local
	sleep 1
	dmesg -c
	insmod $TEST_DRV.ko
	insmod $TEST_DEV.ko
	sleep 1
	${USR_DIR}/usrPalTest.static open /dev/pal0
	${USR_DIR}/usrPalTest.static open /dev/asi0
	${USR_DIR}/usrPalTest.static close /dev/pal0
	${USR_DIR}/usrPalTest.static close /dev/asi0
	sleep 1
	rmmod $TEST_DRV
	rmmod $TEST_DEV	
	dmesg -c
	cd ${TEST_DIR}
}

function test_ko {
	test_ko_mode1
	test_ko_mode2
	test_ko_mode3
	test_ko_mode4
}

function test_all {
	test_usr
	test_ko
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
		test_all
	;;

	usr )
		test_usr
	;;

	ko )
		test_ko
	;;	

	* )
		echo "input parameter error!"
		exit -1
	;;
esac


exit 0

