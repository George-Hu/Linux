#include <linux/types.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/kfifo.h>
#include <linux/uaccess.h>
#include <linux/major.h>
#include <linux/errno.h>
#include <asm/irq.h>
#include <linux/interrupt.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#include "pal.h"

#define DEVICE_NAME 				"pal"
#define ASI_DEVICE_NAME				"asi"
#define DEVICE_MAJOR 				201
#define MINOR_BASE 					0
#define MINOR_NUM 					2

#define  DEBUG

#ifdef DEBUG
static int test_condition = 0;
#endif

/* register definitions */
#define PAL_FPGA_BASE_ADDR          0x40000000
#define PAL_FPGA_REG_LEN			0x100
#define FPAG_VER_NO 			    0x00
#define COMPILE_DATE				0x04
#define COMPILE_TIME				0x08
#define PAL_PING_ADDR				0x10
#define PAL_PONG_ADDR				0x14
#define ASI_PING_ADDR				0x18
#define ASI_PONG_ADDR				0x1C
#define PAL_FRAME_LEN				0x20
#define ASI_FRAME_LEN				0x24
#define PAL_DMA_STATUS              0x28
	#define PAL_PING_DMA_NO_START			((__u8)((0<<1)|(1<<0)))	
	#define PAL_PING_DMA_START              ((__u8)((0<<1)|(1<<0)))
	#define PAL_PING_DMA_PROCESS			((__u8)((1<<1)|(0<<0)))
	#define PAL_PING_DMA_COMPLETE			((__u8)((1<<1)|(1<<0)))
	#define PAL_PONG_DMA_NO_START			((__u8)((0<<3)|(1<<2)))
	#define PAL_PONG_DMA_START              ((__u8)((0<<3)|(1<<2)))
	#define PAL_PONG_DMA_PROCESS			((__u8)((1<<3)|(0<<2)))
	#define PAL_PONG_DMA_COMPLETE			((__u8)((1<<3)|(1<<2)))
#define ASI_DMA_STATUS              0x2C
	#define ASI_PING_DMA_NO_START			((__u8)((0<<1)|(1<<0)))	
	#define ASI_PING_DMA_START              ((__u8)((0<<1)|(1<<0)))
	#define ASI_PING_DMA_PROCESS			((__u8)((1<<1)|(0<<0)))
	#define ASI_PING_DMA_COMPLETE			((__u8)((1<<1)|(1<<0)))
	#define ASI_PONG_DMA_NO_START			((__u8)((0<<3)|(1<<2)))
	#define ASI_PONG_DMA_START              ((__u8)((0<<3)|(1<<2)))
	#define ASI_PONG_DMA_PROCESS			((__u8)((1<<3)|(0<<2)))
	#define ASI_PONG_DMA_COMPLETE			((__u8)((1<<3)|(1<<2)))

/* pal/asi rx base addr */
#define PAL_RX_PING_ADDR 			(PAL_FPGA_BASE_ADDR + 0x1000)
#define PAL_RX_PONG_ADDR 			(PAL_FPGA_BASE_ADDR + 0x2000)
#define ASI_RX_PING_ADDR			(PAL_FPGA_BASE_ADDR + 0x3000)
#define ASI_RX_PONG_ADDR			(PAL_FPGA_BASE_ADDR + 0x4000)
#define PAL_RX_MAX_LEN				414720  //720*576
#define ASI_RX_MAX_LEN				414720  //720*576

/* inner used definitions */
//#define FIFO_LENGTH					16777216     //2^24
#define FIFO_LENGTH					1048576        //1M

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
/* for use MBCH11 board test definitions end */

#define PAL_RX_IRQ_NUMBER			2
#define ASI_RX_IRQ_NUMBER			3

#if 0
static DEFINE_MUTEX(pal_mutex);
static DEFINE_SPINLOCK(pal_state_lock);
struct mutex my_mutex;
mutex_init(&my_mutex);

mutex的使用:
struct mutex my_mutex; /* 定义 mutex /
mutex_init(&my_mutex); / 初始化 mutex /
mutex_lock(&my_mutex); / 获取 mutex /
… / 临界资源 /
mutex_unlock(&my_mutex); / 释放 mutex */
#endif


