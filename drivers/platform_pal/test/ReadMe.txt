

2023-05-01 
1、test_local.sh ko测试结果如下：

root@ubuntu:/home/share/MBCH20_SVN/MBCH20/board/drivers/platform_pal/ksource# ../test/test_local.sh ko
==========Test insmod & rmmod pal_device.ko & pal_driver.ko mode 1==========
/home/share/MBCH20_SVN/MBCH20/board/drivers/platform_pal/ksource
rm -rf  *.ko
rm -rf  *.o
rm -rf Module.*
rm -rf modules.order
rm -rf *mod.c
rm -rf .tmp_versions
rm -rf *.cmd
rm -rf  pal_device.o.ur-safe
rm -rf .pal_device.ko.cmd
rm -rf .pal_device.mod.o.cmd
rm -rf .pal_device.o.cmd
rm -rf  pal_driver.o.ur-safe
rm -rf .pal_driver.ko.cmd
rm -rf .pal_driver.mod.o.cmd
rm -rf .pal_driver.o.cmd
rm -rf .cache.mk
make  -C /lib/modules/4.15.0-142-generic/build M=/home/share/MBCH20_SVN/MBCH20/board/drivers/platform_pal/ksource modules
make[1]: Entering directory '/usr/src/linux-headers-4.15.0-142-generic'
  CC [M]  /home/share/MBCH20_SVN/MBCH20/board/drivers/platform_pal/ksource/pal_device.o
  CC [M]  /home/share/MBCH20_SVN/MBCH20/board/drivers/platform_pal/ksource/pal_driver.o
  Building modules, stage 2.
  MODPOST 2 modules
  CC      /home/share/MBCH20_SVN/MBCH20/board/drivers/platform_pal/ksource/pal_device.mod.o
  LD [M]  /home/share/MBCH20_SVN/MBCH20/board/drivers/platform_pal/ksource/pal_device.ko
  CC      /home/share/MBCH20_SVN/MBCH20/board/drivers/platform_pal/ksource/pal_driver.mod.o
  LD [M]  /home/share/MBCH20_SVN/MBCH20/board/drivers/platform_pal/ksource/pal_driver.ko
make[1]: Leaving directory '/usr/src/linux-headers-4.15.0-142-generic'
open /dev/pal0 Test Success!
open /dev/asi0 Test Success!
close /dev/pal0 Test Success!
close /dev/asi0 Test Success!
[  168.591764] platform_paldev_init ok
[  168.594432] platform_pal_probe
[  168.594433] pal_get_res_pdev OK.
[  168.594448] platform_pal_probe, devm_ioremap OK,io_base is 0000000074085d89.
[  168.594449] platform_pal_probe dev_name(dev) is pal, ppd->dev_id is 0.
[  168.594451] ========000000000000=========pal_chrdev_register ppd->cdev.dev = 0.
[  168.594451] pal_chrdev_register cdev_add 243 0 OK.
[  168.594452] ========1=========pal_chrdev_register ppd->cdev.dev = 254803968.
[  168.594475] pal_chrdev_register class_create pal_class OK.
[  168.594476] ============dev_name(ppd->dev) is pal.
[  168.594623]

               [platform_pal_dev_show ---> platform_pal_dev_show start--->]
[  168.594624] major                            :243
[  168.594625] minor                            :0
[  168.594625] dev                              :254803968
[  168.594625] name                             :pal

[  168.594625] devnum                           :254803968
[  168.594626] dev->kobj.name                   :pal

[  168.594626] dev_id                           :0x0
[  168.594627] cdev.dev                         :254803968
[  168.594627] cdev.count                       :1
[  168.594627] phys_base                        :10
[  168.594628] io_base                          :0000000074085d89
[  168.594628] [platform_pal_dev_show ---> platform_pal_dev_show end--->]

