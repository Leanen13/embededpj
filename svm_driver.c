#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <mach/platform.h>
#include <linux/io.h>

#include <stdio.h>
#include <malloc.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>

#define	SVM_MAJOR	224
#define	SVM_NAME	"SVM_DRIVER"
#define	GPIO_SIZE	256
#define GPIO_OUT 12

static volatile int begin
static volatile int pwm_range
static volatile pthread_t thread

char svm_usage = 0;
static void *svm_map;
volatile unsigned *svm;

static void *PwmThread (void *arg) // set Thread
{
  struct sched_param param ;

  param.sched_priority = sched_get_priority_max (SCHED_RR) ;
  pthread_setschedparam (pthread_self (), SCHED_RR, &param) ;

  pin = *((int *)arg) ;
  free (arg) ;

  pin = GPIO_OUT;
  const int pri = 90;
  struct sched_param sched ;

  memset (&sched, 0, sizeof(sched)) ;

  if (pri > sched_get_priority_max (SCHED_RR))
    sched.sched_priority = sched_get_priority_max (SCHED_RR) ;
  else
    sched.sched_priority = pri ;

  sched_setscheduler (0, SCHED_RR, &sched) ;

  for (;;)
  {
    space = range - begin ;

    if (begin != 0)
      *(svm + 7) |= (0x1 << GPIO_OUT); //output set 1
    delayMicroseconds (begin * 100) ;

    if (space != 0)
      *(svm + 10) |= (0x1 << GPIO_OUT); //output set 0
    delayMicroseconds (space * 100) ;
  }

  return NULL ;
}

int PwmCreate (int pin, int initialValue, int pwmRange)
{
  int res ;
  pthread_t myThread ;
  int *passPin ;

  passPin = malloc (sizeof (*passPin)) ;
  if (passPin == NULL)
    return -1 ;

  *(svm + 10) |= (0x1 << GPIO_OUT); //output set 0

  begin = initialValue ;
  pwm_range = pwmRange ;

  *passPin = pin ;
  res      = pthread_create (&myThread, NULL, softPwmThread, (void *)passPin) ;

  while (newPin != -1)
    delay (1) ;

  thread = myThread ;

  return res ;
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

	svm = (volatile unsigned int *)svm_map;
	*(svm + 1) &= ~(0x7 << (3 * GPIO_OUT%10)); //clear
	*(svm + 1) |= (0x1 << (3 * GPIO_OUT%10)); //outputmode

  PwmCreate(GPIO_OUT,0,200);


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

	// Control LED
	if (tmp_buf == 'c')
		begin = 15;
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
