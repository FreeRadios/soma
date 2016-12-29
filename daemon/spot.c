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

static int spot_data_dir_add_item (spot_data *, char *);
static int spot_data_dir_add_directory (spot_data *, char *);
static spot_list *spot_controller_get_data (void);
static spot_list *spot_controller_append (spot_list *, spot_item *);
static int spot_controller_get_length (spot_list *, int);
static spot_list *spot_controller_set_item (spot_list *, spot_list *, int);

static void
spot_free_memory (void)
{
  spot_data *spot_tmp;

  /* Remove the prev struct */
  spot_tmp = conf->Spot;

  while (spot_tmp)
    {
      spot_item *item, *item2;

      item = spot_tmp->Path;
      while (item)
	{

	  item2 = item;
	  item = item->next;

	  free (item2->item);
	  free (item2);
	}

      if (spot_tmp->timer)
	{
	  if (spot_tmp->timer->Start)
	    free (spot_tmp->timer->Start);
	  if (spot_tmp->timer->Stop)
	    free (spot_tmp->timer->Stop);

	  free (spot_tmp->timer);
	}

      spot_tmp = spot_tmp->next;
      if (spot_tmp)
	free (spot_tmp->prev);
    }

  conf->Spot = NULL;
}

static void
build_spot_null (void *fl, const char *str, ...)
{
  if (conf->Debug >= SOMA_DEBUG_INFO)
    {
      va_list va;

      va_start (va, str);
      vfprintf (stdout, str, va);
      va_end (va);
    }
}

/* (Re)build the spot struct */
int
build_spot (void)
{
  spot_data *spot_tmp;
  xmlDocPtr doc;
  xmlNodePtr cur, cur_pl;

  SOMA_LOG_INFO ("Read spot file...");
  xmlSetGenericErrorFunc (xmlGenericErrorContext, build_spot_null);

  /* Read and parse the conf file */
  if (!(doc = xmlParseFile (conf->Spot_fl)))
    {
      SOMA_LOG_ERR ("Error open spots file %s", conf->Spot_fl);

      if (conf->spot)
	{
	  SOMA_LOG_ERR ("Don't change the actual spot file");
	  return 1;
	}
      else
	{
	  SOMA_LOG_ERR ("No spot file founded!");
	  return 1;
	}
    }

  if (!(cur = xmlDocGetRootElement (doc))
      || xmlStrcmp (cur->name, (xmlChar *) "SpotXML"))
    {
      SOMA_LOG_ERR ("Error open spots file %s", conf->Spot_fl);
      xmlFreeDoc (doc);

      if (conf->spot)
	{
	  SOMA_LOG_ERR ("Don't change the actual spot file");
	  return 1;
	}
      else
	{
	  SOMA_LOG_ERR ("No spot file founded!");
	  return 1;
	}
    }

  spot_free_memory ();

  for (cur = cur->children; cur != NULL; cur = cur->next)
    {

      if (!xmlStrcmp (cur->name, (xmlChar *) "Spot"))
	{
	  char *v_description = NULL;
	  char *v_start = NULL;
	  char *v_stop = NULL;
	  int v_timecontinued = 0;
	  int v_repeat = 0;
	  xmlNodePtr path = NULL;
	  char *c;

	  cur_pl = cur->children;

	  for (; cur_pl != NULL; cur_pl = cur_pl->next)
	    {
	      if (!xmlStrcmp (cur_pl->name, (xmlChar *) "Description"))
		v_description =
		  (char *) xmlNodeListGetString (doc, cur_pl->xmlChildrenNode,
						 1);

	      else if (!xmlStrcmp (cur_pl->name, (xmlChar *) "Start"))
		v_start =
		  (char *) xmlNodeListGetString (doc, cur_pl->xmlChildrenNode,
						 1);

	      else if (!xmlStrcmp (cur_pl->name, (xmlChar *) "Stop"))
		v_stop =
		  (char *) xmlNodeListGetString (doc, cur_pl->xmlChildrenNode,
						 1);

	      else if (!xmlStrcmp (cur_pl->name, (xmlChar *) "TimeContinued"))
		{
		  if (!
		      (c =
		       (char *) xmlNodeListGetString (doc,
						      cur_pl->xmlChildrenNode,
						      1)) || !atoi (c))
		    v_timecontinued = 0;
		  else
		    v_timecontinued = 1;

		  if (c)
		    free (c);
		}

	      else if (!xmlStrcmp (cur_pl->name, (xmlChar *) "Repeat"))
		{
		  if (!
		      (c =
		       (char *) xmlNodeListGetString (doc,
						      cur_pl->xmlChildrenNode,
						      1)) || !atoi (c))
		    v_repeat = 0;
		  else
		    v_repeat = 1;

		  if (c)
		    free (c);
		}

	      else if (!xmlStrcmp (cur_pl->name, (xmlChar *) "Path"))
		path = cur_pl->children;
	    }

	  spot_tmp =
	    spot_data_add (v_description, v_start, v_stop, v_timecontinued,
			   v_repeat);

	  if (v_description)
	    free (v_description);

	  if (v_start)
	    free (v_start);

	  if (v_stop)
	    free (v_stop);

	  if (!spot_tmp)
	    continue;

	  /* Add the Path */
	  if (path)
	    {
	      for (; path; path = path->next)
		{
		  if (!xmlStrcmp (path->name, (xmlChar *) "item")
		      && (c =
			  (char *) xmlNodeListGetString (doc,
							 path->
							 xmlChildrenNode, 1)))
		    {
		      spot_data_dir_add (spot_tmp, c);

		      free (c);
		    }
		}
	    }
	}
    }

  /* The winner is... */
  if (!conf->Background && conf->Spot)
    {
      fprintf (stderr, "Spot info:\n\n");
      spot_tmp = conf->Spot;

      while (spot_tmp)
	{
	  fprintf (stderr, "%s\nStart: %s - Stop: %s\n\n",
		   spot_tmp->Description, spot_tmp->timer->Start,
		   spot_tmp->timer->Stop);

	  spot_tmp = spot_tmp->next;
	}
    }

  return 0;
}

