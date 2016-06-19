/*
 * River Block Driver
 *
 * Copyright (C) 2016 Stuart Wells <swells@stuartwells.net>
 * All rights reserved.
 *
 * Licensed under the GNU General Public License, version 2 (GPLv2)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#ifndef RIVER_DATA_INCLUDE
#define RIVER_DATA_INCLUDE

#include <linux/wait.h>

#define RIVER_LOG(fmt, args...) printk("river::%s "fmt, __func__, args)
#define RIVER_MSG(fmt) printk("river::%s "fmt, __func__)

#define RIVER_STR_VERSION "1.0"
#define RIVER_NAME "river"

typedef struct {
  /* Information for the proc interface */
  struct {
    char pBuffer [4096];
    int iSize;
    struct proc_dir_entry *pEntry;
  } proc;

  /* Need a lock for our structure data */
  spinlock_t sLock;

  bool bActive;
} RIVERData;

void *riverDataInit(void);
void riverDataDeinit(void);
void *riverGetData(void);
#endif
