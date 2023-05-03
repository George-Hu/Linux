#!/bin/bash
KO_DIR=`dirname "${PWD}"`/ksource
USR_DIR=`dirname "${PWD}"`/usource
TEST_DIR=${PWD}
TEST_DRV=pal_driver
TEST_DEV=pal_device

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


# text menu 
function menu {
	clear 
	echo 
	echo -e "\t\t\tPlatform_pal driver Test Menu\n"
	echo -e "\t1. Test all"
	echo -e "\t2. Test usr interfaces"
	echo -e "\t3. Test insmod & rmmod $TEST_DEV.ko & $TEST_DRV.ko"
	echo -e "\t0. Exit menu\n\n"
	echo -en "\t\tEnter option: "

	read -n 1 option
}

while [ 1 ]
do
	menu
	case $option in
	0)
		break ;;
	1)
		test_all ;;
	2) 
		test_usr ;;
	3)
		test_ko ;;
	*)
		clear
		echo "Sorry, wrong selection" ;;
	esac
	echo -en "\n\n\t\t\tHiy any key to continue"
	read -n 1 line
done
clear
exit 0



# using  select in the menu
PS3="Enter option:"
select option in "Test all" "Test usr interfaces" "Test insmod & rmmod $TEST_DEV.ko & $TEST_DRV.ko" "Exit menu"
do 
	case $option in
	"Exit menu")
		break;;
	"Test all")
		test_all;;
	"Test usr interfaces")
		test_usr;;
	"Test insmod & rmmod $TEST_DEV.ko & $TEST_DRV.ko")
		test_ko;;
	esac		
done
clear
exit 0

# old version
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

