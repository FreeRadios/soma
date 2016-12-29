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

soma_stat *
soma_get_stat (soma_controller * c, char *file)
{
  int ret, i;
  char buff[SIZE_BUFFER];
  struct __soma_socket_client *cl;
  soma_stat *stat;

  if (!c)
    return NULL;

  if (!file)
    RETURN_VALUE (c, SOMA_ERR_USER, NULL);

  if (!(stat = (soma_stat *) malloc (sizeof (soma_stat))))
    RETURN_VALUE (c, SOMA_ERR_POSIX, NULL);

  memset (stat, 0, sizeof (soma_stat));

  if ((ret = __soma_socket (c, &cl)) < 0)
    RETURN_VALUE (c, ret, NULL);

  switch (__soma_send_command (c, cl, SOMA_STR_GET_STAT))
    {
    case 0:
      break;

    case SOMA_CODE_ERR_DATA:
      __soma_close (c, cl);
      RETURN_VALUE (c, SOMA_ERR_OK, NULL);

    default:
      __soma_close (c, cl);
      RETURN_VALUE (c, SOMA_ERR_PROTOCOL, NULL);
    }

  __soma_write (c, cl, file, strlen (file));
  __soma_write (c, cl, "\n", 1);

  while (!__soma_get_line (c, cl, buff))
    {
      if (!strncmp (buff, "filename: ", 10))
	strncpy (stat->filename, buff + 10, sizeof (stat->filename));

      else if (!strncmp (buff, "title: ", 7))
	strncpy (stat->title, buff + 7, sizeof (stat->title));

      else if (!strncmp (buff, "author: ", 8))
	strncpy (stat->author, buff + 8, sizeof (stat->author));

      else if (!strncmp (buff, "copyright: ", 11))
	strncpy (stat->copyright, buff + 11, sizeof (stat->copyright));

      else if (!strncmp (buff, "comment: ", 9))
	strncpy (stat->comment, buff + 9, sizeof (stat->comment));

      else if (!strncmp (buff, "album: ", 7))
	strncpy (stat->album, buff + 7, sizeof (stat->album));

      else if (!strncmp (buff, "genre: ", 7))
	strncpy (stat->genre, buff + 7, sizeof (stat->genre));

      else if (!strncmp (buff, "year: ", 6))
	stat->year = atoi (buff + 6);

      else if (!strncmp (buff, "track: ", 7))
	stat->track = atoi (buff + 7);

      else if (!strncmp (buff, "duration: ", 10))
	stat->duration = atoll (buff + 10);

      else if (!strncmp (buff, "start_time: ", 12))
	stat->start_time = atoll (buff + 12);

      else if (!strncmp (buff, "bitrate: ", 9))
	stat->bitrate = atoi (buff + 9);

      else if (!strncmp (buff, "stream", 6))
	{
	  i = 0;
	  soma_stat_stream *stream;

	  while (buff[i] != ' ')
	    i++;
	  i++;

	  if (!
	      (stream =
	       (soma_stat_stream *) malloc (sizeof (soma_stat_stream))))
	    {
	      __soma_close (c, cl);
	      soma_stat_free (stat);
	      RETURN_VALUE (c, SOMA_ERR_POSIX, NULL);
	    }

	  memset (stream, 0, sizeof (soma_stat_stream));

	  strncpy (stream->stream, buff + i, sizeof (stream->stream));

	  stream->next = stat->streams;
	  stat->streams = stream;
	}
    }

  __soma_close (c, cl);

  if (!*stat->filename && !stat->streams)
    {
      free (stat);
      stat = NULL;
    }

  RETURN_VALUE (c, SOMA_ERR_OK, stat);
}

