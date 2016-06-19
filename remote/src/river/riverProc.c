/*
 * River Block Driver
 *
 * Copyright (C) 2013 - 2016 Stuart Wells <swells@stuartwells.net>
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
#include <linux/proc_fs.h>
#include <linux/version.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,11,0))
#include <linux/fs.h>
#include <linux/seq_file.h>
#endif

#include "riverData.h"

/* String information for starting/stopping the system. */
#define COMMAND_STR "command"
#define COMMAND_START_STR "start"
#define COMMAND_STOP_STR "stop"

/*
 * Interface to send information to the proc file system.
 */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,11,0))

static int privProcOutput(char *pKernBuf,
			  char **ppIgnore,
			  off_t iKernOffset,
			  int iKernLen,
			  int *pEOF,
			  void *pPtr)
{
  RIVERData *pData = (RIVERData *)pPtr;
  char *pOutput = pData->proc.pBuffer + iKernOffset;
  int iOutLen = pData->proc.iSize - iKernOffset;
  int rvalue = 0;
  int loop;
  int ufnum = 0;

  spin_lock(&pData->sLock);

  if (0 == iKernOffset) {

    sprintf(pOutput, "RIVER Proc Interface\n\n");
    pOutput += strlen(pOutput);

    sprintf(pOutput, "\n");
    pOutput += strlen(pOutput);

    pOutput = pData->proc.pBuffer;
    pData->proc.iSize = strlen(pOutput);
    iOutLen = pData->proc.iSize;
  }

  if (iOutLen < iKernLen) {
    memcpy(pKernBuf, pOutput, iOutLen);
    rvalue = iOutLen;
    *pEOF = 0;
  } else {
    memcpy(pKernBuf, pOutput, iKernLen);
    rvalue = iOutLen - iKernLen;
    *pEOF = 0;
  }

  spin_unlock(&pData->sLock);

  return rvalue;
}

#else

/* String information for starting/stopping the system. */
static int privProcOutputSeq(struct seq_file *pOutput, void *pBuffer)
{
  RIVERData *pData;


  if (NULL != pOutput) {
    pData = (RIVERData *)pOutput->private;

    if (NULL != pData) {
      seq_printf(pOutput, "RIVER Proc Interface\n\n");
      seq_printf(pOutput, "\n");
    }
  }

  return 0;
}
#endif

/*
 * Interface to recv information from the proc file system.
 * A way to use script files to configure experiments without
 * use netlink.
 *
 * This routine is not secure.  Remove if you care about security.
 */
static int privProcInput(struct file *pFile,
			 const char __user *pUserBuf,
			 unsigned long uiCount,
			 void *pMyData)
{
  RIVERData *pData = (RIVERData *)pMyData;
  const char *pCommand = NULL;
  unsigned int iCommandLen = 0;
  const char *pValue = NULL;
  unsigned int iValueLen = 0;
  bool bCommand = true;
  bool bParse = false;
  unsigned int loop = 0;

  spin_lock(&pData->sLock);

  /* Look for command = values, and act upon it. */
  loop = 0;
  while (loop < strlen(pUserBuf)) {
    switch(pUserBuf [loop])
      {
      case ' ':
      case '\r':
      case '\n':
        /* If we have a pvalue, we found command, value, now parse. */
        if ((NULL != pValue) && (NULL != pCommand)) {
          bParse = true;
          bCommand = true;
        }
        break;
      case '=':
        /* Possible value next */
        bCommand = false;
        break;
      default:
        /* did we find a command */
        if ((NULL == pCommand) && (true == bCommand)) {
          pCommand = &pUserBuf [loop];
        }

        /* Did we find a value */
        if ((NULL == pValue) && (false == bCommand)) {
          pValue = &pUserBuf [loop];
        }

        /* Determine the length of this string run. */
        if (true == bCommand) {
          iCommandLen += 1;
        } else {
          iValueLen += 1;
        }
        break;
      }

    /* Should parsing those command = values */
    if ((NULL != pCommand) && (NULL != pValue) && (true == bParse)) {
      /* Do we have a command */
      if (strncmp(pCommand, COMMAND_STR, iCommandLen) == 0) {
        if (strncmp(pValue, COMMAND_START_STR, iValueLen) == 0) {
          RIVER_MSG("Start command");
        } else if (strncmp(pValue, COMMAND_STOP_STR, iValueLen) == 0) {
          RIVER_MSG("Stop command");
        }
      }

      pCommand = NULL;
      pValue = NULL;
      iCommandLen = 0;
      iValueLen = 0;
      bCommand = false;
      bParse = false;
    }

    bParse = false;
    loop += 1;
  }

  spin_unlock(&pData->sLock);

  return uiCount;
}
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,11,0))

static ssize_t privProcInputSeq(struct file *pFile,
				const char __user *pBuffer,
				size_t iCount,
				loff_t *pPos)
{
  void *pData = NULL;
  size_t rvalue = 0;

  if (NULL != pFile) {
    if (NULL != pBuffer) {
      if (iCount > 0) {
	pData = riverGetData();
	rvalue = privProcInput(pFile, pBuffer, iCount, pData);
      }
    }
  }

  return rvalue;
}

#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,11,0))

static int privProcOpen(struct inode *pINode, struct file *pFile)
{
  return single_open(pFile, privProcOutputSeq, PDE_DATA(pINode));
}

static const struct file_operations priv_proc_fops = {
  .owner   = THIS_MODULE,
  .open    = privProcOpen,
  .read    = seq_read,
  .write   = privProcInputSeq,
  .llseek  = seq_lseek,
  .release = single_release,
};
#endif

/* Proc entry point */
void riverProcInit(void *pPtr)
{
  RIVERData *pData = (RIVERData *)pPtr;

  if (NULL != pData) {
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,11,0))
    pData->proc.pEntry = create_proc_read_entry(RIVER_NAME,
                                                0644,
                                                NULL,
                                                privProcOutput,
                                                pPtr);

    if (NULL == pData->proc.pEntry) {
      RIVER_LOG("Failed to create proc read entry %s\n", RIVER_NAME);
    } else {
      pData->proc.pEntry->write_proc = privProcInput;
    }
#else
    pData->proc.pEntry = proc_create_data(RIVER_NAME,
					  0644,
					  NULL,
					  &priv_proc_fops,
					  pPtr);

#endif
  }
}


void riverProcDeinit(void *pPtr)
{
  RIVERData *pData = (RIVERData *)pPtr;;

  if (NULL != pData) {
    if (NULL != pData->proc.pEntry) {
      /* Don't remove it, if we didn't have it. */
      remove_proc_entry(RIVER_NAME, NULL);
      pData->proc.pEntry = NULL;
    }
  }
}
