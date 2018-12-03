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
#include <linux/time.h>
#include <linux/unistd.h>

#define	LED_MAJOR	221
#define	LED_NAME	"LED_DRIVER"

#define	GPIO_SIZE	256

char led_usage = 0;
static void *led_map;
volatile unsigned *led;

char gauge[] = { 0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F,0x7F, 0xFF };
char count[] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
char num[10][8] = { 
	{ 0x78,0x84,0x8c,0x94,0xa4,0xc4,0x78,0x00 },
	{ 0x30,0x70,0x30,0x30,0x30,0x30,0xfc,0x00 },
	{ 0x78,0xcc,0x0c,0x38,0x60,0xcc,0xfc,0x00 },
	{ 0x78,0xcc,0x0c,0x38,0x0c,0xcc,0x78,0x00 },
	{ 0x1c,0x3c,0x6c,0xcc,0xfe,0x0c,0x1e,0x00 },
	{ 0xfc,0xc0,0xf8,0x0c,0x0c,0xcc,0x78,0x00 },
	{ 0x38,0x60,0xc0,0xf8,0xcc,0xcc,0x78,0x00 },
	{ 0xfc,0xcc,0x0c,0x18,0x30,0x30,0x30,0x00 },
	{ 0x78,0xcc,0xcc,0x78,0xcc,0xcc,0x78,0x00 },
	{ 0x78,0xcc,0xcc,0x7c,0x0c,0x18,0x70,0x00 }
};



static int led_open(struct inode *minode, struct file *mfile)
{
	int i;
	if (led_usage != 0)
		return -EBUSY;
	led_usage = 1;

	led_map = ioremap(GPIO_BASE, GPIO_SIZE);
	if (!led_map)
	{
		printk("error: mapping gpio memory");
		iounmap(led_map);
		return -EBUSY;
	}

	led = (volatile unsigned int *)led_map;
	for (i = 0; i < 8; ++i)
	{
		*(led + 1) &= ~(0x7 << (3 * i));
		*(led + 1) |= (0x1 << (3 * i));
		*(led + 2) &= ~(0x7 << (3 * i));
		*(led + 2) |= (0x1 << (3 * i));
	}
	*(led + 10) = (0xFF << 10);
	*(led + 7) = (0xFF << 20);

	return 0;
}

static int led_release(struct inode *minode, struct file *mfile)
{
	led_usage = 0;
	if (led)
		iounmap(led);

	return 0;
}

static int led_write(struct file *mfile, const char *gdata, size_t length, loff_t *off_what)
{
	char	tmp_buf;
	int	result;
	int i, j;

	result = copy_from_user(&tmp_buf, gdata, length);
	if (result < 0)
	{
		printk("Error: copy from user");
		return result;
	}

	printk("data from app : %d\n", tmp_buf);

	// Control LED
	*(led + 10) = (0xFF << 10);
	*(led + 7) = (0xFF << 20);
	if((tmp_buf/10)==1)
	{
		*(led + 10) = (0xFF <<20);
		*(led + 7) = (gauge[tmp_buf%10] <<10);
	}
	else 
	{
		for(j=0;j<50000;j++)
		{
			for(i=0; i < 8;i++)
			{
				

				*(led + 10) = (count[i] <<20);
				*(led + 7) = (num[tmp_buf][i] <<10);
				udelay(1);

				*(led + 10) = (0xFF <<10);
				*(led + 7) = (0xFF << 20);
			}
		}
	}

	return length;
}

static struct file_operations led_fops =
{
	.owner		= THIS_MODULE,
	.open		= led_open,
	.release	= led_release,
	.write		= led_write,
};

static int led_init(void)
{
	int result;

	result = register_chrdev(LED_MAJOR, LED_NAME, &led_fops);
	if (result < 0)
	{
		printk(KERN_WARNING "Can't get any major!\n");
		return result;
	}
	return 0;
}

static void led_exit(void)
{
	unregister_chrdev(LED_MAJOR, LED_NAME);
	printk("LED module removed.\n");
}

module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");