soma_stat_dir *
soma_get_stat_dir (soma_controller * c, char *dir)
{
  int ret, i;
  char buff[SIZE_BUFFER];
  struct __soma_socket_client *cl;
  soma_stat_dir *stat;
  soma_stat *item;
  soma_stat_list *list;

  if (!c)
    return NULL;

  if (!dir)
    RETURN_VALUE (c, SOMA_ERR_USER, NULL);

  if (!(stat = (soma_stat_dir *) malloc (sizeof (soma_stat_dir))))
    RETURN_VALUE (c, SOMA_ERR_POSIX, NULL);

  memset (stat, 0, sizeof (soma_stat_dir));

  if ((ret = __soma_socket (c, &cl)) < 0)
    RETURN_VALUE (c, ret, NULL);

  switch (__soma_send_command (c, cl, SOMA_STR_GET_STAT_DIR))
    {
    case 0:
      break;

    case SOMA_CODE_ERR_DATA:
      __soma_close (c, cl);
      RETURN_VALUE (c, SOMA_ERR_OK, NULL);

    default:
      __soma_close (c, cl);
      RETURN_VALUE (c, SOMA_ERR_PROTOCOL, NULL);
    }

  __soma_write (c, cl, dir, strlen (dir));
  __soma_write (c, cl, "\n", 1);

  while (!__soma_get_line (c, cl, buff))
    {
      if (strncmp (buff, "dirname: ", 9))
	{
	  free (stat);
	  __soma_close (c, cl);
	  RETURN_VALUE (c, SOMA_ERR_PROTOCOL, NULL);
	}

      else
	{
	  strncpy (stat->dirname, buff + 9, sizeof (stat->dirname));
	  break;
	}
    }

  while (!__soma_get_line (c, cl, buff))
    {
      if (strncmp (buff, "duration: ", 10))
	{
	  free (stat);
	  __soma_close (c, cl);
	  RETURN_VALUE (c, SOMA_ERR_PROTOCOL, NULL);
	}

      else
	{
	  stat->duration = atoll (buff + 10);
	  break;
	}
    }

  item = NULL;
  list = NULL;

  while (!__soma_get_line (c, cl, buff))
    {
      if (!*buff)
	{
	  if (item)
	    {
	      if (!
		  (list =
		   (soma_stat_list *) malloc (sizeof (soma_stat_list))))
		{
		  __soma_close (c, cl);
		  soma_stat_dir_free (stat);
		  RETURN_VALUE (c, SOMA_ERR_POSIX, NULL);
		}

	      memset (list, 0, sizeof (soma_stat_list));
	      list->stat = item;
	      list->next = stat->list;
	      stat->list = list;
	    }

	  if (!(item = (soma_stat *) malloc (sizeof (soma_stat))))
	    {
	      soma_stat_dir_free (stat);
	      free (list);
	      __soma_close (c, cl);
	      RETURN_VALUE (c, SOMA_ERR_POSIX, NULL);
	    }

	  memset (item, 0, sizeof (soma_stat));
	  continue;
	}

      if (!item)
	{
	  __soma_close (c, cl);
	  soma_stat_dir_free (stat);
	  RETURN_VALUE (c, SOMA_ERR_PROTOCOL, NULL);
	}

      else if (!strncmp (buff, "filename: ", 10))
	strncpy (item->filename, buff + 10, sizeof (item->filename));

      else if (!strncmp (buff, "title: ", 7))
	strncpy (item->title, buff + 7, sizeof (item->title));

      else if (!strncmp (buff, "author: ", 8))
	strncpy (item->author, buff + 8, sizeof (item->author));

      else if (!strncmp (buff, "copyright: ", 11))
	strncpy (item->copyright, buff + 11, sizeof (item->copyright));

      else if (!strncmp (buff, "comment: ", 9))
	strncpy (item->comment, buff + 9, sizeof (item->comment));

      else if (!strncmp (buff, "album: ", 7))
	strncpy (item->album, buff + 7, sizeof (item->album));

      else if (!strncmp (buff, "genre: ", 7))
	strncpy (item->genre, buff + 7, sizeof (item->genre));

      else if (!strncmp (buff, "year: ", 6))
	item->year = atoi (buff + 6);

      else if (!strncmp (buff, "track: ", 7))
	item->track = atoi (buff + 7);

      else if (!strncmp (buff, "duration: ", 10))
	item->duration = atoll (buff + 10);

      else if (!strncmp (buff, "start_time: ", 12))
	item->start_time = atoll (buff + 12);

      else if (!strncmp (buff, "bitrate: ", 9))
	item->duration = atoi (buff + 9);

      else if (!strncmp (buff, "stream", 6))
	{
	  i = 0;
	  soma_stat_stream *stream;

	  while (buff[i] != ' ')
	    i++;

	  if (!
	      (stream =
	       (soma_stat_stream *) malloc (sizeof (soma_stat_stream))))
	    {
	      __soma_close (c, cl);
	      soma_stat_dir_free (stat);
	      RETURN_VALUE (c, SOMA_ERR_POSIX, NULL);
	    }

	  memset (stream, 0, sizeof (soma_stat_stream));

	  strncpy (stream->stream, buff + i, sizeof (stream->stream));

	  stream->next = item->streams;
	  item->streams = stream;
	}
    }

  if (item)
    {
      soma_stat_list *list;

      if (!(list = (soma_stat_list *) malloc (sizeof (soma_stat_list))))
	{
	  __soma_close (c, cl);
	  soma_stat_dir_free (stat);
	  RETURN_VALUE (c, SOMA_ERR_POSIX, NULL);
	}

      memset (list, 0, sizeof (soma_stat_list));
      list->stat = item;
      list->next = stat->list;
      stat->list = list;
    }

  __soma_close (c, cl);

  if (!*stat->dirname && !stat->list)
    {
      free (stat);
      stat = NULL;
    }

  RETURN_VALUE (c, SOMA_ERR_OK, stat);
}

