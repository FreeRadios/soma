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

#ifndef __LIB_SOMA_H__
#define __LIB_SOMA_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/param.h>

#ifndef WIN32
#  include <sys/socket.h>
#  include <sys/un.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#  include <netdb.h>
#else
#  include <winsock.h>
#endif

#include "../include/code.h"
#include "../include/commons.h"
#include "../include/controller.h"
#include "../include/code.h"

#ifdef ENABLE_FFMPEG
#  include <ffmpeg/avcodec.h>
#  include <ffmpeg/avformat.h>
#else
#  include "avcodec.h"
#  include "avformat.h"
#endif

#define SIZE_BUFFER 1024

#define RETURN_VALUE(c, err, value) { \
  ((soma_controller *) c )->error= err ; \
  return value; \
}

#ifdef WIN32
#  define lstat stat
#  ifndef SHUT_RDWR
#    define SHUT_RDWR SD_BOTH
#  endif
#endif

struct __soma_socket_client
{
#ifdef SOMA_USE_OPENSSL
  SSL_CTX *ctx;
  SSL *ssl;
  SSL_METHOD *meth;
  X509 *server_cert;
#endif

  char buffer[SIZE_BUFFER];
  int fill;

  int fd;
};

int	__soma_get_line		(soma_controller * c,
				 struct __soma_socket_client *data,
				 char *buf);

int	__soma_get_line_fd	(int fd,
				 char *buf);

int	__soma_send_command	(soma_controller * c,
				 struct __soma_socket_client *data,
				 char *cmd);

size_t	__soma_write		(soma_controller * c,
				 struct __soma_socket_client *data,
				 char *buf,
				 size_t size);

size_t	__soma_read		(soma_controller * c,
				 struct __soma_socket_client *data,
				 char *buf,
				 size_t size);

void	__soma_close		(soma_controller * c,
				 struct __soma_socket_client *data);

int	__soma_socket		(soma_controller * c,
				 struct __soma_socket_client **);

int	__soma_generic		(soma_controller * c,
				 char *buf);

void	__soma_generic_list_free (char **);

#endif

/* EOF */
