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
#ifndef RIVER_LIST_INCLUDE
#define RIVER_LIST_INCLUDE

#include "riverData.h"

typedef struct RiverItemInfo_Def {
  /* Lists information */
  struct list_head list;
} RiverItem;

void *riverListCreate(void);
void riverListStop(void *pListParam);
void riverListDestroy(void *pListParam);
int riverListItemPoll(void *pListParam);
void riverListItemPut(void *pListParam, RiverItem *pItemInfo);
void riverListItemWait(void *pListParam);
RiverItem *riverListItemGet(RIVERData *pInfo, void *pListParam);

#endif
