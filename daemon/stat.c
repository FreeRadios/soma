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

#define HASH_ELEMENT 256

static ght_hash_table_t *hash;

struct hash_data
{
  time_t mtime;			/* time of last modification */
  void *data;
};

static int soma_stat_get_dir_rec (soma_stat_dir * stat_dir, char *path,
				  int check);
static int stat_check_dir (char *path);
static int stat_check_file (char *path);
static int stat_check (char *path);
static char *stat_trim (char *path);
static int stat_check_file_in_palinsesto (char *path, soma_item * list);

void
soma_stat_init (void)
{
  av_register_all ();

  HASH_LOCK ();

  if (!(hash = ght_create (HASH_ELEMENT)))
    SOMA_FATAL ("Error: memory.");

  HASH_UNLOCK ();
}

soma_stat *
soma_stat_get (char *file)
{
  soma_stat *st;
  soma_stat_stream *stream;
  AVFormatContext *fc;
  struct hash_data *hd;
  struct stat access;
  int i;
  int len;

  SOMA_LOG_ALL ("Soma Stat Get - %s", file);

  if (!file)
    return NULL;

  if (lstat (file, &access))
    return NULL;

  len = strlen (file);

  HASH_LOCK ();

  /* Search in the hash table: */
  if ((hd = ght_get (hash, len, file)))
    {
      SOMA_LOG_ALL ("Soma Stat get Hashed - %s", file);

      if (access.st_mtime != hd->mtime)
	{
	  SOMA_LOG_ALL ("Soma Stat - hash table data is obsoleted!");

	  ght_remove (hash, len, file);
	  soma_stat_free (hd->data);
	  free (hd);
	}

      else
	{
	  HASH_UNLOCK ();
	  return (soma_stat *) hd->data;
	}
    }

  HASH_UNLOCK ();

  /* If the element does not exist, create it: */
  if (av_open_input_file (&fc, file, NULL, 0, NULL))
    return NULL;

  if (av_find_stream_info (fc) < 0)
    {
      av_close_input_file (fc);
      return NULL;
    }

  if (!(st = (soma_stat *) malloc (sizeof (soma_stat))))
    {
      av_close_input_file (fc);
      return NULL;
    }

  memset (st, 0, sizeof (soma_stat));

  strcpy (st->filename, fc->filename);
  strcpy (st->title, fc->title);
  strcpy (st->author, fc->author);
  strcpy (st->copyright, fc->copyright);
  strcpy (st->comment, fc->comment);
  strcpy (st->album, fc->album);
  strcpy (st->genre, fc->genre);
  st->year = fc->year;
  st->track = fc->track;
  st->start_time = fc->start_time == AV_NOPTS_VALUE ? -1 : fc->start_time;
  st->duration = fc->duration == AV_NOPTS_VALUE ? -1 : fc->duration;
  st->bitrate = fc->bit_rate;

  for (i = 0; i < fc->nb_streams; i++)
    {
      AVStream *avst = fc->streams[i];

      if (!(stream = (soma_stat_stream *) malloc (sizeof (soma_stat_stream))))
	{
	  av_close_input_file (fc);
	  free (st);
	  return NULL;
	}

      memset (stream, 0, sizeof (soma_stat_stream));

      avcodec_string (stream->stream, sizeof (stream->stream), avst->codec,
		      0);

      stream->next = st->streams;
      st->streams = stream;
    }

  av_close_input_file (fc);

  /* Try to insert in the hash table: */
  if (!(hd = (struct hash_data *) malloc (sizeof (struct hash_data))))
    SOMA_FATAL ("Error: memory.");
  memset (hd, 0, sizeof (struct hash_data));

  hd->mtime = access.st_mtime;
  hd->data = st;

  HASH_LOCK ();

  if (ght_insert (hash, hd, len, file))
    SOMA_FATAL ("Error: memory.");

  HASH_UNLOCK ();

  return st;
}

soma_stat_dir *
soma_stat_get_dir (char *path, int check)
{
  soma_stat_dir *dir;

  SOMA_LOG_ALL ("Soma Stat Get Dir - %s", path);

  if (!path)
    return NULL;

  if (!(dir = (soma_stat_dir *) malloc (sizeof (soma_stat_dir))))
    SOMA_FATAL ("Error: memory");

  memset (dir, 0, sizeof (soma_stat_dir));
  strncpy (dir->dirname, path, sizeof (dir->dirname));

  if (soma_stat_get_dir_rec (dir, path, check))
    {
      free (dir);
      return NULL;
    }

  return dir;
}