[  168.594629] pal_chrdev_register OK
[  168.594629] ========2=========platform_pal_probe ppd->cdev.dev = 254803968.
[  168.594629] platform_pal_probe ok
[  168.594645] platform_pal_probe
[  168.594645] pal_get_res_pdev OK.
[  168.594652] platform_pal_probe, devm_ioremap OK,io_base is 00000000a071a7b4.
[  168.594653] platform_pal_probe dev_name(dev) is asi, ppd->dev_id is 1.
[  168.594653] ========000000000000=========pal_chrdev_register ppd->cdev.dev = 0.
[  168.594654] pal_chrdev_register cdev_add 243 1 OK.
[  168.594655] ========1=========pal_chrdev_register ppd->cdev.dev = 254803969.
[  168.594655] ============dev_name(ppd->dev) is asi.
[  168.594801]

               [platform_pal_dev_show ---> platform_pal_dev_show start--->]
[  168.594802] major                            :243
[  168.594802] minor                            :1
[  168.594802] dev                              :254803969
[  168.594803] name                             :asi

[  168.594803] devnum                           :254803969
[  168.594803] dev->kobj.name                   :asi

[  168.594804] dev_id                           :0x1
[  168.594804] cdev.dev                         :254803969
[  168.594804] cdev.count                       :1
[  168.594805] phys_base                        :20
[  168.594805] io_base                          :00000000a071a7b4
[  168.594806] [platform_pal_dev_show ---> platform_pal_dev_show end--->]

[  168.594806] pal_chrdev_register OK
[  168.594806] ========2=========platform_pal_probe ppd->cdev.dev = 254803969.
[  168.594807] platform_pal_probe ok
[  169.597190] request irq 2 OK.
[  169.597192] ppd->io_base=0x0000000074085d89,ppd->phys_base=0x10
[  169.597767] request irq 3 OK.
[  169.597768] ppd->io_base=0x00000000a071a7b4,ppd->phys_base=0x20
[  170.602253] platform_pal_remove
[  170.602255]

               [platform_pal_dev_show ---> platform_pal_dev_show start--->]
[  170.602256] major                            :243
[  170.602256] minor                            :1
[  170.602257] dev                              :254803969
[  170.602257] name                             :asi

[  170.602257] devnum                           :254803969
[  170.602258] dev->kobj.name                   :asi

[  170.602258] dev_id                           :0x1
[  170.602258] cdev.dev                         :254803969
[  170.602258] cdev.count                       :1
[  170.602259] phys_base                        :20
[  170.602260] io_base                          :00000000a071a7b4
[  170.602260] [platform_pal_dev_show ---> platform_pal_dev_show end--->]

[  170.602261] pal_chrdev_deregister, ppd->devnum = 254803969,ppd->dev_id = 1
[  170.602263] pal_chrdev_deregister, ppd->devnum = 254803969,ppd->dev_id = 1
[  170.602560] pal_chrdev_deregister free_irq 3 ok
[  170.602565] pal_chrdev_deregister devm_iounmap 0x00000000a071a7b4 ok
[  170.602567] pal_chrdev_deregister ok
[  170.602567] platform_pal_remove OK
[  170.602577] platform_pal_remove
[  170.602577]

               [platform_pal_dev_show ---> platform_pal_dev_show start--->]
[  170.602578] major                            :243
[  170.602578] minor                            :0
[  170.602579] dev                              :254803968
[  170.602579] name                             :pal

[  170.602579] devnum                           :254803968
[  170.602579] dev->kobj.name                   :pal

[  170.602580] dev_id                           :0x0
[  170.602580] cdev.dev                         :254803968
[  170.602580] cdev.count                       :1
[  170.602581] phys_base                        :10
[  170.602581] io_base                          :0000000074085d89
[  170.602581] [platform_pal_dev_show ---> platform_pal_dev_show end--->]

