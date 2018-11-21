#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>

#define	DEV_MAJOR_NUMBER	220
#define	DEV_NAME		"hello_driver"

static struct file_operations hello_fops = 
{
	.owner = THIS_MODULE,
};

static int dev_init(void)
{
	printk("[hello_driver] Hello\n");
	register_chrdev(DEV_MAJOR_NUMBER, DEV_NAME, &hello_fops);
	return 0;
}

static void dev_exit(void)
{
	printk("[hello_driver] Goodbye\n");
	unregister_chrdev(DEV_MAJOR_NUMBER, DEV_NAME);
}

module_init(dev_init);
module_exit(dev_exit);

MODULE_LICENSE("GPL");

