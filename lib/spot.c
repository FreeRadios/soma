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

int
soma_new_spot_file (soma_controller * c, char *file)
{
  struct __soma_socket_client *data;
  int ret;
  char buf[SIZE_BUFFER];
  int fl;

  if (!c)
    return 1;

  if (!file)
    RETURN_VALUE (c, SOMA_ERR_USER, 1);

  if ((ret = __soma_socket (c, &data)) != SOMA_ERR_OK)
    RETURN_VALUE (c, ret, 1);

  if (__soma_send_command (c, data, SOMA_STR_NEW_SPOT))
    {
      __soma_close (c, data);
      RETURN_VALUE (c, SOMA_ERR_PROTOCOL, 1);
    }
  if ((fl = open (file, O_RDONLY)) < 0)
    {
      __soma_close (c, data);
      RETURN_VALUE (c, SOMA_ERR_POSIX, 1);
    }
  while ((ret = read (fl, buf, sizeof (buf))) > 0)
    __soma_write (c, data, buf, ret);

  close (fl);
  __soma_close (c, data);

  RETURN_VALUE (c, SOMA_ERR_OK, 0);
}

int
soma_new_spot_buffer (soma_controller * c, char *buff, size_t size)
{
  int ret;
  struct __soma_socket_client *data;

  if (!c)
    return 1;

  if (!buff)
    RETURN_VALUE (c, SOMA_ERR_USER, 1);

  if ((ret = __soma_socket (c, &data)) != SOMA_ERR_OK)
    RETURN_VALUE (c, ret, 1);

  if (__soma_send_command (c, data, SOMA_STR_NEW_SPOT))
    {
      __soma_close (c, data);
      RETURN_VALUE (c, SOMA_ERR_PROTOCOL, 1);
    }
  if (size)
    __soma_write (c, data, buff, size);
  else
    __soma_write (c, data, buff, strlen (buff));

  __soma_close (c, data);

  RETURN_VALUE (c, SOMA_ERR_OK, 0);
}

char *
soma_get_spot (soma_controller * c)
{
  int ret;
  char buff[SIZE_BUFFER];
  char *data = NULL;
  int s;
  int size = 0;
  struct __soma_socket_client *d;

  if (!c)
    return NULL;

  if ((ret = __soma_socket (c, &d)) != SOMA_ERR_OK)
    RETURN_VALUE (c, ret, NULL);

  switch (__soma_send_command (c, d, SOMA_STR_GET_SPOT))
    {
    case 0:
      break;

    case SOMA_CODE_ERR_DATA:
      __soma_close (c, d);
      RETURN_VALUE (c, SOMA_ERR_OK, NULL);

    default:
      __soma_close (c, d);
      RETURN_VALUE (c, SOMA_ERR_PROTOCOL, NULL);
    }

  while ((s = __soma_read (c, d, buff, sizeof (buff))) > 0)
    {
      if (data)
	{
	  if (!(data = realloc (data, (size + s + 1) * sizeof (char))))
	    {
	      __soma_close (c, d);
	      RETURN_VALUE (c, SOMA_ERR_POSIX, NULL);
	    }
	}
      else
	{
	  if (!(data = malloc ((s + 1) * sizeof (char))))
	    {
	      __soma_close (c, d);
	      RETURN_VALUE (c, SOMA_ERR_POSIX, NULL);
	    }
	}

      memcpy (data + size, buff, s);
      size += s;
    }

  if (data)
    data[size] = 0;

  __soma_close (c, d);
  RETURN_VALUE (c, SOMA_ERR_OK, data);
}

char *
soma_get_old_spot (soma_controller * c)
{
  int ret;
  char buff[SIZE_BUFFER];
  char *data = NULL;
  int s;
  int size = 0;
  struct __soma_socket_client *d;

  if (!c)
    return NULL;

  if ((ret = __soma_socket (c, &d)) != SOMA_ERR_OK)
    RETURN_VALUE (c, ret, NULL);

  switch (__soma_send_command (c, d, SOMA_STR_GET_OLD_SPOT))
    {
    case 0:
      break;

    case SOMA_CODE_ERR_DATA:
      __soma_close (c, d);
      RETURN_VALUE (c, SOMA_ERR_OK, NULL);

    default:
      __soma_close (c, d);
      RETURN_VALUE (c, SOMA_ERR_PROTOCOL, NULL);
    }

  while ((s = __soma_read (c, d, buff, sizeof (buff))) > 0)
    {
      if (data)
	{
	  if (!(data = realloc (data, (size + s + 1) * sizeof (char))))
	    {
	      __soma_close (c, d);
	      RETURN_VALUE (c, SOMA_ERR_POSIX, NULL);
	    }
	}
      else
	{
	  if (!(data = malloc ((s + 1) * sizeof (char))))
	    {
	      __soma_close (c, d);
	      RETURN_VALUE (c, SOMA_ERR_POSIX, NULL);
	    }
	}

      memcpy (data + size, buff, s);
      size += s;
    }

  if (data)
    data[size] = 0;

  __soma_close (c, d);
  RETURN_VALUE (c, SOMA_ERR_OK, data);
}

int
soma_old_spot (soma_controller * c)
{
  return __soma_generic (c, SOMA_STR_OLD_SPOT);
}

int
soma_set_default_spot (soma_controller * c)
{
  return __soma_generic (c, SOMA_STR_DEFAULT_SPOT);
}

/* EOF */
