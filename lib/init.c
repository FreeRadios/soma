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

soma_controller *
soma_open_tcp (char *server, int port, char *password, int ssl)
{
  struct __soma_socket_client *data;
  soma_controller *c;
  int ret;

  if (!(c = (soma_controller *) malloc (sizeof (soma_controller))))
    return NULL;
  memset (c, 0, sizeof (soma_controller));

  if (!server)
    c->server = strdup ("localhost");

  else
    c->server = strdup (server);

  if (!c->server)
    {
      free (c);
      return NULL;
    }
  if (!port)
    c->port = SOMA_PORT;
  else
    c->port = port;

  if (!password)
    c->password = strdup ("");
  else
    c->password = strdup (password);

  if (!c->password)
    {
      free (c->server);
      free (c);
      return NULL;
    }
  c->unixsocket = 0;
#ifdef SOMA_USE_OPENSSL
  c->activated_ssl = ssl;
#endif

  if ((ret = __soma_socket (c, &data)) != SOMA_ERR_OK)
    RETURN_VALUE (c, ret, c);

  __soma_close (c, data);

  RETURN_VALUE (c, SOMA_ERR_OK, c);
}

soma_controller *
soma_open_unix (char *unixpath, char *password, int ssl)
{
  struct __soma_socket_client *data;
  soma_controller *c;
  int ret;

  if (!(c = (soma_controller *) malloc (sizeof (soma_controller))))
    return NULL;
  memset (c, 0, sizeof (soma_controller));

  if (!unixpath)
    c->unixpath = strdup (SOMA_UNIX_SOCK);

  else
    c->unixpath = strdup (unixpath);

  if (!c->unixpath)
    {
      free (c);
      return NULL;
    }

  if (!password)
    c->password = strdup ("");

  else
    c->password = strdup (password);

  if (!c->password)
    {
      free (c->unixpath);
      free (c);
      return NULL;
    }

  c->unixsocket = 1;
#ifdef SOMA_USE_OPENSSL
  c->activated_ssl = ssl;
#endif

  if ((ret = __soma_socket (c, &data)) != SOMA_ERR_OK)
    RETURN_VALUE (c, ret, c);

  __soma_close (c, data);

  RETURN_VALUE (c, SOMA_ERR_OK, c);
}

int
soma_free (soma_controller * c)
{
  if (!c)
    return 1;

  if (c->unixpath)
    free (c->unixpath);

  if (c->server)
    free (c->server);

  if (c->password)
    free (c->password);

  free (c);

  return 0;
}

/* EOF */
