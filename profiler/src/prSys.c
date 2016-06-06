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

#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/debugfs.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/stat.h>

#include "pr.h"
#include "prSys.h"
#include "prProbe.h"

static ssize_t priv_sys_stat_show(struct device *pDev,
				  struct device_attribute *pAttr,
				  char *pBuffer)
{
  PRData *pData = getData();
  ssize_t count = 0;

  PR_LOG("A Request to read %p %p %p\n",
	 pDev,
	 pAttr,
	 pBuffer);

  count = prProbeProcess(pData->pProfile, pBuffer);
  return count;
}
static DEVICE_ATTR(stats, S_IRUGO,
		   &priv_sys_stat_show, NULL);

int prSysInit(PRData *pData)
{
  int iErr;

  if (NULL != pData) {
    pData->pClass = NULL;
    pData->pDevice = NULL;
    pData->bStat = false;
    pData->pClass = class_create(THIS_MODULE, PR_CLASS_NAME);
    if ((IS_ERR)(pData->pClass)) {
      iErr = PTR_ERR(pData->pClass);
      pData->pClass = NULL;
      PR_LOG("Error Creating Class %d\n", iErr);
      goto sys_failed;
    }

    pData->pDevice = device_create(pData->pClass,
				   NULL,
				   0,
				   NULL,
				   PR_DEVICE_NAME);
    if (IS_ERR(pData->pDevice)) {
      iErr = PTR_ERR(pData->pDevice);
      pData->pDevice = NULL;
      PR_LOG("Error Creating Device %d\n", iErr);
      goto sys_failed;
    }

    iErr = device_create_file(pData->pDevice,
			      &dev_attr_stats);
    if (iErr) {
      PR_LOG("Error creating stat %d\n", iErr);
      goto sys_failed;
    }

    pData->bStat = true;
  }

  return 0;

 sys_failed:
  //  prSysDeinit(pData);
  return 1;
}

void prSysDeinit(PRData *pData)
{
  if (NULL != pData) {
    if (true == pData->bStat) {
      device_remove_file(pData->pDevice, &dev_attr_stats);
      pData->bStat = false;
    }

    if (NULL != pData->pDevice) {
      device_unregister(pData->pDevice);
      pData->pDevice = NULL;
    }

    if (NULL != pData->pClass) {
      class_destroy(pData->pClass);
      pData->pClass = NULL;
    }
  }
}
