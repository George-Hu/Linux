#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <asm/io.h>     //io读取函数
#include <asm/uaccess.h>


#define X86_64

#ifndef X86_64
#define PAL_FPGA_BASE_ADDR          0x40000000
#define PAL_PING_ADDR               0x10
#define ASI_PING_ADDR               0x20
#define PAL_RX_IRQ_NUMBER           61
#define ASI_RX_IRQ_NUMBER           62
#else
#define PAL_FPGA_BASE_ADDR          0x00000000 //x86_64
#define PAL_PING_ADDR               0x10
#define ASI_PING_ADDR               0x20
#define PAL_RX_IRQ_NUMBER           2
#define ASI_RX_IRQ_NUMBER           3
#endif


/* 1、定义设备资源，填写资源表  */
static struct resource pal_resource[] = {
    [0] = {
        .name   = "pal-iomem",
        .start  = (PAL_FPGA_BASE_ADDR + PAL_PING_ADDR),
        .end    = (PAL_FPGA_BASE_ADDR + PAL_PING_ADDR + 15),
        .flags  = IORESOURCE_MEM,
    },
    [1] = {
        .name   = "pal-irq",
        .start  = PAL_RX_IRQ_NUMBER,
        .flags  = IORESOURCE_IRQ,
    },
};

static struct resource asi_resource[] = {
    [0] = {
        .name   = "asi-iomem",
        .start  = (PAL_FPGA_BASE_ADDR + ASI_PING_ADDR),
        .end    = (PAL_FPGA_BASE_ADDR + ASI_PING_ADDR + 15),
        .flags  = IORESOURCE_MEM,
    },
    [1] = {
        .name   = "asi-irq",
        .start  = ASI_RX_IRQ_NUMBER,
        .flags  = IORESOURCE_IRQ,
    },   
};

/* 2、填写platform_device信息 */
static void platform_device_fake_release(struct device *dev)
{
    pr_info("%s ok. ",__func__);
    dev = NULL;
}

static struct platform_device platform_pal_pdev[]={
    {
        .name           = "platform_pal",
        .id             = 0,
        .dev  = {
            .init_name  = "pal",
            .release = platform_device_fake_release,
        },
        .num_resources = ARRAY_SIZE(pal_resource),
        .resource = pal_resource,
    },
    {
        .name           = "platform_pal",
        .id             = 1, 
        .dev  = {
            .init_name  = "asi",
            .release = platform_device_fake_release,
        },
        .num_resources = ARRAY_SIZE(asi_resource),
        .resource = asi_resource,        
    },    
};
#define NR_DEVICE ARRAY_SIZE(platform_pal_pdev)


/* 3、注册设备资源信息 */

static int __init platform_paldev_init(void)
{
    int i,err;

    for (i=0; i<NR_DEVICE; i++) {
        err = platform_device_register(&platform_pal_pdev[i]);
        if (err)
            goto err;
    }
    pr_info("%s ok \n", __func__);
    return 0;

err:
    while (i--) {
        platform_device_unregister(&platform_pal_pdev[i]);
    }
    pr_info("%s error \n", __func__);
    return err;
}
module_init(platform_paldev_init);


static void __exit platform_paldev_exit(void)
{
    int i;

    for (i=0; i<NR_DEVICE; i++) {
        platform_device_unregister(&platform_pal_pdev[i]);
    }
    pr_info("%s ok \n", __func__);
}
module_exit(platform_paldev_exit);


MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("huqiqiao <huqq@zoonghe.com>");
MODULE_DESCRIPTION("Linux pl platform pal device");
MODULE_INFO(intree, "Y");
