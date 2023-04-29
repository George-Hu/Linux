#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/major.h>
#include <linux/errno.h>
#include <asm/irq.h>
#include <linux/interrupt.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/platform_device.h>

#include "../include/pal.h"

#include <asm/io.h>
#include <asm/uaccess.h>

#include <linux/io.h> 

static int pal_major=0;
static struct class *pal_class;

#define DEVICE_NAME 				"pal"

#define PONG_ADDR_OFFSET	0x4
#define RX_LEN_OFFSET		0x8
#define DMA_STATUS_OFFSET	0xC
	#define PING_DMA_NO_START			((__u8)((0<<1)|(1<<0)))	
	#define PING_DMA_START              ((__u8)((0<<1)|(1<<0)))
	#define PING_DMA_PROCESS			((__u8)((1<<1)|(0<<0)))
	#define PING_DMA_COMPLETE			((__u8)((1<<1)|(1<<0)))
	#define PONG_DMA_NO_START			((__u8)((0<<3)|(1<<2)))
	#define PONG_DMA_START              ((__u8)((0<<3)|(1<<2)))
	#define PONG_DMA_PROCESS			((__u8)((1<<3)|(0<<2)))
	#define PONG_DMA_COMPLETE			((__u8)((1<<3)|(1<<2)))


/* 1、定义platform设备结构体变量 */
struct platform_pal_pdata {
	unsigned int (*read_fn)(void __iomem *);
	void (*write_fn)(u32, void __iomem *);
	irqreturn_t	(*intr_hdlr)(int irq, void *dev_id);
};

/* 2、 定义pal_proc_fs_t用于故障定位 */
struct pal_proc_fs_t {
	int         pal_interrupt_cnt;
	int 		pal_rx_ping_int_cnt;
	int  		pal_rx_pong_int_cnt;
	int         pal_kfifo_empty_cnt;
	int         pal_kfifo_full_cnt;
	int         pal_not_ping_pong_int_cnt;

	int         asi_interrupt_cnt;
	int 		asi_rx_ping_int_cnt;
	int  		asi_rx_pong_int_cnt;
	int         asi_kfifo_empty_cnt;
	int         asi_kfifo_full_cnt;
	int         asi_not_ping_pong_int_cnt;

	int         unknow_read_error_cnt;
}pal_proc_fs;

struct platform_pal_dev {
    dev_t devnum;              //设备号
    struct cdev 		cdev;
    struct device 		*dev;
    unsigned long		phys_base;
    /* device resource start */
    void __iomem 		*io_base;
    int                 irq;              /* irq number */
    /* device resource end */

    spinlock_t          lock;
    wait_queue_head_t   pal_queue;
    int                 rx_interrupt_flag; /* used to wake up read wait queue*/

    int                 irq_init_flag;    /* request irq flag */
    //irq_handler_t       handler;
    const char *        name;             /* interrupt name */
    spinlock_t          irq_lock;
    
    /* for user used */
    struct pal_info     pal_info;
    struct pal_fpga_ver pal_fpga_ver;

    const struct platform_pal_pdata	*pdata;
};

/* 2、字符设备操作函数集实现 */
int pal_open (struct inode *inode, struct file *filp) 
{
	struct platform_pal_dev *ppd;
	int err;
	
	ppd = container_of(inode->i_cdev, struct platform_pal_dev, cdev);
	if (ppd) {
		filp->private_data = ppd;
		kobject_get(&ppd->dev->kobj);
	}

/*	
	pr_info("%s ppd->cdev.dev=%d,  ppd->cdev.count=%d",__func__,ppd->cdev.dev,ppd->cdev.dev);
	pr_info("ppd->irq_init_flag=%d,ppd->name=%s.\n",ppd->irq_init_flag,ppd->name);
*/

	if (!ppd->irq_init_flag) {
	    err = devm_request_irq(ppd->dev, ppd->irq, ppd->pdata->intr_hdlr,
				IRQF_TRIGGER_HIGH, dev_name(ppd->dev), ppd);
	    if (err) {
	    	pr_err("%s unable to request irq %d, err = %d\n",__func__, ppd->irq, err);
	    	return -1;
	    }

	    /* Initial wait queue */
		init_waitqueue_head(&ppd->pal_queue);

	    pr_info("request irq %d OK.\n",ppd->irq);
	    ppd->irq_init_flag = 1;
	    pr_info("ppd->io_base=0x%p,ppd->phys_base=0x%lx\n",ppd->io_base,ppd->phys_base);
	}

	return 0;
}

