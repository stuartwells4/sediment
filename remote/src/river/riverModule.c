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
#include <linux/version.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/delay.h>

#include "riverData.h"
#include "riverProc.h"

/*
  The linux kernel module insmod entry point.
*/
static int __init privRiverInit(void)
{
  int rvalue = 1;
  RIVERData *pData = NULL;

  pData = riverDataInit();
  if (NULL != pData) {
    riverProcInit(pData);

    RIVER_MSG("Module sucessfully installed\n");
    rvalue = 0;
  }

  return rvalue;
}

/*
   Inform the linux kernel of the entry point.
*/
module_init(privRiverInit);

/*
  The linux kernel module insmod exit point.
*/
static void __exit privRiverExit(void)
{
  RIVERData *pData  = riverGetData();

  if (NULL != pData) {
    riverProcDeinit(pData);
    riverDataDeinit();
    pData = NULL;
  }

  RIVER_MSG("Module has been removed\n");
}

/*
   Inform the linux kernel of the exit point.
*/
module_exit(privRiverExit);

/* Module Information for modinfo */
MODULE_VERSION(RIVER_STR_VERSION);

/*
 * This driver uses GPL only facilities of the linux kernel, so
 * it may not load if you try and change the license.
 */
MODULE_LICENSE("GPL");

MODULE_AUTHOR("Stuart Wells swells@stuartwells.net");
MODULE_DESCRIPTION("River Blok driver");