static int major = DEVICE_MAJOR;
static int minor_base = MINOR_BASE;
static int minor_num = MINOR_NUM;
static dev_t pal_first;
static struct class *pal_class;

void *dev_base;
void *pal_rx_ping_base;
void *pal_rx_pong_base;
void *asi_rx_ping_base;
void *asi_rx_pong_base;
int  pal_rx_ping_addr = PAL_RX_PING_ADDR;
int  pal_rx_pong_addr = PAL_RX_PONG_ADDR;
int  asi_rx_ping_addr = ASI_RX_PING_ADDR;
int  asi_rx_pong_addr = ASI_RX_PONG_ADDR;
int  asi_rx_max_len	  = ASI_RX_MAX_LEN;
int  pal_rx_max_len	  =	PAL_RX_MAX_LEN;

static DEFINE_KFIFO(g_pal_fifo,char,FIFO_LENGTH);
static DEFINE_KFIFO(g_asi_fifo,char,FIFO_LENGTH);

/* half interrupt definitions */
static irqreturn_t pal_rx_interrupt(int irq, void *dev_id);
static irqreturn_t asi_rx_interrupt(int irq, void *dev_id);

/* bottom interrupt definitions */
/*static void pal_tasklet_func(unsigned long);*/
/* 定义一个tasklet结构pal_tasklet，与pal_tasklet_func(data)关联  */
/*DECLARE_TASKLET(pal_tasklet,pal_tasklet_func,0);*/

/*static void asi_tasklet_func(unsigned long);
DECLARE_TASKLET(asi_tasklet,asi_tasklet_func,0);*/

static void pal_write32(u32 val, void __iomem *addr) {
	iowrite32(val, addr);
}

static unsigned int pal_read32(void __iomem *addr) {
	return ioread32(addr);
}

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

/* 设备结构体 */
struct pal_dev {
	dev_t devnum;			//设备号
	struct cdev cdev;
	struct device *pal_device;
	struct kfifo *fifo;	
	struct mutex pal_mutex;

	spinlock_t 			lock;
	wait_queue_head_t	pal_queue;
	int                 rx_interrupt_flag;
	struct pal_info 	pal_info;
	struct pal_fpga_ver pal_fpga_ver;
	int    				irq;		      /* irq number */
	int    				irq_init_flag;    /* request irq flag */
	irq_handler_t 		handler;
	const char *		name;
	spinlock_t			irq_lock;
	//void *dev_base;
	unsigned int (*read_fn)(void __iomem *);
	void (*write_fn)(u32, void __iomem *);
};

static struct pal_dev paldev[]={
	{
		.devnum = 0,
		.fifo   				= (struct kfifo *)&g_pal_fifo,
		.irq    				= PAL_RX_IRQ_NUMBER,
		.irq_init_flag 			= 0,
		.handler 		    	= pal_rx_interrupt,
		.name 					= "pal rx",
		.read_fn				= pal_read32,
		.write_fn				= pal_write32,
		.rx_interrupt_flag 		= 0,
	},
	{
		.devnum = 1,
		.fifo    				= (struct kfifo *)&g_pal_fifo,
		.irq     				= ASI_RX_IRQ_NUMBER,
		.irq_init_flag 			= 0,
		.handler 		    	= asi_rx_interrupt,
		.name 					= "asi rx",
		.read_fn				= pal_read32,
		.write_fn				= pal_write32,
		.rx_interrupt_flag  	= 0,
	}
};
#define NBR_DEVICE ARRAY_SIZE(paldev)

ssize_t pal_read (struct file *filp, char __user *buf, size_t len, loff_t *pos);
ssize_t pal_write (struct file *filp, const char __user *buf, size_t count, loff_t *pos);
long pal_unlocked_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
int pal_open(struct inode *inode, struct file *filp);
int pal_release(struct inode *inode, struct file *filp);
int (*pal_mmap) (struct file *, struct vm_area_struct *);


