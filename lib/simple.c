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

char *
soma_status (soma_controller * c)
{
  struct __soma_socket_client *data;
  int ret;
  char buff[SIZE_BUFFER];
  int i;
  char *what;

  if (!c)
    return NULL;

  if ((ret = __soma_socket (c, &data)) != SOMA_ERR_OK)
    RETURN_VALUE (c, ret, NULL);

  if (__soma_send_command (c, data, SOMA_STR_STATUS))
    {
      __soma_close (c, data);
      RETURN_VALUE (c, SOMA_ERR_PROTOCOL, NULL);
    }

  i = 0;
  while (i < sizeof (buff)
	 && (ret = __soma_read (c, data, buff + i, sizeof (buff) - i)) > 0)
    i += ret;

  buff[i] = 0;

  if (i != 0)
    what = strdup (buff);
  else
    what = NULL;

  __soma_close (c, data);

  RETURN_VALUE (c, SOMA_ERR_OK, what);
}

time_t
soma_time (soma_controller * c)
{
  struct __soma_socket_client *data;
  int ret;
  char buff[SIZE_BUFFER];
  int i;
  time_t t;

  if (!c)
    return 0;

  if ((ret = __soma_socket (c, &data)) != SOMA_ERR_OK)
    RETURN_VALUE (c, ret, 0);

  if (__soma_send_command (c, data, SOMA_STR_TIME))
    {
      __soma_close (c, data);
      RETURN_VALUE (c, SOMA_ERR_PROTOCOL, 0);
    }

  i = 0;
  while (i < sizeof (buff)
	 && (ret = __soma_read (c, data, buff + i, sizeof (buff) - i)) > 0)
    i += ret;

  buff[i] = 0;

  t = atoi (buff);

  __soma_close (c, data);
  RETURN_VALUE (c, SOMA_ERR_OK, t);
}

time_t
soma_get_time_play (soma_controller * c)
{
  struct __soma_socket_client *data;
  int ret;
  char buff[SIZE_BUFFER];
  int i;
  time_t t;

  if (!c)
    return 0;

  if ((ret = __soma_socket (c, &data)) != SOMA_ERR_OK)
    RETURN_VALUE (c, ret, 0);

  if (__soma_send_command (c, data, SOMA_STR_GET_TIME_PLAY))
    {
      __soma_close (c, data);
      RETURN_VALUE (c, SOMA_ERR_PROTOCOL, 0);
    }

  i = 0;
  while (i < sizeof (buff)
	 && (ret = __soma_read (c, data, buff + i, sizeof (buff) - i)) > 0)
    i += ret;

  buff[i] = 0;

  t = atoi (buff);

  __soma_close (c, data);
  RETURN_VALUE (c, SOMA_ERR_OK, t);
}

int
soma_check_password (soma_controller * c)
{
  struct __soma_socket_client *data;
  int ret;

  if (!c)
    return 1;

  if ((ret = __soma_socket (c, &data)) != SOMA_ERR_OK)
    RETURN_VALUE (c, ret, 1);

  __soma_close (c, data);
  RETURN_VALUE (c, SOMA_ERR_OK, 0);
}

int
soma_quit (soma_controller * c)
{
  return __soma_generic (c, SOMA_STR_QUIT);
}

int
soma_skip (soma_controller * c)
{
  return __soma_generic (c, SOMA_STR_SKIP);
}

int
soma_skip_next (soma_controller * c)
{
  return __soma_generic (c, SOMA_STR_SKIP_FOLLOW_ITEM);
}

int
soma_stop (soma_controller * c, int time)
{
  struct __soma_socket_client *data;
  int ret;
  char buff[SIZE_BUFFER];

  if (!c)
    return 1;

  if ((ret = __soma_socket (c, &data)) != SOMA_ERR_OK)
    RETURN_VALUE (c, ret, 1);

  if (__soma_send_command (c, data, SOMA_STR_STOP))
    {
      __soma_close (c, data);
      RETURN_VALUE (c, SOMA_ERR_PROTOCOL, 1);
    }
  snprintf (buff, sizeof (buff), "%d\n", time);

  if (__soma_send_command (c, data, buff))
    {
      __soma_close (c, data);
      RETURN_VALUE (c, SOMA_ERR_PROTOCOL, 1);
    }
  __soma_close (c, data);
  RETURN_VALUE (c, SOMA_ERR_OK, 0);
}

