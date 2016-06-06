#
# Module: merc.ko
#
# Description: Wireless 802.11 Kernel Link Emulation
#
# Author: Stuart Wells
#
# (C) Copyright 2016 Stuart Wells
#
#

SRCMERC := ../src/mercury/

# Specify adition include paths
EXTRA_CFLAGS=-I$(PWD)/$(SRC) -I$(PWD)
#NOSTDINC_FLAGS := -I$(PWD)

obj-m := merc.o
merc-objs := $(SRCMERC)/mercuryModule.o $(SRCMERC)/socketNet.o
KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default: merc

merc:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	rm -rf *.o .*.cmd *.ko Module.* *.mod.c .tmp_versions modules.order
	rm -rf $(SRCMERC)/*.o
	rm -rf $(SRCMERC)/.*.cmd