int pal_release (struct inode *inode, struct file *filp) 
{
	struct platform_pal_dev *ppd;

	ppd = container_of(inode->i_cdev, struct platform_pal_dev, cdev);
	if(ppd) {
		filp->private_data = NULL;
		kobject_put(&ppd->dev->kobj);
		return 0;
	}
	return -1;
}


//DMA中断唤醒阻塞读，然后获取当前设备的DMA取数据的地址(可读写)和可取数据的长度(只读)
//正常返回值是sizeof(struct pal_phycisal_info)
ssize_t pal_read (struct file *filp, char __user *buf, size_t count, loff_t *pos) 
{
	int ret = -1;
	int err;

	struct pal_phycisal_info params;
	struct platform_pal_dev *ppd = filp->private_data;
	int dma_status = 0;

	memset(&params,0,sizeof(struct pal_phycisal_info));

	wait_event_interruptible(ppd->pal_queue, ppd->rx_interrupt_flag);
	ppd->rx_interrupt_flag = 0;

	params.pal_data_len 	 =  ppd->pdata->read_fn(ppd->io_base + RX_LEN_OFFSET);	
	dma_status  = ppd->pdata->read_fn(ppd->io_base + DMA_STATUS_OFFSET);
	if (PING_DMA_COMPLETE == (dma_status | PING_DMA_COMPLETE)) {
		params.pal_data_addr =  ppd->pdata->read_fn(ppd->io_base);					
		if (!ppd->devnum)		pal_proc_fs.pal_rx_ping_int_cnt += 1;
		else 		     		pal_proc_fs.asi_rx_ping_int_cnt += 1;
		ret = 0;		
	} else if (PONG_DMA_COMPLETE == (dma_status | PONG_DMA_COMPLETE)) {
		params.pal_data_addr =  ppd->pdata->read_fn(ppd->io_base + PONG_ADDR_OFFSET);
		if (!ppd->devnum)	    pal_proc_fs.pal_rx_pong_int_cnt += 1;
		else  				    pal_proc_fs.asi_rx_pong_int_cnt += 1;
		ret = 0;
	} else {		
		params.pal_data_addr =  0;		
		if (!ppd->devnum)	 	pal_proc_fs.pal_not_ping_pong_int_cnt += 1;
		else  				    pal_proc_fs.asi_not_ping_pong_int_cnt += 1;
	}

	err = copy_to_user(buf, &params, sizeof(struct pal_phycisal_info));
	if (err)
		return -EFAULT;
	
	if(ret != 0)
		return -EIO;
	
	return sizeof(struct pal_phycisal_info);
}

ssize_t pal_write (struct file *filp, const char __user *buf, size_t count, loff_t *pos) 
{	
	unsigned char val[512];
	struct platform_pal_dev *ppd = filp->private_data;

	if (count > 512)
		return -EINVAL;

	pr_info("%s run %s.\n",dev_name(ppd->dev),__func__);
	if (copy_from_user(val, buf, count))
		return -EFAULT;

	pr_info("%s write  0x%lx bytes data.\n",__func__,count);

	return count;
}

