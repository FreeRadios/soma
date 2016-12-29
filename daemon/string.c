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

string_t *
string_new (void)
{
  string_t *st;

  if (!(st = (string_t *) malloc (sizeof (string_t))))
    SOMA_FATAL ("No memory!");

  memset (st, 0, sizeof (string_t));
  return st;
}

void
string_destroy (string_t * st)
{
  if (!st)
    return;

  if (st->string)
    free (st->string);

  free (st);
}

int
string_add (string_t * st, char *what, size_t size)
{
  if (!st || !*what)
    return 1;

  if (size <= 0)
    size = strlen (what);

  if (!st->size)
    {
      if (!(st->string = (char *) malloc (sizeof (char) * (size + 1))))
	SOMA_FATAL ("No memory!");
    }
  else
    {
      if (!
	  (st->string =
	   (char *) realloc (st->string,
			     sizeof (char) * (size + st->size + 1))))
	SOMA_FATAL ("No memory!");
    }

  memcpy (st->string + st->size, what, size);
  st->size += size;
  st->string[st->size] = 0;

  return 0;
}

int
string_printf (string_t * st, char *str, ...)
{
  va_list va;
  char s[1024];

  if (!st)
    return 1;

  va_start (va, str);
  vsnprintf (s, sizeof (s), str, va);
  va_end (va);

  return string_add (st, s, 0);
}

/* EOF */