static int
soma_stat_get_dir_rec (soma_stat_dir * stat_dir, char *path, int check)
{
  struct dirent **dir;
  char *tmp_dir;
  int len;
  int n, d;
  struct stat st;

  if ((n = scandir (path, &dir, 0, alphasort)) < 0)
    return 1;

  for (d = 0; d < n; d++)
    {
      if (*dir[d]->d_name == '.')
	continue;

      len = strlen (dir[d]->d_name) + 2 + strlen (path);

      if (!(tmp_dir = (char *) malloc (sizeof (char) * len)))
	SOMA_FATAL ("Error: memory.");

      snprintf (tmp_dir, len, "%s/%s", path, dir[d]->d_name);

      if (lstat (tmp_dir, &st));	/* No operations! */

      else if (check && stat_check_file (tmp_dir));

      else if (S_ISDIR (st.st_mode))
	soma_stat_get_dir_rec (stat_dir, tmp_dir, check);

      else if (S_ISREG (st.st_mode)
	       || (conf->SymLinks && S_ISLNK (st.st_mode)))
	{
	  soma_stat *stat;
	  soma_stat_list *list;

	  if ((stat = soma_stat_get (tmp_dir)))
	    {
	      stat_dir->duration += stat->duration;

	      if (!
		  (list =
		   (soma_stat_list *) malloc (sizeof (soma_stat_list))))
		SOMA_FATAL ("Error: memory.");
	      memset (list, 0, sizeof (soma_stat_list));

	      list->stat = stat;
	      list->next = stat_dir->list;
	      stat_dir->list = list;
	    }
	}

      free (tmp_dir);
    }

  for (d = 0; d < n; d++)
    free (dir[d]);

  free (dir);

  return 0;
}

void
soma_stat_free (soma_stat * stat)
{
  soma_stat_stream *stream, *old;

  if (!stat)
    return;

  stream = stat->streams;
  while (stream)
    {
      old = stream;
      stream = stream->next;
      free (old);
    }

  free (stat);
}

void
soma_stat_dir_free (soma_stat_dir * stat)
{
  soma_stat_list *list, *old;

  if (!stat)
    return;

  list = stat->list;
  while (list)
    {
      old = list;
      list = list->next;

      soma_stat_free (old->stat);
      free (old);
    }

  free (stat);
}

string_t *
stat_string_dir (soma_stat_dir * stat)
{
  soma_stat_list *list;
  string_t *str;
  string_t *n;

  str = string_new ();
  string_add (str, "dirname: ", 9);
  string_add (str, stat->dirname, strlen (stat->dirname));
  string_add (str, "\n", 1);

  string_add (str, "duration: ", 10);
  string_printf (str, "%lld\n", stat->duration);

  list = stat->list;
  while (list)
    {
      string_add (str, "\n", 1);

      if ((n = stat_string (list->stat)))
	{
	  if (n->size)
	    string_add (str, n->string, n->size);
	  string_destroy (n);
	}

      list = list->next;
    }

  return str;
}

string_t *
stat_string (soma_stat * stat)
{
  int i;
  soma_stat_stream *stream;
  string_t *str;

  str = string_new ();

  string_printf (str, "filename: %s\n", stat->filename);
  string_printf (str, "title: %s\n", stat->title);
  string_printf (str, "author: %s\n", stat->author);
  string_printf (str, "copyright: %s\n", stat->copyright);
  string_printf (str, "comment: %s\n", stat->comment);
  string_printf (str, "album: %s\n", stat->album);
  string_printf (str, "genre: %s\n", stat->genre);
  string_printf (str, "year: %d\n", stat->year);
  string_printf (str, "track: %d\n", stat->track);
  string_printf (str, "start_time: %lld\n", stat->start_time);
  string_printf (str, "duration: %lld\n", stat->duration);
  string_printf (str, "bitrate: %d\n", stat->bitrate);

  stream = stat->streams;
  i = 0;

  while (stream)
    {
      string_printf (str, "stream%d: %s\n", i++, stream->stream);
      stream = stream->next;
    }

  return str;
}

