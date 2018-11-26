#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <mach/platform.h>
#include <linux/io.h>

#define	SVM_MAJOR	224
#define	SVM_NAME	"SVM_DRIVER"
#define	GPIO_SIZE	256
#define GPIO_OUT 12

static volatile int begin=0;
static volatile int pwm_range=200;

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
	*(svm + 1) &= ~(0x7 << (3 * GPIO_OUT%10)); //clear
	*(svm + 1) |= (0x1 << (3 * GPIO_OUT%10)); //outputmode

	return 0;
}

static int svm_release(struct inode *minode, struct file *mfile)
{
	svm_usage = 0;
	if (svm)
		iounmap(svm);

 pthread_cancel (threads);

	return 0;
}

static int svm_write(struct file *mfile, const char *gdata, size_t length, loff_t *off_what)
{
	char tmp_buf;
	char result;
  pthread_t myThread;
  int *pin;

	result = copy_from_user(&tmp_buf, gdata, length);
	if (result < 0)
	{
		printk("Error: copy from user");
		return result;
	}

	printk("char from app : %c\n", tmp_buf);

	// Control svm
	if (tmp_buf == 'c'){
   	 while(1){
		begin = 15;
  		space = pwm_range - begin;
   		if (begin != 0)
     			*(svm + 7) |= (0x1 << GPIO_OUT); //output set 1
  		 delay(1);

  		if (space != 0)
    			*(svm + 10) |= (0x1 << GPIO_OUT); //output set 0
    		delay(1);
  	}
  }
	else if(tmp_buf == 'r')
   		begin = 24;
 	else if(tmp_buf == 'l')
		begin = 5;
 	else if(tmp_buf == 'q')
 		return 0;

	return length;
}

static struct file_operations svm_fops =
{
	.owner	= THIS_MODULE,
	.open	= svm_open,
	.release	= svm_release,
	.write	= svm_write,
};

static int svm_init(void)
{
	int result;

	result = register_chrdev(SVM_MAJOR, SVM_NAME, &svm_fops);
	if (result < 0)
	{
		printk(KERN_WARNING "Can't get any major!\n");
		return result;
	}
	return 0;
}

static void svm_exit(void)
{
	unregister_chrdev(SVM_MAJOR, SVM_NAME);
	printk("servo motor module removed.\n");
}

module_init(svm_init);
module_exit(svm_exit);
MODULE_LICENSE("GPL");