int pal_open (struct inode *inode, struct file *filp)
{
	struct pal_dev *pal_dev;
	int i;

#ifdef DEBUG
	int major = imajor(inode);
	int minor = iminor(inode);
	pr_info("%s: major=%d, minor=%d\n", __func__, major, minor);
#endif
	i = iminor(inode);
	switch(i) 
	{
		case 0:
		case 1:
			if (0 == paldev[i].irq_init_flag) {
				/* Request pal/asi rx irq */
#ifndef DEBUG
			if (request_irq(paldev[i].irq, paldev[i].handler, IRQF_SHARED, paldev[i].name, &paldev[i])) {
					pr_err("Can't alloc %s rx IRQ",((i == 0) ? DEVICE_NAME : ASI_DEVICE_NAME));
					return -1;
				}
#endif				
				/* Initial wait queue */
				init_waitqueue_head(&paldev[i].pal_queue); 
				paldev[i].irq_init_flag = 1;
			}

			pal_dev = container_of(inode->i_cdev,struct pal_dev,cdev);
			filp->private_data = pal_dev;
			kobject_get(&pal_dev->pal_device->kobj);

			return 0;
		default:
			return (-ENODEV);
	}
}

int pal_release (struct inode *inode, struct file *filp)
{
	struct pal_dev *pal_dev;

	//pr_info("%s \n", __func__);
	switch(iminor(inode)) 
	{
		case 0:
		case 1:
			pal_dev = container_of(inode->i_cdev,struct pal_dev,cdev);
			filp->private_data = pal_dev;
			kobject_put(&pal_dev->pal_device->kobj);
			return 0;
		default:
			return (-ENODEV);
	}
}

