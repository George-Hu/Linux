#!/bin/sh


insmod /usr/lib/modules/led_device.ko
insmod /usr/lib/modules/led_driver.ko

led_test &

# 测试5分钟
count=0
while [ $count -le 60 ];do
	count=$((count+1))
	sleep 5
done
count=0
ps -ef | grep -v grep | grep 'led_test' | awk '{print $1}' | xargs kill -9

echo "led_device.ko and led_driver.ko Test Finished! Test OK
"
rmmod led_device
rmmod led_driver

#测试结果:

# ./led.sh
# FMQL platform_led driver
# [ 1723.195947] platform_led_open
# [ 1727.201660] platform_led_ioctl arg = 0x2.
# [ 1728.205843] platform_led_ioctl arg = 0x2.
# [ 1743.211323] platform_led_ioctl arg = 0x2.
# [ 1744.215557] platform_led_ioctl arg = 0x2.
# [ 1759.220996] platform_led_ioctl arg = 0x2.
# [ 1760.225210] platform_led_ioctl arg = 0x2.
# [ 1775.230619] platform_led_ioctl arg = 0x2.
# [ 1776.234821] platform_led_ioctl arg = 0x2.
# [ 1791.240193] platform_led_ioctl arg = 0x2.
# [ 1792.244453] platform_led_ioctl arg = 0x2.
# [ 1807.249857] platform_led_ioctl arg = 0x2.
# [ 1808.254025] platform_led_ioctl arg = 0x2.
# [ 1823.259373] platform_led_ioctl arg = 0x2.
# [ 1824.263549] platform_led_ioctl arg = 0x2.
# [ 1839.268914] platform_led_ioctl arg = 0x2.
# [ 1840.273057] platform_led_ioctl arg = 0x2.
# [ 1855.278494] platform_led_ioctl arg = 0x2.
# [ 1856.282632] platform_led_ioctl arg = 0x2.
# [ 1871.288033] platform_led_ioctl arg = 0x2.
# [ 1872.292179] platform_led_ioctl arg = 0x2.
# [ 1887.297603] platform_led_ioctl arg = 0x2.
# [ 1888.301746] platform_led_ioctl arg = 0x2.
# [ 1903.307143] platform_led_ioctl arg = 0x2.
# [ 1904.311288] platform_led_ioctl arg = 0x2.
# [ 1919.316744] platform_led_ioctl arg = 0x2.
# [ 1920.320883] platform_led_ioctl arg = 0x2.
