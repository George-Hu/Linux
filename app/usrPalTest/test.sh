#!/bin/sh

# 16777216 / 16 = 1048576
# 1048576  / 2  = 524288
#for ((i=1;i<=524288;i++));
#for((i=1;i<=1000;i++));

# д1M���ݲ���
for ((i=1;i<=32768;i++))
do
	echo "0x00 0x11 0x22 0x33 0x44 0x55 0x66 0x77 0x88 0x99 0xaa 0xbb 0xcc 0xdd 0xee 0xff 
		  0x00 0x11 0x22 0x33 0x44 0x55 0x66 0x77 0x88 0x99 0xaa 0xbb 0xcc 0xdd 0xee 0xff" > /dev/asi0
	# cat /dev/asi0
	# if [ $? != 0 ]; then
	# 	break;
	# fi


done
#cat /dev/asi0

# for ((i=1;i<=524288;i++));
# do
# 	echo "0x00 0x11 0x22 0x33 0x44 0x55 0x66 0x77 0x88 0x99 0xaa 0xbb 0xcc 0xdd 0xee 0xff 
# 		  0x00 0x11 0x22 0x33 0x44 0x55 0x66 0x77 0x88 0x99 0xaa 0xbb 0xcc 0xdd 0xee 0xff" >> /dev/pal0
# 	cat /dev/pal0
# 	if [ $? != 0 ]; then
# 		break;
# 	fi
# done
# cat /dev/pal0