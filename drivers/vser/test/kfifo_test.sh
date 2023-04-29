#!/bin/sh

# 写128K数据测试
#dmesg -c
insmod /usr/lib/modules/vser.ko
count=0
while [ $count -le 1024 ]; do
	# 写128字节
	echo "123456789abcdef123456789abcdef123456789abcdef123456789abcdef123123456789abcdef123456789abcdef123456789abcdef123456789abcdef1234" > /dev/vser0
	
	if [ $? != 0 ]; then
		echo "write data to /dev/vser0 the $((count+1)) times get error!"		
		break;
	fi	
	count=$((count+1))
	if [ $count -eq 1024 ]; then
		break;
	fi
	
done
cat /dev/vser0 > 1.txt
echo $count
echo "First Test Finished! Test OK"


count=0
while [ $count -le 1025 ]; do 
	# 写128字节
	echo "123456789abcdef123456789abcdef123456789abcdef123456789abcdef123123456789abcdef123456789abcdef123456789abcdef123456789abcdef1234" > /dev/vser0
	
	if [ $? != 0 ]; then		
		echo "write data to /dev/vser0 the $((count+1)) times get error!"	
		break;
	fi	
	count=$((count+1))
	if [ $count -eq 1025 ]; then
		break;
	fi
	
done
cat /dev/vser0 > 2.txt
echo $count
echo "Second Test Finished! Test OK"
rmmod vser



# 测试正确打印如下:

# ./kfifo_test.sh
# [  387.657400] vser_exit
# [  399.374166] vser_init
# [  399.376475] vser_init,kfifo_dev is 211812352.
# [  399.562727] kfifo is empty, kfifo_len=0x0,kfifo_avail=0x20000.
# cat: read error: Operation not permitted
# 1024
# First Test Finished! Test OK
# [  399.750592] kfifo is full, kfifo_len=0x20000,kfifo_avail=0x0.
# sh: write error: Operation not permitted
# write data to /dev/vser0 the 1025 times get error!
# [  399.762176] kfifo is empty, kfifo_len=0x0,kfifo_avail=0x20000.
# cat: read error: Operation not permitted
# 1024
# Second Test Finished! Test OK

