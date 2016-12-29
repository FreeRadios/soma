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

char *shell_history (void);

/* opening readline functions like auto completion and history */
void
shell (void)
{
  char *a, *c;
  char *h;

  fprintf (stdout, "somaclient %s - shell mode (readline %s)\n\n",
	   SOMA_VERSION, rl_library_version);

  rl_readline_name = "somaclient";
  rl_attempted_completion_function = soma_completion;

  using_history ();

  if ((h = shell_history ()))
    read_history (h);

  while (!set_quit)
    {
      a = readline ("[somaclient] ");

      if (!a)
	continue;

      c = trim (a);

      if (*c != 0)
	{
	  add_history (c);
	  execute (c);
	}

      free (a);
    }

  write_history (h);

  fprintf (stdout, "somaclient %s - %s - %s\n", SOMA_VERSION, SOMA_COPYRIGHT,
	   SOMA_AUTHOR_EMAIL);
}

char *
shell_history (void)
{
  static char *h = NULL;
  char *t;
  int i;

  if (!h)
    {
      if (!(t = getenv ("HOME")))
	return NULL;

      i = strlen (t) + 25;
      if (!(h = (char *) malloc (i * sizeof (char))))
	return NULL;

      snprintf (h, i, "%s/.somaclient_history", t);
    }

  return h;
}

/* EOF */