[  170.602582] pal_chrdev_deregister, ppd->devnum = 254803968,ppd->dev_id = 0
[  170.602582] pal_chrdev_deregister, ppd->devnum = 254803968,ppd->dev_id = 0
[  170.602718] pal_chrdev_deregister free_irq 2 ok
[  170.602721] pal_chrdev_deregister devm_iounmap 0x0000000074085d89 ok
[  170.602721] pal_chrdev_deregister ok
[  170.602722] platform_pal_remove OK
[  170.627844] platform_device_fake_release ok.
[  170.627854] platform_device_fake_release ok.
[  170.627855] platform_paldev_exit ok
==========Test insmod & rmmod pal_device.ko & pal_driver.ko mode 2==========
/home/share/MBCH20_SVN/MBCH20/board/drivers/platform_pal/ksource
rm -rf  *.ko
rm -rf  *.o
rm -rf Module.*
rm -rf modules.order
rm -rf *mod.c
rm -rf .tmp_versions
rm -rf *.cmd
rm -rf  pal_device.o.ur-safe
rm -rf .pal_device.ko.cmd
rm -rf .pal_device.mod.o.cmd
rm -rf .pal_device.o.cmd
rm -rf  pal_driver.o.ur-safe
rm -rf .pal_driver.ko.cmd
rm -rf .pal_driver.mod.o.cmd
rm -rf .pal_driver.o.cmd
rm -rf .cache.mk
make  -C /lib/modules/4.15.0-142-generic/build M=/home/share/MBCH20_SVN/MBCH20/board/drivers/platform_pal/ksource modules
make[1]: Entering directory '/usr/src/linux-headers-4.15.0-142-generic'
  CC [M]  /home/share/MBCH20_SVN/MBCH20/board/drivers/platform_pal/ksource/pal_device.o
  CC [M]  /home/share/MBCH20_SVN/MBCH20/board/drivers/platform_pal/ksource/pal_driver.o
  Building modules, stage 2.
  MODPOST 2 modules
  CC      /home/share/MBCH20_SVN/MBCH20/board/drivers/platform_pal/ksource/pal_device.mod.o
  LD [M]  /home/share/MBCH20_SVN/MBCH20/board/drivers/platform_pal/ksource/pal_device.ko
  CC      /home/share/MBCH20_SVN/MBCH20/board/drivers/platform_pal/ksource/pal_driver.mod.o
  LD [M]  /home/share/MBCH20_SVN/MBCH20/board/drivers/platform_pal/ksource/pal_driver.ko
make[1]: Leaving directory '/usr/src/linux-headers-4.15.0-142-generic'
open /dev/pal0 Test Success!
open /dev/asi0 Test Success!
close /dev/pal0 Test Success!
close /dev/asi0 Test Success!
[  173.522204] platform_paldev_init ok
[  173.526330] platform_pal_probe
[  173.526331] pal_get_res_pdev OK.
[  173.526348] platform_pal_probe, devm_ioremap OK,io_base is 0000000074085d89.
[  173.526349] platform_pal_probe dev_name(dev) is pal, ppd->dev_id is 0.
[  173.526351] ========000000000000=========pal_chrdev_register ppd->cdev.dev = 0.
[  173.526352] pal_chrdev_register cdev_add 243 0 OK.
[  173.526352] ========1=========pal_chrdev_register ppd->cdev.dev = 254803968.
[  173.526377] pal_chrdev_register class_create pal_class OK.
[  173.526377] ============dev_name(ppd->dev) is pal.
[  173.526494]

               [platform_pal_dev_show ---> platform_pal_dev_show start--->]
[  173.526494] major                            :243
[  173.526495] minor                            :0
[  173.526495] dev                              :254803968
[  173.526495] name                             :pal

[  173.526496] devnum                           :254803968
[  173.526496] dev->kobj.name                   :pal

[  173.526496] dev_id                           :0x0
[  173.526497] cdev.dev                         :254803968
[  173.526497] cdev.count                       :1
[  173.526497] phys_base                        :10
[  173.526498] io_base                          :0000000074085d89
[  173.526498] [platform_pal_dev_show ---> platform_pal_dev_show end--->]