long pal_unlocked_ioctl (struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct platform_pal_dev *ppd = filp->private_data;
	struct pal_info params;
	struct pal_fpga_ver vers;
	void __user *uarg = (void __user *) arg;
	int err;

	pr_info("%s run %s  \n",dev_name(ppd->dev),__func__);

	switch(cmd) {
		case PAL_GET_PING_ADDR:
			spin_lock_irq(&ppd->lock);		
			params.pal_ping_addr = ppd->pdata->read_fn(ppd->io_base);
			spin_unlock_irq(&ppd->lock);

			err = copy_to_user(uarg, &params, sizeof(struct pal_info));
			if (err)
				return -EFAULT;
			break;			

		case PAL_SET_PING_ADDR:
			err = copy_from_user(&params, uarg, sizeof(struct pal_info));
			if (err)
				return -EFAULT;

			spin_lock_irq(&ppd->lock);	
			ppd->pdata->write_fn(params.pal_ping_addr, ppd->io_base);	
			spin_unlock_irq(&ppd->lock);
			break;			

		case PAL_GET_PONG_ADDR:
			spin_lock_irq(&ppd->lock);
			params.pal_pong_addr = ppd->pdata->read_fn(ppd->io_base + PONG_ADDR_OFFSET);
			spin_unlock_irq(&ppd->lock);

			err = copy_to_user(uarg, &params, sizeof(struct pal_info));
			if (err)
				return -EFAULT;
			break;

		case PAL_SET_PONG_ADDR:
			err = copy_from_user(&params, uarg, sizeof(struct pal_info));
			if (err)
				return -EFAULT;

			spin_lock_irq(&ppd->lock);					
			ppd->pdata->write_fn(params.pal_pong_addr, ppd->io_base + PONG_ADDR_OFFSET);
			spin_unlock_irq(&ppd->lock);
			break;

		case PAL_GET_RX_LEN:
			spin_lock_irq(&ppd->lock);
			params.pal_rx_max_len 			= ppd->pdata->read_fn(ppd->io_base + RX_LEN_OFFSET);
			spin_unlock_irq(&ppd->lock);

			err = copy_to_user(uarg, &params, sizeof(struct pal_info));
			if (err)
				return -EFAULT;
			break;			

		case PAL_GET_FPGA_VER:
			spin_lock_irq(&ppd->lock);
			vers.fpga_ver_num 			= 0;
			vers.fpag_compile_date 		= 0;
			vers.fpga_compile_time		= 0;
			spin_unlock_irq(&ppd->lock);
			err = copy_to_user(uarg, &vers, sizeof(struct pal_fpga_ver));
			if (err)
				return -EFAULT;
			break;

		default:
			break;
	}
	return 0;
}

static const struct file_operations platform_pal_fops 	= {
	.owner  			= THIS_MODULE,
	.read   			= pal_read,
	.write  			= pal_write,
	.unlocked_ioctl 	= pal_unlocked_ioctl,
	.open   			= pal_open,
	.release 			= pal_release,
};

static void pal_write32(u32 val, void __iomem *addr) 
{
	iowrite32(val, addr);
}

static unsigned int pal_read32(void __iomem *addr) 
{
	return ioread32(addr);
}

static irqreturn_t platform_rx_irq_hdlr(int irq, void *dev_id) 
{
	int handled = IRQ_NONE;
	unsigned long flags;

	struct platform_pal_dev *ppd = dev_id;
	if(!ppd)
		pr_info("%s.\n",__func__);

	spin_lock_irqsave(&ppd->irq_lock, flags);

	if(!ppd->devnum) {
		pal_proc_fs.pal_interrupt_cnt += 1;
	} else {
		pal_proc_fs.asi_interrupt_cnt += 1;
	}

	wake_up(&ppd->pal_queue);
	ppd->rx_interrupt_flag  = 1;
	spin_unlock_irqrestore(&ppd->irq_lock, flags);

	handled = IRQ_HANDLED;
	return IRQ_RETVAL(handled);
}

static const struct platform_pal_pdata platform_pal_pdata_info = {	
	.read_fn	= pal_read32,
	.write_fn   = pal_write32,
	.intr_hdlr	= platform_rx_irq_hdlr,
};

