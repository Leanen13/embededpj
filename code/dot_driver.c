#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <mach/gpio.h>
#include <mach/regs-gpio.h>
#include <plat/gpio-cfg.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/kernel.h>
#include <linux/ioport.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/version.h>
#include "./dothead.h"
#define IOM_FPGA_DOT_MAJOR 262 	    // ioboard led device major number
#define IOM_FPGA_DOT_NAME "fpga_dot"           // ioboard led device name
#define IOM_FPGA_DOT_ADDRESS 0x04000210   // pysical address
#define IOM_FPGA_DEMO_ADDRESS 0x04000300
#define UON   0x00    // IOM
#define UOFF 0x01    // IOM
//Global variable
static int fpga_dot_port_usage = 0;
static unsigned char *iom_fpga_dot_addr;// define functions...
ssize_t iom_fpga_dot_write(struct file *inode, const char *gdata, size_t length, loff_t
*off_what);
int iom_fpga_dot_open(struct inode *minode, struct file *mfile);
int iom_fpga_dot_release(struct inode *minode, struct file *mfile);

// define file_operations structure
struct file_operations iom_fpga_dot_fops =
{
	owner:  THIS_MODULE,
	open:     iom_fpga_dot_open,
	write:     iom_fpga_dot_write,
	release: iom_fpga_dot_release,
};

int iom_fpga_dot_open(struct inode *minode, struct file *mfile)  // fpga_dot device  opens
{
	if(fpga_dot_port_usage != 0) return -EBUSY;
	fpga_dot_port_usage = 1;
	return 0;
}
int iom_fpga_dot_release(struct inode *minode, struct file *mfile) // fpga_dot device closes
{
	fpga_dot_port_usage = 0;
	return 0;
}
ssize_t iom_fpga_dot_write(struct file *inode, const char *gdata, size_t length, loff_t *off_what)
{
	int i;
	unsigned char value[10];
	const char *tmp = gdata;
	if (copy_from_user(&value, tmp, length))
		return -EFAULT;
	for(i=0;i<length;i++)
		outb(value[i],(unsigned int)iom_fpga_dot_addr+i);
	return length;
}
int __init iom_fpga_dot_init(void)
{
	int result;
	result = register_chrdev(IOM_FPGA_DOT_MAJOR, IOM_FPGA_DOT_NAME, 	         &iom_fpga_dot_fops);
	if(result < 0) {
		printk(KERN_WARNING"Can't get any major\n");
		return result;
	}
	iom_fpga_dot_addr = ioremap(IOM_FPGA_DOT_ADDRESS, 0x10);
	iom_demo_addr = ioremap(IOM_FPGA_DEMO_ADDRESS, 0x1);
	outb(UON,(unsigned int)iom_demo_addr);
	printk("init module, %s major number : %d\n", IOM_FPGA_DOT_NAME, \
	                                                                                   IOM_FPGA_DOT_MAJOR);
	return 0;
}
void __exit iom_fpga_dot_exit(void)
{
	iounmap(iom_fpga_dot_addr);
	unregister_chrdev(IOM_FPGA_DOT_MAJOR, IOM_FPGA_DOT_NAME);
}

module_init(iom_fpga_dot_init);
module_exit(iom_fpga_dot_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Huins");
