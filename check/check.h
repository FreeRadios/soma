/* Soma - Copyright (C) 2003-7 bakunin - Andrea Marchesini 
 *                                       <bakunin@autistici.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
 * 02110-1301, USA.
 *
 * This program is released under the GPL with the additional exemption that
 * compiling, linking, and/or using OpenSSL is allowed.
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <libxml/parser.h>
#include <fcntl.h>
#include <pwd.h>

#include "../include/code.h"
#include "../include/commons.h"
#include "../include/soma.h"

#ifdef ENABLE_CONFUSE
#include "../confuse/confuse.h"
#else
# include <confuse.h>
#endif

void fatal (char *, ...);
void msg (char *, ...);

void check_palinsesto (char *fl);

void check_spot (char *fl);

int check_time(char *start, char *stop);

int host_check(char *ip);

/* EOF */