soma_stat *
soma_get_stat_path (soma_controller * c, char *file)
{
  int ret, i;
  char buff[SIZE_BUFFER];
  struct __soma_socket_client *cl;
  soma_stat *stat;

  if (!c)
    return NULL;

  if (!file)
    RETURN_VALUE (c, SOMA_ERR_USER, NULL);

  if (!(stat = (soma_stat *) malloc (sizeof (soma_stat))))
    RETURN_VALUE (c, SOMA_ERR_POSIX, NULL);

  memset (stat, 0, sizeof (soma_stat));

  if ((ret = __soma_socket (c, &cl)) < 0)
    RETURN_VALUE (c, ret, NULL);

  switch (__soma_send_command (c, cl, SOMA_STR_GET_STAT_PATH))
    {
    case 0:
      break;

    case SOMA_CODE_ERR_DATA:
      __soma_close (c, cl);
      RETURN_VALUE (c, SOMA_ERR_OK, NULL);

    default:
      __soma_close (c, cl);
      RETURN_VALUE (c, SOMA_ERR_PROTOCOL, NULL);
    }

  __soma_write (c, cl, file, strlen (file));
  __soma_write (c, cl, "\n", 1);

  while (!__soma_get_line (c, cl, buff))
    {
      if (!strncmp (buff, "filename: ", 10))
	strncpy (stat->filename, buff + 10, sizeof (stat->filename));

      else if (!strncmp (buff, "title: ", 7))
	strncpy (stat->title, buff + 7, sizeof (stat->title));

      else if (!strncmp (buff, "author: ", 8))
	strncpy (stat->author, buff + 8, sizeof (stat->author));

      else if (!strncmp (buff, "copyright: ", 11))
	strncpy (stat->copyright, buff + 11, sizeof (stat->copyright));

      else if (!strncmp (buff, "comment: ", 9))
	strncpy (stat->comment, buff + 9, sizeof (stat->comment));

      else if (!strncmp (buff, "album: ", 7))
	strncpy (stat->album, buff + 7, sizeof (stat->album));

      else if (!strncmp (buff, "genre: ", 7))
	strncpy (stat->genre, buff + 7, sizeof (stat->genre));

      else if (!strncmp (buff, "year: ", 6))
	stat->year = atoi (buff + 6);

      else if (!strncmp (buff, "track: ", 7))
	stat->track = atoi (buff + 7);

      else if (!strncmp (buff, "duration: ", 10))
	stat->duration = atoll (buff + 10);

      else if (!strncmp (buff, "start_time: ", 12))
	stat->start_time = atoll (buff + 12);

      else if (!strncmp (buff, "bitrate: ", 9))
	stat->bitrate = atoi (buff + 9);

      else if (!strncmp (buff, "stream", 6))
	{
	  i = 0;
	  soma_stat_stream *stream;

	  while (buff[i] != ' ')
	    i++;
	  i++;

	  if (!
	      (stream =
	       (soma_stat_stream *) malloc (sizeof (soma_stat_stream))))
	    {
	      __soma_close (c, cl);
	      soma_stat_free (stat);
	      RETURN_VALUE (c, SOMA_ERR_POSIX, NULL);
	    }

	  memset (stream, 0, sizeof (soma_stat_stream));

	  strncpy (stream->stream, buff + i, sizeof (stream->stream));

	  stream->next = stat->streams;
	  stat->streams = stream;
	}
    }

  __soma_close (c, cl);

  if (!*stat->filename && !stat->streams)
    {
      free (stat);
      stat = NULL;
    }

  RETURN_VALUE (c, SOMA_ERR_OK, stat);
}

