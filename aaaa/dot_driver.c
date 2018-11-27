#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <mach/platform.h>
#include <linux/io.h>
#include <linux/delay.h>
#define DOT_MAJOR	262
#define DOT_NAME        "DOT_DRIVER"

#define GPIO_SIZE       256

char dot_usage = 0;
static void *dot_map;
volatile unsigned *dot;

static int dot_open(struct inode *minode, struct file *mfile)
{
	int index,index2;
	if (dot_usage != 0)
		return -EBUSY;
	dot_usage = 1;

	dot_map = ioremap(GPIO_BASE, GPIO_SIZE);
	if (!dot_map)
	{
		printk("error: mapping gpio memory");
		iounmap(dot_map);
		return -EBUSY;
	}

	dot = (volatile unsigned int *)dot_map;

	for (index = 0; index < 8; ++index) // GPIO 10~17
	{
		*(dot + 1) &= ~(0x07 << (3 * index));
		*(dot + 1) |= (0x01 << (3 * index)); //10~17 OUT SET
	}

	for (index2 = 0; index2 < 8; ++index2) // GPIO 20~27
	{
		*(dot + 2) &= ~(0x07 << (3 * index2));
		*(dot + 2) |= (0x01 << (3 * index2)); //20~27 OUT SET
	}
	return 0;
}

static int dot_release(struct inode *minode, struct file *mfile)                                                                                                                                         
{
	dot_usage = 0;
	if (dot)
		iounmap(dot);

	return 0;
}

static int dot_write(struct file *mfile, const char *gdata, size_t length, loff_t *off_what)
{
	char    tmp_buf[8];
	int     result,i;
	int	index;
	char	val[8];

	result = copy_from_user(&tmp_buf, gdata, length); //1byte read from test progarm
	if (result < 0)
	{
		printk("Error: copy from user");
		return result;
	}

	//printk("data from app : %d\n", tmp_buf);

	// Control DOT
	
	for(i=0;i<8;i++){
		val[i] = gdata[i];
	}

	for(index =0; index<8; index++){
		*(dot + 7) =(val[index] << 10);
		*(dot + 7) =(0x01 << (20+index));
		sleep(1000);
	
	*(dot + 10) = (0xFF << 20);
	*(dot + 10) = (0xFF << 10);
	}

	return length;
}

static struct file_operations dot_fops =
{
	.owner          = THIS_MODULE,
	.open           = dot_open,
	.release        = dot_release,
	.write          = dot_write,
};

static int dot_init(void)
{
	int result;
	
	result = register_chrdev(DOT_MAJOR, DOT_NAME, &dot_fops);
	if (result < 0)
	{
		printk(KERN_WARNING "Can't get any major!\n");
		return result;
	}
	return 0;
}

static void dot_exit(void)
{
	unregister_chrdev(DOT_MAJOR, DOT_NAME);
	printk("DOT module removed.\n");
}

module_init(dot_init);
module_exit(dot_exit);

MODULE_LICENSE("GPL");

