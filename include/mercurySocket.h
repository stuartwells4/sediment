/*
 * Mercury Socket Interface - Copied from klem driver by Stuart Wells
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

#ifndef MERC_SOCKET_INCLUDE
#define MERC_SOCKET_INCLUDE
void *mercNetConnectRaw(void *pData, char *pDevLabel);
void mercNetDisconnectRaw(void *pPtr);
unsigned int mercTransmitRaw(void *pPtr,
			     struct sk_buff *pSkb,
			     char *pHdr, unsigned int uHdrSize);

#endif