[  173.526499] pal_chrdev_register OK
[  173.526499] ========2=========platform_pal_probe ppd->cdev.dev = 254803968.
[  173.526500] platform_pal_probe ok
[  173.526515] platform_pal_probe
[  173.526515] pal_get_res_pdev OK.
[  173.526526] platform_pal_probe, devm_ioremap OK,io_base is 00000000a071a7b4.
[  173.526526] platform_pal_probe dev_name(dev) is asi, ppd->dev_id is 1.
[  173.526527] ========000000000000=========pal_chrdev_register ppd->cdev.dev = 0.
[  173.526528] pal_chrdev_register cdev_add 243 1 OK.
[  173.526528] ========1=========pal_chrdev_register ppd->cdev.dev = 254803969.
[  173.526529] ============dev_name(ppd->dev) is asi.
[  173.526717]

               [platform_pal_dev_show ---> platform_pal_dev_show start--->]
[  173.526718] major                            :243
[  173.526719] minor                            :1
[  173.526719] dev                              :254803969
[  173.526720] name                             :asi

[  173.526720] devnum                           :254803969
[  173.526721] dev->kobj.name                   :asi

[  173.526721] dev_id                           :0x1
[  173.526722] cdev.dev                         :254803969
[  173.526722] cdev.count                       :1
[  173.526722] phys_base                        :20
[  173.526723] io_base                          :00000000a071a7b4
[  173.526724] [platform_pal_dev_show ---> platform_pal_dev_show end--->]

[  173.526724] pal_chrdev_register OK
[  173.526725] ========2=========platform_pal_probe ppd->cdev.dev = 254803969.
[  173.526725] platform_pal_probe ok
[  174.529079] request irq 2 OK.
[  174.529081] ppd->io_base=0x0000000074085d89,ppd->phys_base=0x10
[  174.529623] request irq 3 OK.
[  174.529624] ppd->io_base=0x00000000a071a7b4,ppd->phys_base=0x20
[  175.532969] platform_pal_remove
[  175.532971]

               [platform_pal_dev_show ---> platform_pal_dev_show start--->]
[  175.532972] major                            :243
[  175.532972] minor                            :0
[  175.532973] dev                              :254803968
[  175.532973] name                             :pal

[  175.532973] devnum                           :254803968
[  175.532974] dev->kobj.name                   :pal

[  175.532974] dev_id                           :0x0
[  175.532974] cdev.dev                         :254803968
[  175.532975] cdev.count                       :1
[  175.532975] phys_base                        :10
[  175.532976] io_base                          :0000000074085d89
[  175.532976] [platform_pal_dev_show ---> platform_pal_dev_show end--->]

[  175.532977] pal_chrdev_deregister, ppd->devnum = 254803968,ppd->dev_id = 0
[  175.532979] pal_chrdev_deregister, ppd->devnum = 254803968,ppd->dev_id = 0
[  175.533195] pal_chrdev_deregister free_irq 2 ok
[  175.533200] pal_chrdev_deregister devm_iounmap 0x0000000074085d89 ok
[  175.533202] pal_chrdev_deregister ok
[  175.533202] platform_pal_remove OK
[  175.533211] platform_device_fake_release ok.
[  175.533221] platform_pal_remove
[  175.533222]

               [platform_pal_dev_show ---> platform_pal_dev_show start--->]
[  175.533222] major                            :243
[  175.533223] minor                            :1
[  175.533223] dev                              :254803969
[  175.533223] name                             :asi

[  175.533224] devnum                           :254803969
[  175.533224] dev->kobj.name                   :asi

[  175.533224] dev_id                           :0x1
[  175.533225] cdev.dev                         :254803969
[  175.533225] cdev.count                       :1
[  175.533225] phys_base                        :20
[  175.533226] io_base                          :00000000a071a7b4
[  175.533226] [platform_pal_dev_show ---> platform_pal_dev_show end--->]