ssize_t pal_read_kfifo (struct file *filp, char __user *buf, size_t count, loff_t *pos) {
	int ret;
	unsigned int copied_count = 0;
	struct pal_dev *dev = filp->private_data;
	pr_info("%s \n", __func__);	

	if ((0 == count) || (count > FIFO_LENGTH) || (count  > kfifo_len(dev->fifo)) || kfifo_len(dev->fifo))
		count = kfifo_len(dev->fifo);

	ret = kfifo_to_user(dev->fifo, buf, count, &copied_count);
	if(ret != 0) {
		return -EIO;
	}
	pr_info("%s ：%s, count=%d, copied_count=%d\n", __func__, dev->fifo->buf,  count, copied_count);

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
	struct pal_dev *dev = filp->private_data;

	int pal_rx_len = 0;
	int pal_dma_status = 0;

#ifndef DEBUG
	wait_event_interruptible(dev->pal_queue, dev->rx_interrupt_flag);
	dev->rx_interrupt_flag = 0;

	if (0 == dev->devnum) {
		pal_rx_len 		= paldev[dev->devnum].read_fn(dev_base + PAL_FRAME_LEN);
		pal_dma_status 	= paldev[dev->devnum].read_fn(dev_base + PAL_DMA_STATUS);
		pal_dma_status = PAL_PING_DMA_COMPLETE;
		if (PAL_PING_DMA_COMPLETE == (pal_dma_status | PAL_PING_DMA_COMPLETE)) {
			pal_proc_fs.pal_rx_ping_int_cnt += 1;

			params.pal_data_addr = dev->read_fn(dev_base + PAL_PING_ADDR);
			params.pal_data_len  = dev->read_fn(dev_base + PAL_FRAME_LEN);
			ret = 0;
		}
		else if (PAL_PONG_DMA_COMPLETE == (pal_dma_status | PAL_PONG_DMA_COMPLETE)) {
			pal_proc_fs.pal_rx_pong_int_cnt += 1;

			params.pal_data_addr = dev->read_fn(dev_base + PAL_PONG_ADDR);
			params.pal_data_len  = dev->read_fn(dev_base + PAL_FRAME_LEN);
			ret = 0;
		}
		else {
			pal_proc_fs.pal_not_ping_pong_int_cnt += 1;
			params.pal_data_addr = 0;
			params.pal_data_len  = 0;
		}
	}
	else if (1 == dev->devnum) {
		pal_rx_len 		= paldev[dev->devnum].read_fn(dev_base + ASI_FRAME_LEN);
		pal_dma_status 	= paldev[dev->devnum].read_fn(dev_base + ASI_DMA_STATUS);
		if (ASI_PING_DMA_COMPLETE == (pal_dma_status | ASI_PING_DMA_COMPLETE)) {
			//ret = copy_to_user(buf,asi_rx_ping_base,pal_rx_len)
			pal_proc_fs.asi_rx_ping_int_cnt += 1;

			params.pal_data_addr = dev->read_fn(dev_base + ASI_PING_ADDR);
			params.pal_data_len  = dev->read_fn(dev_base + ASI_FRAME_LEN);
			ret = 0;
		}
		else if (PAL_PONG_DMA_COMPLETE == (pal_dma_status | ASI_PONG_DMA_COMPLETE)) {
			//ret = copy_to_user(buf,asi_rx_pong_base,pal_rx_len);
			pal_proc_fs.asi_rx_pong_int_cnt += 1;
			params.pal_data_addr = dev->read_fn(dev_base + ASI_PONG_ADDR);
			params.pal_data_len  = dev->read_fn(dev_base + ASI_FRAME_LEN);
			ret = 0;
		}
		else {
			pal_proc_fs.asi_not_ping_pong_int_cnt += 1;
			params.pal_data_addr = 0;
			params.pal_data_len  = 0;
		}
	}
	else {
		pal_proc_fs.unknow_read_error_cnt += 1;
		params.pal_data_addr = 0;
		params.pal_data_len  = -1;
	}

	err = copy_to_user(buf, &params, sizeof(struct pal_phycisal_info));
	if (err)
		return -EFAULT;

	if(ret != 0) {
		return -EIO;
	}
	return params.pal_data_len;

#else
	pr_info("%s %s.\n",(dev->devnum == 0) ? "pal" : "asi",__func__);
	wait_event_interruptible(dev->pal_queue, test_condition);
	test_condition = 0;

	if (0 == dev->devnum) {
		if (PAL_PING_DMA_COMPLETE == (pal_dma_status | PAL_PING_DMA_COMPLETE)) {
			pal_proc_fs.pal_rx_ping_int_cnt += 1;
			if( pal_proc_fs.pal_rx_ping_int_cnt == 1) {
				params.pal_data_addr = 0x43c40000;
			}
			else if ( pal_proc_fs.pal_rx_ping_int_cnt == 2) {
				params.pal_data_addr = 0x43c41000;
			}
			else if ( pal_proc_fs.pal_rx_ping_int_cnt == 3) {
				params.pal_data_addr = 0x43c42000;
			}
			else if ( pal_proc_fs.pal_rx_ping_int_cnt == 4) {
				params.pal_data_addr = 0x40000000;
			}
			else if ( pal_proc_fs.pal_rx_ping_int_cnt == 5) {
				params.pal_data_addr = 0xE0023000;
			}
			else if ( pal_proc_fs.pal_rx_ping_int_cnt == 6) {
				params.pal_data_addr = 0xE0047000;
			}
			else if ( pal_proc_fs.pal_rx_ping_int_cnt == 7) {
				params.pal_data_addr = 0xE0049000;
			}
			else if ( pal_proc_fs.pal_rx_ping_int_cnt == 8) {
				params.pal_data_addr = 0xE0002000;
			}
			else if ( pal_proc_fs.pal_rx_ping_int_cnt == 9) {
				params.pal_data_addr = 0xE0022000;
			}
			else if ( pal_proc_fs.pal_rx_ping_int_cnt == 10) {
				params.pal_data_addr = 0xE0003000;
			}
			else if ( pal_proc_fs.pal_rx_ping_int_cnt == 11) {				
				params.pal_data_addr = 0xE0001000;
			}
			else if ( pal_proc_fs.pal_rx_ping_int_cnt == 12){
				pal_proc_fs.pal_rx_ping_int_cnt = 0;
				params.pal_data_addr = 0xE0021000;
			}			
			
			params.pal_data_len  = 0x1000;
			ret = 0;
		}
		else if (PAL_PONG_DMA_COMPLETE == (pal_dma_status | PAL_PONG_DMA_COMPLETE)) {
			pal_proc_fs.pal_rx_pong_int_cnt += 1;
			params.pal_data_addr = 0x99999999;
			params.pal_data_len  = 0x40000;
			ret = 0;
		}
		else {
			pal_proc_fs.pal_not_ping_pong_int_cnt += 1;
			params.pal_data_addr = 0;
			params.pal_data_len  = 0;
		}
	}	
	else if (1 == dev->devnum) {
		pal_rx_len 		= paldev[dev->devnum].read_fn(dev_base + ASI_FRAME_LEN);
		pal_dma_status 	= paldev[dev->devnum].read_fn(dev_base + ASI_DMA_STATUS);
		if (ASI_PING_DMA_COMPLETE == (pal_dma_status | ASI_PING_DMA_COMPLETE)) {			
			pal_proc_fs.asi_rx_ping_int_cnt += 1;
			params.pal_data_addr = 0xaaaaaaaa;
			params.pal_data_len  = 0x80000;
			ret = 0;
		}
		else if (PAL_PONG_DMA_COMPLETE == (pal_dma_status | ASI_PONG_DMA_COMPLETE)) {
			pal_proc_fs.asi_rx_pong_int_cnt += 1;
			params.pal_data_addr = 0xbbbbbbbb;
			params.pal_data_len  = 0xa0000;
			ret = 0;
		}
		else {
			pal_proc_fs.asi_not_ping_pong_int_cnt += 1;
			params.pal_data_addr = 0;
			params.pal_data_len  = 0;
		}
	}
	else {
		pal_proc_fs.unknow_read_error_cnt += 1;
		params.pal_data_addr = 0;
		params.pal_data_len  = -1;
	}

	err = copy_to_user(buf, &params, sizeof(struct pal_phycisal_info));
	if (err)
		return -EFAULT;

	if(ret != 0) {
		return -EIO;
	}
	return params.pal_data_len;

#endif
}

