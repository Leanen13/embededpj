#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <mach/platform.h>
#include <linux/io.h>

#define	SVM_MAJOR	221
#define	SVM_NAME	"SVM_DRIVER"

#define	GPIO_SIZE	256

char svm_usage = 0;
static void *svm_map;
volatile unsigned *svm;

static int svm_open(struct inode *minode, struct file *mfile)
{
	if (svm_usage != 0)
		return -EBUSY;
	svm_usage = 1;

	svm_map = ioremap(GPIO_BASE, GPIO_SIZE);
	if (!svm_map)
	{
		printk("error: mapping gpio memory");
		iounmap(svm_map);
		return -EBUSY;
	}

	svm = (volatile unsigned int *)svm_map;
	*(svm + 1) &= ~(0x7 << (3 * 7));
	*(svm + 1) |= (0x1 << (3 * 7));

	return 0;
}

static int svm_release(struct inode *minode, struct file *mfile)
{
	svm_usage = 0;
	if (svm)
		iounmap(svm);

	return 0;
}

static int svm_write(struct file *mfile, const char *gdata, size_t length, loff_t *off_what)
{
	char	tmp_buf;
	int	result;

	result = copy_from_user(&tmp_buf, gdata, length);
	if (result < 0)
	{
		printk("Error: copy from user");
		return result;
	}

	printk("data from app : %d\n", tmp_buf);

	// Control svm
	if (tmp_buf == 0)
		*(svm + 7) |= (0x1 << 17);
	else
		*(svm + 10) |= (0x1 << 17);

	return length;
}

static struct file_operations svm_fops =
{
	.owner		= THIS_MODULE,
	.open		= svm_open,
	.release	= svm_release,
	.write		= svm_write,
};

static int svm_init(void)
{
	int result;

	result = register_chrdev(svm_MAJOR, svm_NAME, &svm_fops);
	if (result < 0)
	{
		printk(KERN_WARNING "Can't get any major!\n");
		return result;
	}
	return 0;
}

static void svm_exit(void)
{
	unregister_chrdev(svm_MAJOR, svm_NAME);
	printk("svm module removed.\n");
}

module_init(svm_init);
module_exit(svm_exit);

MODULE_LICENSE("GPL");