[  175.533227] pal_chrdev_deregister, ppd->devnum = 254803969,ppd->dev_id = 1
[  175.533227] pal_chrdev_deregister, ppd->devnum = 254803969,ppd->dev_id = 1
[  175.533435] pal_chrdev_deregister free_irq 3 ok
[  175.533438] pal_chrdev_deregister devm_iounmap 0x00000000a071a7b4 ok
[  175.533439] pal_chrdev_deregister ok
[  175.533439] platform_pal_remove OK
[  175.533448] platform_device_fake_release ok.
[  175.533449] platform_paldev_exit ok
==========Test insmod & rmmod pal_device.ko & pal_driver.ko mode 3==========
/home/share/MBCH20_SVN/MBCH20/board/drivers/platform_pal/ksource
rm -rf  *.ko
rm -rf  *.o
rm -rf Module.*
rm -rf modules.order
rm -rf *mod.c
rm -rf .tmp_versions
rm -rf *.cmd
rm -rf  pal_device.o.ur-safe
rm -rf .pal_device.ko.cmd
rm -rf .pal_device.mod.o.cmd
rm -rf .pal_device.o.cmd
rm -rf  pal_driver.o.ur-safe
rm -rf .pal_driver.ko.cmd
rm -rf .pal_driver.mod.o.cmd
rm -rf .pal_driver.o.cmd
rm -rf .cache.mk
make  -C /lib/modules/4.15.0-142-generic/build M=/home/share/MBCH20_SVN/MBCH20/board/drivers/platform_pal/ksource modules
make[1]: Entering directory '/usr/src/linux-headers-4.15.0-142-generic'
  CC [M]  /home/share/MBCH20_SVN/MBCH20/board/drivers/platform_pal/ksource/pal_device.o
  CC [M]  /home/share/MBCH20_SVN/MBCH20/board/drivers/platform_pal/ksource/pal_driver.o
  Building modules, stage 2.
  MODPOST 2 modules
  CC      /home/share/MBCH20_SVN/MBCH20/board/drivers/platform_pal/ksource/pal_device.mod.o
  LD [M]  /home/share/MBCH20_SVN/MBCH20/board/drivers/platform_pal/ksource/pal_device.ko
  CC      /home/share/MBCH20_SVN/MBCH20/board/drivers/platform_pal/ksource/pal_driver.mod.o
  LD [M]  /home/share/MBCH20_SVN/MBCH20/board/drivers/platform_pal/ksource/pal_driver.ko
make[1]: Leaving directory '/usr/src/linux-headers-4.15.0-142-generic'
open /dev/pal0 Test Success!
open /dev/asi0 Test Success!
close /dev/pal0 Test Success!
close /dev/asi0 Test Success!
[  178.405074] platform_pal_probe
[  178.405076] pal_get_res_pdev OK.
[  178.405099] platform_pal_probe, devm_ioremap OK,io_base is 0000000020ac4ebb.
[  178.405100] platform_pal_probe dev_name(dev) is pal, ppd->dev_id is 0.
[  178.405103] ========000000000000=========pal_chrdev_register ppd->cdev.dev = 0.
[  178.405104] pal_chrdev_register cdev_add 243 0 OK.
[  178.405104] ========1=========pal_chrdev_register ppd->cdev.dev = 254803968.
[  178.405114] pal_chrdev_register class_create pal_class OK.
[  178.405114] ============dev_name(ppd->dev) is pal.
[  178.405240]

               [platform_pal_dev_show ---> platform_pal_dev_show start--->]
[  178.405241] major                            :243
[  178.405241] minor                            :0
[  178.405242] dev                              :254803968
[  178.405243] name                             :pal

[  178.405243] devnum                           :254803968
[  178.405244] dev->kobj.name                   :pal

[  178.405244] dev_id                           :0x0
[  178.405244] cdev.dev                         :254803968
[  178.405245] cdev.count                       :1
[  178.405245] phys_base                        :10
[  178.405246] io_base                          :0000000020ac4ebb
[  178.405246] [platform_pal_dev_show ---> platform_pal_dev_show end--->]

