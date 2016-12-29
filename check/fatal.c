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

#ifdef HAVE_CONFIG_H
#include <config.h>
#else
# error Use configure; make; make install
#endif

#include "check.h"

/* Esce con un msg di insulto */
void
fatal (char *s, ...)
{
  va_list va;

  if (!s)
    exit (1);

  va_start (va, s);
  vfprintf (stdout, s, va);
  fprintf (stdout, "\n");
  va_end (va);

  exit (1);
}

void
msg (char *s, ...)
{
  va_list va;

  va_start (va, s);
  fprintf (stdout, "* ");
  vfprintf (stdout, s, va);
  fprintf (stdout, "\n");
  va_end (va);
}

/* EOF */
