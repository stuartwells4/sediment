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
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/delay.h>

#include "pr.h"
#include "prSys.h"
#include "prProbe.h"

static PRData *pGlobalData = NULL;
PRData *getData(void)
{
  return pGlobalData;
}

/* Silly parameter from insmod */
static char *funcs;

module_param(funcs, charp, 0);

/*
  The linux kernel module insmod entry point.
*/
static int __init privPRInit(void)
{
  int iErr = 2;

  char *pToken = NULL;

  pGlobalData = kmalloc(sizeof(PRData), GFP_KERNEL);
  if (NULL != pGlobalData) {
    pGlobalData->pProfile = prProbeInit();
    if (NULL != funcs) {
      pToken = strsep(&funcs, ",");
      while(NULL != pToken) {
	prProbeAdd(pGlobalData->pProfile, pToken);
	pToken = strsep(&funcs, ",");
      }
    }

    prProbeStart(pGlobalData->pProfile);
    PR_MSG("init Sys\n");
    iErr = prSysInit(pGlobalData);
    PR_MSG("done init Sys\n");

    if (0 == iErr) {
      PR_MSG("Module has been Added\n");
    } else {
      PR_MSG("Modile failed to install\n");
    }
  }

  return iErr;
}

/*
   Inform the linux kernel of the entry point.
*/
module_init(privPRInit);

/*
  The linux kernel module insmod exit point.
*/
static void __exit privPRExit(void)
{
  if (NULL != pGlobalData) {
    prSysDeinit(pGlobalData);
    prProbeStop(pGlobalData->pProfile);
    prProbeDeinit(pGlobalData->pProfile);
    pGlobalData->pProfile = NULL;
    kfree(pGlobalData);
    pGlobalData = NULL;
  }
  PR_MSG("Module has been Removed\n");
}

/*
   Inform the linux kernel of the exit point.
*/
module_exit(privPRExit);

/* Module Information for modinfo */
MODULE_VERSION(PR_STR_VERSION);

/*
 * This driver uses GPL only facilities of the linux kernel, so
 * it may not load if you try and change the license.
 */
MODULE_LICENSE("GPL");

MODULE_AUTHOR("Stuart Wells swells@stuartwells.net");
MODULE_DESCRIPTION("Profile Sys Test");