ssize_t pal_write (struct file *filp, const char __user *buf, size_t count, loff_t *pos)
{
	int ret;
	unsigned int copied_count=0;
	struct pal_dev *dev = filp->private_data;	

#ifdef DEBUG
	pr_info("%s \n", __func__);
	test_condition=1;
	wake_up(&dev->pal_queue);

	ret = kfifo_from_user(dev->fifo, buf, count, &copied_count);
	if(ret != 0) {
		return -EIO;
	}
	pr_info("%s ：%s, count=%d, copied_count=%d\n", __func__, dev->fifo->buf,  count, copied_count);
#endif	

	return copied_count;
}

long pal_unlocked_ioctl (struct file *filp, unsigned int cmd, unsigned long arg)
{	
	struct pal_dev *dev = filp->private_data;
	struct pal_info params;
	struct pal_fpga_ver vers;
	void __user *uarg = (void __user *) arg;
	//int __user *iuarg = (int __user *) arg;
	int err;

	switch(cmd) {

		case PAL_GET_PAL_PING_ADDR:
			pr_info("%s ioctl PAL_GET_PAL_PING_ADDR.\n",(dev->devnum == 0) ? "pal" : "asi");

			spin_lock_irq(&dev->lock);

			/* Get the current parameters */
#ifndef DEBUG			
			params.pal_ping_addr = dev->read_fn(dev_base + PAL_PING_ADDR);
#else
			params.pal_ping_addr = dev->read_fn(dev_base + FPGA_REG_TEST2);			
#endif
			spin_unlock_irq(&dev->lock);

			err = copy_to_user(uarg, &params, sizeof(struct pal_info));
			if (err)
				return -EFAULT;

		break;

		case PAL_SET_PAL_PING_ADDR:
			pr_info("%s ioctl PAL_SET_PAL_PING_ADDR.\n",(dev->devnum == 0) ? "pal" : "asi");		

			err = copy_from_user(&params, uarg, sizeof(struct pal_info));
			if (err)
				return -EFAULT;		

			spin_lock_irq(&dev->lock);

			/* Save the new parameters */			
#ifndef DEBUG			
			dev->write_fn(params.pal_ping_addr, dev_base + PAL_PING_ADDR);
#else		
			dev->write_fn(params.pal_ping_addr, dev_base + FPGA_REG_TEST2);
#endif
			spin_unlock_irq(&dev->lock);

		break;

		case PAL_GET_PAL_PONG_ADDR:
			pr_info("%s ioctl PAL_GET_PAL_PONG_ADDR.\n",(dev->devnum == 0) ? "pal" : "asi");

			spin_lock_irq(&dev->lock);

			/* Get the current parameters */
			params.pal_pong_addr = dev->read_fn(dev_base + PAL_PING_ADDR);

			spin_unlock_irq(&dev->lock);

			err = copy_to_user(uarg, &params, sizeof(struct pal_info));
			if (err)
				return -EFAULT;

		break;

		case PAL_SET_PAL_PONG_ADDR:
			pr_info("%s ioctl PAL_SET_PAL_PONG_ADDR.\n",(dev->devnum == 0) ? "pal" : "asi");			

			err = copy_from_user(&params, uarg, sizeof(struct pal_info));
			if (err)
				return -EFAULT;		

			spin_lock_irq(&dev->lock);

			/* Save the new parameters */			
			dev->write_fn(params.pal_pong_addr, dev_base + PAL_PONG_ADDR);				

			spin_unlock_irq(&dev->lock);

		break;

		case PAL_GET_ASI_PING_ADDR:			
			pr_info("%s ioctl PAL_GET_ASI_PING_ADDR.\n",(dev->devnum == 0) ? "pal" : "asi");

			spin_lock_irq(&dev->lock);

			/* Get the current parameters */
			params.asi_ping_addr = dev->read_fn(dev_base + ASI_PING_ADDR);

			spin_unlock_irq(&dev->lock);

			err = copy_to_user(uarg, &params, sizeof(struct pal_info));
			if (err)
				return -EFAULT;

		break;

		case PAL_SET_ASI_PING_ADDR:			
			pr_info("%s ioctl PAL_SET_ASI_PING_ADDR.\n",(dev->devnum == 0) ? "pal" : "asi");			

			err = copy_from_user(&params, uarg, sizeof(struct pal_info));
			if (err)
				return -EFAULT;		

			spin_lock_irq(&dev->lock);

			/* Save the new parameters */
			dev->write_fn(params.asi_ping_addr, dev_base + ASI_PING_ADDR);

			spin_unlock_irq(&dev->lock);

		break;

		case PAL_GET_ASI_PONG_ADDR:
			pr_info("%s ioctl PAL_GET_ASI_PONG_ADDR.\n",(dev->devnum == 0) ? "pal" : "asi");	

			spin_lock_irq(&dev->lock);

			/* Get the current parameters */
			params.asi_pong_addr = dev->read_fn(dev_base + ASI_PING_ADDR);

			spin_unlock_irq(&dev->lock);

			err = copy_to_user(uarg, &params, sizeof(struct pal_info));
			if (err)
				return -EFAULT;

		break;

		case PAL_SET_ASI_PONG_ADDR:
			pr_info("%s ioctl PAL_SET_ASI_PONG_ADDR.\n",(dev->devnum == 0) ? "pal" : "asi");			

			err = copy_from_user(&params, uarg, sizeof(struct pal_info));
			if (err)
				return -EFAULT;		

			spin_lock_irq(&dev->lock);

			/* Save the new parameters */			
			dev->write_fn(params.asi_pong_addr, dev_base + ASI_PONG_ADDR);

			spin_unlock_irq(&dev->lock);

		break;

		case PAL_GET_FPGA_VER:
			pr_info("%s ioctl PAL_GET_FPGA_VER.\n",(dev->devnum == 0) ? "pal" : "asi");

			spin_lock_irq(&dev->lock);

			/* Get the current parameters */
			vers.fpga_ver_num 			= (__u8)(dev->read_fn(dev_base + FPAG_VER_NO));
			vers.fpag_compile_date 		= dev->read_fn(dev_base + COMPILE_DATE);
			vers.fpga_compile_time		= dev->read_fn(dev_base + COMPILE_TIME);

			spin_unlock_irq(&dev->lock);

			err = copy_to_user(uarg, &vers, sizeof(struct pal_fpga_ver));
			if (err)
				return -EFAULT;
		break;

		case PAL_GET_PAL_RX_LEN:
			pr_info("%s ioctl PAL_GET_PAL_RX_LEN.\n",(dev->devnum == 0) ? "pal" : "asi");

			spin_lock_irq(&dev->lock);

			/* Get the current parameters */
#ifndef DEBUG
			params.pal_rx_max_len 			= dev->read_fn(dev_base + PAL_FRAME_LEN);
			pr_info("dev_base:0x%p,PAL_FRAME_LEN:0x%x.len:0x%x.\n",dev_base,PAL_FRAME_LEN,params.pal_rx_max_len);
#else
			params.pal_rx_max_len 			= dev->read_fn(dev_base + FPGA_REG_TEST1);
			pr_info("dev_base:0x%p,FPGA_REG_TEST1:0x%x.len:0x%x.\n",dev_base,FPGA_REG_TEST1,params.pal_rx_max_len);
#endif
			spin_unlock_irq(&dev->lock);

			err = copy_to_user(uarg, &params, sizeof(struct pal_info));
			if (err)
				return -EFAULT;
		break;

		case PAL_GET_ASI_RX_LEN:
			pr_info("%s ioctl PAL_GET_ASI_RX_LEN.\n",(dev->devnum == 0) ? "pal" : "asi");

			spin_lock_irq(&dev->lock);

			/* Get the current parameters */
			params.asi_rx_max_len 			= dev->read_fn(dev_base + ASI_FRAME_LEN);

			spin_unlock_irq(&dev->lock);

			err = copy_to_user(uarg, &params, sizeof(struct pal_info));
			if (err)
				return -EFAULT;
		break;		



		default:
			break;
	}

	return 0;
}


