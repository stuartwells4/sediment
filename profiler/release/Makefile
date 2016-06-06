#
# Module: profiler.ko
#
#
# Author: Stuart Wells
#
# (C) Copyright 2016 Stuart Wells
#
#

SRC := ../src/

# Specify adition include paths
EXTRA_CFLAGS=-I$(PWD)/$(SRC) -I$(PWD)
#NOSTDINC_FLAGS := -I$(PWD)

obj-m := profiler.o
profiler-objs := $(SRC)/prModule.o $(SRC)/prSys.o $(SRC)/prProbe.o
KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default: profiler

profiler:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

ptest:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	rm -rf *.o .*.cmd *.ko Module.* *.mod.c .tmp_versions modules.order
	rm -rf $(SRC)/*.o
	rm -rf $(SRC)/.*.cmd
