MODULE_NAME = hello_driver
KDIR = /root/rasp/linux
TOOLCHAIN = arm-linux-gnueabihf-
TARGET = arm
PWD	:= $(shell pwd)
obj-m	:= $(MODULE_NAME).o

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) ARCH=$(TARGET) CROSS_COMPILE=$(TOOLCHAIN) modules

install:
	cp -f $(MODULE_NAME).ko /root/rasp/nfs

clean:
	rm -f *.ko
	rm -f *.o
	rm -f *.mod.*
	rm -f .$(MODULE_NAME).*	
	rm -f modules.order
	rm -f Module.symvers
	rm -rf .tmp_versions