static struct file_operations fops 	= {
	.owner  			= THIS_MODULE,
	.read   			= pal_read_kfifo,//pal_read,
	.write  			= pal_write,
	.unlocked_ioctl 	= pal_unlocked_ioctl,
	.open   			= pal_open,
	.release 			= pal_release,
};

static irqreturn_t pal_rx_interrupt(int irq, void *dev_id)
{
	int handled = 0;
	unsigned long flags;

	spin_lock_irqsave(&paldev[0].irq_lock, flags);

	pal_proc_fs.pal_interrupt_cnt += 1;

	paldev[0].rx_interrupt_flag = 1;
	wake_up(&paldev[0].pal_queue);

	spin_unlock_irqrestore(&paldev[0].irq_lock, flags);

	handled = 1;
	return IRQ_RETVAL(handled);
} /* pal_rx_interrupt */

static irqreturn_t asi_rx_interrupt(int irq, void *dev_id)
{
	int handled = 0;
	unsigned long flags;

	spin_lock_irqsave(&paldev[1].irq_lock, flags);

	pal_proc_fs.asi_interrupt_cnt += 1;

	paldev[1].rx_interrupt_flag = 1;
	wake_up(&paldev[1].pal_queue);

	handled = 1;
	spin_unlock_irqrestore(&paldev[1].irq_lock, flags);
	return IRQ_RETVAL(handled);
} /* asi_rx_interrupt */

