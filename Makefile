MODULE_NAME = svm_driver
APP_NAME = test_svm
KDIR = /root/raspberry/linux
TOOLCHAIN = arm-linux-gnueabihf-
CC = $(TOOLCHAIN)gcc
TARGET = arm
PWD	:= $(shell pwd)
obj-m	:= $(MODULE_NAME).o

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
