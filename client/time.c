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

#include "client.h"

int
check_time (char *str)
{
  int i;
  int h = 0;
  int m = 0;
  int s = 0;
  int part = 0;

  for (i = 0; i < strlen (str); i++)
    {

      if (str[i] >= '0' && str[i] <= '9')
	part = part * 10 + str[i] - '0';

      else if (str[i] == 'h' || str[i] == 'H')
	{
	  h += part;
	  part = 0;
	}

      else if (str[i] == 'm' || str[i] == 'M')
	{
	  m += part;
	  part = 0;
	}

      else if (str[i] == 's' || str[i] == 'S')
	{
	  s += part;
	  part = 0;
	}
    }

  s += part;
  s += m * 60;
  s += h * 3600;

  return s;
}

/* EOF */
