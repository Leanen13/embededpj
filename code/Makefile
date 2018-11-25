#Makefile for a basic kernel module
obj-m := fpga_dot_driver.o
KDIR :=/root/raspberry/linux
TOOLCHAIN = arm-linux-gnueabihf-
CC = $(TOOLCHAIN)gcc
TARGET = arm
PWD :=$(shell pwd)
APP := fpga_test_dot
all: driver app
#all: driver
all: app driver
driver:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) ARCH=$(TARGET) CROSS_COMPILE=$(TOOLCHAIN) modules
app:
	$(CC) -o $(APP_NAME) $(APP_NAME).c

install:
	cp -f $(MODULE_NAME).ko /root/raspberry/nfs
	cp -f $(APP_NAME) /root/raspberry/nfs

clean:
	rm -f *.ko
	rm -f *.o
	rm -f *.mod.*
	rm -f .$(MODULE_NAME).*
	rm -f $(APP_NAME)	
	rm -f modules.order
	rm -f Module.symvers
	rm -rf .tmp_versions
