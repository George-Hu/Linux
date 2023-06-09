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
#include <linux/kfifo.h>


#define  X86_64
#undef USING_ALLOC_CHRDEV_REGION

static int pal_first=0;
#ifdef USING_ALLOC_CHRDEV_REGION
static int minor_count=2;
#endif

#define PAL_NAME 			"pal"

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

struct pal_proc_fs_t {
	int         interrupt_cnt;
	int 		rx_ping_int_cnt;
	int  		rx_pong_int_cnt;
	int         kfifo_empty_cnt;
	int         kfifo_full_cnt;
	int         not_ping_pong_int_cnt;
	int         unknow_read_error_cnt;

	struct proc_dir_entry  *pal_proc_root;
}pal_proc_fs;

struct platform_pal_dev {
    dev_t 					devnum;            /* 设备号 MKDEV(major,minor) */
    int 					dev_id;		       /* 设备id,minor              */
    struct cdev 			cdev;
    struct device 			*dev;
    struct class        	*pal_class;
    unsigned long			phys_base;         
    /* device resource start */
    void __iomem 			*io_base;
    int                 	irq;               /* irq number */
    /* device resource end */

    spinlock_t          	lock;
    wait_queue_head_t   	pal_queue;
    int                 	rx_interrupt_flag; /* used to wake up read wait queue*/

    int                 	irq_init_flag;     /* request irq flag */
    const char *        	name;              /* interrupt name */
    spinlock_t          	irq_lock;
    
    /* for user used */
    struct pal_info     	pal_info;
    struct pal_fpga_ver 	pal_fpga_ver;
    struct pal_proc_fs_t	pal_proc_fs;       //for debug
    struct kfifo 		    *fifo;             // for debug

    const struct platform_pal_pdata	*pdata;
};

 /* 2、定义pal_proc_fs_t用于故障定位 */
#define FIFO_LENGTH					131072        //128K
/* for use MBCH11 board test definitions start */
#define FPGA_REG_BASE_ADDR			0x40000000 
#define FPGA_REG_VER_MAX 			0x00
#define FPGA_REG_VER_MIN 			0x04
#define FPGA_REG_RT_ADDR 			0x08
#define FPGA_REG_RT_PARITY			0x0c
#define FPGA_REG_SEC_PULSE_ENABLE 	0x10
#define FPGA_REG_SEC_PULSE_WIDTH 	0x14
#define FPGA_REG_TEST1 				0x18
#define FPGA_REG_TEST2 				0x1c
#define FPGA_REG_GPIO_IN  			0x20
#define FPGA_REG_GPIO_OUT  			0x24
static DEFINE_KFIFO(g_pal_fifo,char,FIFO_LENGTH);
static DEFINE_KFIFO(g_asi_fifo,char,FIFO_LENGTH);