int
soma_start (soma_controller * c)
{
  return __soma_generic (c, SOMA_STR_START);
}

int
soma_running (soma_controller * c)
{
  struct __soma_socket_client *data;
  int ret;

  if (!c)
    return 1;

  if ((ret = __soma_socket (c, &data)) != SOMA_ERR_OK)
    RETURN_VALUE (c, ret, 1);

  switch (__soma_send_command (c, data, SOMA_STR_RUNNING))
    {
    case -1:
      __soma_close (c, data);
      RETURN_VALUE (c, SOMA_ERR_PROTOCOL, 1);

    case 0:
      __soma_close (c, data);
      RETURN_VALUE (c, SOMA_ERR_OK, 0);

    default:
      __soma_close (c, data);
      RETURN_VALUE (c, SOMA_ERR_OK, 1);
    }
}

int
soma_error (soma_controller * c)
{
  if (!c)
    return SOMA_ERR_USER;
  return c->error;
}

char *
soma_get_item (soma_controller * c)
{
  struct __soma_socket_client *data;
  int ret;
  char buff[SIZE_BUFFER];
  int i;
  char *what;

  if (!c)
    return NULL;

  if ((ret = __soma_socket (c, &data)) != SOMA_ERR_OK)
    RETURN_VALUE (c, ret, NULL);

  if (__soma_send_command (c, data, SOMA_STR_GET_ITEM))
    {
      __soma_close (c, data);
      RETURN_VALUE (c, SOMA_ERR_PROTOCOL, NULL);
    }

  i = 0;
  while (i < sizeof (buff)
	 && (ret = __soma_read (c, data, buff + i, sizeof (buff) - i)) > 0)
    i += ret;

  buff[i] = 0;

  if (i != 0)
    what = strdup (buff);
  else
    what = NULL;

  __soma_close (c, data);

  RETURN_VALUE (c, SOMA_ERR_OK, what);
}

char *
soma_get_item_next (soma_controller * c)
{
  struct __soma_socket_client *data;
  int ret;
  char buff[SIZE_BUFFER];
  int i;
  char *what;

  if (!c)
    return NULL;

  if ((ret = __soma_socket (c, &data)) != SOMA_ERR_OK)
    RETURN_VALUE (c, ret, NULL);

  if (__soma_send_command (c, data, SOMA_STR_GET_FOLLOW_ITEM))
    {
      __soma_close (c, data);
      RETURN_VALUE (c, SOMA_ERR_PROTOCOL, NULL);
    }

  i = 0;
  while (i < sizeof (buff)
	 && (ret = __soma_read (c, data, buff + i, sizeof (buff) - i)) > 0)
    i += ret;

  buff[i] = 0;

  if (i != 0)
    what = strdup (buff);
  else
    what = NULL;

  __soma_close (c, data);

  RETURN_VALUE (c, SOMA_ERR_OK, what);
}

int
soma_read_palinsesto (soma_controller * c)
{
  return __soma_generic (c, SOMA_STR_READ_PL);
}

int
soma_read_spot (soma_controller * c)
{
  return __soma_generic (c, SOMA_STR_READ_SPOT);
}

int
soma_read_directory (soma_controller * c)
{
  return __soma_generic (c, SOMA_STR_READ_DIR);
}

char *
soma_palinsesto_name (soma_controller * c)
{
  struct __soma_socket_client *data;
  int ret;
  char buff[SIZE_BUFFER];
  int i;
  char *what;

  if (!c)
    return NULL;

  if ((ret = __soma_socket (c, &data)) != SOMA_ERR_OK)
    RETURN_VALUE (c, ret, NULL);

  if (__soma_send_command (c, data, SOMA_STR_GET_PL_NAME))
    {
      __soma_close (c, data);
      RETURN_VALUE (c, SOMA_ERR_PROTOCOL, NULL);
    }

  i = 0;
  while (i < sizeof (buff)
	 && (ret = __soma_read (c, data, buff + i, sizeof (buff) - i)) > 0)
    i += ret;

  buff[i] = 0;

  if (i != 0)
    what = strdup (buff);
  else
    what = NULL;

  __soma_close (c, data);

  RETURN_VALUE (c, SOMA_ERR_OK, what);
}

