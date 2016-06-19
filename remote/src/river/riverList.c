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
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/err.h>
#include <linux/kthread.h>
#include <linux/list.h>
#include <linux/wait.h>

#include <linux/slab.h>

#include "riverList.h"
#include "riverData.h"

typedef struct {
  spinlock_t sSpinLock;
  wait_queue_head_t sWaitQueue;
  struct list_head pListHead;
  bool bActive;
} RiverListInfo;

static RiverItem *privateItemGet(void *pListParam);

void *riverListCreate(void)
{
  RiverListInfo *pList = NULL;

  pList = kmalloc(sizeof(RiverListInfo), GFP_KERNEL);
  if (NULL != pList) {
    INIT_LIST_HEAD(&pList->pListHead);
    spin_lock_init(&pList->sSpinLock);
    init_waitqueue_head(&pList->sWaitQueue);
    pList->bActive = true;
  }

  return ((void *)pList);
}

void riverListStop(void *pListParam)
{
  RiverListInfo *pList = (RiverListInfo *)pListParam;
  
  if (NULL != pList) {
    pList->bActive = false;
    wake_up_all(&pList->sWaitQueue);
  }
}

void riverListDestroy(void *pListParam)
{
  RiverListInfo *pList = (RiverListInfo *)pListParam;
  void *pItem = NULL;

  if (NULL != pList) {
    pList->bActive = false;
    /* Get all items on the list, toss them out. */
    do {
      pItem = privateItemGet(pList);
    } while (NULL != pItem);
  }

  wake_up_all(&pList->sWaitQueue);
  
  kfree(pListParam);
}


int riverListItemPoll(void *pListParam)
{
  RiverListInfo *pList = (RiverListInfo *)pListParam;
  int rvalue = 0;
  unsigned long ulSigFlags;

  if (NULL != pList) {
    if (true == pList->bActive) {
      spin_lock_irqsave(&pList->sSpinLock, ulSigFlags);
      rvalue = !list_empty(&pList->pListHead);
      spin_unlock_irqrestore(&pList->sSpinLock, ulSigFlags);
    } else {
      rvalue = 1;
    }
  }

  return rvalue;
}

void riverListItemPut(void *pListParam, RiverItem *pItemInfo)
{
  RiverListInfo *pList = (RiverListInfo *)pListParam;
  unsigned long ulSigFlags;

  if (NULL != pList) {
    if (NULL != pItemInfo) {
      spin_lock_irqsave(&pList->sSpinLock, ulSigFlags);
      list_add_tail(&pItemInfo->list, &pList->pListHead);
      spin_unlock_irqrestore(&pList->sSpinLock, ulSigFlags);
    }

    /* Once we add, wake up the sleeper. */
    wake_up_all(&pList->sWaitQueue);    
  }
}


void riverListItemWait(void *pListParam)
{
  RiverListInfo *pList = (RiverListInfo *)pListParam;
  int rvalue;

  if (NULL != pList) {
    wait_event_interruptible_timeout(pList->sWaitQueue, 
				     ((rvalue = riverListItemPoll(pListParam)) > 0), 1 * HZ);
  }
}

static RiverItem *privateItemGet(void *pListParam)
{
  RiverItem *pItemInfo = NULL;
  RiverListInfo *pList = (RiverListInfo *)pListParam;
  unsigned long ulSigFlags;

  if (NULL != pListParam) {
    spin_lock_irqsave(&pList->sSpinLock, ulSigFlags);
    if (!list_empty(&pList->pListHead)) {

      pItemInfo = list_entry(pList->pListHead.next,
			     RiverItem,
			     list);
      list_del(pList->pListHead.next);
    }
    spin_unlock_irqrestore(&pList->sSpinLock, ulSigFlags);
  }

  return pItemInfo;
}

RiverItem *riverListItemGet(RIVERData *pInfo, void *pListParam)
{ 
  RiverItem *pValue;

  do {
    riverListItemWait(pListParam);
    
    pValue = privateItemGet(pListParam);
  } while ((NULL == pValue) && (true == pInfo->bActive));

  return pValue;
}