/* pal proc fs for debug start*/
static struct proc_dir_entry *pal_proc_root = NULL;
static int pal_proc_show(struct seq_file *m, void *v) 
{
	seq_printf(m, "pal_interrupt_cnt 				: 0x%08X\n", pal_proc_fs.pal_interrupt_cnt);
    seq_printf(m, "pal_rx_ping_int_cnt 				: 0x%08X\n", pal_proc_fs.pal_rx_ping_int_cnt);
	seq_printf(m, "pal_rx_pong_int_cnt				: 0x%08X\n", pal_proc_fs.pal_rx_pong_int_cnt);
	seq_printf(m, "pal_not_ping_pong_int_cnt		  	: 0x%08X\n\n", pal_proc_fs.pal_not_ping_pong_int_cnt);
    seq_printf(m, "pal_kfifo_empty_cnt 				: 0x%08X\n", pal_proc_fs.pal_kfifo_empty_cnt);
	seq_printf(m, "pal_kfifo_full_cnt  				: 0x%08X\n\n", pal_proc_fs.pal_kfifo_full_cnt);	
	seq_printf(m, "asi_interrupt_cnt 				: 0x%08X\n", pal_proc_fs.asi_interrupt_cnt);
    seq_printf(m, "asi_rx_ping_int_cnt 				: 0x%08X\n", pal_proc_fs.asi_rx_ping_int_cnt);
	seq_printf(m, "asi_rx_pong_int_cnt 				: 0x%08X\n", pal_proc_fs.asi_rx_pong_int_cnt);
	seq_printf(m, "asi_not_ping_pong_int_cn  			: 0x%08X\n\n", pal_proc_fs.asi_not_ping_pong_int_cnt);	
    seq_printf(m, "asi_kfifo_empty_cnt 				: 0x%08X\n", pal_proc_fs.asi_kfifo_empty_cnt);
	seq_printf(m, "asi_kfifo_full_cnt  				: 0x%08X\n\n", pal_proc_fs.asi_kfifo_full_cnt);
	seq_printf(m, "unknow_read_error_cnt  				: 0x%08X\n\n", pal_proc_fs.unknow_read_error_cnt);

	return 0;
}

static int pal_proc_open(struct inode *inode, struct file *file) 
{
    return single_open(file, pal_proc_show, NULL);
}

static const struct file_operations pal_proc_fops = {
    .owner      = THIS_MODULE,
    .open       = pal_proc_open,
    .read       = seq_read,
    .llseek     = seq_lseek,
    .release    = single_release,
};

static int pal_init_proc(void) 
{
    pal_proc_root = proc_mkdir("pal", NULL);

    if(pal_proc_root) {
        proc_create("statistics",  0, pal_proc_root, &pal_proc_fops);
        return 0;
    }
    return -1;
}

static int init_pal_procfs(void) 
{
    pal_init_proc();
    return 0;
}

static void cleanup_pal_procfs(void) 
{
    if (pal_proc_root) {
        remove_proc_entry("statistics",  pal_proc_root);
        remove_proc_entry("pal",NULL);
    }
}
/* pal proc fs for debug end*/


static int pal_get_res_pdev(struct platform_pal_dev *ppd,
		struct platform_device *pdev, struct resource *res)
{
	//struct device *dev = &pdev->dev;
	struct resource *mem;
	int err = 0;

	/* Get the base address */
	mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!mem) {		
		pr_err("%s no MEM resource info\n",__func__);
		err = -ENODEV;
		goto err;
	}
	memcpy(res, mem, sizeof(*res));

	/* Get the IRQ */
	ppd->irq = platform_get_irq(pdev, 0);
	if (ppd->irq < 0) {
		pr_err("%s no IRQ resource info\n",__func__);
		err = ppd->irq;
		goto err;
	}

	ppd->pdata = &platform_pal_pdata_info;
	pr_info("%s OK.\n",__func__);

err:
	return err;
}