spot_data *
spot_data_add (char *description, char *start, char *stop,
	       int timecontinued, int repeat)
{
  spot_data *tmp;

  if (!(tmp = (spot_data *) malloc (sizeof (spot_data))))
    SOMA_FATAL ("Error memory.");
  memset (tmp, 0, sizeof (spot_data));

  tmp->Repeat = repeat;

  if (description && *description)
    tmp->Description = strdup (description);
  else
    tmp->Description = strdup ("No description.");

  if (!tmp->Description)
    SOMA_FATAL ("Error memory.");

  if (!(tmp->timer = soma_parse_time (start, stop, timecontinued)))
    {
      SOMA_LOG_WARN ("Start/Stop syntax error.");

      free (tmp->Description);
      free (tmp);

      return NULL;
    }

  tmp->next = conf->Spot;

  if (conf->Spot)
    conf->Spot->prev = tmp;

  tmp->prev = NULL;
  conf->Spot = tmp;

  return tmp;
}

void
spot_data_dir_add (spot_data * spot, char *arg)
{
  struct stat st;
  char s[SOMA_MAX_BUFF];
  char *path;

  if (!arg)
    return;

  if (remote_file (arg))
    {
      spot_data_dir_add_item (spot, arg);
      return;
    }

  if (distribuited_file (arg))
    {

      if ((path = distribuited_get_path (arg + 1)))
	{
	  strncpy (s, path, sizeof (s));
	  free (path);
	}

      else
	{
	  SOMA_LOG_ERR ("Error no DistribuitedPath for %s.", arg);
	  return;
	}
    }

  else
    strncpy (s, arg, sizeof (s));

  if (lstat (s, &st))
    {
      SOMA_LOG_ERR ("Error file/directory %s.", s);
      return;
    }

  if (S_ISDIR (st.st_mode) && spot_data_dir_add_directory (spot, s))
    {
      SOMA_LOG_ERR ("Error file/directory %s.", s);
      return;
    }

  else if (S_ISREG (st.st_mode) && spot_data_dir_add_item (spot, s))
    {
      SOMA_LOG_ERR ("Error file/directory %s.", s);
      return;
    }

  else if (conf->SymLinks && S_ISLNK (st.st_mode))
    {
      char buf[SOMA_MAX_BUFF];

      if (readlink (s, buf, sizeof (buf)))
	{
	  spot_data_dir_add (spot, buf);
	  return;
	}
    }
}

static int
spot_data_dir_add_directory (spot_data * spot, char *arg)
{
  struct dirent **dir;
  int n, d;
  struct stat st;
  char *tmp_dir;
  int len;

  if ((n = scandir (arg, &dir, 0, alphasort)) < 0)
    return 1;

  for (d = 0; d < n; d++)
    {

      if (*dir[d]->d_name == '.')
	continue;

      len = strlen (dir[d]->d_name) + 2 + strlen (arg);
      if (!(tmp_dir = (char *) malloc (sizeof (char) * len)))
	{
	  free (dir);
	  return 1;
	}

      snprintf (tmp_dir, len, "%s/%s", arg, dir[d]->d_name);

      if (lstat (tmp_dir, &st))
	{
	  free (tmp_dir);
	  free (dir);
	  return 1;
	}

      /* No link! */
      if (S_ISLNK (st.st_mode))
	{
	  free (tmp_dir);
	  continue;
	}

      /* If it is a directory, restart */
      else if (S_ISDIR (st.st_mode))
	{
	  if (spot_data_dir_add_directory (spot, tmp_dir))
	    {
	      free (tmp_dir);
	      free (dir);
	      return 1;
	    }

	  free (tmp_dir);
	}

      else if (S_ISREG (st.st_mode))
	{
	  if (spot_data_dir_add_item (spot, tmp_dir))
	    {
	      free (tmp_dir);
	      free (dir);
	      return 1;
	    }

	  free (tmp_dir);
	}
    }

  for (d = 0; d < n; d++)
    free (dir[d]);

  free (dir);
  return 0;
}

