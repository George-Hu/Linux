#include <linux/module.h>  
#include <linux/kernel.h>  
#include <linux/init.h>  
#include <linux/proc_fs.h>  
#include <linux/jiffies.h>  
#include <linux/seq_file.h>

static struct proc_dir_entry *pal_proc_root = NULL;

int pal_open_count  = 100;
int pal_close_count = 200;
int pal_read_count  = 300;
int pal_write_count = 400;

static int pal_proc_show(struct seq_file *m, void *v) {
    seq_printf(m, "pal_open_count : 0x%08X\n", pal_open_count);
    seq_printf(m, "pal_close_count: 0x%08X\n", pal_close_count);
    seq_printf(m, "pal_read_count : 0x%08X\n", pal_read_count);
    seq_printf(m, "pal_write_count: 0x%08X\n", pal_write_count);
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
        proc_create("pal_statistics",  0, pal_proc_root, &pal_proc_fops);
        return 0;
    }
    return -1;
}

static int __init init_procfs_example(void) {
    pal_init_proc();
    return 0;
}
module_init(init_procfs_example);  

static void __exit cleanup_procfs_example(void) {
    if (pal_proc_root) {
        remove_proc_entry("pal_statistics",  pal_proc_root);
        remove_proc_entry("pal",NULL);
    }
}
module_exit(cleanup_procfs_example);  
  
MODULE_AUTHOR("huqq@zhoonghe.mail.com");  
MODULE_DESCRIPTION("procfs examples");  
MODULE_LICENSE("GPL");  