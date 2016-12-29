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

void
nextitem_path_add (char *data)
{
  static char buf[SOMA_MAX_BUFF];

  SOMA_LOG_ALL ("NextItem Path Adding: %s", data);

  if (!data)
    return;

  snprintf (buf, sizeof (buf), "%c%s", DISTRIBUITED_CHAR, data);

  nextitem_add (buf);
}

void
nextitem_set_mix (int a)
{
  SOMA_LOG_ALL ("NextItem Set Mix to %d", a);

  conf->nextitem_spot = a;
}

void
nextitem_add (char *data)
{

  struct nextitem *ns;
  struct nextitem *tmp;
  struct stat st;

  SOMA_LOG_ALL ("NextItem Add %s", data);

  if (!data)
    return;

  if (!remote_file (data) && !distribuited_file (data)
      && (lstat (data, &st) || !S_ISREG (st.st_mode)))
    {
      SOMA_LOG_WARN ("%s file does not exist!", data);
      return;
    }

  if (!(ns = (struct nextitem *) malloc (sizeof (struct nextitem))))
    SOMA_FATAL ("Error: memory.");

  ns->next = NULL;
  if (!(ns->item = strdup (data)))
    SOMA_FATAL ("Error: memory.");

  LOCK ();

  if (!conf->next_item)
    {
      conf->next_item = ns;
      UNLOCK ();
      return;
    }

  tmp = conf->next_item;
  while (tmp->next)
    tmp = tmp->next;

  tmp->next = ns;

  UNLOCK ();
}

void
nextitem_remove (void)
{
  struct nextitem *ns;

  SOMA_LOG_ALL ("NextItem Remove");

  if (!conf->next_item)
    return;

  LOCK ();

  ns = conf->next_item;
  conf->next_item = conf->next_item->next;

  free (ns->item);
  free (ns);

  UNLOCK ();
}

void
nextitem_remove_n (int n)
{
  struct nextitem *ns, *old = NULL;

  SOMA_LOG_ALL ("NextItem Remove Number %d", n);

  if (!conf->next_item)
    return;

  LOCK ();

  ns = conf->next_item;
  while (ns)
    {
      if (!n)
	{
	  if (!old)
	    conf->next_item = conf->next_item->next;
	  else
	    old->next = ns->next;

	  free (ns->item);
	  free (ns);
	  break;
	}

      n--;

      old = ns;
      ns = ns->next;
    }

  UNLOCK ();
}

void
nextitem_remove_first (void)
{
  SOMA_LOG_ALL ("NextItem Remove First");

  nextitem_remove_n (0);
}

void
nextitem_remove_last (void)
{
  struct nextitem *ns, *old = NULL;

  SOMA_LOG_ALL ("NextItem Remove Last");

  if (!conf->next_item)
    return;

  LOCK ();

  ns = conf->next_item;
  while (ns)
    {

      if (!ns->next)
	{
	  if (!old)
	    conf->next_item = conf->next_item->next;
	  else
	    old->next = ns->next;

	  free (ns->item);
	  free (ns);
	  break;

	}

      old = ns;
      ns = ns->next;
    }

  UNLOCK ();
}

/* EOF */
