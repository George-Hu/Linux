


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

#if 1
/* pal proc fs for debug */
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

#endif



static int __init paldev_init(void) 
{

}
module_init(paldev_init);

static void __exit paldev_exit(void) 
{
	int i;	
	pr_info("%s \n", __func__);
	
	release_mem_region(PAL_FPGA_BASE_ADDR, 0x100);
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