[  178.405247] pal_chrdev_register OK
[  178.405247] ========2=========platform_pal_probe ppd->cdev.dev = 254803968.
[  178.405248] platform_pal_probe ok
[  178.405307] platform_pal_probe
[  178.405309] pal_get_res_pdev OK.
[  178.405322] platform_pal_probe, devm_ioremap OK,io_base is 0000000097db023e.
[  178.405322] platform_pal_probe dev_name(dev) is asi, ppd->dev_id is 1.
[  178.405323] ========000000000000=========pal_chrdev_register ppd->cdev.dev = 0.
[  178.405324] pal_chrdev_register cdev_add 243 1 OK.
[  178.405324] ========1=========pal_chrdev_register ppd->cdev.dev = 254803969.
[  178.405324] ============dev_name(ppd->dev) is asi.
[  178.405450]

               [platform_pal_dev_show ---> platform_pal_dev_show start--->]
[  178.405451] major                            :243
[  178.405451] minor                            :1
[  178.405451] dev                              :254803969
[  178.405452] name                             :asi

[  178.405452] devnum                           :254803969
[  178.405452] dev->kobj.name                   :asi

[  178.405453] dev_id                           :0x1
[  178.405453] cdev.dev                         :254803969
[  178.405453] cdev.count                       :1
[  178.405454] phys_base                        :20
[  178.405454] io_base                          :0000000097db023e
[  178.405454] [platform_pal_dev_show ---> platform_pal_dev_show end--->]

[  178.405455] pal_chrdev_register OK
[  178.405455] ========2=========platform_pal_probe ppd->cdev.dev = 254803969.
[  178.405455] platform_pal_probe ok
[  178.405465] platform_paldev_init ok
[  179.408914] request irq 2 OK.
[  179.408917] ppd->io_base=0x0000000020ac4ebb,ppd->phys_base=0x10
[  179.409437] request irq 3 OK.
[  179.409439] ppd->io_base=0x0000000097db023e,ppd->phys_base=0x20
[  180.412844] platform_pal_remove
[  180.412845]

               [platform_pal_dev_show ---> platform_pal_dev_show start--->]
[  180.412846] major                            :243
[  180.412846] minor                            :0
[  180.412847] dev                              :254803968
[  180.412847] name                             :pal

[  180.412847] devnum                           :254803968
[  180.412848] dev->kobj.name                   :pal

[  180.412848] dev_id                           :0x0
[  180.412848] cdev.dev                         :254803968
[  180.412848] cdev.count                       :1
[  180.412849] phys_base                        :10
[  180.412850] io_base                          :0000000020ac4ebb
[  180.412850] [platform_pal_dev_show ---> platform_pal_dev_show end--->]

[  180.412850] pal_chrdev_deregister, ppd->devnum = 254803968,ppd->dev_id = 0
[  180.412852] pal_chrdev_deregister, ppd->devnum = 254803968,ppd->dev_id = 0
[  180.413079] pal_chrdev_deregister free_irq 2 ok
[  180.413083] pal_chrdev_deregister devm_iounmap 0x0000000020ac4ebb ok
[  180.413085] pal_chrdev_deregister ok
[  180.413085] platform_pal_remove OK
[  180.413094] platform_device_fake_release ok.
[  180.413104] platform_pal_remove
[  180.413104]

               [platform_pal_dev_show ---> platform_pal_dev_show start--->]
[  180.413105] major                            :243
[  180.413105] minor                            :1
[  180.413105] dev                              :254803969
[  180.413106] name                             :asi

[  180.413106] devnum                           :254803969
[  180.413106] dev->kobj.name                   :asi

[  180.413107] dev_id                           :0x1
[  180.413107] cdev.dev                         :254803969
[  180.413107] cdev.count                       :1
[  180.413108] phys_base                        :20
[  180.413108] io_base                          :0000000097db023e
[  180.413108] [platform_pal_dev_show ---> platform_pal_dev_show end--->]

