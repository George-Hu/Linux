#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>
 
#include <linux/gpio.h> //gpio 操作函数
#include <asm/io.h>     //io读取函数
#include <asm/uaccess.h>
 
static struct platform_device *platform_led_pdev;

static int __init platformleddev_init(void)
{
    int ret;

    platform_led_pdev = platform_device_alloc("platform_led",-1);
    if(!platform_led_pdev)
        return -ENOMEM;

    ret = platform_device_add(platform_led_pdev);
    if(ret) {
        platform_device_put(platform_led_pdev);
        return ret;
    }
    return 0;
}
module_init(platformleddev_init);


static void __exit platformleddev_exit(void)
{
    platform_device_unregister(platform_led_pdev);
}
module_exit(platformleddev_exit);


MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("huqiqiao <huqq@zoonghe.com>");
MODULE_DESCRIPTION("Linux pl platform led device");
MODULE_INFO(intree, "Y");

