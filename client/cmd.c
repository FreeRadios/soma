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

/* comando exit, uscita dal client. Abilitare set_quit interrompe tutti i
 * cicli while e quindi porta al termine il sw. */
int
cmd_exit (char *arg)
{
  set_quit = 1;

  return 0;
}

int
cmd_shutdown (char *arg)
{
  soma_quit (controller);
  set_quit = 1;

  return 0;
}

/* comando cd. Cambia directory. Se ci sono argomenti, prova a cambiare
 * directory, viceversa, entraa dentro alla HOME. */
int
cmd_cd (char *arg)
{
  char *home;

  if (arg)
    {
      if (chdir (arg) == -1)
	{
	  perror (arg);
	  return -1;
	}
      else
	return 0;

    }

  home = getenv ("HOME");
  if (home)
    {
      if (!chdir (arg))
	{
	  perror (arg);
	  return -1;
	}
      else
	return 0;
    }

  fprintf (stderr, "No home variable.\n");
  return -1;
}

/* pwd: current work directory. */
int
cmd_pwd (char *arg)
{
  char a[1024];

  getcwd (a, 1024);

  if (a)
    {
      fprintf (stdout, "Cwd: %s\n", a);
      return 0;
    }

  fprintf (stderr, "Error getcwd.\n");
  return -1;
}

/* Esegue il comando ls. */
int
cmd_ls (char *arg)
{

  char a[1024];

  snprintf (a, 1024, "ls %s", arg);
  return system (a);
}

int
cmd_nextitem (char *arg)
{
  char *tmp;

  tmp = arg;

  while (*tmp != 0 && *tmp != ' ')
    tmp++;

  *tmp = 0;

  tmp++;

  if (!strcasecmp (arg, "set"))
    {
      char s[1024];

      if (*tmp != '/')
	{
	  getcwd (s, sizeof (s));
	  strncat (s, "/", sizeof (s));
	  strncat (s, tmp, sizeof (s));
	  CHECK_INT (soma_nextitem_set (controller, s));
	}
      else
	{
	  CHECK_INT (soma_nextitem_set (controller, tmp));
	}
    }

  else if (!strcasecmp (arg, "set_spot"))
    {
      if (!strcasecmp (tmp, "yes") || !strcasecmp (tmp, "1"))
	soma_nextitem_set_spot (controller, 1);
      else
	soma_nextitem_set_spot (controller, 0);
    }

  else if (!strcasecmp (arg, "get_spot"))
    {
      if (soma_nextitem_get_spot (controller))
	message (YES_NEXTITEM_SPOT);
      else
	message (NO_NEXTITEM_SPOT);
    }

  else if (!strcasecmp (arg, "get_path"))
    {
      char **list, **old;
      list = old = soma_get_path (controller, tmp);

      if (list)
	{
	  while (*list)
	    {
	      puts (*list);
	      list++;
	    }
	}

      soma_get_path_free (old);
    }

  else if (!strcasecmp (arg, "list"))
    {
      char **list, **old;
      list = old = soma_nextitem_list (controller);

      if (list)
	{
	  while (*list)
	    {
	      puts (*list);
	      list++;
	    }
	}

      soma_nextitem_list_free (old);
    }

  else if (!strcasecmp (arg, "remove"))
    {
      if (!strcmp (tmp, "first"))
	{
	  CHECK_INT (soma_nextitem_remove (controller, 0));
	}

      else if (!strcmp (tmp, "last"))
	{
	  CHECK_INT (soma_nextitem_remove (controller, -1));
	}

      else
	{
	  CHECK_INT (soma_nextitem_remove (controller, atoi (tmp)));
	}
    }

  else
    {
      fprintf (stdout,
	       "Use set, set_spot, get_spot, get_path, list or remove item!\n\n");
      return 0;
    }

  return 0;
}

/* file palinsesto */
int
cmd_filepalinsesto (char *arg)
{
  char *tmp;

  tmp = arg;

  while (*tmp != 0 && *tmp != ' ')
    tmp++;

  *tmp = 0;

  puts (arg);
  CHECK_INT (soma_new_palinsesto_file (controller, arg));

  return 0;
}

/* file spot */
int
cmd_filespot (char *arg)
{
  char *tmp;

  tmp = arg;

  while (*tmp != 0 && *tmp != ' ')
    tmp++;

  *tmp = 0;

  puts (arg);
  CHECK_INT (soma_new_spot_file (controller, arg));

  return 0;
}

int
cmd_readdirectory (char *arg)
{
  CHECK_INT (soma_read_directory (controller));

  return 0;
}

int
cmd_readpalinsesto (char *arg)
{
  CHECK_INT (soma_read_palinsesto (controller));

  return 0;
}

int
cmd_readspot (char *arg)
{
  CHECK_INT (soma_read_spot (controller));

  return 0;
}

int
cmd_oldpalinsesto (char *arg)
{
  CHECK_INT (soma_old_palinsesto (controller));

  return 0;
}

int
cmd_oldspot (char *arg)
{
  CHECK_INT (soma_old_spot (controller));

  return 0;
}

int
cmd_defaultpalinsesto (char *arg)
{
  CHECK_INT (soma_set_default_palinsesto (controller));

  return 0;
}

