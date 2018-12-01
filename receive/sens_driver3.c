#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <mach/platform.h>
#include <linux/io.h>
#include <linux/poll.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/delay.h>

#define	SENS_MAJOR	218
#define	SENS_NAME	"SENS_DRIVER"

#define	GPIO_SIZE	256

char sens_usage = 0;
static void *sens_map;
volatile unsigned *sens;
static char tmp_buf;


static int sens_open(struct inode *minode, struct file *mfile)
{
	//int result;

	if (sens_usage != 0)
		return -EBUSY;
	sens_usage = 1;

	sens_map = ioremap(GPIO_BASE, GPIO_SIZE);
	if (!sens_map)
	{
		printk("error: mapping gpio memory");
		iounmap(sens_map);
		return -EBUSY;
	}

	sens = (volatile unsigned int *)sens_map;

	// sens output(GPIO 6)
	*(sens) &= ~(0x7 << (3 * 8));
	*(sens) |= (0x0 << (3 * 8));

	return 0;
}

static int sens_release(struct inode *minode, struct file *mfile)
{
	sens_usage = 0;
	if (sens)
		iounmap(sens);
	free_irq(gpio_to_irq(8), NULL);
	return 0;
}

static int sens_read(struct file *mfile, char *gdata, size_t length, loff_t *off_what)
{
	int result;
	mdelay(100);//delay

	tmp_buf = (*(sens + 13) & (1 << 8)) == 0 ? 0 : 1; 
	
	result = copy_to_user(gdata, &tmp_buf, length);	

	if (result < 0)
	{
		printk("error: copy_to_user()");
		return result;
	}
	return length;
}


static struct file_operations sens_fops =
{
	.owner		= THIS_MODULE,
	.open		= sens_open,
	.release	= sens_release,
	.read		= sens_read
};

static int _sens_init(void)
{
	int result;

	result = register_chrdev(SENS_MAJOR, SENS_NAME, &sens_fops);
	if (result < 0)
	{
		printk(KERN_WARNING "Can't get any major!\n");
		return result;
	}
	return 0;
}

static void sens_exit(void)
{
	unregister_chrdev(SENS_MAJOR, SENS_NAME);
	printk("SENS module removed.\n");
}

module_init(_sens_init);
module_exit(sens_exit);

MODULE_LICENSE("GPL");