/* 3、字符设备操作函数集实现 */
int pal_open (struct inode *inode, struct file *filp) 
{
	struct platform_pal_dev *ppd;
	int err;
	
	ppd = container_of(inode->i_cdev, struct platform_pal_dev, cdev);
	if (ppd) {
		filp->private_data = ppd;
		kobject_get(&ppd->dev->kobj);
	}

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


ssize_t pal_read_fifo (struct file *filp, char __user *buf, size_t count, loff_t *pos) 
{
	int ret;
	unsigned int copied_count = 0;
	struct platform_pal_dev *ppd = filp->private_data;
	pr_info("%s ppd->dev_id is %d.\n", __func__,ppd->dev_id);

	wait_event_interruptible(ppd->pal_queue, ppd->rx_interrupt_flag);
	ppd->rx_interrupt_flag = 0;

	if ((0 == count) || (count > FIFO_LENGTH) || \
			(count  > kfifo_len(ppd->fifo)) || kfifo_len(ppd->fifo))
		count = kfifo_len(ppd->fifo);

	ret = kfifo_to_user(ppd->fifo, buf, count, &copied_count);
	if(ret != 0) {
		return -EIO;
	}
	pr_info("%s ：%s, count=%lu, copied_count=%u\n", __func__, ppd->fifo->buf,  count, copied_count);

	return copied_count;
}

/*
	DMA中断唤醒阻塞读，然后获取当前设备的DMA取数据的地址(可读写)和可取数据的长度(只读)
*/
ssize_t pal_read (struct file *filp, char __user *buf, size_t count, loff_t *pos) 
{
	int ret = -1;
	int err;	
	struct pal_phycisal_info params;
	struct platform_pal_dev *ppd = filp->private_data;

	int pal_rx_len;
	int pal_dma_status;

	pr_info("%s ppd->dev_id is %d.\n", __func__,ppd->dev_id);
	wait_event_interruptible(ppd->pal_queue, ppd->rx_interrupt_flag);
	ppd->rx_interrupt_flag = 0;


#ifdef X86_64
	pal_rx_len = 0;
	pal_dma_status = 0;
	ppd->pal_proc_fs.rx_ping_int_cnt += 1;
	params.pal_data_addr = 0x00090000;
	params.pal_data_len  = 0x100;
	ret = 0;
	err = copy_to_user(buf, &params, sizeof(struct pal_phycisal_info));
	if (err) {
		pr_err("%s copy_to_user error!\n",__func__);
		return -EFAULT;
	}	
	return params.pal_data_len;
#else
	if (0 == ppd->dev_id) {
		if (PING_DMA_COMPLETE == (pal_dma_status | PING_DMA_COMPLETE)) {
			ppd->pal_proc_fs.rx_ping_int_cnt += 1;			
			if( ppd->pal_proc_fs.rx_ping_int_cnt == 1) {
				params.pal_data_addr = 0x43c40000;
			} else if ( ppd->pal_proc_fs.rx_ping_int_cnt == 2) {
				params.pal_data_addr = 0x43c41000;
			} else if ( ppd->pal_proc_fs.rx_ping_int_cnt == 3) {
				params.pal_data_addr = 0x43c42000;
			} else if ( ppd->pal_proc_fs.rx_ping_int_cnt == 4) {
				params.pal_data_addr = 0x40000000;
			} else if ( ppd->pal_proc_fs.rx_ping_int_cnt == 5) {
				params.pal_data_addr = 0xE0023000;
			} else if ( ppd->pal_proc_fs.rx_ping_int_cnt == 6) {
				params.pal_data_addr = 0xE0047000;
			} else if ( ppd->pal_proc_fs.rx_ping_int_cnt == 7) {
				params.pal_data_addr = 0xE0049000;
			} else if ( ppd->pal_proc_fs.rx_ping_int_cnt == 8) {
				params.pal_data_addr = 0xE0002000;
			} else if ( ppd->pal_proc_fs.rx_ping_int_cnt == 9) {
				params.pal_data_addr = 0xE0022000;
			} else if ( ppd->pal_proc_fs.rx_ping_int_cnt == 10) {
				params.pal_data_addr = 0xE0003000;
			} else if ( ppd->pal_proc_fs.rx_ping_int_cnt == 11) {				
				params.pal_data_addr = 0xE0001000;
			} else if ( ppd->pal_proc_fs.rx_ping_int_cnt == 12) {
				ppd->pal_proc_fs.rx_ping_int_cnt = 0;
				params.pal_data_addr = 0xE0021000;
			}			
			
			params.pal_data_len  = 0x100;
			ret = 0;
		} else if (PONG_DMA_COMPLETE == (pal_dma_status | PONG_DMA_COMPLETE)) {
			ppd->pal_proc_fs.rx_pong_int_cnt += 1;
			params.pal_data_addr = 0x99999999;
			params.pal_data_len  = 0x40000;
			ret = 0;
		} else {
			ppd->pal_proc_fs.not_ping_pong_int_cnt += 1;
			params.pal_data_addr = 0;
			params.pal_data_len  = 0;
		}
	} else if (1 == ppd->dev_id) {
		pal_rx_len 		= ppd->pdata->read_fn(ppd->io_base + ASI_FRAME_LEN);
		pal_dma_status 	= ppd->pdata->read_fn(ppd->io_base + ASI_DMA_STATUS);
		if (PING_DMA_COMPLETE == (pal_dma_status | PING_DMA_COMPLETE)) {			
			ppd->pal_proc_fs._rx_ping_int_cnt += 1;
			params.pal_data_addr = 0xaaaaaaaa;
			params.pal_data_len  = 0x80000;
			ret = 0;
		} else if (PONG_DMA_COMPLETE == (pal_dma_status | PONG_DMA_COMPLETE)) {
			ppd->pal_proc_fs.rx_pong_int_cnt += 1;
			params.pal_data_addr = 0xbbbbbbbb;
			params.pal_data_len  = 0xa0000;
			ret = 0;
		} else {
			ppd->pal_proc_fs.not_ping_pong_int_cnt += 1;
			params.pal_data_addr = 0;
			params.pal_data_len  = 0;
		}
	} else {
		ppd->pal_proc_fs.unknow_read_error_cnt += 1;
		params.pal_data_addr = 0;
		params.pal_data_len  = -1;
	}

	err = copy_to_user(buf, &params, sizeof(struct pal_phycisal_info));
	if (err)
		return -EFAULT;	

	if(ret != 0)
		return -EIO;
	
	return params.pal_data_len;
#endif //X86_64
}


//DMA中断唤醒阻塞读，然后获取当前设备的DMA取数据的地址(可读写)和可取数据的长度(只读)
//正常返回值是sizeof(struct pal_phycisal_info)
ssize_t pal_read_xxx (struct file *filp, char __user *buf, size_t count, loff_t *pos) 
{
	int ret = -1;
	int err;

	struct pal_phycisal_info params;
	struct platform_pal_dev *ppd = filp->private_data;

	int pal_rx_len;
	int pal_dma_status;

	memset(&params,0,sizeof(struct pal_phycisal_info));

	wait_event_interruptible(ppd->pal_queue, ppd->rx_interrupt_flag);
	ppd->rx_interrupt_flag = 0;

#ifdef X86_64
	pal_rx_len = 0;
	pal_dma_status = 0;
	ppd->pal_proc_fs.rx_ping_int_cnt += 1;
	params.pal_data_addr = 0x00090000;
	params.pal_data_len  = 0x100;
	ret = 0;
	err = copy_to_user(buf, &params, sizeof(struct pal_phycisal_info));
	if (err) {
		pr_err("%s copy_to_user error!\n",__func__);
		return -EFAULT;
	}
	return sizeof(struct pal_phycisal_info);
	//return params.pal_data_len;
#else
	if (0 == ppd->dev_id) {
		if (PING_DMA_COMPLETE == (pal_dma_status | PING_DMA_COMPLETE)) {
			ppd->pal_proc_fs.rx_ping_int_cnt += 1;
			pr_info("%s rx_ping_int_cnt + 1\n",__func__);
			if( ppd->pal_proc_fs.rx_ping_int_cnt == 1) {
				params.pal_data_addr = 0x43c40000;
			} else if ( ppd->pal_proc_fs.rx_ping_int_cnt == 2) {
				params.pal_data_addr = 0x43c41000;
			} else if ( ppd->pal_proc_fs.rx_ping_int_cnt == 3) {
				params.pal_data_addr = 0x43c42000;
			} else if ( ppd->pal_proc_fs.rx_ping_int_cnt == 4) {
				params.pal_data_addr = 0x40000000;
			} else if ( ppd->pal_proc_fs.rx_ping_int_cnt == 5) {
				params.pal_data_addr = 0xE0023000;
			} else if ( ppd->pal_proc_fs.rx_ping_int_cnt == 6) {
				params.pal_data_addr = 0xE0047000;
			} else if ( ppd->pal_proc_fs.rx_ping_int_cnt == 7) {
				params.pal_data_addr = 0xE0049000;
			} else if ( ppd->pal_proc_fs.rx_ping_int_cnt == 8) {
				params.pal_data_addr = 0xE0002000;
			} else if ( ppd->pal_proc_fs.rx_ping_int_cnt == 9) {
				params.pal_data_addr = 0xE0022000;
			} else if ( ppd->pal_proc_fs.rx_ping_int_cnt == 10) {
				params.pal_data_addr = 0xE0003000;
			} else if ( ppd->pal_proc_fs.rx_ping_int_cnt == 11) {				
				params.pal_data_addr = 0xE0001000;
			} else if ( ppd->pal_proc_fs.rx_ping_int_cnt == 12) {
				ppd->pal_proc_fs.rx_ping_int_cnt = 0;
				params.pal_data_addr = 0xE0021000;
			}		
			
			params.pal_data_len  = 0x100;
			ret = 0;
		} else if (PONG_DMA_COMPLETE == (pal_dma_status | PONG_DMA_COMPLETE)) {
			ppd->pal_proc_fs.rx_pong_int_cnt += 1;
			params.pal_data_addr = 0x99999999;
			params.pal_data_len  = 0x40000;
			ret = 0;
		} else {
			ppd->pal_proc_fs.not_ping_pong_int_cnt += 1;
			params.pal_data_addr = 0;
			params.pal_data_len  = 0;
		}
	} else if (1 == ppd->dev_id) {
		pal_rx_len 		= ppd->pdata->read_fn(ppd->io_base + RX_LEN_OFFSET);
		pal_dma_status 	= ppd->pdata->read_fn(ppd->io_base + DMA_STATUS_OFFSET);
		if (PING_DMA_COMPLETE == (pal_dma_status | PING_DMA_COMPLETE)) {			
			ppd->pal_proc_fs.rx_ping_int_cnt += 1;
			params.pal_data_addr = 0xaaaaaaaa;
			params.pal_data_len  = 0x80000;
			ret = 0;
		} else if (PONG_DMA_COMPLETE == (pal_dma_status | PONG_DMA_COMPLETE)) {
			ppd->pal_proc_fs.rx_pong_int_cnt += 1;
			params.pal_data_addr = 0xbbbbbbbb;
			params.pal_data_len  = 0xa0000;
			ret = 0;
		} else {
			ppd->pal_proc_fs.not_ping_pong_int_cnt += 1;
			params.pal_data_addr = 0;
			params.pal_data_len  = 0;
		}
	} else {
		ppd->pal_proc_fs.unknow_read_error_cnt += 1;
		params.pal_data_addr = 0;
		params.pal_data_len  = -1;
	}

	err = copy_to_user(buf, &params, sizeof(struct pal_phycisal_info));
	if (err)
		return -EFAULT;	

	if(ret != 0)
		return -EIO;
	
	return params.pal_data_len;
#endif //X86_64

	
	return sizeof(struct pal_phycisal_info);
}

ssize_t pal_write (struct file *filp, const char __user *buf, size_t count, loff_t *pos) 
{	
	int ret;
	unsigned int copied_count=0;
	struct platform_pal_dev *ppd = filp->private_data;

	pr_info("%s ppd->dev_id is %d.\n", __func__,ppd->dev_id);
	ppd->rx_interrupt_flag	=	1;
	ppd->pal_proc_fs.rx_ping_int_cnt += 1;
	wake_up(&ppd->pal_queue);

	ret = kfifo_from_user(ppd->fifo, buf, count, &copied_count);
	if(ret != 0)
		return -EIO;
	pr_info("%s ：%s, count=%lu, copied_count=%u\n", __func__, ppd->fifo->buf,  count, copied_count);
	

	return copied_count;
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
	
	ppd->pal_proc_fs.interrupt_cnt += 1;	

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
static int pal_proc_show(struct seq_file *m, void *v) 
{
	struct platform_pal_dev *info = m->private;

	if (info) {
		seq_printf(m, "interrupt_cnt 				: 0x%08X\n", info->pal_proc_fs.interrupt_cnt);
		seq_printf(m, "rx_ping_int_cnt 			: 0x%08X\n", info->pal_proc_fs.rx_ping_int_cnt);
		seq_printf(m, "rx_pong_int_cnt				: 0x%08X\n", info->pal_proc_fs.rx_pong_int_cnt);
		seq_printf(m, "not_ping_pong_int_cnt		  	: 0x%08X\n\n",  info->pal_proc_fs.not_ping_pong_int_cnt);
		seq_printf(m, "kfifo_empty_cnt 			: 0x%08X\n", info->pal_proc_fs.kfifo_empty_cnt);
		seq_printf(m, "kfifo_full_cnt  			: 0x%08X\n\n",  info->pal_proc_fs.kfifo_full_cnt);
		seq_printf(m, "unknow_read_error_cnt  			: 0x%08X\n\n",  info->pal_proc_fs.unknow_read_error_cnt);
	}
	else
		pr_info("%s %d info is nullprt\n",__func__,__LINE__);
	return 0;
}

static int pal_proc_open(struct inode *inode, struct file *file)
{
/* 
   在open时，使用PDE_DATA(inode)作为私有数据向下传。其实PDE_DATA(inode) 就是 ppd.
   这个私有数据的保存在seq_file的private里。
   在write和show函数中可以直接使用file->private来找到私有数据。 
*/
	return single_open(file, pal_proc_show, PDE_DATA(inode));
}

static const struct file_operations pal_proc_fops = {
    .owner      = THIS_MODULE,
    .open       = pal_proc_open, //需实现
    .read       = seq_read,
    .llseek     = seq_lseek,
    .release    = single_release,
};

static void pal_init_proc(struct platform_pal_dev *ppd) 
{
	if (!ppd->pal_proc_fs.pal_proc_root) {
		ppd->pal_proc_fs.pal_proc_root = proc_mkdir(dev_name(ppd->dev), NULL); 			// 创建/proc/pal 或者 /proc/asi
/*
	使用proc_create_data，其中最后一个参数是私有数据。这里是 ppd.
*/	      
		if (!proc_create_data("info",S_IFREG | S_IRUGO, ppd->pal_proc_fs.pal_proc_root, &pal_proc_fops, ppd)) {
			pr_err("%s unable to initialize /proc/%s\n",__func__, dev_name(ppd->dev));  // 创建/proc/pal/info 或者 /proc/asi/info
			return;
		}
		pr_info("%s %d initialize /proc/%s\n",__func__, __LINE__, dev_name(ppd->dev));
	}
	return;
}

static int init_pal_procfs(struct platform_pal_dev *ppd)
{
    pal_init_proc(ppd);
    return 0;
}

static void cleanup_pal_procfs(struct platform_pal_dev *ppd) 
{
	remove_proc_entry("info",ppd->pal_proc_fs.pal_proc_root); //  从/proc/asi/info 移除info，变为 /proc/asi
	remove_proc_entry(dev_name(ppd->dev),NULL);				  //  从/proc/asi 移除 asi
}
/* pal proc fs for debug end*/

static int pal_get_res_pdev(struct platform_pal_dev *ppd,
		struct platform_device *pdev, struct resource *res)
{
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

static void platform_pal_dev_show(struct platform_pal_dev *pdev) {
	pr_info("\n\n[%s ---> platform_pal_dev_show start--->]\n",__func__);
	pr_info("major				:%d\n",MAJOR(pdev->cdev.dev));
	pr_info("minor				:%d\n",MINOR(pdev->cdev.dev));
	pr_info("dev				:%d\n",MKDEV(MAJOR(pdev->cdev.dev),MINOR(pdev->cdev.dev)));
	pr_info("name    				:%s\n\n",pdev->name);
	pr_info("devnum    			:%d\n",pdev->devnum);
	pr_info("dev->kobj.name  			:%s\n\n",pdev->dev->kobj.name);
	pr_info("dev_id      			:0x%x\n",pdev->dev_id);
	pr_info("cdev.dev 			:%d\n",pdev->cdev.dev);
	pr_info("cdev.count 			:%d\n",pdev->cdev.count);
	
	pr_info("phys_base          		:%lx\n",pdev->phys_base);
	pr_info("io_base            		:%p\n",pdev->io_base);

	pr_info("[%s ---> platform_pal_dev_show end--->]\n\n",__func__);
	return;
}

static int pal_chrdev_register(struct platform_pal_dev *ppd)
{
	int err = 0;
	int major;
	dev_t pal_dev;
	struct device *class_dev;

#ifdef USING_ALLOC_CHRDEV_REGION
    /* Allocate dynamic major number. */
    if(!pal_first) {
		if (alloc_chrdev_region(&pal_first, 0, minor_count, PAL_NAME)) {
			pr_err("%s, Failed to allocate character device region\n",__func__);
			return -1;
		}    	
    }
#else
    if (!pal_first) {
    	pal_first = register_chrdev(0, PAL_NAME, &platform_pal_fops);
		if (pal_first < 0) {
			pr_err("%s: unable to register device\n",__func__);
			return pal_first;
		}
    }
#endif

    cdev_init(&ppd->cdev,&platform_pal_fops);
    ppd->cdev.owner = THIS_MODULE;
    /* fifo only for debug use */
    ppd->fifo       = (ppd->dev_id == 0) ? (struct kfifo *)&g_pal_fifo : (struct kfifo *)&g_asi_fifo;      

#ifdef USING_ALLOC_CHRDEV_REGION
    major   = MAJOR(pal_first);
    pal_dev = (pal_first + ppd->dev_id);    

#else
    major   = pal_first;
	pal_dev = MKDEV(pal_first,ppd->dev_id);
#endif
	ppd->devnum = pal_dev;
	ppd->name   = ppd->dev->kobj.name;
    
    err = cdev_add(&ppd->cdev, pal_dev, 1);// ppd->cdev.dev <=== pal_dev
    if (err)
    	goto out_cdev;
    else
   		pr_info("%s cdev_add %d %d OK.",__func__,major,ppd->dev_id); 
   	pr_info("========1=========%s ppd->cdev.dev = %d.\n", __func__,ppd->cdev.dev);

#if 1
   	ppd->pal_class = class_create(THIS_MODULE,dev_name(ppd->dev));
   	if (IS_ERR(ppd->pal_class)) {
			pr_err("%s failed to create a sysfs class for %s\n",__func__,dev_name(ppd->dev));
			goto out_class;
		}
		else
			pr_info("%s class_create %s OK.\n",__func__,dev_name(ppd->dev));   		
#else
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
#endif	

	pr_info("============dev_name(ppd->dev) is %s.\n",dev_name(ppd->dev));

	
#if 1
	/* create /dev/pal0 /dev/asi0 */
	class_dev = device_create(ppd->pal_class, NULL, pal_dev, NULL,
			    "%s%d", dev_name(ppd->dev),0);
#else
	/* create /dev/pal0 /dev/asi0 */
	class_dev = device_create(pal_class, NULL, pal_dev, NULL,
			    "%s%d", dev_name(ppd->dev),0);
#endif
	if (IS_ERR(class_dev)) {
		err = PTR_ERR(class_dev);
		pr_err("%s device_create failed, major is %d, minor is %d.\n",__func__, pal_first, ppd->dev_id);
		goto out_chrdev;
	}

	init_pal_procfs(ppd);

	platform_pal_dev_show(ppd);
	pr_info("%s OK\n",__func__);
	return 0;

out_chrdev:
#ifdef USING_ALLOC_CHRDEV_REGION
	unregister_chrdev_region(pal_first, minor_count);
#else
	unregister_chrdev(pal_first, PAL_NAME);
#endif

out_class:
	device_destroy(ppd->pal_class, pal_dev);
	class_destroy(ppd->pal_class);

out_cdev:
	cdev_del(&ppd->cdev);

	return err;
}

static void pal_chrdev_deregister(struct platform_pal_dev *ppd)
{
	int major;
	dev_t pal_dev;

	platform_pal_dev_show(ppd);

	pr_info("%s, ppd->devnum = %d,ppd->dev_id = %d\n",__func__,ppd->devnum,ppd->dev_id);

#ifdef USING_ALLOC_CHRDEV_REGION
	major   = MAJOR(pal_first);
    pal_dev = (pal_first + ppd->dev_id);
	unregister_chrdev_region(pal_first, minor_count);
#else
	major   = pal_first;
	pal_dev = MKDEV(pal_first,ppd->dev_id);
	unregister_chrdev(pal_first, PAL_NAME);
#endif
	ppd->devnum = pal_dev;
	pr_info("%s, ppd->devnum = %d,ppd->dev_id = %d\n",__func__,ppd->devnum,ppd->dev_id);

	device_destroy(ppd->pal_class, pal_dev);
	class_destroy(ppd->pal_class);

	cdev_del(&ppd->cdev);

	if (ppd->irq_init_flag == 1) {
		devm_free_irq(ppd->dev, ppd->irq, ppd);
		pr_info("%s free_irq %d ok \n", __func__, ppd->irq);
	}
	
	if(ppd->io_base) {
		devm_iounmap(ppd->dev,ppd->io_base);
		pr_info("%s devm_iounmap 0x%p ok \n", __func__, ppd->io_base);
	}	

	cleanup_pal_procfs(ppd);

	if(ppd->dev->kobj.name) //pal/asi -> parent is platform
		sysfs_remove_link(&ppd->dev->kobj, "platform");
	
	pr_info("%s ok \n", __func__);
	return;
}

static int platform_pal_probe(struct platform_device *pdev) 
{
	int err;

	struct platform_pal_dev *ppd;
	struct resource res;
	struct device *dev = &pdev->dev;	

	pr_info("%s \n", __func__);
	ppd = devm_kzalloc(dev, sizeof(struct platform_pal_dev), GFP_KERNEL);
	if (ppd == NULL) {
		pr_err("%s unable to alloc data struct.\n",__func__);
		err = -ENOMEM;
		goto data_err;
	}
	ppd->dev 	= dev;
	ppd->dev_id = pdev->id;

	spin_lock_init(&ppd->lock);
	spin_lock_init(&ppd->irq_lock);

	err = pal_get_res_pdev(ppd,pdev,&res);
    if (err)
    	goto data_err;

    ppd->phys_base = res.start;

#if 0
    ppd->io_base =  devm_ioremap_resource(dev, &res);// 不可以映射已经map的区域
    if (IS_ERR(ppd->io_base)) {
    	//err = PTR_ERR(ppd->io_base);
    	//goto data_err;  //x86-64机器调试，暂时屏蔽 04-28
    	ppd->io_base = ioremap(res.start,res.end);
    	if(IS_ERR(ppd->io_base)) {
    		err = PTR_ERR(ppd->io_base);
    		goto data_err;
    	}
    }
 #else 
    ppd->io_base =devm_ioremap(dev, res.start, res.end - res.start + 1);
    if(IS_ERR(ppd->io_base)) {
    	err = PTR_ERR(ppd->io_base);
    	goto data_err;
    }
    pr_info("%s, devm_ioremap OK,io_base is %p.\n",__func__,ppd->io_base);
 #endif   

    pr_info("%s dev_name(dev) is %s, ppd->dev_id is %d.\n",__func__,dev_name(dev), ppd->dev_id);

    err = pal_chrdev_register(ppd);
    if (err)
    	goto data_err;
    pr_info("========2=========%s ppd->cdev.dev = %d.\n", __func__,ppd->cdev.dev);
    platform_set_drvdata(pdev, ppd);
    pr_info("%s ok \n", __func__);
    return 0;
data_err:
	pr_err("%s failed.\n",__func__);
	return err;
}

static int platform_pal_remove(struct platform_device *pdev) 
{
	struct platform_pal_dev *ppd;
	pr_info("%s \n",__func__);

	ppd = platform_get_drvdata(pdev);
	if (!ppd)
		return -ENODEV;

	pal_chrdev_deregister(ppd);
	pr_info("%s OK\n",__func__);
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
