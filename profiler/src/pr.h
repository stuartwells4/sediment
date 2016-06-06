/*
 *
 * Copyright (C) 2016 Stuart Wells <swells@stuartwells.net> All rights reserved.
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
#ifndef PR_DATA_INCLUDE
#define PR_DATA_INCLUDE

#include <linux/wait.h>

#define PR_STR_VERSION "1.0"
#define PR_CLASS_NAME "profiler"
#define PR_DEVICE_NAME "trace"

#define PR_LOG(fmt, args...) printk("profiler::%s-(%d) "fmt, __func__, __LINE__, args)
#define PR_MSG(fmt) printk("profiler::%s-(%d) "fmt, __func__, __LINE__)

typedef struct pr_data {
  struct class *pClass;
  struct device *pDevice;
  bool bStat;
  void *pProfile;
} PRData;

PRData *getData(void);
#endif
