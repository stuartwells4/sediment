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
#include <linux/kallsyms.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/kprobes.h>
#include <linux/jiffies.h>
#include <linux/spinlock.h>
#include <linux/time.h>

#include "pr.h"
#include "prProbe.h"

typedef struct KRPData_def {
  struct kprobe kp;
  struct list_head list;
  char symbolName [64];
  kprobe_opcode_t *pAddr;
  unsigned int count;
  s64 pretime;
  s64 ntime;
  spinlock_t sLock;
} KRPData;

typedef struct KRPList_def {
  struct list_head list;
} KRPList;

static int priv_prehandler(struct kprobe *kp, struct pt_regs *pRegs)
{
  KRPData *pKRP = (KRPData *)kp;
  struct timespec newtime;
  unsigned long ulFlags;

  spin_lock_irqsave(&pKRP->sLock, ulFlags);
  getnstimeofday(&newtime);
  pKRP->pretime = timespec_to_ns(&newtime);
  spin_unlock_irqrestore(&pKRP->sLock, ulFlags);

  return 0;
}

static void priv_posthandler(struct kprobe *kp,
			     struct pt_regs *pRegs,
			     unsigned long flags)
{
  KRPData *pKRP = (KRPData *)kp;
  unsigned long ulFlags;
  struct timespec newtime;

  spin_lock_irqsave(&pKRP->sLock, ulFlags);
  getnstimeofday(&newtime);
  pKRP->ntime += (timespec_to_ns(&newtime) - pKRP->pretime);
  pKRP->count += 1;
  spin_unlock_irqrestore(&pKRP->sLock, flags);
}

/*
   Don't need it, but just in case.
 */
static int priv_faulthandler(struct kprobe *kp,
			     struct pt_regs *pRegs, int iTrap)
{
  return 0;
}

void *prProbeInit(void)
{
  KRPList *pList = NULL;

 pList = kmalloc(sizeof(KRPList), GFP_KERNEL);
  if (NULL != pList) {
    INIT_LIST_HEAD(&pList->list);
  }

  return (void *)pList;
}

bool prProbeAdd(void *pInfo, char *pName)
{
  bool rvalue = false;
  KRPData *pKRP = NULL;
  KRPList *pList = (KRPList *)pInfo;

  if ((NULL != pList) && (NULL != pName)) {
    pKRP = kmalloc(sizeof(KRPData), GFP_KERNEL);
    if (NULL != pKRP) {
      memset(pKRP->symbolName, 0, sizeof(pKRP->symbolName));
      pKRP->count = 0;
      pKRP->ntime = 0;
      pKRP->pAddr = NULL;
      spin_lock_init(&pKRP->sLock);

      PR_LOG("Adding symbol %s\n", pName);
      strncpy(pKRP->symbolName,  pName, sizeof(pKRP->symbolName));
      pKRP->pAddr = (kprobe_opcode_t *)kallsyms_lookup_name(pName);
      list_add(&pKRP->list, &pList->list);
    }
  }

  return rvalue;
}

void prProbeStart(void *pInfo)
{
  KRPData *pKRP = NULL;
  KRPList *pList = (KRPList *)pInfo;
  int rvalue = 0;

  if (NULL != pInfo) {
    list_for_each_entry(pKRP, &pList->list, list) {
      if (NULL != pKRP) {
	if (NULL != pKRP->pAddr) {
	  pKRP->kp.symbol_name = pKRP->symbolName;
	  pKRP->kp.pre_handler = priv_prehandler;
	  pKRP->kp.post_handler = priv_posthandler;
	  pKRP->kp.fault_handler = priv_faulthandler;
	  rvalue = register_kprobe(&pKRP->kp);
	  PR_LOG("Registered %s %p %d\n", pKRP->symbolName, pKRP->kp.addr, rvalue);
	}
      }
    }
  }
}

void prProbeStop(void *pInfo)
{
  KRPData *pKRP;
  KRPList *pList = (KRPList *)pInfo;

  if (NULL != pInfo) {
    list_for_each_entry(pKRP, &pList->list, list) {
      if (NULL != pKRP->pAddr) {
	unregister_kprobe(&pKRP->kp);
	PR_LOG("Removed probes %p\n", pKRP->symbolName );
      }
    }
  }
}

int prProbeProcess(void *pInfo, char *pBuffer)
{
  KRPData *pKRP;
  KRPList *pList = (KRPList *)pInfo;
  char *pCurrent = pBuffer;
  int count = 0;
  int length = 0;

  if (NULL != pInfo) {
    list_for_each_entry(pKRP, &pList->list, list) {
      count = 0;
      if (NULL != pKRP) {
	if (NULL != pKRP->kp.addr) {
	  spin_lock(&pKRP->sLock);

	  if (pKRP->count > 0) {
	    count = sprintf(pCurrent, "%s:%lu:%u\n",
			    pKRP->symbolName,
			    (unsigned long)pKRP->ntime / pKRP->count,
			    pKRP->count);
	  } else {
	    count = sprintf(pCurrent, "%s:0:0\n",
			    pKRP->symbolName);
	  }

	  spin_unlock(&pKRP->sLock);
	} else {
	  count = sprintf(pCurrent, "%s:no symbol found\n",
			  pKRP->symbolName);
	}
      }
      pCurrent += count;
      length += count;
    }
  }

  return length;
}

void prProbeDeinit(void *pInfo)
{
  KRPList *pList = (KRPList *)pInfo;
  KRPData *pKRP, *pTmp;

  if (NULL != pList) {
    list_for_each_entry_safe(pKRP, pTmp, &pList->list, list) {
	list_del(&pKRP->list);
	memset(pKRP->symbolName, 0, sizeof(pKRP->symbolName));
	kfree(pKRP);
	pKRP = NULL;
    }
  }

  PR_MSG("done\n");
}
