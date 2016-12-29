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
distribuited_trim (char *path)
{
  int i, len, j, k;
  char e[SOMA_MAX_BUFF];

  SOMA_LOG_ALL ("Distribuited Trim - Started with %s", path);

  len = strlen (path);
  for (i = j = k = 0; i < len; i++)
    {
      if (path[i] == '/')
	k++;
      else
	k = 0;

      if (k < 2)
	e[j++] = path[i];
    }

  e[j] = 0;

  strcpy (path, e);

  SOMA_LOG_ALL ("Distribuited Trim - Exit with %s", path);
  return path;
}

char *
distribuited_get_path (char *path)
{
  int i;
  soma_item *item;
  char s[SOMA_MAX_BUFF];
  char k[SOMA_MAX_BUFF], t[SOMA_MAX_BUFF], *e2, *e1;
  int len;
  struct stat st;

  SOMA_LOG_ALL ("Distribuited Get Path: %s", path);

  if (!conf->DistribuitedPath || !path)
    return NULL;

  if (*path != '/')
    return NULL;

  if (*(path + 1) == '0')
    return NULL;

  snprintf (k, sizeof (k), "%s/", path);
  for (i = strlen (k) - 3; i > 0; i--)
    if (!strncmp (k + i, "/../", 4))
      return NULL;

  path = distribuited_trim (path);
  strncpy (k, path, sizeof (k));

  e1 = k;
  while (*e1 == '/' && *e1)
    e1++;

  if (!e1 || !*e1)
    return NULL;

  len = strlen (e1);
  for (i = 0; i < len; i++)
    if (e1[i] == '/')
      {
	e1[i] = 0;
	break;
      }

  if (!e1 || !*e1)
    return NULL;

  item = conf->DistribuitedPath;
  while (item)
    {
      strncpy (t, item->item, sizeof (t));

      len = strlen (t) - 1;
      while (t[len] == '/' && len >= 0)
	len--;
      t[len + 1] = 0;

      while (t[len] != '/' && len >= 0)
	len--;

      e2 = t + len + 1;
      if (!e2 || !*e2)
	return NULL;

      if (!strcmp (e1, e2))
	{
	  snprintf (s, sizeof (s), "%s/%s", item->item,
		    path + strlen (e1) + 1);

	  if (!lstat (s, &st))
	    return strdup (s);
	}

      item = item->next;
    }

  return NULL;
}

string_t *
distribuited_write (char *path)
{
  char *real;
  soma_item *t = NULL;
  int d;
  int n;
  struct dirent **dir;
  struct stat st;
  char buf[SOMA_MAX_BUFF];
  char *e;
  string_t *str;

  SOMA_LOG_ALL ("Distribuited Write - Processing: %s", path);

  if (!path)
    return NULL;

  str = string_new ();

  if (!strcmp (path, "/"))
    {
      t = conf->DistribuitedPath;

      while (t)
	{
	  if (!lstat (t->item, &st))
	    {

	      strncpy (buf, t->item, sizeof (buf));

	      for (e = NULL, n = strlen (buf) - 1; n; n--)
		{
		  if (buf[n] == '/')
		    {
		      e = buf + n + 1;
		      break;
		    }
		}

	      if (!e)
		e = buf;

	      while (*e == '/')
		e++;

	      if (!e)
		{
		  string_destroy (str);
		  return NULL;
		}

	      string_add (str, e, strlen (e));

	      if (S_ISDIR (st.st_mode))
		string_add (str, "/", 1);

	      string_add (str, "\n", 1);
	    }

	  t = t->next;
	}

      return str;
    }

  if (!(real = distribuited_get_path (path)))
    {
      string_destroy (str);
      return NULL;
    }

  if (lstat (real, &st) || !S_ISDIR (st.st_mode))
    {
      free (real);
      string_destroy (str);
      return NULL;
    }

  if ((n = scandir (real, &dir, 0, alphasort)) < 0)
    {
      free (real);
      string_destroy (str);
      return NULL;
    }

  for (d = 0; d < n; d++)
    {
      if (*dir[d]->d_name == '.')
	continue;

      snprintf (buf, sizeof (buf), "%s/%s", real, dir[d]->d_name);

      if (lstat (buf, &st))
	continue;

      if (S_ISDIR (st.st_mode))
	{
	  string_add (str, dir[d]->d_name, strlen (dir[d]->d_name));
	  string_add (str, "/", 1);
	}

      else if (S_ISLNK (st.st_mode))
	{
	  char t[SOMA_MAX_BUFF];
	  memset (t, 0, sizeof (t));

	  if (readlink (buf, t, sizeof (t)) < 0)
	    continue;

	  if (lstat (t, &st))
	    continue;

	  string_add (str, dir[d]->d_name, strlen (dir[d]->d_name));
	  if (S_ISDIR (st.st_mode))
	    string_add (str, "/", 1);

	}

      else
	string_add (str, dir[d]->d_name, strlen (dir[d]->d_name));

      string_add (str, "\n", 1);
    }

  for (d = 0; d < n; d++)
    free (dir[d]);

  free (dir);

  free (real);

  SOMA_LOG_ALL ("Distribuited Write - Exit");
  return str;
}

string_t *
distribuited_stat (char *file)
{
  char *real;
  soma_stat *stat;
  string_t *str;

  SOMA_LOG_ALL ("Distribuited Stat - Processing: %s", file);

  if (!file)
    return NULL;

  if (!(real = distribuited_get_path (file)))
    return NULL;

  if (!(stat = soma_stat_get (real)))
    return NULL;

  str = stat_string (stat);

  /* No free for the stat element because the data struct is
   * in the hash table */

  free (real);

  SOMA_LOG_ALL ("Distribuited Stat - Exit");

  return str;
}

string_t *
distribuited_stat_dir (char *file)
{
  char *real;
  soma_stat_dir *stat;
  string_t *str;

  SOMA_LOG_ALL ("Distribuited Stat Dir - Processing: %s", file);

  if (!file)
    return NULL;

  if (!(real = distribuited_get_path (file)))
    return NULL;

  if (!(stat = soma_stat_get_dir (real, 0)))
    return NULL;

  str = stat_string_dir (stat);

  /* No free for the stat element because the data struct is
   * in the hash table */

  free (real);

  SOMA_LOG_ALL ("Distribuited Stat Dir - Exit");

  return str;
}

/* EOF */