static int
spot_data_dir_add_item (spot_data * spot, char *arg)
{
  spot_item *tmp;
  tmp = spot->Path;

  while (tmp)
    {
      if (!strcmp (tmp->item, arg))
	{
	  SOMA_LOG_WARN
	    ("%s: Directory already in the tree. Check your config file!",
	     tmp->item);
	  return 1;
	}

      tmp = tmp->next;
    }

  if (!(tmp = (spot_item *) malloc (sizeof (spot_item))))
    SOMA_FATAL ("Error memory.");

  if (!(tmp->item = strdup (arg)))
    SOMA_FATAL ("Error memory.");

  tmp->repeat = spot->Repeat;
  tmp->next = spot->Path;
  spot->Path = tmp;

  return 0;
}

static int
spot_controller_get_length (spot_list * list, int repeat)
{
  spot_list *l;
  int ret = 0;

  l = list;
  while (l)
    {
      if (l->data->repeat == repeat)
	ret++;

      else if (ret)
	break;

      l = l->next;
    }

  return ret;
}

static spot_list *
spot_controller_set_item (spot_list * list, spot_list * data, int value)
{
  spot_list *l;
  spot_list *old;

  if (!list)
    {
      data->next = NULL;
      return list;
    }

  old = l = list;
  while (l)
    {

      if (l->next && l->next->data->repeat == data->data->repeat)
	{

	  while (value)
	    {
	      value--;

	      if (!l->next || l->next->data->repeat < data->data->repeat)
		break;

	      l = l->next;
	    }

	  data->next = l->next;
	  l->next = data;

	  return list;
	}

      old = l;
      l = l->next;
    }

  data->next = NULL;
  old->next = data;
  return list;
}

static spot_list *
spot_controller_append (spot_list * list, spot_item * item)
{
  spot_list *data;
  int n;

  if (!(data = (spot_list *) malloc (sizeof (spot_list *))))
    SOMA_FATAL ("Error memory!");
  memset (data, 0, sizeof (spot_list));

  data->data = item;

  if (!list)
    {
      data->next = NULL;
      list = data;
    }

  else
    {

      n = spot_controller_get_length (list, item->repeat);

      if (!n)
	list = spot_controller_set_item (list, data, 0);
      else
	list = spot_controller_set_item (list, data, getrandom (0, n));

    }

  return list;
}

static spot_list *
spot_controller_get_data (void)
{
  spot_data *tmp;
  spot_item *item;
  spot_list *list = NULL;

  tmp = conf->Spot;

  while (tmp)
    {

      if (!start_check (tmp->timer))
	{

	  item = tmp->Path;
	  while (item)
	    {

	      if (item->repeat)
		list = spot_controller_append (list, item);

	      item = item->next;
	    }
	}

      tmp = tmp->next;
    }

  return list;
}

static void
spot_refresh (void)
{
  spot_data *data;
  spot_item *item;

  data = conf->Spot;
  while (data)
    {

      item = data->Path;
      while (item)
	{
	  item->repeat = data->Repeat;
	  item = item->next;
	}

      data = data->next;
    }
}

void
spot_controller (int number, int softstop)
{
  char *item;
  int a;
  spot_list *data, *old;
  static int day = -1;
  time_t j;
  struct tm *k;

  j = time (NULL);
  k = localtime (&j);

  if (k->tm_mday != day)
    {
      day = k->tm_mday;
      spot_refresh ();
    }

  if (!palinsesto)
    return;

  if (!(data = spot_controller_get_data ()))
    {
      SOMA_LOG_WARN ("No correct data for the spot controller!");
      return;
    }

  old = data;

  for (a = 0; a < number && !set_stop && !set_quit; a++)
    {
      if (!data)
	break;

      if (conf->next_item && !conf->nextitem_spot)
	break;

      if (start_check (palinsesto->timer))
	break;

      if (!(item = strdup (data->data->item)))
	SOMA_FATAL ("Error memory.");

      if (data->data->repeat > 0)
	data->data->repeat--;

      data = data->next;

      UNLOCK ();

      play (softstop, item, 0);
      free (item);

      LOCK ();
    }

  data = old;
  while (data)
    {
      old = data;
      data = data->next;

      free (old);
    }
}

/* EOF */
