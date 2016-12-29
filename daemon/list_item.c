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

/* This functions adds a directory in the PathItem/Spot trees. */
void
soma_item_add (int k, char *arg)
{
  soma_item *tmp;

  SOMA_LOG_ALL ("Soma - Directory adding: %s", arg);

  if (k == SOMA_PATH_ITEM)
    tmp = conf->PathItem;

  else if (k == SOMA_PATH_SPOT)
    tmp = conf->PathSpot;

  else
    tmp = conf->DistribuitedPath;

  while (tmp)
    {
      if (!strcmp (tmp->item, arg))
	{
	  SOMA_LOG_WARN
	    ("%s: directory is already in the tree. Check your config file!",
	     tmp->item);
	  return;
	}
      tmp = tmp->next;
    }

  if (!(tmp = (soma_item *) malloc (sizeof (soma_item))))
    SOMA_FATAL ("Error memory.");

  tmp->item = arg;

  if (k == SOMA_PATH_ITEM)
    {
      tmp->next = conf->PathItem;
      conf->PathItem = tmp;
    }

  else if (k == SOMA_PATH_SPOT)
    {
      tmp->next = conf->PathSpot;
      conf->PathSpot = tmp;
    }

  else if (k == SOMA_DISTRIBUITED_PATH)
    {
      tmp->next = conf->DistribuitedPath;
      conf->DistribuitedPath = tmp;
    }
}

/* EOF */