int
soma_remove_item (soma_controller * c, int id)
{
  struct __soma_socket_client *data;
  int ret;
  char buff[SIZE_BUFFER];

  if (!c)
    return 1;

  if ((ret = __soma_socket (c, &data)) != SOMA_ERR_OK)
    RETURN_VALUE (c, ret, 1);

  if (__soma_send_command (c, data, SOMA_STR_REMOVE_ITEM))
    {
      __soma_close (c, data);
      RETURN_VALUE (c, SOMA_ERR_PROTOCOL, 1);
    }

  snprintf (buff, sizeof (buff), "%d", id);

  __soma_write (c, data, buff, strlen (buff));

  __soma_close (c, data);
  RETURN_VALUE (c, SOMA_ERR_OK, 0);
}

int
soma_remove_spot (soma_controller * c, int id)
{
  struct __soma_socket_client *data;
  int ret;
  char buff[SIZE_BUFFER];

  if (!c)
    return 1;

  if ((ret = __soma_socket (c, &data)) != SOMA_ERR_OK)
    RETURN_VALUE (c, ret, 1);

  if (__soma_send_command (c, data, SOMA_STR_REMOVE_SPOT))
    {
      __soma_close (c, data);
      RETURN_VALUE (c, SOMA_ERR_PROTOCOL, 1);
    }

  snprintf (buff, sizeof (buff), "%d", id);

  __soma_write (c, data, buff, strlen (buff));

  __soma_close (c, data);
  RETURN_VALUE (c, SOMA_ERR_OK, 0);
}

int
soma_set_pause (soma_controller * c)
{
  return __soma_generic (c, SOMA_STR_PAUSE);
}

int
soma_set_unpause (soma_controller * c)
{
  return __soma_generic (c, SOMA_STR_UNPAUSE);
}

int
soma_get_pause (soma_controller * c, int *p)
{
  struct __soma_socket_client *data;
  int ret;
  char buff[SIZE_BUFFER];
  int i;

  if (!c || !p)
    return 0;

  if ((ret = __soma_socket (c, &data)) != SOMA_ERR_OK)
    RETURN_VALUE (c, ret, 0);

  if (__soma_send_command (c, data, SOMA_STR_GET_PAUSE))
    {
      __soma_close (c, data);
      RETURN_VALUE (c, SOMA_ERR_PROTOCOL, 0);
    }

  i = 0;
  while (i < sizeof (buff)
	 && (ret = __soma_read (c, data, buff + i, sizeof (buff) - i)) > 0)
    i += ret;

  buff[i] = 0;

  if (buff[0] == '0')
    *p = 0;
  else
    *p = 1;

  __soma_close (c, data);
  RETURN_VALUE (c, SOMA_ERR_OK, 0);
}

int
soma_get_stop (soma_controller * c, int *p)
{
  struct __soma_socket_client *data;
  int ret;
  char buff[SIZE_BUFFER];
  int i;

  if (!c || !p)
    return 0;

  if ((ret = __soma_socket (c, &data)) != SOMA_ERR_OK)
    RETURN_VALUE (c, ret, 0);

  if (__soma_send_command (c, data, SOMA_STR_GET_STOP))
    {
      __soma_close (c, data);
      RETURN_VALUE (c, SOMA_ERR_PROTOCOL, 0);
    }

  i = 0;
  while (i < sizeof (buff)
	 && (ret = __soma_read (c, data, buff + i, sizeof (buff) - i)) > 0)
    i += ret;

  buff[i] = 0;

  if (buff[0] == '0')
    *p = 0;
  else
    *p = 1;

  __soma_close (c, data);
  RETURN_VALUE (c, SOMA_ERR_OK, 0);
}

/* EOF */
