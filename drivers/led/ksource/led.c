#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/cdev.h>
#include<linux/fs.h>
#include <linux/ioctl.h>
#include<linux/types.h>
#include<linux/delay.h>
#include <linux/miscdevice.h>
 
#include <linux/gpio.h> //gpio 操作函数
#include <asm/io.h>     //io读取函数
#include <asm/uaccess.h>
 
#define DEVICE_NAME         "led"
 
#define LED_BASEADDR        0x43C00000
unsigned int* LED_Address   = 0;
 
int led_open(struct inode* inode,struct file* pfile);
int led_release(struct inode* inode,struct file* pfile);
long led_ioctl(struct file* pfile,unsigned int cmd,unsigned long arg);
 
static const struct file_operations led_fops = {
    .owner          = THIS_MODULE,
    .open           = led_open,
    .release        = led_release,
    .unlocked_ioctl = led_ioctl
};
 
int led_open(struct inode* inode,struct file* pfile)
{
    pr_info("%s \n", __func__);
    LED_Address = ioremap(LED_BASEADDR,12);
    return 0;
}
 
int led_release(struct inode* inode,struct file* pfile) 
{
    pr_info("%s \n", __func__);
    iounmap((void*)(LED_BASEADDR));    
    return 0;
}
 
long led_ioctl(struct file* pfile,unsigned int cmd,unsigned long arg) 
{
    pr_info("%s \n", __func__);
    *LED_Address = arg;
    return 0;
}
 
static struct miscdevice LED_misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name  = DEVICE_NAME,
    .fops  = &led_fops,
};

static int __init led_init(void) 
{
    int ret;
    ret = misc_register(&LED_misc);
    if(ret) {
        pr_info("%s: misc_register failed!\n", __func__);
        return 0;
    }
    else
        pr_info("%s: LED module register successfully!\n", __func__);

    return 0;
}
module_init(led_init);
 
static void __exit led_exit(void) 
{
    misc_deregister(&LED_misc);
    pr_info("%s:Exit module\n", __func__);
}
module_exit(led_exit);



MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("huqiqiao <huqq@zoonghe.com>");
MODULE_DESCRIPTION("Linux PL LED driver");
MODULE_INFO(intree, "Y");

