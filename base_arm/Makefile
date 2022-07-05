MOD := hello
obj-m := $(MOD).o

CROSS = ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-

#KDIR := /lib/modules/$(shell uname -r)/build
KDIR := /home/ubuntu/udoo_linux_bsp/kernel/3.14.1 
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules $(CROSS)
	cp $(MOD).ko /srv/nfs
clean:
	rm -rf *.mod.*
	rm -rf .*.cmd
	rm -rf *.o
	rm -rf *.mod
	rm -rf *.order
	rm -rf *.symvers