string_t *
stat_normal (char *file)
{
  soma_stat *stat;
  string_t *str;

  SOMA_LOG_ALL ("Stat Normal - Processing: %s", file);

  if (!file)
    return NULL;

  if (stat_check_file (file))
    return NULL;

  if (!(stat = soma_stat_get (file)))
    return NULL;

  str = stat_string (stat);

  SOMA_LOG_ALL ("Stat Normal - Exit");

  return str;
}

string_t *
stat_normal_dir (char *file)
{
  soma_stat_dir *stat;
  string_t *str;

  SOMA_LOG_ALL ("Stat Normal Dir - Processing: %s", file);

  if (!file)
    return NULL;

  if (stat_check_dir (file))
    return NULL;

  if (!(stat = soma_stat_get_dir (file, 1)))
    return NULL;

  str = stat_string_dir (stat);

  SOMA_LOG_ALL ("Stat Normal Dir - Exit");

  return str;
}

static int
stat_check (char *path)
{
  char k[SOMA_MAX_BUFF];
  int i;

  SOMA_LOG_ALL ("Stat Check: %s", path);

  if (*path != '/')
    return 1;

  if (*(path + 1) == 0)
    return 1;

  snprintf (k, sizeof (k), "%s/", path);
  for (i = strlen (k) - 3; i > 0; i--)
    if (!strncmp (k + i, "/../", 4))
      return 1;

  return 0;
}

static int
stat_check_dir (char *path)
{
  struct stat st;

  SOMA_LOG_ALL ("Stat Check Dir: %s", path);

  if (stat_check (path))
    return 1;

  if (lstat (path, &st))
    return 1;

  if (S_ISLNK (st.st_mode))
    {
      char s[SOMA_MAX_BUFF];

      if (!readlink (path, s, sizeof (s)))
	return stat_check_dir (path);

      else
	return 1;
    }

  if (!S_ISDIR (st.st_mode))
    return 1;

  return 0;
}

static int
stat_check_file (char *path)
{
  struct stat st;

  SOMA_LOG_ALL ("Stat Check File: %s", path);

  if (stat_check (path))
    return 1;

  if (lstat (path, &st))
    return 1;

  if (S_ISLNK (st.st_mode))
    {
      char s[SOMA_MAX_BUFF];

      if (!readlink (path, s, sizeof (s)))
	return stat_check_file (path);

      else
	return 1;
    }

  if (!S_ISREG (st.st_mode))
    return 1;

  LOCK ();

  if (stat_check_file_in_palinsesto (path, conf->PathItem))
    {
      UNLOCK ();
      return 0;
    }

  if (stat_check_file_in_palinsesto (path, conf->PathSpot))
    {
      UNLOCK ();
      return 0;
    }

  if (palinsesto)
    {
      if (stat_check_file_in_palinsesto (path, palinsesto->PathItem))
	{
	  UNLOCK ();
	  return 0;
	}

      if (stat_check_file_in_palinsesto (path, palinsesto->PathSpot))
	{
	  UNLOCK ();
	  return 0;
	}
    }

  UNLOCK ();

  return 1;
}

static char *
stat_trim (char *path)
{
  char ret[SOMA_MAX_BUFF];
  int i, j, q;
  int len = strlen (path);

  SOMA_LOG_ALL ("Stat Trim: %s", path);

  for (i = j = q = 0; i < len && j < sizeof (ret) - 1; i++)
    {
      if (path[i] == '/')
	{
	  if (!q)
	    {
	      ret[j++] = path[i];
	      q = 1;
	    }
	}
      else
	{
	  q = 0;
	  ret[j++] = path[i];
	}
    }

  ret[j] = 0;

  SOMA_LOG_ALL ("Stat Trim End: %s", ret);

  return strdup (ret);
}

static int
stat_check_file_in_palinsesto (char *path, soma_item * list)
{
  soma_item *item;
  char *a, *b;
  char *file;

  SOMA_LOG_ALL ("Stat Check File in palinsesto: %s", path);

  b = stat_trim (path);

  item = list;
  while (item)
    {
      if (distribuited_file (item->item))
	file = distribuited_get_path (item->item + 1);
      else
	file = item->item;

      if (!file)
	continue;

      a = stat_trim (file);

      if (!strncmp (a, b, strlen (a)))
	{
	  free (a);
	  break;
	}

      free (a);

      item = item->next;
    }

  free (b);

  if (item)
    return 1;

  return 0;
}

/* EOF */
