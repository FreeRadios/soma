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

static void marker_rebuild (void);

/* (Re)build the tree of items. This function run:
 * build -> for item/spot of conf file
 * build_pl -> for item/spot of this palinsesto */
int
build_data ()
{
  int ret = 0;

  SOMA_LOG_ALL ("Build Data - Started...");

  if (LOCK ())
    {
      SOMA_LOG_WARN ("Lock error.");
      return 1;
    }

  if (build ())
    {
      SOMA_LOG_ALL ("Build Data - Exit");

      UNLOCK ();
      return 1;
    }

  if (palinsesto && build_pl ())
    {
      SOMA_LOG_ALL ("Build Data - Exit");

      UNLOCK ();
      return 1;
    }

  ret = build_spot ();

  UNLOCK ();

  SOMA_LOG_ALL ("Build Data - Exit");
  return ret;
}

/* Build the item/spot tree from conf file */
int
build (void)
{
  item *tmp;
  marker *tmp_marker;

  SOMA_LOG_ALL ("Build - Started...");

  /* If there are the prev tree, i remove it */
  if (conf->item || conf->spot)
    {
      SOMA_LOG_INFO ("Remove data...");

      while (conf->item)
	{
	  tmp = conf->item;
	  conf->item = conf->item->next;
	  free (tmp->name);
	  free (tmp);
	}

      conf->item_last = NULL;

      while (conf->marker_item)
	{
	  tmp_marker = conf->marker_item;
	  conf->marker_item = tmp_marker->next;
	  free (tmp_marker);
	}

      conf->marker_item_last = NULL;

      while (conf->spot)
	{
	  tmp = conf->spot;
	  conf->spot = conf->spot->next;
	  free (tmp->name);
	  free (tmp);
	}

      conf->spot_last = NULL;

      while (conf->marker_spot)
	{
	  tmp_marker = conf->marker_spot;
	  conf->marker_spot = tmp_marker->next;
	  free (tmp_marker);
	}

      conf->marker_spot_last = NULL;

      conf->n_item = 0;
      conf->n_spot = 0;
    }

  SOMA_LOG_INFO ("Build audio file trees...");

  /* Build the tree... */
  build_tree (conf->PathItem, SOMA_DEFAULT_ITEM);
  build_tree (conf->PathSpot, SOMA_DEFAULT_SPOT);

  SOMA_LOG_INFO ("Audio Files - Items: %d -Spot: %d", conf->n_item,
		 conf->n_spot);

  SOMA_LOG_ALL ("Build - Exit");

  return 0;
}

/* Remove the item/spot when this file are by palinsesto and no config file. */
int
delete_pl (void)
{
  item *tmp;
  marker *tmp_marker;

  SOMA_LOG_ALL ("Delete Palinsesto - Started...");

  if (conf->item || conf->spot)
    {
      SOMA_LOG_INFO ("Remove data...");

      tmp = conf->item;

      while (tmp)
	{

	  if (!tmp->pl)		/* If is a soma.cfg path... */
	    tmp = tmp->next;

	  else
	    {
	      item *k = tmp->next;

	      if (tmp->next)
		tmp->next->prev = tmp->prev;

	      else
		conf->item_last = tmp->prev;

	      if (tmp->prev)
		tmp->prev->next = tmp->next;

	      else
		conf->item = tmp->next;

	      free (tmp->name);
	      free (tmp);

	      conf->n_item--;

	      tmp = k;
	    }
	}

      while (conf->marker_item)
	{
	  tmp_marker = conf->marker_item;
	  conf->marker_item = conf->marker_item->next;
	  free (tmp_marker);
	}

      conf->marker_item_last = NULL;

      tmp = conf->spot;

      while (tmp)
	{

	  if (!tmp->pl)
	    tmp = tmp->next;

	  else
	    {
	      item *k = tmp->next;

	      if (tmp->next)
		tmp->next->prev = tmp->prev;

	      else
		conf->spot_last = tmp->prev;

	      if (tmp->prev)
		tmp->prev->next = tmp->next;

	      else
		conf->spot = tmp->next;

	      free (tmp->name);
	      free (tmp);

	      conf->n_spot--;

	      tmp = k;
	    }
	}

      while (conf->marker_spot)
	{
	  tmp_marker = conf->marker_spot;
	  conf->marker_spot = conf->marker_spot->next;
	  free (tmp_marker);
	}

      conf->marker_spot_last = NULL;

      marker_rebuild ();

    }

  SOMA_LOG_ALL ("Delete Palinsesto - Exit");

  return 0;
}

/* Build the item/spot trees of this palinsesto */
int
build_pl (void)
{
  if (delete_pl ())
    return 1;

  SOMA_LOG_INFO ("Build palinsesto audio file trees...");

  build_tree (palinsesto->PathItem, SOMA_PALINSESTO_ITEM);
  build_tree (palinsesto->PathSpot, SOMA_PALINSESTO_SPOT);

  SOMA_LOG_INFO ("Audio Files - Items: %d -Spot: %d", conf->n_item,
		 conf->n_spot);

  return 0;
}

void
build_tree (soma_item * tmp_item, int type)
{
  SOMA_LOG_ALL ("Build Tree - Started...");

  while (tmp_item)
    {
      add_item (type, tmp_item->item);

      tmp_item = tmp_item->next;
    }

  SOMA_LOG_ALL ("Build Tree - Exit");
}

static void
marker_rebuild (void)
{
  marker *tmp;
  item *item;
  int id;

  item = conf->item;
  id = 0;
  tmp = NULL;

  while (item)
    {
      if (id == SOMA_MAX_MARKER || !tmp)
	{
	  id = 0;

	  tmp = (marker *) malloc (sizeof (marker));
	  memset (tmp, 0, sizeof (marker));

	  tmp->item = item;
	  tmp->prev = conf->marker_item_last;

	  if (conf->marker_item_last)
	    conf->marker_item_last->next = tmp;
	  else
	    conf->marker_item = tmp;

	  conf->marker_item_last = tmp;
	}

      tmp->elements++;
      id++;

      item = item->next;
    }

  item = conf->spot;
  id = 0;
  tmp = NULL;

  while (item)
    {
      if (id == SOMA_MAX_MARKER || !tmp)
	{
	  id = 0;

	  tmp = (marker *) malloc (sizeof (marker));
	  memset (tmp, 0, sizeof (marker));

	  tmp->item = item;
	  tmp->prev = conf->marker_spot_last;

	  if (conf->marker_spot_last)
	    conf->marker_spot_last->next = tmp;
	  else
	    conf->marker_spot = tmp;

	  conf->marker_spot_last = tmp;
	}

      tmp->elements++;
      id++;

      item = item->next;
    }
}

/* EOF */
