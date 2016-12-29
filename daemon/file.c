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

char *
soma_mktemp (void)
{
  char *r;
  struct stat st;
  FILE *fl;
  int d = 0;

  SOMA_LOG_ALL ("Soma - mktemp...");

  if (lstat (SOMA_TMP_DIR, &st))
    SOMA_FATAL ("Error directory %s.", SOMA_TMP_DIR);

  if (!S_ISDIR (st.st_mode))
    SOMA_FATAL ("%s is not a directory.", SOMA_TMP_DIR);

  if (!(r = (char *) malloc ((strlen (SOMA_TMP_DIR) + 20) * sizeof (char))))
    SOMA_FATAL ("Errore memory.");

  while (d < 1000)
    {
      sprintf (r, "%s/somad%.3d.pl", SOMA_TMP_DIR, d);

      if (lstat (r, &st))
	{
	  if ((fl = fopen (r, "w")))
	    fclose (fl);
	  return r;
	}

      d++;
    }

  return NULL;
}

/* Remote ? */
int
remote_file (char *f)
{
  if (!strncmp (f, "http://", 7) || !strncmp (f, "https://", 8)
      || !strncmp (f, "ftp://", 6) || !strncmp (f, "ftps://", 7)
      || !strncmp (f, "mms://", 6))
    return 1;

  return 0;
}

/* Distribuited ? */
int
distribuited_file (char *f)
{
  if (f && *f == DISTRIBUITED_CHAR)
    return 1;

  return 0;
}

/* EOF */
