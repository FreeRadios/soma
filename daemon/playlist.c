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

int
playlist_is (char *f)
{
  SOMA_LOG_ALL ("Playlist is %s?", f);

  if (!strcasecmp (".pls", f + strlen (f) - 4))
    return 1;

  if (!strcasecmp (".m3u", f + strlen (f) - 4))
    return 1;

  return 0;
}

/* When i read a playlist file, i read single lines: */
static int
playlist_get_line (FILE * fl, char *s)
{
  char ch;
  int k = 0;

  while ((ch = getc (fl)) != EOF)
    {
      if (ch == '\n')
	{
	  if (k)
	    {
	      s[k] = 0;
	      if (s[0] != '#')
		return k;

	      k = 0;

	    }
	}
      else if (k < SOMA_MAX_BUFF)
	{
	  s[k] = ch;
	  k++;
	}
    }

  return 0;
}

/* Analize the playlist type */
int
add_playlist (int list, char *str)
{

  FILE *fl;
  char s[SOMA_MAX_BUFF];
  int ok, k;

  SOMA_LOG_ALL ("Add Playlist %s", str);

  if (!(fl = fopen (str, "r")))
    return 1;

  if (playlist_get_line (fl, s))
    {
      if (!strncmp ("[playlist]", s, 10))
	ok = 1;
      else
	ok = 2;
    }
  else
    ok = 0;


  switch (ok)
    {
    case 1:			/* playlist type */
      ok = 0;
      if (playlist_get_line (fl, s) && !strncmp (s, "NumberOfEntries=", 16))
	{
	  SOMA_LOG_WARN ("Playlist file: syntax error.");
	  fclose (fl);
	  return 1;
	}

      ok = atoi (s + 16);
      SOMA_LOG_ALL ("Playlist element: %d", ok);
      while (playlist_get_line (fl, s))
	{
	  if (!strncmp (s, "File", 4))
	    {
	      char *p = s;

	      k = atoi (p + 4);
	      if (!k)
		{
		  SOMA_LOG_WARN ("Playlist file: syntax error.");
		  break;
		}

	      SOMA_LOG_ALL ("Insert playlist element: %d", k);

	      while (p && *p != '=')
		p++;

	      p++;

	      if (!p || add_item (list, s))
		continue;

	      ok--;
	    }
	}

      break;

    case 2:			/* list */
      if (add_item (list, s))
	{
	  fclose (fl);
	  return 1;
	}

      while (playlist_get_line (fl, s))
	if (add_item (list, s))
	  {
	    fclose (fl);
	    return 1;
	  }

      break;
    }

  fclose (fl);

  return 0;
}

/* EOF */
