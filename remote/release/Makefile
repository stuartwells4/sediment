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
SRCRIVER := ../src/river/

# Specify adition include paths
EXTRA_CFLAGS=-I$(PWD)/$(SRC) -I$(PWD)
#NOSTDINC_FLAGS := -I$(PWD)

obj-m := merc.o river.o
merc-objs := $(SRCMERC)/mercuryModule.o $(SRCMERC)/socketNet.o
river-objs := $(SRCRIVER)/riverModule.o $(SRCRIVER)/riverProc.o $(SRCRIVER)/riverData.o $(SRCRIVER)/riverList.o
KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default: merc river

merc:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

river:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	rm -rf *.o .*.cmd *.ko Module.* *.mod.c .tmp_versions modules.order
	rm -rf $(SRCMERC)/*.o
	rm -rf $(SRCMERC)/.*.cmd
	rm -rf $(SRCRIVER)/*.o
	rm -rf $(SRCRIVER)/.*.cmd