soma_stat_dir *
soma_get_stat_dir_path (soma_controller * c, char *dir)
{
  int ret, i;
  char buff[SIZE_BUFFER];
  struct __soma_socket_client *cl;
  soma_stat_dir *stat;
  soma_stat *item;
  soma_stat_list *list;

  if (!c)
    return NULL;

  if (!dir)
    RETURN_VALUE (c, SOMA_ERR_USER, NULL);

  if (!(stat = (soma_stat_dir *) malloc (sizeof (soma_stat_dir))))
    RETURN_VALUE (c, SOMA_ERR_POSIX, NULL);

  memset (stat, 0, sizeof (soma_stat_dir));

  if ((ret = __soma_socket (c, &cl)) < 0)
    RETURN_VALUE (c, ret, NULL);

  switch (__soma_send_command (c, cl, SOMA_STR_GET_STAT_DIR_PATH))
    {
    case 0:
      break;

    case SOMA_CODE_ERR_DATA:
      __soma_close (c, cl);
      RETURN_VALUE (c, SOMA_ERR_OK, NULL);

    default:
      __soma_close (c, cl);
      RETURN_VALUE (c, SOMA_ERR_PROTOCOL, NULL);
    }

  __soma_write (c, cl, dir, strlen (dir));
  __soma_write (c, cl, "\n", 1);

  while (!__soma_get_line (c, cl, buff))
    {
      if (strncmp (buff, "dirname: ", 9))
	{
	  free (stat);
	  __soma_close (c, cl);
	  RETURN_VALUE (c, SOMA_ERR_PROTOCOL, NULL);
	}

      else
	{
	  strncpy (stat->dirname, buff + 9, sizeof (stat->dirname));
	  break;
	}
    }

  while (!__soma_get_line (c, cl, buff))
    {
      if (strncmp (buff, "duration: ", 10))
	{
	  free (stat);
	  __soma_close (c, cl);
	  RETURN_VALUE (c, SOMA_ERR_PROTOCOL, NULL);
	}

      else
	{
	  stat->duration = atoll (buff + 10);
	  break;
	}
    }

  item = NULL;
  list = NULL;

  while (!__soma_get_line (c, cl, buff))
    {
      if (!*buff)
	{
	  if (item)
	    {
	      if (!
		  (list =
		   (soma_stat_list *) malloc (sizeof (soma_stat_list))))
		{
		  __soma_close (c, cl);
		  soma_stat_dir_free (stat);
		  RETURN_VALUE (c, SOMA_ERR_POSIX, NULL);
		}

	      memset (list, 0, sizeof (soma_stat_list));
	      list->stat = item;
	      list->next = stat->list;
	      stat->list = list;
	    }

	  if (!(item = (soma_stat *) malloc (sizeof (soma_stat))))
	    {
	      soma_stat_dir_free (stat);
	      free (list);
	      __soma_close (c, cl);
	      RETURN_VALUE (c, SOMA_ERR_POSIX, NULL);
	    }

	  memset (item, 0, sizeof (soma_stat));
	  continue;
	}

      if (!item)
	{
	  __soma_close (c, cl);
	  soma_stat_dir_free (stat);
	  RETURN_VALUE (c, SOMA_ERR_PROTOCOL, NULL);
	}

      else if (!strncmp (buff, "filename: ", 10))
	strncpy (item->filename, buff + 10, sizeof (item->filename));

      else if (!strncmp (buff, "title: ", 7))
	strncpy (item->title, buff + 7, sizeof (item->title));

      else if (!strncmp (buff, "author: ", 8))
	strncpy (item->author, buff + 8, sizeof (item->author));

      else if (!strncmp (buff, "copyright: ", 11))
	strncpy (item->copyright, buff + 11, sizeof (item->copyright));

      else if (!strncmp (buff, "comment: ", 9))
	strncpy (item->comment, buff + 9, sizeof (item->comment));

      else if (!strncmp (buff, "album: ", 7))
	strncpy (item->album, buff + 7, sizeof (item->album));

      else if (!strncmp (buff, "genre: ", 7))
	strncpy (item->genre, buff + 7, sizeof (item->genre));

      else if (!strncmp (buff, "year: ", 6))
	item->year = atoi (buff + 6);

      else if (!strncmp (buff, "track: ", 7))
	item->track = atoi (buff + 7);

      else if (!strncmp (buff, "duration: ", 10))
	item->duration = atoll (buff + 10);

      else if (!strncmp (buff, "start_time: ", 12))
	item->start_time = atoll (buff + 12);

      else if (!strncmp (buff, "bitrate: ", 9))
	item->duration = atoi (buff + 9);

      else if (!strncmp (buff, "stream", 6))
	{
	  i = 0;
	  soma_stat_stream *stream;

	  while (buff[i] != ' ')
	    i++;

	  if (!
	      (stream =
	       (soma_stat_stream *) malloc (sizeof (soma_stat_stream))))
	    {
	      __soma_close (c, cl);
	      soma_stat_dir_free (stat);
	      RETURN_VALUE (c, SOMA_ERR_POSIX, NULL);
	    }

	  memset (stream, 0, sizeof (soma_stat_stream));

	  strncpy (stream->stream, buff + i, sizeof (stream->stream));

	  stream->next = item->streams;
	  item->streams = stream;
	}

    }

  if (item)
    {
      soma_stat_list *list;

      if (!(list = (soma_stat_list *) malloc (sizeof (soma_stat_list))))
	{
	  __soma_close (c, cl);
	  soma_stat_dir_free (stat);
	  RETURN_VALUE (c, SOMA_ERR_POSIX, NULL);
	}

      memset (list, 0, sizeof (soma_stat_list));
      list->stat = item;
      list->next = stat->list;
      stat->list = list;
    }

  __soma_close (c, cl);

  if (!*stat->dirname && !stat->list)
    {
      free (stat);
      stat = NULL;
    }

  RETURN_VALUE (c, SOMA_ERR_OK, stat);
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

/* EOF */
