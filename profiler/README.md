Profiler
====

Simple driver interface to ktrace, so one can determine how many times a function listed in the symbol table has been called.

Usage
-----

The interface to specify the symbol is passed using the funcs parameter when installing the device driver:

     #insmod profiler.ko funcs=kalloc,(next symbol)

A /sys file is created, allowing one to see the symbols found, and determine how often they have been called.

     #cat /sys/class/profiler/trace/stats
     kalloc:0:0


The output should be symbol:time:count:

     symbol  The symbol passed through func

     time    The average time called in ns.

     count   The number of times it has been called.

Build
-----

To build the profiler driver, type make from the release directory:

    #cd profiler/release
    #make

If your build kernel Module.symvers is not located in /lib/moduiles/`uname -r`/build then you must specify the built kernel location:

    #make KERNELDIR=/usr/foo/kernel
