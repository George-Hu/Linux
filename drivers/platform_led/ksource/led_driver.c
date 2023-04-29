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
 
static struct platform_led_dev {
    //struct cdev cdev;
    struct miscdevice miscdev;
};
 
#define LED_BASEADDR        0x43C00000
unsigned int* LED_Address   = 0;



static int platform_led_open(struct inode* inode,struct file* flip)
{
    pr_info("%s \n", __func__);
    //struct platform_led_dev *dev = container_of(flip->private_data,struct platform_led_dev,miscdev);

    LED_Address = ioremap(LED_BASEADDR,12);
    return 0;
}
 
static int platform_led_release(struct inode* inode,struct file* flip) 
{
    pr_info("%s \n", __func__);
    //struct platform_led_dev *dev = container_of(flip->private_data,struct platform_led_dev,miscdev);

    iounmap((void*)(LED_BASEADDR));    
    return 0;
}
 
static long platform_led_ioctl(struct file* flip,unsigned int cmd,unsigned long arg) 
{
    if (2 == arg)
        pr_info("%s arg = 0x%lx.\n", __func__,arg);
    //struct platform_led_dev *dev = container_of(flip->private_data,struct platform_led_dev,miscdev);

    *LED_Address = arg;
    return 0;
}

static const struct file_operations platform_led_fops = {
    .open           = platform_led_open,
    .release        = platform_led_release,
    .unlocked_ioctl = platform_led_ioctl,
};
 
static int platform_led_probe(struct platform_device *pdev)
{
    struct platform_led_dev *pl;
    int ret;

    pl = devm_kzalloc(&pdev->dev,sizeof(*pl),GFP_KERNEL);
    if(!pl)
        return -ENOMEM;

    pl->miscdev.minor = MISC_DYNAMIC_MINOR;
    pl->miscdev.name  = "platform_led";
    pl->miscdev.fops  = &platform_led_fops;

    platform_set_drvdata(pdev,pl);

    ret = misc_register(&pl->miscdev);
    if(ret < 0)
        goto err;

    return 0;

err:
    return ret;
}

static int platform_led_remove(struct platform_device *pdev)
{
    struct platform_led_dev *pl = platform_get_drvdata(pdev);
    misc_deregister(&pl->miscdev);
    return 0;
}

static struct platform_driver platform_led_driver = {
    .driver = {
        .name   = "platform_led",
        .owner  = THIS_MODULE,
    },
    .probe  = platform_led_probe,
    .remove = platform_led_remove,
};
module_platform_driver(platform_led_driver);


MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("huqiqiao <huqq@zoonghe.com>");
MODULE_DESCRIPTION("Linux pl platform led driver");
MODULE_INFO(intree, "Y");

