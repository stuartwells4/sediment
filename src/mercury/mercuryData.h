/*
 * Mercury communication driver
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
#ifndef MERC_DATA_INCLUDE
#define MERC_DATA_INCLUDE

#define MERC_LOG(fmt, args...) printk("merc::%s "fmt, __func__, args)
#define MERC_MSG(fmt) printk("merc::%s "fmt, __func__)

#endif
