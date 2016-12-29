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

static void soma_opt_add (int, char *);

/* The options list */
static void
soma_opt_add (int k, char *arg)
{
  soma_opt *tmp, *tmp2;

  SOMA_LOG_ALL ("Options adding: %s", arg);

  if (!(tmp = (soma_opt *) malloc (sizeof (soma_opt))))
    SOMA_FATAL ("Error memory.");

  if (!(tmp->opt = (char *) malloc (strlen (arg) * sizeof (char))))
    SOMA_FATAL ("Error memory.");

  strcpy (tmp->opt, arg);
  tmp->next = NULL;

  if (!k)
    {
      conf->n_OptionsItem++;

      if (!conf->OptionsItem)
	{
	  conf->OptionsItem = tmp;
	  return;
	}

      tmp2 = conf->OptionsItem;

      while (tmp2->next)
	tmp2 = tmp2->next;

      tmp2->next = tmp;

    }
  else
    {

      conf->n_OptionsStream++;

      if (!conf->OptionsStream)
	{
	  conf->OptionsStream = tmp;
	  return;
	}

      tmp2 = conf->OptionsStream;

      while (tmp2->next)
	tmp2 = tmp2->next;

      tmp2->next = tmp;

    }
}

void
soma_options (int flag, char *ar)
{
  char opt[SOMA_MAX_BUFF];
  int a = 0;
  int q1 = 0;
  int q2 = 0;

  SOMA_LOG_ALL ("Options parsing: %s", ar);

  while (*ar)
    {
      if (*ar == '\"' && !q2)
	{
	  q1 = !q1;
	  ar++;
	}

      else if (*ar == '\'' && !q1)
	{
	  q2 = !q2;
	  ar++;
	}

      else if ((*ar == ' ' || *ar == '\t') && !q1 && !q2)
	{
	  ar++;
	  if (a)
	    {
	      opt[a] = 0;
	      soma_opt_add (flag, opt);
	    }
	  a = 0;
	}
      else if (a < SOMA_MAX_BUFF)
	opt[a++] = *ar++;
    }

  if (a)
    {
      opt[a] = 0;
      soma_opt_add (flag, opt);
    }
}

char *
soma_search_path (char *cmd)
{
  char *path;
  char buf[SOMA_MAX_BUFF];
  int len = strlen (cmd);
  int i = 0;
  struct stat st;

  if (!(path = getenv ("PATH")))
    return NULL;

  while (*path)
    {
      if (*path != ':' && i < sizeof (buf) - (len + 2))
	{
	  buf[i] = *path;
	  i++;
	}

      else if (*path == ':')
	{
	  snprintf (buf + i, sizeof (buf) - i, "/%s", cmd);

	  if (!lstat (buf, &st))
	    return strdup (buf);

	  i = 0;
	}

      path++;
    }

  return NULL;
}

/* EOF */
