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

/* This function adds a item in the correct tree.
 * int k can be: 
 * 	DEFAULT_ITEM -> PathItem generic
 * 	DEFAULT_SPOT -> PathSpot generic
 * 	PALINSESTO_ITEM -> PathItem palinsesto
 * 	PALINSESTO_SPOT -> PathSpot palinsesto
 *
 * char *fl: the item
 */
int
add_item (int type, char *file)
{
  char s[SOMA_MAX_BUFF];
  struct stat st;

  /* Is it a remote file ? */
  if (remote_file (file))
    {
      if (add_file (type, file))
	{
	  SOMA_LOG_ERR ("Error file: %s.", file);
	  return 1;
	}

      return 0;
    }

  else if (distribuited_file (file))
    {
      char *path;

      if ((path = distribuited_get_path (file + 1)))
	{
	  strncpy (s, path, sizeof (s));
	  free (path);
	}
      else
	{
	  SOMA_LOG_ERR ("Error no DistribuitedPath for %s.", file);
	  return 1;
	}
    }

  else
    strncpy (s, file, sizeof (s));

  /* In s there is the real path... */
  if (lstat (s, &st))
    {
      SOMA_LOG_ERR ("Error file/directory %s.", s);
      return 1;
    }

  if (S_ISDIR (st.st_mode))
    {

      /* Add_directory adds all directory file recursive */
      if (add_directory (type, s))
	{
	  SOMA_LOG_ERR ("Error directory: %s.", s);
	  return 1;
	}

    }

  else if (S_ISREG (st.st_mode))
    {

      /* Add_file adds only 1 file */
      if (add_file (type, s))
	{
	  SOMA_LOG_ERR ("Error file: %s.", s);
	  return 1;
	}
    }

  else if (conf->SymLinks && S_ISLNK (st.st_mode))
    {
      char buf[SOMA_MAX_BUFF];

      /* Recursive function for symlink */
      if (!readlink (s, buf, sizeof (buf)))
	return add_item (type, buf);
    }

  return 0;
}

/* This function adds a file in the correct tree.
 * int k can be: 
 * 	DEFAULT_ITEM -> PathItem generic
 * 	DEFAULT_SPOT -> PathSpot generic
 * 	PALINSESTO_ITEM -> PathItem palinsesto
 * 	PALINSESTO_SPOT -> PathSpot palinsesto
 *
 * char *fl: the file
 */
int
add_file (int k, char *fl)
{
  item *tmp;
  marker *tmp_marker;

  SOMA_LOG_ALL ("Add File - Started with %s", fl);

  if (playlist_is (fl))
    return add_playlist (k, fl);

  if (!(tmp = (item *) malloc (sizeof (item))))
    SOMA_FATAL ("Error memory.");

  memset (tmp, 0, sizeof (item));

  if (!(tmp->name = strdup (fl)))
    SOMA_FATAL ("Error memory.");

  tmp->play = 0;

  if (k > 1)
    tmp->pl = 1;
  else
    tmp->pl = 0;

  switch (k)
    {
    case SOMA_DEFAULT_ITEM:
    case SOMA_PALINSESTO_ITEM:

      if (!conf->item)
	{
	  conf->item = tmp;

	  if (!(tmp_marker = (marker *) malloc (sizeof (marker))))
	    SOMA_FATAL ("Error memory.");

	  memset (tmp_marker, 0, sizeof (marker));
	  conf->marker_item = conf->marker_item_last = tmp_marker;

	  tmp_marker->item = tmp;
	}

      else
	{
	  conf->item_last->next = tmp;
	  tmp->prev = conf->item_last;

	  if (conf->marker_item_last->elements >= SOMA_MAX_MARKER)
	    {
	      if (!(tmp_marker = (marker *) malloc (sizeof (marker))))
		SOMA_FATAL ("Error memory.");

	      memset (tmp_marker, 0, sizeof (marker));

	      tmp_marker->prev = conf->marker_item_last;
	      conf->marker_item_last->next = tmp_marker;
	      conf->marker_item_last = tmp_marker;

	      tmp_marker->item = tmp;
	    }
	}

      conf->marker_item_last->elements++;
      conf->item_last = tmp;
      conf->n_item++;
      break;

    case SOMA_DEFAULT_SPOT:
    case SOMA_PALINSESTO_SPOT:
      if (!conf->spot)
	{
	  conf->spot = tmp;

	  if (!(tmp_marker = (marker *) malloc (sizeof (marker))))
	    SOMA_FATAL ("Error memory.");

	  memset (tmp_marker, 0, sizeof (marker));
	  conf->marker_spot = conf->marker_spot_last = tmp_marker;

	  tmp_marker->item = tmp;
	}

      else
	{
	  conf->spot_last->next = tmp;
	  tmp->prev = conf->spot_last;

	  if (conf->marker_spot_last->elements >= SOMA_MAX_MARKER)
	    {
	      if (!(tmp_marker = (marker *) malloc (sizeof (marker))))
		SOMA_FATAL ("Error memory.");

	      memset (tmp_marker, 0, sizeof (marker));

	      tmp_marker->prev = conf->marker_spot_last;
	      conf->marker_spot_last->next = tmp_marker;
	      conf->marker_spot_last = tmp_marker;

	      tmp_marker->item = tmp;
	    }
	}

      conf->marker_spot_last->elements++;
      conf->spot_last = tmp;
      conf->n_spot++;
      break;
    }

  SOMA_LOG_ALL ("Add File - Exit");
  return 0;
}

