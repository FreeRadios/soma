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

#include "libsoma.h"

int
__soma_socket (soma_controller * c, struct __soma_socket_client **data)
{
  char buff[SIZE_BUFFER];
  int cmd, i;
  char *server = NULL;
  char *version = NULL;
  char *ssl = NULL;

#ifdef SOMA_USE_OPENSSL
  static int ssl_started = 0;
#endif

  if (!
      (*data =
       (struct __soma_socket_client *)
       malloc (sizeof (struct __soma_socket_client))))
    return SOMA_ERR_POSIX;

  memset (*data, 0, sizeof (struct __soma_socket_client));

#ifdef SOMA_USE_OPENSSL
  if (c->activated_ssl)
    {
      if (!ssl_started)
	{
	  SSL_library_init ();
	  ssl_started = 1;
	}

      (*data)->meth = SSLv23_client_method ();

      if (!((*data)->ctx = SSL_CTX_new ((*data)->meth)))
	{
	  free (*data);
	  return SOMA_ERR_POSIX;
	}
    }
#endif

  if (!c->unixsocket)
    {
      struct sockaddr_in sock;
      struct hostent *hp;

#ifdef WIN32
      WSADATA wsaData;
      WSAStartup (MAKEWORD (2, 0), &wsaData);
#endif

      if (((*data)->fd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
	{
#ifdef SOMA_USE_OPENSSL
	  if ((*data)->ctx)
	    SSL_CTX_free ((*data)->ctx);
#endif
	  free (*data);
	  return SOMA_ERR_POSIX;
	}

      if (!(hp = gethostbyname (c->server)))
	{
	  close ((*data)->fd);
#ifdef SOMA_USE_OPENSSL
	  if ((*data)->ctx)
	    SSL_CTX_free ((*data)->ctx);
#endif
	  free (*data);
	  return SOMA_ERR_HOST;
	}

      memset ((void *) &sock, 0, sizeof (sock));
      sock.sin_family = AF_INET;
      sock.sin_addr.s_addr = ((struct in_addr *) (hp->h_addr))->s_addr;
      sock.sin_port = htons (c->port);

      if (connect ((*data)->fd, (struct sockaddr *) &sock, sizeof (sock)) < 0)
	{
	  close ((*data)->fd);
#ifdef SOMA_USE_OPENSSL
	  if ((*data)->ctx)
	    SSL_CTX_free ((*data)->ctx);
#endif
	  free (*data);
	  return SOMA_ERR_CONNECT;
	}
    }

#ifndef WIN32
  else
    {
      struct sockaddr_un saddr;

      if (((*data)->fd = socket (AF_UNIX, SOCK_STREAM, 0)) < 0)
	{
#ifdef SOMA_USE_OPENSSL
	  if ((*data)->ctx)
	    SSL_CTX_free ((*data)->ctx);
#endif
	  free (*data);
	  return SOMA_ERR_POSIX;
	}

      saddr.sun_family = AF_UNIX;
      strncpy (saddr.sun_path, c->unixpath, sizeof (saddr.sun_path));

      if (connect ((*data)->fd, (struct sockaddr *) &saddr, sizeof (saddr)) <
	  0)
	{
	  close ((*data)->fd);
#ifdef SOMA_USE_OPENSSL
	  if ((*data)->ctx)
	    SSL_CTX_free ((*data)->ctx);
#endif
	  free (*data);
	  return SOMA_ERR_CONNECT;
	}
    }
#endif

  /* Header admin: */
  if (__soma_get_line_fd ((*data)->fd, buff))
    {
      close ((*data)->fd);
#ifdef SOMA_USE_OPENSSL
      if ((*data)->ctx)
	SSL_CTX_free ((*data)->ctx);
#endif
      free (*data);
      return SOMA_ERR_PROTOCOL;
    }

  cmd = strlen (buff);

  server = buff;
  for (i = 0; i < cmd; i++)
    if (buff[i] == ' ')
      {
	buff[i] = 0;
	break;
      }

  if (i < cmd)
    {
      version = buff + i + 1;
      for (i++; i < cmd; i++)
	if (buff[i] == ' ')
	  {
	    buff[i] = 0;
	    break;
	  }
      if (i < cmd)
	ssl = buff + i + 1;
    }

#ifdef SOMA_USE_OPENSSL
  if (c->activated_ssl)
    {
      if (ssl && !strcmp (ssl, "NO_SSL"))
	{
	  close ((*data)->fd);
	  if ((*data)->ctx)
	    SSL_CTX_free ((*data)->ctx);
	  free (*data);
	  return SOMA_ERR_NO_SSL_REQUEST;
	}
    }

  else if (!c->activated_ssl)
    {
      if (ssl && !strcmp (ssl, "SSL"))
	{
	  close ((*data)->fd);
	  if ((*data)->ctx)
	    SSL_CTX_free ((*data)->ctx);
	  free (*data);
	  return SOMA_ERR_SSL_REQUEST;
	}
    }

#else
  if (ssl && !strcmp (ssl, "SSL"))
    {
      close ((*data)->fd);
      free (*data);
      return SOMA_ERR_SSL_REQUEST;
    }
#endif

  /* Ok! */
  if (send ((*data)->fd, SOMA_STR_OK, strlen (SOMA_STR_OK), 0) !=
      strlen (SOMA_STR_OK))
    {
      close ((*data)->fd);
#ifdef SOMA_USE_OPENSSL
      if ((*data)->ctx)
	SSL_CTX_free ((*data)->ctx);
#endif
      free (*data);
      return SOMA_ERR_PROTOCOL;
    }

#ifdef SOMA_USE_OPENSSL
  if (c->activated_ssl)
    {
      if (!((*data)->ssl = SSL_new ((*data)->ctx)))
	{
	  close ((*data)->fd);
	  SSL_CTX_free ((*data)->ctx);
	  free (*data);
	  return SOMA_ERR_SSL;
	}

      SSL_set_fd ((*data)->ssl, (*data)->fd);

      if (SSL_connect ((*data)->ssl) <= 0)
	{
	  close ((*data)->fd);
	  SSL_free ((*data)->ssl);
	  SSL_CTX_free ((*data)->ctx);
	  shutdown ((*data)->fd, SHUT_RDWR);
	  close ((*data)->fd);
	  free (*data);
	  return SOMA_ERR_SSL;
	}
    }
#endif

  if (__soma_get_line (c, *data, buff))
    {
      __soma_close (c, (*data));
      return SOMA_ERR_PROTOCOL;
    }

  buff[3] = 0;
  cmd = atoi (buff);

  if (cmd != SOMA_CODE_OK)
    {
      __soma_close (c, *data);
      return SOMA_ERR_PROTOCOL;
    }

  /* Check password */
  if (__soma_write (c, *data, c->password, strlen (c->password)) !=
      strlen (c->password))
    {
      __soma_close (c, *data);
      return SOMA_ERR_PROTOCOL;
    }

  if (__soma_write (c, *data, "\n", 1) != 1)
    {
      __soma_close (c, *data);
      return SOMA_ERR_PROTOCOL;
    }

  if (__soma_get_line (c, *data, buff))
    {
      __soma_close (c, *data);
      return SOMA_ERR_PROTOCOL;
    }

  buff[3] = 0;
  cmd = atoi (buff);

  if (cmd != SOMA_CODE_OK)
    {
      __soma_close (c, *data);
      return SOMA_ERR_PASSWORD;
    }

  return SOMA_ERR_OK;
}

int
__soma_get_line_fd (int fd, char *buf)
{
  int i, len;

  if ((len = recv (fd, buf, SIZE_BUFFER, 0)) <= 0)
    return 1;

  for (i = 0; i < len; i++)
    {
      if (buf[i] == '\n')
	{
	  buf[i] = 0;
	  return 0;
	}
    }

  return 0;
}

int
__soma_get_line (soma_controller * client, struct __soma_socket_client *c,
		 char *buf)
{
  int i = 0;

  while (c->fill <= SIZE_BUFFER)
    {
      for (i = 0; i < c->fill; i++)
	{
	  if (*(c->buffer + i) == '\n')
	    {
	      memcpy (buf, c->buffer, i);

	      c->fill -= i + 1;
	      memmove (c->buffer, c->buffer + i + 1, c->fill);
	      *(buf + i) = 0;

	      return 0;
	    }
	}

#ifdef SOMA_USE_OPENSSL
      if (!client->activated_ssl)
	{
#endif
	  if ((i =
	       recv (c->fd, c->buffer + c->fill, SIZE_BUFFER - c->fill,
		     0)) <= 0)
	    return 1;
#ifdef SOMA_USE_OPENSSL
	}
      else
	{
	  if ((i =
	       SSL_read (c->ssl, c->buffer + c->fill,
			 SIZE_BUFFER - c->fill)) <= 0)
	    return 1;
	}
#endif

      c->fill += i;
    }

  memcpy (buf, c->buffer, SIZE_BUFFER);
  c->fill = 0;

  return 0;
}

int
__soma_send_command (soma_controller * c, struct __soma_socket_client *data,
		     char *cmd)
{
  char buff[SIZE_BUFFER];

  __soma_write (c, data, cmd, strlen (cmd));

  if (__soma_get_line (c, data, buff))
    return -1;

  buff[3] = 0;

  if (atoi (buff) == SOMA_CODE_OK)
    return 0;

  return atoi (buff);
}

size_t
__soma_write (soma_controller * c, struct __soma_socket_client * data,
	      char *buf, size_t size)
{
#ifdef SOMA_USE_OPENSSL
  if (c->activated_ssl)
    return SSL_write (data->ssl, buf, size);
#endif
  return send (data->fd, buf, size, 0);
}

size_t
__soma_read (soma_controller * c, struct __soma_socket_client * data,
	     char *buf, size_t size)
{
  size_t done = 0;

  if (data->fill != 0)
    {
      if (data->fill < size)
	{
	  memcpy (buf, data->buffer, data->fill);
	  done = data->fill;
	  size -= data->fill;
	  data->fill = 0;
	}
      else
	{
	  memcpy (buf, data->buffer, size);

	  data->fill -= size;
	  memmove (data->buffer, data->buffer + size, data->fill);
	  return size;
	}
    }

#ifdef SOMA_USE_OPENSSL
  if (c->activated_ssl)
    return SSL_read (data->ssl, buf + done, size) + done;
#endif
  return recv (data->fd, buf + done, size, 0) + done;
}

void
__soma_close (soma_controller * c, struct __soma_socket_client *data)
{
#ifdef SOMA_USE_OPENSSL
  if (c->activated_ssl)
    SSL_shutdown (data->ssl);
#endif

  shutdown (data->fd, SHUT_RDWR);
  close (data->fd);

#ifdef SOMA_USE_OPENSSL
  if (c->activated_ssl)
    {
      SSL_free (data->ssl);
      SSL_CTX_free (data->ctx);
    }
#endif

  free (data);
}

int
__soma_generic (soma_controller * c, char *str)
{
  int ret;
  struct __soma_socket_client *data;

  if (!c)
    return 1;

  if ((ret = __soma_socket (c, &data)) != SOMA_ERR_OK)
    RETURN_VALUE (c, ret, 1);

  if (__soma_send_command (c, data, str))
    {
      __soma_close (c, data);
      RETURN_VALUE (c, SOMA_ERR_PROTOCOL, 1);
    }

  __soma_close (c, data);
  RETURN_VALUE (c, SOMA_ERR_OK, 0);
}

void
__soma_generic_list_free (char **output)
{
  char **old;

  if (!output)
    return;

  old = output;

  while (*output)
    {
      free (*output);
      output++;
    }

  free (old);
}

/* EOF */
