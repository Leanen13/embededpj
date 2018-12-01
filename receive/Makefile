MODULE_NAME1 = key_driver
MODULE_NAME2 = key_driver2
MODULE_NAME3 = key_driver3
MODULE_NAME4 = key_driver4
MODULE_NAME5 = sens_driver
MODULE_NAME6 = sens_driver2
MODULE_NAME7 = sens_driver3
MODULE_NAME8 = dot_driver
MODULE_NAME9 = mot_driver
APP_NAME = test_mac
KDIR = /root/raspberry/linux
TOOLCHAIN = arm-linux-gnueabihf-
CC = $(TOOLCHAIN)gcc
TARGET = arm
PWD	:= $(shell pwd)
obj-m	:= $(MODULE_NAME1).o $(MODULE_NAME2).o $(MODULE_NAME3).o $(MODULE_NAME4).o $(MODULE_NAME5).o $(MODULE_NAME6).o $(MODULE_NAME7).o $(MODULE_NAME8).o $(MODULE_NAME9).o

all: app driver

driver:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) ARCH=$(TARGET) CROSS_COMPILE=$(TOOLCHAIN) modules

app:
	$(CC) -o $(APP_NAME) $(APP_NAME).c -lpthread

install:
	cp -f $(MODULE_NAME1).ko /root/raspberry/nfs
	cp -f $(MODULE_NAME2).ko /root/raspberry/nfs
	cp -f $(MODULE_NAME3).ko /root/raspberry/nfs	
	cp -f $(MODULE_NAME4).ko /root/raspberry/nfs
	cp -f $(MODULE_NAME5).ko /root/raspberry/nfs
	cp -f $(MODULE_NAME6).ko /root/raspberry/nfs	
	cp -f $(MODULE_NAME7).ko /root/raspberry/nfs
	cp -f $(MODULE_NAME8).ko /root/raspberry/nfs
	cp -f $(MODULE_NAME9).ko /root/raspberry/nfs
	cp -f $(APP_NAME) /root/raspberry/nfs

clean:
	rm -f *.ko
	rm -f *.o
	rm -f *.mod.*
	rm -f .$(MODULE_NAME1).*	
	rm -f .$(MODULE_NAME2).*	
	rm -f .$(MODULE_NAME3).*	
	rm -f .$(MODULE_NAME4).*	
	rm -f .$(MODULE_NAME5).*	
	rm -f .$(MODULE_NAME6).*	
	rm -f .$(MODULE_NAME7).*	
	rm -f .$(MODULE_NAME8).*		
	rm -f .$(MODULE_NAME9).*		
	rm -f $(APP_NAME)
	rm -f modules.order
	rm -f Module.symvers
	rm -rf .tmp_versions