[  180.413109] pal_chrdev_deregister, ppd->devnum = 254803969,ppd->dev_id = 1
[  180.413109] pal_chrdev_deregister, ppd->devnum = 254803969,ppd->dev_id = 1
[  180.413322] pal_chrdev_deregister free_irq 3 ok
[  180.413325] pal_chrdev_deregister devm_iounmap 0x0000000097db023e ok
[  180.413326] pal_chrdev_deregister ok
[  180.413326] platform_pal_remove OK
[  180.413335] platform_device_fake_release ok.
[  180.413336] platform_paldev_exit ok
==========Test insmod & rmmod pal_device.ko & pal_driver.ko mode 4==========
/home/share/MBCH20_SVN/MBCH20/board/drivers/platform_pal/ksource
rm -rf  *.ko
rm -rf  *.o
rm -rf Module.*
rm -rf modules.order
rm -rf *mod.c
rm -rf .tmp_versions
rm -rf *.cmd
rm -rf  pal_device.o.ur-safe
rm -rf .pal_device.ko.cmd
rm -rf .pal_device.mod.o.cmd
rm -rf .pal_device.o.cmd
rm -rf  pal_driver.o.ur-safe
rm -rf .pal_driver.ko.cmd
rm -rf .pal_driver.mod.o.cmd
rm -rf .pal_driver.o.cmd
rm -rf .cache.mk
make  -C /lib/modules/4.15.0-142-generic/build M=/home/share/MBCH20_SVN/MBCH20/board/drivers/platform_pal/ksource modules
make[1]: Entering directory '/usr/src/linux-headers-4.15.0-142-generic'
  CC [M]  /home/share/MBCH20_SVN/MBCH20/board/drivers/platform_pal/ksource/pal_device.o
  CC [M]  /home/share/MBCH20_SVN/MBCH20/board/drivers/platform_pal/ksource/pal_driver.o
  Building modules, stage 2.
  MODPOST 2 modules
  CC      /home/share/MBCH20_SVN/MBCH20/board/drivers/platform_pal/ksource/pal_device.mod.o
  LD [M]  /home/share/MBCH20_SVN/MBCH20/board/drivers/platform_pal/ksource/pal_device.ko
  CC      /home/share/MBCH20_SVN/MBCH20/board/drivers/platform_pal/ksource/pal_driver.mod.o
  LD [M]  /home/share/MBCH20_SVN/MBCH20/board/drivers/platform_pal/ksource/pal_driver.ko
make[1]: Leaving directory '/usr/src/linux-headers-4.15.0-142-generic'
open /dev/pal0 Test Success!
open /dev/asi0 Test Success!
close /dev/pal0 Test Success!
close /dev/asi0 Test Success!
[  183.420308] platform_pal_probe
[  183.420309] pal_get_res_pdev OK.
[  183.420323] platform_pal_probe, devm_ioremap OK,io_base is 0000000020ac4ebb.
[  183.420324] platform_pal_probe dev_name(dev) is pal, ppd->dev_id is 0.
[  183.420326] ========000000000000=========pal_chrdev_register ppd->cdev.dev = 0.
[  183.420326] pal_chrdev_register cdev_add 243 0 OK.
[  183.420327] ========1=========pal_chrdev_register ppd->cdev.dev = 254803968.
[  183.420332] pal_chrdev_register class_create pal_class OK.
[  183.420333] ============dev_name(ppd->dev) is pal.
[  183.420415]

               [platform_pal_dev_show ---> platform_pal_dev_show start--->]
[  183.420416] major                            :243
[  183.420416] minor                            :0
[  183.420416] dev                              :254803968
[  183.420417] name                             :pal

[  183.420417] devnum                           :254803968
[  183.420417] dev->kobj.name                   :pal

[  183.420418] dev_id                           :0x0
[  183.420418] cdev.dev                         :254803968
[  183.420418] cdev.count                       :1
[  183.420419] phys_base                        :10
[  183.420419] io_base                          :0000000020ac4ebb
[  183.420420] [platform_pal_dev_show ---> platform_pal_dev_show end--->]

[  183.420420] pal_chrdev_register OK
[  183.420420] ========2=========platform_pal_probe ppd->cdev.dev = 254803968.
[  183.420421] platform_pal_probe ok
[  183.420450] platform_pal_probe
[  183.420451] pal_get_res_pdev OK.
[  183.420458] platform_pal_probe, devm_ioremap OK,io_base is 0000000097db023e.
[  183.420458] platform_pal_probe dev_name(dev) is asi, ppd->dev_id is 1.
[  183.420459] ========000000000000=========pal_chrdev_register ppd->cdev.dev = 0.
[  183.420459] pal_chrdev_register cdev_add 243 1 OK.
[  183.420460] ========1=========pal_chrdev_register ppd->cdev.dev = 254803969.
[  183.420460] ============dev_name(ppd->dev) is asi.
[  183.420592]

               [platform_pal_dev_show ---> platform_pal_dev_show start--->]