/* pal proc fs for debug */
static struct proc_dir_entry *pal_proc_root = NULL;
static int pal_proc_show(struct seq_file *m, void *v) {
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

static int pal_init_proc(void) {
    pal_proc_root = proc_mkdir("pal", NULL);

    if(pal_proc_root) {
        proc_create("statistics",  0, pal_proc_root, &pal_proc_fops);
        return 0;
    }
    return -1;
}

static int init_pal_procfs(void) {
    pal_init_proc();
    return 0;
}

static void cleanup_pal_procfs(void) {
    if (pal_proc_root) {
        remove_proc_entry("statistics",  pal_proc_root);
        remove_proc_entry("pal",NULL);
    }
}

static int __init paldev_init(void)
{
	int i;
	int ret;
	pr_info("%s \n", __func__);	

	dev_base = ioremap(PAL_FPGA_BASE_ADDR,PAL_FPGA_REG_LEN);
	if (dev_base == NULL) {
		pr_err("%s ioremap 0x%x error!\n",__func__,PAL_FPGA_BASE_ADDR);
		goto ioremap_err;
	}
	pr_info("%s dev_base is 0x%p.\n",__func__,dev_base);

	if(major) { //静态申请设备号		
		pal_first = MKDEV(major, minor_base);
		if (register_chrdev_region(pal_first, minor_num, DEVICE_NAME)) {
			pr_err("%s failed to register major %d.\n",__func__,major);
			goto register_err;
		}
		pr_info("%s alloc device number static OK.\n",__func__);

	} else { //动态申请设备号		
		if (alloc_chrdev_region(&pal_first, minor_base, minor_num, DEVICE_NAME)) {
			pr_err("%s failed to allocate character device region\n",__func__);
			goto register_err;		
		}
		pr_info("%s alloc device number dynamic %d OK.\n",__func__,minor_num);
	}

	for (i = 0; i < minor_num; i++) {
		cdev_init(&paldev[i].cdev,&fops);
		paldev[i].cdev.owner = THIS_MODULE;
		paldev[i].fifo = i == 0 ? (struct kfifo *)&g_pal_fifo : (struct kfifo *)&g_asi_fifo;

		ret = cdev_add(&paldev[i].cdev, pal_first + i, 1);
		if(ret)
			goto cdev_add_err;
		pr_info("%s cdev_add %d OK.",__func__,i);
	}
	pr_info("%s success to add cdev.\n",__func__);

	/* Create a sysfs class for pal */
	pal_class = class_create(THIS_MODULE, "pal_class");
	if (IS_ERR(pal_class)) {
		pr_err("%s failed to create a sysfs class for pal\n",__func__);
		goto class_create_err;
	}
	pr_info("%s class_create pal_class OK.\n",__func__);

	/* Register with sysfs so udev can pick it up. */
	for (i = 0; i < minor_num; i++) {
		paldev[i].pal_device = device_create(pal_class, NULL, pal_first + i, NULL, "%s%d",((i == 0) ? DEVICE_NAME : ASI_DEVICE_NAME),0);
		if (!paldev[i].pal_device) {
			pr_err("%s create device %s%d failed.",__func__,((i == 0) ? DEVICE_NAME : ASI_DEVICE_NAME),0);		
			
			goto device_create_err;
		}
		pr_info("%s device_create %d OK.",__func__,i);
	}

	/* Initial spinlock */
	for (i = 0; i < minor_num ; i++) {
		spin_lock_init(&paldev[i].lock);
	}

	init_pal_procfs();
	pr_info("%s OK.\n", __func__);
	return 0;

device_create_err: 
    /* del_cdev  */
	for (--i; i >0; --i) {
		cdev_del(&paldev[i].cdev);
	}

class_create_err: 
	/* del_dev_node */
	for(i = 0; i < minor_num; i++) {
		device_destroy(pal_class, pal_first + i);
	}
	class_destroy(pal_class);

cdev_add_err:
	for (i = 0; i < minor_num; i++) {
		cdev_del(&paldev[i].cdev);
	}

register_err: 
	/* free_devno */
	unregister_chrdev_region(pal_first, minor_num);

ioremap_err:
	iounmap(dev_base); 
	pr_err("%s failed.\n", __func__);
	return -1;
}
module_init(paldev_init);

static void __exit paldev_exit(void)
{
	int i;	
	pr_info("%s \n", __func__);
	
	iounmap(dev_base);

	for (i = 0; i< minor_num; i++) {
		cdev_del(&paldev[i].cdev);
		device_destroy(pal_class, pal_first + i);

		if (paldev[i].irq_init_flag == 1) {
			free_irq(paldev[i].irq, &paldev[i]);
		}
	}
	class_destroy(pal_class);
	unregister_chrdev_region(pal_first, minor_num);

	cleanup_pal_procfs();
}
module_exit(paldev_exit);


MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("huqq@zoonghe.com");
