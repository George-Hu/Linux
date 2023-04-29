#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/major.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/kfifo.h>

#define VSER_MAJOR		202
#define VSER_MINOR		0
#define VSER_DEV_CNT	2
#define VSER_DEV_NAME	"vser"

int vser_major 		= VSER_MAJOR;
int vser_minor 		= VSER_MINOR;
int vser_dev_cnt 	= VSER_DEV_CNT;


static dev_t kfifo_dev;
static struct class *kfifo_class;

#define TEST_KFIFO_SIZE		0x20000  //128K

static DEFINE_KFIFO(vsfifo0, unsigned char, TEST_KFIFO_SIZE); //32
static DEFINE_KFIFO(vsfifo1, unsigned char, TEST_KFIFO_SIZE); //32

struct vser_dev {
	struct cdev cdev;
	struct device *kfifo_device;
	struct kfifo *fifo;
};

static struct vser_dev vsdev[2];

static int vser_open(struct inode *inode, struct file *filp)
{
	filp->private_data = container_of(inode->i_cdev, struct vser_dev, cdev);
	return 0;
}

static int vser_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static ssize_t vser_read(struct file *filp, char __user *buf, size_t count, loff_t *pos)
{
	int ret;
	unsigned int copied = 0;
	struct vser_dev *dev = filp->private_data;

	if(kfifo_is_empty(dev->fifo)) {
		pr_info("kfifo is empty, kfifo_len=0x%x,kfifo_avail=0x%x.\n",kfifo_len(dev->fifo),kfifo_avail(dev->fifo));
		return -1;
	}
	ret = kfifo_to_user(dev->fifo, buf, count, &copied);

	return copied;
}

static ssize_t vser_write(struct file *filp, const char __user *buf, size_t count, loff_t *pos)
{
	int ret;
	unsigned int copied = 0;
	struct vser_dev *dev = filp->private_data;
	
	if (kfifo_is_full(dev->fifo)) {
		pr_info("kfifo is full, kfifo_len=0x%x,kfifo_avail=0x%x.\n",kfifo_len(dev->fifo),kfifo_avail(dev->fifo));
		return -1;
	}

	ret = kfifo_from_user(dev->fifo, buf, count, &copied);

	return copied;
}

static struct file_operations vser_ops = {
	.owner 		= THIS_MODULE,
	.open 		= vser_open,
	.release 	= vser_release,
	.read 		= vser_read,
	.write 		= vser_write,
};

static int __init vser_init(void)
{
	int i;
	int ret;
	printk("%s",__func__);
	//pr_info("%s", __func__);
	if (vser_major) {
		kfifo_dev = MKDEV(vser_major, vser_minor);
		pr_info("%s,kfifo_dev is %d.",__func__,kfifo_dev);
		if (register_chrdev_region(kfifo_dev, vser_dev_cnt, VSER_DEV_NAME)) {
			pr_info("%s failed to register major %d.\n",__func__,vser_major);
			goto reg_err;
		}
	} else {
		if (alloc_chrdev_region(&kfifo_dev, vser_minor, vser_dev_cnt, VSER_DEV_NAME)) {
			pr_info("%s failed to allocate character device region\n",__func__);
			goto reg_err;		
		}
	}	

	for (i = 0; i < vser_dev_cnt; i++) {
		cdev_init(&vsdev[i].cdev, &vser_ops);
		vsdev[i].cdev.owner = THIS_MODULE;
		vsdev[i].fifo = (i == 0) ? (struct kfifo *)&vsfifo0 : (struct kfifo*)&vsfifo1;
		ret = cdev_add(&vsdev[i].cdev, kfifo_dev + i, 1);	
		if (ret) {
			pr_info("%s cdev_add failed.", __func__);
			goto add_err;
		}		
	}

	kfifo_class = class_create(THIS_MODULE, "kfifo_class");
	if (IS_ERR(kfifo_class)) {
		pr_err("%s failed to create a sysfs class for pal\n",__func__);
		goto class_create_err;
	}

	/* Register with sysfs so udev can pick it up. */
	for (i = 0; i < vser_dev_cnt; i++) {
		vsdev[i].kfifo_device = device_create(kfifo_class, NULL, kfifo_dev + i, NULL, "%s%d",VSER_DEV_NAME,i);
		if (!vsdev[i].kfifo_device) {
			pr_err("%s create device %s%d failed.",__func__,VSER_DEV_NAME,i);	
			goto add_err;
		}
	}
	return 0;


add_err:
	for (--i; i > 0; --i)
		cdev_del(&vsdev[i].cdev);	

class_create_err:
	/* del_dev_node */
	for(i = 0; i < vser_minor; i++) {
		device_destroy(kfifo_class, kfifo_dev + i);
	}
	class_destroy(kfifo_class);

reg_err:
	unregister_chrdev_region(kfifo_dev, vser_dev_cnt);
	pr_info("%s failed.", __func__);
	return ret;
}
module_init(vser_init);

static void __exit vser_exit(void)
{
	int i;
	printk("%s",__func__);
	//pr_info("%s", __func__);
	for (i = 0; i < vser_dev_cnt; i++) {
		cdev_del(&vsdev[i].cdev);
		device_destroy(kfifo_class, kfifo_dev + i);
	}
	class_destroy(kfifo_class);
	unregister_chrdev_region(kfifo_dev, vser_dev_cnt);
}
module_exit(vser_exit);



MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("huqiqiao <huqq@zoonghe.com>");
MODULE_DESCRIPTION("A simple character device driver");
MODULE_INFO(intree, "Y");