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

#define	KEY_MAJOR	212
#define	KEY_NAME2	"KEY_DRIVER2"

#define	GPIO_SIZE	256

char key_usage = 0;
static void *key_map;
volatile unsigned *key2;
static char tmp_buf2;
static int event_flag2 = 0;
DECLARE_WAIT_QUEUE_HEAD(waitqueue);

static irqreturn_t ind_interrupt_handler(int irq, void *pdata)
{
	int tmp_key;
	tmp_key = (*(key2 + 13) & (1 << 3)) == 0 ? 0 : 1;
	
	if (tmp_key == 0)
		++tmp_buf2;

	wake_up_interruptible(&waitqueue);
	++event_flag2;

	return IRQ_HANDLED;
}

static unsigned key_poll(struct file *mfile, struct poll_table_struct *pt)
{
	int mask = 0;

	poll_wait(mfile, &waitqueue, pt); // interruptible_sleep_on..
	if (event_flag2 > 0)
		mask |= (POLLIN | POLLRDNORM);

	event_flag2 = 0;

	return mask;
}

static int key_open(struct inode *minode, struct file *mfile)
{
	int result;

	if (key_usage != 0)
		return -EBUSY;
	key_usage = 1;

	key_map = ioremap(GPIO_BASE, GPIO_SIZE);
	if (!key_map)
	{
		printk("error: mapping gpio memory");
		iounmap(key_map);
		return -EBUSY;
	}

	key2 = (volatile unsigned int *)key_map;

	// input(GPIO 3)
	*(key2) &= ~(0x7 << (3 * 3));
	*(key2) |= (0x0 << (3 * 3));

	// Edge
	*(key2 + 22) |= (0x1 << 3);
	result = request_irq(gpio_to_irq(3), ind_interrupt_handler, IRQF_TRIGGER_FALLING, "gpio_irq_key", NULL);
	if (result < 0)
	{
		printk("error: request_irq()");
		return result;
	}

	return 0;
}

static int key_release(struct inode *minode, struct file *mfile)
{
	key_usage = 0;
	if (key2)
		iounmap(key2);
	free_irq(gpio_to_irq(3), NULL);
	return 0;
}

static int key_read(struct file *mfile, char *gdata, size_t length, loff_t *off_what)
{
	int result;

	printk("key_read = %d\n", tmp_buf2);
	result = copy_to_user((void *)gdata, &tmp_buf2, length);
	if (result < 0)
	{
		printk("error: copy_to_user()");
		return result;
	}

	return length;
}

static int key_write(struct file *mfile, const char *gdata, size_t length, loff_t *off_what)
{
	char	tmp_buf2;
	int	result;

	result = copy_from_user(&tmp_buf2, gdata, length);
	if (result < 0)
	{
		printk("Error: copy from user");
		return result;
	}

	printk("data from app : %d\n", tmp_buf2);

	return length;
}

static struct file_operations key_fops =
{
	.owner		= THIS_MODULE,
	.open		= key_open,
	.release	= key_release,
	.read		= key_read,
	.write		= key_write,
	.poll		= key_poll,
};

static int _key_init(void)
{
	int result;

	result = register_chrdev(KEY_MAJOR, KEY_NAME2, &key_fops);
	if (result < 0)
	{
		printk(KERN_WARNING "Can't get any major!\n");
		return result;
	}
	return 0;
}

static void key_exit(void)
{
	unregister_chrdev(KEY_MAJOR, KEY_NAME2);
	printk("KEY module removed.\n");
}

module_init(_key_init);
module_exit(key_exit);

MODULE_LICENSE("GPL");
