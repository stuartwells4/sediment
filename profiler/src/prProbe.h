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
#ifndef PR_PROBE_INCLUDE
#define PR_PROBE_INCLUDE
#include "pr.h"

void *prProbeInit(void);
bool prProbeAdd(void *pInfo, char *pName);
void prProbeStart(void *pInfo);
void prProbeStop(void *pInfo);
int prProbeProcess(void *pInfo, char *pBuffer);
void prProbeDeinit(void *pInfo);

#endif