/* Recursive function. The arguments are the same of add_file */
int
add_directory (int k, char *path)
{
  struct dirent **dir;
  char *tmp_dir;
  int len;
  int d, n;

  SOMA_LOG_ALL ("Add Directory - Started with %s", path);

  if ((n = scandir (path, &dir, 0, alphasort)) < 0)
    return 1;

  for (d = 0; d < n; d++)
    {
      if (*dir[d]->d_name == '.')
	continue;

      len = strlen (dir[d]->d_name) + 2 + strlen (path);

      if (!(tmp_dir = (char *) malloc (sizeof (char) * len)))
	{
	  free (dir);
	  return 1;
	}

      snprintf (tmp_dir, len, "%s/%s", path, dir[d]->d_name);

      add_item (k, tmp_dir);
      free (tmp_dir);
    }

  for (d = 0; d < n; d++)
    free (dir[d]);

  free (dir);

  SOMA_LOG_ALL ("Add Directory - Exit");
  return 0;
}

int
search_item (int n, int k, int softstop)
{
  item *tmp;
  marker *tmp_marker;
  int random;
  int ret;

  SOMA_LOG_ALL ("Search Item - started...");

  if (LOCK ())
    {
      SOMA_LOG_WARN ("Lock error.");
      return 0;			/* 0 because 1 is for a play error */
    }

  if (!k)
    {
      if (id_item >= conf->n_item)
	id_item = 0;
      tmp_marker = conf->marker_item;
    }
  else
    {
      if (id_spot >= conf->n_spot)
	id_spot = 0;
      tmp_marker = conf->marker_spot;
    }

  while (tmp_marker)
    {

      /* Oky find! */
      if (n <= tmp_marker->elements)
	{
	  tmp = tmp_marker->item;

	  if (n)
	    {
	      while (tmp && n)
		{
		  n--;
		  tmp = tmp->next;
		}
	    }

	  if (!tmp)
	    {
	      if (!k)
		{
		  tmp_marker = conf->marker_item;
		  continue;
		}
	      else
		{
		  tmp_marker = conf->marker_spot;
		  continue;
		}
	    }

	  /* 0... no problem */
	  if (!tmp->play)
	    {
	      char *str;

	      tmp->play = 1;

	      if (!(str = strdup (tmp->name)))
		SOMA_FATAL ("Error memory.");

	      UNLOCK ();

	      ret = play (softstop, str, 0);
	      free (str);

	      SOMA_LOG_ALL ("Search Item - exit with value %d", ret);

	      return ret;

	    }

	  /* Search another one */
	  else
	    {
	      if (!k)
		tmp = conf->item;
	      else
		tmp = conf->spot;

	      while (tmp)
		{

		  if (!tmp->play)
		    {
		      char *str;

		      tmp->play = 1;

		      if (!(str = strdup (tmp->name)))
			SOMA_FATAL ("Error memory.");

		      UNLOCK ();

		      ret = play (softstop, str, 0);
		      free (str);

		      SOMA_LOG_ALL ("Search Item - exit with value %d", ret);

		      return ret;
		    }

		  tmp = tmp->next;
		}

	      /* No file ?... Reset */
	      if (!k)
		{
		  tmp = conf->item;
		  id_item = 0;
		}
	      else
		{
		  tmp = conf->spot;
		  id_spot = 0;
		}

	      while (tmp)
		{
		  tmp->play = 0;
		  tmp = tmp->next;
		}

	      if (!k)
		random = palinsesto->RandomItem;
	      else
		random = palinsesto->RandomSpot;

	      UNLOCK ();

	      /* Ristart */
	      if (!k)
		ret =
		  search_item (random ? getrandom (0, conf->n_item) :
			       id_item++, 0, softstop);
	      else
		ret =
		  search_item (random ? getrandom (0, conf->n_spot) :
			       id_spot++, 1, softstop);

	      SOMA_LOG_ALL ("Search Item - exit with value %d", ret);

	      return ret;
	    }
	}

      n -= tmp_marker->elements;
      tmp_marker = tmp_marker->next;
    }

  UNLOCK ();
  SOMA_LOG_ALL ("Search Item - exit with value 0");

  return 0;
}

