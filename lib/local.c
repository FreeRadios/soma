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
#error Use configure; make; make install
#endif

#include "libsoma.h"

static int soma_stat_local_dir_rec (soma_stat_dir * stat_dir, char *path,
				    int SymLinks);

void
soma_local_init (void)
{
  av_register_all ();
}

soma_stat *
soma_local_stat (char *file)
{
  soma_stat *st;
  soma_stat_stream *stream;
  AVFormatContext *fc;
  struct stat access;
  int i;
  int len;

  if (!file)
    return NULL;

  if (lstat (file, &access))
    return NULL;

  len = strlen (file);

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

  return st;
}

soma_stat_dir *
soma_local_stat_dir (char *path, int symlink)
{
  soma_stat_dir *dir;

  if (!path)
    return NULL;

  if (!(dir = (soma_stat_dir *) malloc (sizeof (soma_stat_dir))))
    return NULL;

  memset (dir, 0, sizeof (soma_stat_dir));
  strncpy (dir->dirname, path, sizeof (dir->dirname));

  if (soma_stat_local_dir_rec (dir, path, symlink))
    {
      free (dir);
      return NULL;
    }

  return dir;
}

static int
soma_stat_local_dir_rec (soma_stat_dir * stat_dir, char *path, int SymLinks)
{
  DIR *d;
  struct dirent *dir;
  char *tmp_dir;
  int len;
  struct stat st;

  if (!(d = opendir (path)))
    return 1;

  while ((dir = readdir (d)) != NULL)
    {
      if (*dir->d_name == '.')
	continue;

      len = strlen (dir->d_name) + 2 + strlen (path);

      if (!(tmp_dir = (char *) malloc (sizeof (char) * len)))
	return 1;

      snprintf (tmp_dir, len, "%s/%s", path, dir->d_name);

      if (lstat (tmp_dir, &st));	/* No operations! */

      else if (S_ISDIR (st.st_mode))
	soma_stat_local_dir_rec (stat_dir, tmp_dir, SymLinks);

      else if (S_ISREG (st.st_mode)
#ifndef WIN32
	       || (SymLinks && S_ISLNK (st.st_mode))
#endif
	)
	{
	  soma_stat *stat;
	  soma_stat_list *list;

	  if ((stat = soma_local_stat (tmp_dir)))
	    {
	      stat_dir->duration += stat->duration;

	      if (!
		  (list =
		   (soma_stat_list *) malloc (sizeof (soma_stat_list))))
		return 1;
	      memset (list, 0, sizeof (soma_stat_list));

	      list->stat = stat;
	      list->next = stat_dir->list;
	      stat_dir->list = list;
	    }
	}

      free (tmp_dir);
    }

  closedir (d);

  return 0;
}

void
soma_local_stat_free (soma_stat * stat)
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
soma_local_stat_dir_free (soma_stat_dir * stat)
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

/* EOF */