static int platform_pal_probe(struct platform_device *pdev) 
{
	int err;

	struct platform_pal_dev *ppd;
	struct device *dev = &pdev->dev;	
	struct resource res;

	pr_info("%s \n", __func__);

	ppd = devm_kzalloc(dev, sizeof(struct platform_pal_dev), GFP_KERNEL);
	if (ppd == NULL) {
		pr_err("%s unable to alloc data struct.\n",__func__);
		err = -ENOMEM;
		goto data_err;
	}
	ppd->dev = dev;
	platform_set_drvdata(pdev, ppd);

	spin_lock_init(&ppd->lock);
	spin_lock_init(&ppd->irq_lock);

    err = pal_get_res_pdev(ppd,pdev,&res);
    if (err)
    	goto data_err;

    ppd->io_base =  devm_ioremap_resource(dev, &res);
    if (IS_ERR(ppd->io_base)) {
    	err = PTR_ERR(ppd->io_base);
    	//goto data_err;  //x86-64机器调试，暂时屏蔽 04-28
    }
    ppd->phys_base = res.start;

    ppd->devnum = pdev->id;

    pr_info("%s dev_name(dev) is %s,pdev->id is %d.\n",__func__,dev_name(dev),pdev->id);

    if (!pal_major) {
    	pal_major = register_chrdev(0, DEVICE_NAME, &platform_pal_fops);
		if (pal_major < 0) {
			pr_err("%s: unable to register device\n",__func__);
			return pal_major;
		}
    }

    cdev_init(&ppd->cdev,&platform_pal_fops);
    ppd->cdev.owner = THIS_MODULE;
    err = cdev_add(&ppd->cdev, MKDEV(pal_major,pdev->id), 1);
    if (err)
    	goto out_cdev;
   	else
   		pr_info("%s cdev_add %d %d OK.",__func__,pal_major,pdev->id);

	/* Create a sysfs class for pal */
	if(!pal_class) {
		pal_class = class_create(THIS_MODULE, "pal_class");
		if (IS_ERR(pal_class)) {
			pr_err("%s failed to create a sysfs class for pal\n",__func__);
			goto out_class;
		}
		else
			pr_info("%s class_create pal_class OK.\n",__func__);
	}

	/* create /dev/pal0 /dev/asi0 */
	dev = device_create(pal_class, NULL, MKDEV(pal_major, pdev->id), NULL,
			    "%s%d", dev_name(dev), 0);
	if (IS_ERR(dev)) {
		err = PTR_ERR(dev);
		pr_err("%s device_create failed, major is %d, minor is %d.\n",__func__, pal_major, pdev->id);
		goto out_chrdev;
	}

	init_pal_procfs();
    pr_info("%s ok \n", __func__);
    return 0;

out_chrdev:
	unregister_chrdev(pal_major, DEVICE_NAME);

out_class:
	device_destroy(pal_class, MKDEV(pal_major, pdev->id));
	class_destroy(pal_class);

out_cdev:
	cdev_del(&ppd->cdev);

data_err:
	pr_err("%s initialization failed.\n",__func__);
	return err;
}

static int platform_pal_remove(struct platform_device *pdev) 
{
	struct platform_pal_dev *ppd;

	//pr_info("%s pdev->id is %d.\n", __func__,pdev->id);
	ppd = platform_get_drvdata(pdev);
	if (!ppd)
		return -ENODEV;

	//pr_info("%s [ppd->cdev.count = %d, ppd->cdev.dev = %d ]\n ",__func__,ppd->cdev.count, ppd->cdev.dev);

	unregister_chrdev(pal_major, DEVICE_NAME);
	device_destroy(pal_class, MKDEV(pal_major, pdev->id));
	if (!pdev->id)
		class_destroy(pal_class);
	cdev_del(&ppd->cdev);

	if (ppd->irq_init_flag == 1) {
		devm_free_irq(ppd->dev, ppd->irq, ppd);
		pr_info("%s free_irq %d ok \n", __func__, ppd->irq);
	}

	if(!ppd->io_base) {
		devm_iounmap(ppd->dev,ppd->io_base);
		pr_info("%s devm_iounmap 0x%p ok \n", __func__, ppd->io_base);
	}

	cleanup_pal_procfs();
	pr_info("%s ok \n", __func__);
	return 0;
}


static struct platform_driver platform_pal_driver = {
	.driver = {
		.name 	= 		"platform_pal",
		.owner	=		THIS_MODULE,
	},
	.probe 		= platform_pal_probe,
	.remove 	= platform_pal_remove,
};
module_platform_driver(platform_pal_driver);



MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("huqiqiao <huqq@zoonghe.com>");
MODULE_DESCRIPTION("Linux PAL/ASI get dma data driver");
MODULE_INFO(intree, "Y");

