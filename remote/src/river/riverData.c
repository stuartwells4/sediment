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
#include "riverData.h"

#include <linux/slab.h>
#include <linux/version.h>

static RIVERData *pLocalData = NULL;

void *riverDataInit(void)
{
  RIVERData *pData = NULL;

  pLocalData = kmalloc(sizeof(RIVERData), GFP_KERNEL);
  pData = pLocalData;
  if (NULL != pData) {
    /* Make sure we set everything to zero */
    pData->proc.pEntry = NULL;

    /* We need a spin lock for calls from interrupts to lock data structure */
    spin_lock_init(&pData->sLock);
    pData->bActive = false;

    RIVER_LOG("malloc internal data at %p\n", pData);
  } else {
    RIVER_MSG("failed to allocate memory\n");
  }
  return (void *)pData;
}

void riverDataDeinit(void)
{
  RIVER_LOG("free memory at %p\n", pLocalData);
  if (NULL != pLocalData) {
    pLocalData->bActive = false;
    kfree(pLocalData);
    pLocalData = NULL;
  }
}

void *riverGetData(void)
{
  return (void *)pLocalData;
}
