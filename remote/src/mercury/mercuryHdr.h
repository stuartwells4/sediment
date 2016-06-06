/*
 * Wireless Kernel Link Emulator
 *
 * Copyright (C) 2013 - 2016 Stuart Wells <swells@stuartwells.net>
 * All rights reserved.
 *
 * This portion of the interface is licensed under LGPL
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#ifndef MERC_PROTOCOL_HDR
#define MERC_PROTOCOL_HDR

#include <linux/if_ether.h>

#define MERC_INT_VERSION 1
#define MERC_STR_VERSION "1"
#define MERC_NAME "merc"
#define MERC_PROTOCOL 0xdeed

typedef struct MERC_RAW_HEADER_DEF {
  u8 pDstMac [ETH_ALEN];
  u8 pSrcMac [ETH_ALEN];
  u16 uProtocol;
  u32 uHeader;
  u32 uVersion;
} __attribute__((packed)) MERC_RAW_HEADER;
#endif
