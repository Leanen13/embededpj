#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <mach/platform.h>
#include <linux/io.h>
#include <sys/time.h>
#include <unistd.h>

#define	SVM_MAJOR	224
#define	SVM_NAME	"SVM_DRIVER"
#define	GPIO_SIZE	256
#define GPIO_OUT 12

static volatile int begin
static volatile int pwm_range

char svm_usage = 0;
static void *svm_map;
volatile unsigned *svm;

void delayMicrosecondsHard (unsigned int howLong)
  {
    struct timeval tNow, tLong, tEnd ;

    gettimeofday (&tNow, NULL) ;
    tLong.tv_sec  = howLong / 1000000 ;
    tLong.tv_usec = howLong % 1000000 ;
    timeradd (&tNow, &tLong, &tEnd) ;

    while (timercmp (&tNow, &tEnd, <))
      gettimeofday (&tNow, NULL) ;
  }

void delayMicroseconds (unsigned int howLong)
{
  struct timespec sleeper ;
  unsigned int uSecs = howLong % 1000000 ;
  unsigned int wSecs = howLong / 1000000 ;

  if (howLong ==   0)
    return ;
  else if (howLong  < 100)
    delayMicrosecondsHard (howLong) ;
  else
  {
    sleeper.tv_sec  = wSecs ;
    sleeper.tv_nsec = (long)(uSecs * 1000L) ;
    nanosleep (&sleeper, NULL) ;
  }
}

static struct file_operations svm_fops =
{
	.owner	= THIS_MODULE,
	.open	= svm_open,
	.release	= svm_release,
	.write	= svm_write,
};

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

  pwm_rage=200;

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
    delayMicroseconds (begin * 100) ;

    if (space != 0)
      *(svm + 10) |= (0x1 << GPIO_OUT); //output set 0
    delayMicroseconds (space * 100) ;
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
