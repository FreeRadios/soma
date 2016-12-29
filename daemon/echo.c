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

#include "somad.h"

int fd_null, fd_1, fd_2;

/* Active or not the file descriptor */
void
echo_off (void)
{
  SOMA_LOG_ALL ("Echo off");

  if (!conf->Background)
    return;

  if ((fd_null = open (SOMA_DEV_NULL, O_WRONLY)) < 0)
    SOMA_FATAL ("Open /dev/null error.");

  fd_1 = dup (1);
  fd_2 = dup (2);

  close (1);
  close (2);

  dup2 (1, fd_null);
  dup2 (2, fd_null);
}

void
echo_on (void)
{
  SOMA_LOG_ALL ("Echo on");

  if (!conf->Background)
    return;

  close (fd_null);

  dup2 (fd_1, 1);
  dup2 (fd_2, 2);

  close (fd_1);
  close (fd_2);
}

/* EOF */