int
cmd_defaultspot (char *arg)
{
  CHECK_INT (soma_set_default_spot (controller));

  return 0;
}

int
cmd_getpalinsesto (char *arg)
{
  char *buf = soma_get_palinsesto (controller);

  if (buf)
    {
      puts (buf);
      free (buf);
    }
  else
    message (PROTOCOL_ERROR);

  return 0;
}

int
cmd_getspot (char *arg)
{
  char *buf = soma_get_spot (controller);

  if (buf)
    {
      puts (buf);
      free (buf);
    }
  else
    message (PROTOCOL_ERROR);

  return 0;
}

int
cmd_getoldpalinsesto (char *arg)
{
  char *buf = soma_get_old_palinsesto (controller);

  if (buf)
    {
      puts (buf);
      free (buf);
    }
  else
    message (PROTOCOL_ERROR);

  return 0;
}

int
cmd_getoldspot (char *arg)
{
  char *buf = soma_get_old_spot (controller);

  if (buf)
    {
      puts (buf);
      free (buf);
    }
  else
    message (PROTOCOL_ERROR);

  return 0;
}

int
cmd_password (char *arg)
{
  CHECK_INT (soma_check_password (controller));

  return 0;
}

int
cmd_status (char *arg)
{
  char *buf;

  buf = soma_status (controller);

  if (!buf)
    message (PROTOCOL_ERROR);
  else
    {
      puts (buf);
      free (buf);
    }

  return 0;
}

int
cmd_pl_name (char *arg)
{
  char *buf;

  buf = soma_palinsesto_name (controller);

  if (!buf)
    {
      puts (buf);
      free (buf);
    }

  return 0;
}

int
cmd_get_item (char *arg)
{
  char *buf;

  buf = soma_get_item (controller);

  if (!buf)
    message (PROTOCOL_ERROR);
  else
    {
      puts (buf);
      free (buf);
    }

  return 0;
}

int
cmd_get_item_next (char *arg)
{
  char *buf;

  buf = soma_get_item_next (controller);

  if (!buf)
    message (PROTOCOL_ERROR);
  else
    {
      puts (buf);
      free (buf);
    }

  return 0;
}

int
cmd_time (char *arg)
{
  time_t t;

  t = soma_time (controller);

  if (!t)
    message (PROTOCOL_ERROR);
  else
    printf ("%ld\n", t);

  return 0;
}

int
cmd_running (char *arg)
{
  if (soma_running (controller))
    message (YES_RUNNING);
  else
    message (NO_STOPPED);

  return 0;
}

int
cmd_quit (char *arg)
{
  CHECK_INT (soma_quit (controller));

  set_quit = 1;

  return 0;
}

int
cmd_skip (char *arg)
{
  CHECK_INT (soma_skip (controller));

  return 0;
}

int
cmd_skip_next (char *arg)
{
  CHECK_INT (soma_skip_next (controller));

  return 0;
}

int
cmd_start (char *arg)
{
  CHECK_INT (soma_start (controller));

  return 0;
}

int
cmd_stop (char *arg)
{
  char *tmp;
  int stop = 0;

  tmp = arg;

  while (*tmp != 0 && *tmp != ' ')
    tmp++;

  *tmp = 0;

  if (arg)
    stop = check_time (arg);

  if (!stop)
    stop = -1;

  CHECK_INT (soma_stop (controller, stop));

  return 0;
}

int
cmd_remove_item (char *arg)
{
  char *tmp;

  tmp = arg;

  while (*tmp != 0 && *tmp != ' ')
    tmp++;

  *tmp = 0;

  if (arg)
    CHECK_INT (soma_remove_item (controller, atoi (arg)));

  return 0;
}

int
cmd_remove_spot (char *arg)
{
  char *tmp;

  tmp = arg;

  while (*tmp != 0 && *tmp != ' ')
    tmp++;

  *tmp = 0;

  if (arg)
    CHECK_INT (soma_remove_spot (controller, atoi (arg)));

  return 0;
}

int
cmd_item_list (char *arg)
{
  char **list, **old;

  old = list = soma_get_item_list (controller);

  if (!old)
    return 0;

  while (*list)
    {
      puts (*list);
      list++;
    }

  soma_item_list_free (old);

  return 0;
}

int
cmd_spot_list (char *arg)
{
  char **list, **old;

  old = list = soma_get_spot_list (controller);

  if (!old)
    return 0;

  while (*list)
    {
      puts (*list);
      list++;
    }

  soma_spot_list_free (old);

  return 0;
}

int
cmd_pause (char *arg)
{
  CHECK_INT (soma_set_pause (controller));
  return 0;
}

int
cmd_unpause (char *arg)
{
  CHECK_INT (soma_set_unpause (controller));
  return 0;
}

int
cmd_get_pause (char *arg)
{
  int p;
  CHECK_INT (soma_get_pause (controller, &p));
  printf ("%s\n", p ? "true" : "false");
  return 0;
}

int
cmd_get_stop (char *arg)
{
  int p;
  CHECK_INT (soma_get_stop (controller, &p));
  printf ("%s\n", p ? "true" : "false");
  return 0;
}

/* EOF */
