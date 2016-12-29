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

static int host_parse_single (char *ip);
static int host_is_numb (char *str);

int
host_check (char *ip_str)
{
  char a[100];
  int i, k, j;

  sprintf (a, "%s.", ip_str);

  k = i = j = 0;
  while (*(a + i))
    {
      if (*(a + i) == '.')
	{
	  *(a + i) = 0;

	  if (host_parse_single (a + k))
	    return 1;

	  j++;
	  k = i + 1;
	}
      i++;
    }

  return 0;
}

static int
host_parse_single (char *str)
{
  /* is empty ? */
  if (!strncasecmp (str, "x", 1))
    return 0;

  if (host_is_numb (str))
    return 0;

  /* is a condition ? */
  if (*str == '>' || *str == '<')
    {
      if (*(str + 1) == '=' && host_is_numb (str + 2))
	return 0;
      else if (host_is_numb (str + 1))
	return 0;
      else
	return 1;
    }

  if (*str == '!' && host_is_numb (str + 1))
    return 0;

  return 1;
}

static int
host_is_numb (char *str)
{
  int i = 0;

  while (*(str + i))
    {

      if (*(str + i) < '0' || *(str + i) > '9')
	return 0;
      i++;
    }

  return 1;
}

/* EOF */