[  183.420593] major                            :243
[  183.420593] minor                            :1
[  183.420594] dev                              :254803969
[  183.420594] name                             :asi

[  183.420595] devnum                           :254803969
[  183.420595] dev->kobj.name                   :asi

[  183.420595] dev_id                           :0x1
[  183.420596] cdev.dev                         :254803969
[  183.420596] cdev.count                       :1
[  183.420597] phys_base                        :20
[  183.420597] io_base                          :0000000097db023e
[  183.420598] [platform_pal_dev_show ---> platform_pal_dev_show end--->]

[  183.420598] pal_chrdev_register OK
[  183.420599] ========2=========platform_pal_probe ppd->cdev.dev = 254803969.
[  183.420599] platform_pal_probe ok
[  183.420614] platform_paldev_init ok
[  184.423580] request irq 2 OK.
[  184.423582] ppd->io_base=0x0000000020ac4ebb,ppd->phys_base=0x10
[  184.424285] request irq 3 OK.
[  184.424287] ppd->io_base=0x0000000097db023e,ppd->phys_base=0x20
[  185.428886] platform_pal_remove
[  185.428888]

               [platform_pal_dev_show ---> platform_pal_dev_show start--->]
[  185.428889] major                            :243
[  185.428889] minor                            :1
[  185.428889] dev                              :254803969
[  185.428890] name                             :asi

[  185.428890] devnum                           :254803969
[  185.428890] dev->kobj.name                   :asi

[  185.428891] dev_id                           :0x1
[  185.428891] cdev.dev                         :254803969
[  185.428891] cdev.count                       :1
[  185.428892] phys_base                        :20
[  185.428893] io_base                          :0000000097db023e
[  185.428894] [platform_pal_dev_show ---> platform_pal_dev_show end--->]

[  185.428894] pal_chrdev_deregister, ppd->devnum = 254803969,ppd->dev_id = 1
[  185.428896] pal_chrdev_deregister, ppd->devnum = 254803969,ppd->dev_id = 1
[  185.429181] pal_chrdev_deregister free_irq 3 ok
[  185.429186] pal_chrdev_deregister devm_iounmap 0x0000000097db023e ok
[  185.429188] pal_chrdev_deregister ok
[  185.429188] platform_pal_remove OK
[  185.429198] platform_pal_remove
[  185.429198]

               [platform_pal_dev_show ---> platform_pal_dev_show start--->]
[  185.429199] major                            :243
[  185.429199] minor                            :0
[  185.429199] dev                              :254803968
[  185.429200] name                             :pal

[  185.429200] devnum                           :254803968
[  185.429200] dev->kobj.name                   :pal

[  185.429201] dev_id                           :0x0
[  185.429201] cdev.dev                         :254803968
[  185.429201] cdev.count                       :1
[  185.429202] phys_base                        :10
[  185.429202] io_base                          :0000000020ac4ebb
[  185.429202] [platform_pal_dev_show ---> platform_pal_dev_show end--->]

[  185.429203] pal_chrdev_deregister, ppd->devnum = 254803968,ppd->dev_id = 0
[  185.429203] pal_chrdev_deregister, ppd->devnum = 254803968,ppd->dev_id = 0
[  185.429404] pal_chrdev_deregister free_irq 2 ok
[  185.429408] pal_chrdev_deregister devm_iounmap 0x0000000020ac4ebb ok
[  185.429409] pal_chrdev_deregister ok
[  185.429409] platform_pal_remove OK
[  185.450530] platform_device_fake_release ok.
[  185.450541] platform_device_fake_release ok.
[  185.450542] platform_paldev_exit ok
