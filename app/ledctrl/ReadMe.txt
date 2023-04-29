
调试前使用stty功能配置一下单板上的422串口

stty -F /dev/ttyPS2 speed 115200 cs8 -cstopb parenb -parodd
stty -F /dev/ttyPS3 speed 115200 cs8 -cstopb parenb -parodd
stty -F /dev/ttyPS4 speed 115200 cs8 -cstopb parenb -parodd
stty -F /dev/ttyPS5 speed 115200 cs8 -cstopb parenb -parodd