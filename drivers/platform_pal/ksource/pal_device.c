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
 
/*#include <asm/io.h>
#include <asm/uaccess.h>*/

#define X86_64
/*
root@ubuntu:/home/share/MBCH20_SVN/MBCH20/board/drivers/platform_pal/ksource# cat /proc/iomem
00000000-00000fff : Reserved
00001000-0009e7ff : System RAM
0009e800-0009ffff : Reserved
000a0000-000bffff : PCI Bus 0000:00
000c0000-000c7fff : Video ROM
000ca000-000cafff : Adapter ROM
000cb000-000ccfff : Adapter ROM
000d0000-000d3fff : PCI Bus 0000:00
000d4000-000d7fff : PCI Bus 0000:00
000d8000-000dbfff : PCI Bus 0000:00
000dc000-000fffff : Reserved
  000f0000-000fffff : System ROM
00100000-bfecffff : System RAM
  61800000-624031d0 : Kernel code
  624031d1-62e6c93f : Kernel data
  630ed000-63395fff : Kernel bss
bfed0000-bfefefff : ACPI Tables
bfeff000-bfefffff : ACPI Non-volatile Storage
bff00000-bfffffff : System RAM
c0000000-febfffff : PCI Bus 0000:00
  c0008000-c000bfff : 0000:00:10.0
  e5b00000-e5bfffff : PCI Bus 0000:22
  e5c00000-e5cfffff : PCI Bus 0000:1a
  e5d00000-e5dfffff : PCI Bus 0000:12
  e5e00000-e5efffff : PCI Bus 0000:0a
  e5f00000-e5ffffff : PCI Bus 0000:21
  e6000000-e60fffff : PCI Bus 0000:19
  e6100000-e61fffff : PCI Bus 0000:11
  e6200000-e62fffff : PCI Bus 0000:09
  e6300000-e63fffff : PCI Bus 0000:20
  e6400000-e64fffff : PCI Bus 0000:18
  e6500000-e65fffff : PCI Bus 0000:10
  e6600000-e66fffff : PCI Bus 0000:08
  e6700000-e67fffff : PCI Bus 0000:1f
  e6800000-e68fffff : PCI Bus 0000:17
  e6900000-e69fffff : PCI Bus 0000:0f
  e6a00000-e6afffff : PCI Bus 0000:07
  e6b00000-e6bfffff : PCI Bus 0000:1e
  e6c00000-e6cfffff : PCI Bus 0000:16
  e6d00000-e6dfffff : PCI Bus 0000:0e
  e6e00000-e6efffff : PCI Bus 0000:06
  e6f00000-e6ffffff : PCI Bus 0000:1d
  e7000000-e70fffff : PCI Bus 0000:15
  e7100000-e71fffff : PCI Bus 0000:0d
  e7200000-e72fffff : PCI Bus 0000:05
  e7300000-e73fffff : PCI Bus 0000:1c
  e7400000-e74fffff : PCI Bus 0000:14
  e7500000-e75fffff : PCI Bus 0000:0c
  e7600000-e76fffff : PCI Bus 0000:04
  e7700000-e77fffff : PCI Bus 0000:1b
  e7800000-e78fffff : PCI Bus 0000:13
  e7900000-e79fffff : PCI Bus 0000:0b
  e7a00000-e7afffff : PCI Bus 0000:03
  e7b00000-e7ffffff : PCI Bus 0000:02
  e8000000-efffffff : 0000:00:0f.0
    e8000000-efffffff : vmwgfx probe
  f0000000-f7ffffff : PCI MMCONFIG 0000 [bus 00-7f]
    f0000000-f7ffffff : Reserved
      f0000000-f7ffffff : pnp 00:06
  fb500000-fb5fffff : PCI Bus 0000:22
  fb600000-fb6fffff : PCI Bus 0000:1a
  fb700000-fb7fffff : PCI Bus 0000:12
  fb800000-fb8fffff : PCI Bus 0000:0a
  fb900000-fb9fffff : PCI Bus 0000:21
  fba00000-fbafffff : PCI Bus 0000:19
  fbb00000-fbbfffff : PCI Bus 0000:11
  fbc00000-fbcfffff : PCI Bus 0000:09
  fbd00000-fbdfffff : PCI Bus 0000:20
  fbe00000-fbefffff : PCI Bus 0000:18
  fbf00000-fbffffff : PCI Bus 0000:10
  fc000000-fc0fffff : PCI Bus 0000:08
  fc100000-fc1fffff : PCI Bus 0000:1f
  fc200000-fc2fffff : PCI Bus 0000:17
  fc300000-fc3fffff : PCI Bus 0000:0f
  fc400000-fc4fffff : PCI Bus 0000:07
  fc500000-fc5fffff : PCI Bus 0000:1e
  fc600000-fc6fffff : PCI Bus 0000:16
  fc700000-fc7fffff : PCI Bus 0000:0e
  fc800000-fc8fffff : PCI Bus 0000:06
  fc900000-fc9fffff : PCI Bus 0000:1d
  fca00000-fcafffff : PCI Bus 0000:15
  fcb00000-fcbfffff : PCI Bus 0000:0d
  fcc00000-fccfffff : PCI Bus 0000:05
  fcd00000-fcdfffff : PCI Bus 0000:1c
  fce00000-fcefffff : PCI Bus 0000:14
  fcf00000-fcffffff : PCI Bus 0000:0c
  fd000000-fd0fffff : PCI Bus 0000:04
  fd100000-fd1fffff : PCI Bus 0000:1b
  fd200000-fd2fffff : PCI Bus 0000:13
  fd300000-fd3fffff : PCI Bus 0000:0b
  fd400000-fd4fffff : PCI Bus 0000:03
  fd500000-fdffffff : PCI Bus 0000:02
    fd500000-fd50ffff : 0000:02:01.0
    fd510000-fd51ffff : 0000:02:05.0
    fd5c0000-fd5dffff : 0000:02:01.0
      fd5c0000-fd5dffff : e1000
    fd5ee000-fd5eefff : 0000:02:05.0
      fd5ee000-fd5eefff : ahci
    fd5ef000-fd5effff : 0000:02:03.0
      fd5ef000-fd5effff : ehci_hcd
    fdff0000-fdffffff : 0000:02:01.0
      fdff0000-fdffffff : e1000
  fe000000-fe7fffff : 0000:00:0f.0
    fe000000-fe7fffff : vmwgfx probe
  fe800000-fe9fffff : pnp 00:06
  feba0000-febbffff : 0000:00:10.0
    feba0000-febbffff : mpt
  febc0000-febdffff : 0000:00:10.0
    febc0000-febdffff : mpt
  febfe000-febfffff : 0000:00:07.7
fec00000-fec0ffff : Reserved
  fec00000-fec003ff : IOAPIC 0
fed00000-fed003ff : HPET 0
  fed00000-fed003ff : pnp 00:04
fee00000-fee00fff : Local APIC
  fee00000-fee00fff : Reserved
fffe0000-ffffffff : Reserved
100000000-13fffffff : System RAM

*/

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
static struct platform_device platform_pal_pdev[]={
    {
        .name           = "platform_pal",
        .id             = 0,
        .dev.init_name  = "pal",
/*
        .dev  = {
            .platform_data = &paldev[0];
        },
*/
        .num_resources = ARRAY_SIZE(pal_resource),
        .resource = pal_resource,
    },
    {
        .name           = "platform_pal",
        .id             = 1,
        .dev.init_name  = "asi",        
/*        
        .dev  = {
            .platform_data = &paldev[1];
        },
*/
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
