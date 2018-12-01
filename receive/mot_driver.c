#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <mach/platform.h>
#include <linux/io.h>

#define MOTOR_MAJOR       219
#define MOTOR_NAME        "MOT_DRIVER"

#define GPIO_SIZE       256

char motor_usage = 0;
static void *motor_map;
volatile unsigned *motor;

static int motor_open(struct inode *minode, struct file *mfile)
{
		if (motor_usage != 0)
			return -EBUSY;
		motor_usage = 1;

		motor_map = ioremap(GPIO_BASE, GPIO_SIZE);
		if (!motor_map)
		{
				printk("error: mapping gpio memory");
				iounmap(motor_map);
				return -EBUSY;
		}
		//gpio 9 output
		motor = (volatile unsigned int *)motor_map;
		*(motor ) &= ~(0x7 << (3 * 9));
		*(motor ) |= (0x1 << (3 * 9));

		return 0;
}

static int motor_release(struct inode *minode, struct file *mfile)
{
		motor_usage = 0;
		if (motor)
				iounmap(motor);

        return 0;
}
static int motor_write(struct file *mfile, const char *gdata, size_t length, loff_t *off_what)
{
        char    tmp_buf;
        int     result;

        result = copy_from_user(&tmp_buf, gdata, length);
        if (result < 0)
        {
                printk("Error: copy from user");
                return result;
        }

		printk("data from app : %d\n", tmp_buf);

		if( tmp_buf ==1)
				*(motor + 7) |= (0x1 << 9); //motor on
		if( tmp_buf ==0)
                *(motor + 10) |= (0x1 << 9); //motor off
                

        return length;
}

static struct file_operations motor_fops =
{
        .owner          = THIS_MODULE,
        .open           = motor_open,
        .release        = motor_release,
        .write          = motor_write,
};

static int motor_init(void)
{
        int result;

        result = register_chrdev(MOTOR_MAJOR, MOTOR_NAME, &motor_fops);
        if (result < 0)
        {
                printk(KERN_WARNING "Can't get any major!\n");
                return result;
        }
        return 0;
}

static void motor_exit(void)
{
        unregister_chrdev(MOTOR_MAJOR, MOTOR_NAME);
		printk("motor module removed.\n");
}

module_init(motor_init);
module_exit(motor_exit);

MODULE_LICENSE("GPL");
                                 
