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

char **
soma_get_path (soma_controller * c, char *path)
{
  int ret;
  char buff[SIZE_BUFFER];
  char **output;
  struct __soma_socket_client *cl;

  struct data
  {
    struct data *next;
    char *file;
  } *d = NULL, *start = NULL;

  if (!c)
    return NULL;

  if (!path)
    RETURN_VALUE (c, SOMA_ERR_USER, NULL);

  if ((ret = __soma_socket (c, &cl)) < 0)
    RETURN_VALUE (c, ret, NULL);

  switch (__soma_send_command (c, cl, SOMA_STR_GET_PATH))
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

  __soma_write (c, cl, path, strlen (path));
  __soma_write (c, cl, "\n", 1);

  while (!__soma_get_line (c, cl, buff))
    {
      if (d)
	{
	  if (!(d->next = (struct data *) malloc (sizeof (struct data))))
	    {
	      __soma_close (c, cl);
	      RETURN_VALUE (c, SOMA_ERR_POSIX, NULL);
	    }
	  d = d->next;
	}
      else
	{
	  if (!(d = (struct data *) malloc (sizeof (struct data))))
	    {
	      __soma_close (c, cl);
	      RETURN_VALUE (c, SOMA_ERR_POSIX, NULL);
	    }
	  start = d;
	}

      d->next = NULL;
      d->file = strdup (buff);
    }

  __soma_close (c, cl);

  if (!start)
    RETURN_VALUE (c, SOMA_ERR_OK, NULL);

  d = start;
  ret = 0;
  while (d)
    {
      ret++;
      d = d->next;
    }

  if (!(output = (char **) malloc (sizeof (char *) * (ret + 1))))
    RETURN_VALUE (c, SOMA_ERR_POSIX, NULL);

  d = start;
  ret = 0;
  while (d)
    {
      output[ret++] = d->file;
      start = d;
      d = d->next;
      free (start);
    }

  output[ret] = NULL;
  RETURN_VALUE (c, SOMA_ERR_OK, output);
}

void
soma_get_path_free (char **output)
{
  __soma_generic_list_free (output);
}

/* EOF */