void
remove_item (int k, int id)
{
  int id_saved = id;
  item *tmp;
  marker *tmp_marker;

  SOMA_LOG_ALL ("Remove Item : %d", id);

  if (LOCK ())
    {
      SOMA_LOG_WARN ("Lock error.");
      return;
    }

  if (!k)
    tmp_marker = conf->marker_item;

  else
    tmp_marker = conf->marker_spot;

  while (tmp_marker)
    {

      /* Oky find! */
      if (id <= tmp_marker->elements)
	{
	  tmp = tmp_marker->item;

	  if (id)
	    {
	      while (tmp && id)
		{
		  id--;
		  tmp = tmp->next;
		}
	    }

	  if (!tmp)
	    {
	      UNLOCK ();
	      return;
	    }

	  if (tmp->next)
	    tmp->next->prev = tmp->prev;

	  else if (!k)
	    conf->item_last = tmp->prev;

	  else
	    conf->spot_last = tmp->prev;

	  if (tmp->prev)
	    tmp->prev->next = tmp->next;

	  else if (!k)
	    conf->item = tmp->next;

	  else
	    conf->spot = tmp->next;

	  if (tmp_marker->item == tmp)
	    tmp_marker->item = tmp->next;

	  tmp_marker->elements--;

	  if (!tmp_marker->elements)
	    {
	      if (tmp_marker->next)
		tmp_marker->next->prev = tmp_marker->prev;

	      else if (!k)
		conf->marker_item_last = tmp_marker->prev;

	      else
		conf->marker_spot_last = tmp_marker->prev;

	      if (tmp_marker->prev)
		tmp_marker->prev->next = tmp_marker->next;

	      else if (!k)
		conf->marker_item = tmp_marker->next;

	      else
		conf->marker_spot = tmp_marker->next;

	      free (tmp_marker);
	    }

	  free (tmp->name);
	  free (tmp);

	  if (!k)
	    {
	      conf->n_item--;

	      if (id_saved < id_item)
		id_item--;
	    }
	  else
	    {
	      conf->n_spot--;

	      if (id_saved < id_spot)
		id_spot--;
	    }

	  UNLOCK ();
	  return;
	}

      id -= tmp_marker->elements;
      tmp_marker = tmp_marker->next;
    }

  UNLOCK ();
}

/* EOF */
