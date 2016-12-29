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

/* Disactive all signals */
static void
signal_null (int signo)
{
  SOMA_LOG_ALL ("Signal %d blocked!", signo);
}

void
signal_off (void)
{
  int i;

  for (i = 1; i != NSIG; i++)
    if (signal (i, signal_null) == SIG_ERR)
      SOMA_NO_LOG_INFO ("Signal return error with %d!", i);

  if (signal (SIGHUP, log_restart) == SIG_ERR)
    SOMA_NO_LOG_INFO ("Signal return error with SIGHUP!");
}

void
signal_small_off (void)
{
  if (signal (SIGPIPE, signal_null) == SIG_ERR)
    SOMA_NO_LOG_INFO ("Signal return error with SIGPIPE!");

  if (signal (SIGHUP, log_restart) == SIG_ERR)
    SOMA_NO_LOG_INFO ("Signal return error with SIGHUP!");
}

/* Reactive the signals */
void
signal_on (void)
{
  int i;

  for (i = 1; i != NSIG; i++)
    signal (i, SIG_DFL);
}

void
signal_small_on (void)
{
  signal (SIGPIPE, SIG_DFL);
  signal (SIGHUP, SIG_DFL);
}

/* EOF */
