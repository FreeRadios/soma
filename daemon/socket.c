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

struct socket_client
{
  pthread_t th;
#ifdef SOMA_USE_OPENSSL
  SSL *ssl;
#endif
  int fd;

  int flag;
} socket_client;

#ifdef SOMA_USE_OPENSSL
static SSL_CTX *ctx;
static SSL_METHOD *meth;
#endif

static int get_line (struct socket_client *c, char *buf, size_t size);
static int get_line_fd (int fd, char *buf, size_t size);
static void socket_new_client (int fd);
static void *soma_socket_thread (void *);
static size_t soma_write (struct socket_client *c, char *buf, size_t size);
static size_t soma_read (struct socket_client *c, char *buf, size_t size);

static size_t
soma_write (struct socket_client *c, char *buf, size_t size)
{
#ifdef SOMA_USE_OPENSSL
  if (conf->Ssl)
    return SSL_write (c->ssl, buf, size);
#endif
  return write (c->fd, buf, size);
}

static size_t
soma_read (struct socket_client *c, char *buf, size_t size)
{
#ifdef SOMA_USE_OPENSSL
  if (conf->Ssl)
    return SSL_read (c->ssl, buf, size);
#endif
  return read (c->fd, buf, size);
}

void
soma_close (struct socket_client *c)
{
  SOMA_NO_LOG_ALL ("Connection closed.");

  shutdown (c->fd, SHUT_RDWR);
  close (c->fd);

#ifdef SOMA_USE_OPENSSL
  if (conf->Ssl)
    SSL_free (c->ssl);
#endif
}

#ifdef SOMA_USE_OPENSSL
static void
soma_certificate (void)
{
  SSL_load_error_strings ();
  SSL_library_init ();

  meth = SSLv23_server_method ();
  if (!(ctx = SSL_CTX_new (meth)))
    SOMA_FATAL ("SSL CTX error.");

  if (SSL_CTX_use_certificate_file (ctx, conf->Certificate, SSL_FILETYPE_PEM)
      < 1)
    SOMA_FATAL ("SSL CTX certificate error.");

  if (SSL_CTX_use_PrivateKey_file (ctx, conf->PrivateKey, SSL_FILETYPE_PEM) <
      1)
    SOMA_FATAL ("SSL CTX privatekey error.");

  if (!SSL_CTX_check_private_key (ctx))
    SOMA_FATAL
      ("SSL CTX privatekey does not math the certificate public key.");
}
#endif

static int
soma_socket_create (void)
{
  int fd, yes = 1;
  struct sockaddr_in sock;
  struct hostent *hp;
  struct sockaddr_un saddr;

  if (!conf->UnixSocket)
    {
      if ((fd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
	SOMA_FATAL ("Sock error.");

      if (setsockopt (fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (int)))
	SOMA_FATAL ("Sock error.");

      memset ((void *) &sock, 0, sizeof (sock));
      sock.sin_family = AF_INET;
      sock.sin_port = htons (conf->Port);

      if (!conf->ServerName)
	sock.sin_addr.s_addr = htonl (INADDR_ANY);

      else
	{
	  if (!(hp = gethostbyname (conf->ServerName)))
	    SOMA_FATAL ("Unknow %s", conf->ServerName);

	  sock.sin_family = hp->h_addrtype;
	  memcpy ((caddr_t) & sock.sin_addr, hp->h_addr, hp->h_length);
	}

      if (bind (fd, (struct sockaddr *) &sock, sizeof (sock)) < 0)
	SOMA_FATAL ("Binding error.");

    }
  else
    {
      char path[sizeof (saddr.sun_path)];
      struct stat st;

      if ((fd = socket (AF_UNIX, SOCK_STREAM, 0)) < 0)
	SOMA_FATAL ("Sock error.");

      saddr.sun_family = AF_UNIX;

      if (conf->UnixPath)
	strncpy (path, conf->UnixPath, sizeof (path));
      else
	strncpy (path, SOMA_UNIX_SOCK, sizeof (path));

      if (!lstat (path, &st))
	{
	  unlink (path);

	  if (!lstat (path, &st))
	    SOMA_FATAL ("Error opening %s socket", path);
	}

      strncpy (saddr.sun_path, path, 108);

      if (bind (fd, (struct sockaddr *) &saddr, sizeof (saddr)) < 0)
	SOMA_FATAL ("Binding error.");
    }

  if (listen (fd, conf->Listen) < 0)
    SOMA_FATAL ("Listen error.");

  return fd;
}

/* This func is a separated thread that creates the socket interface for
 * admin */
void *
soma_socket (void *arg)
{
  int fd;

  if (conf->Background)
    signal_off ();
  else
    signal_small_off ();

#ifdef SOMA_USE_OPENSSL
  if (conf->Ssl)
    soma_certificate ();
#endif

  fd = soma_socket_create ();

  /* The SOMA protocol 
   * 
   *    SERVER                          CLIENT
   *                            <-      connection
   *    soma_ok                 ->
   *                            <-      password
   *    soma_ok or soma_err     ->
   *                            <-      (if soma_ok) comand
   *    soma_ok or soma_err     ->
   *                            ....    
   *                                    exit
   */

  /* Infinite loop */
  while (!set_quit)
    {
      int c_fd;
      struct timeval tv;
      fd_set fd_read;

      FD_ZERO (&fd_read);
      FD_SET (fd, &fd_read);

      tv.tv_sec = 0;
      tv.tv_usec = 100000;

      if (select (fd + 1, &fd_read, NULL, NULL, &tv) < 1)
	continue;

      if (conf->UnixSocket)
	{
	  if ((c_fd = accept (fd, 0, 0)) < 0)
	    {
	      SOMA_LOG_ERR ("Accept error.");
	      continue;
	    }

	  SOMA_NO_LOG_ALL ("New request accepted");
	}

      else
	{
	  struct sockaddr_in incoming;
	  size_t size_in = sizeof (struct sockaddr_in);
	  int port, addr;

	  if ((c_fd =
	       accept (fd, (struct sockaddr *) &incoming, &size_in)) < 0)
	    {
	      SOMA_LOG_ERR ("Accept error.");
	      continue;
	    }

	  port = ntohs (incoming.sin_port);
	  addr = ntohl (incoming.sin_addr.s_addr);

	  if (denyallow_parser (addr))
	    {
	      close (c_fd);
	      continue;
	    }

	  SOMA_NO_LOG_ALL ("New client request (%d - %d.%d.%d.%d:%5d)", c_fd,
			   (unsigned int) addr >> 24,
			   (unsigned int) (addr >> 16) % 256,
			   (unsigned int) (addr >> 8) % 256,
			   (unsigned int) addr % 256, port);
	}

      socket_new_client (c_fd);
    }

  close (fd);

#ifdef SOMA_USE_OPENSSL
  if (conf->Ssl)
    {
      SSL_CTX_free (ctx);
    }
#endif

  if (conf->Background)
    signal_on ();
  else
    signal_small_on ();

  pthread_exit (arg);
  return NULL;
}

static void
socket_new_client (int fd)
{
  struct socket_client *cl;

  cl = (struct socket_client *) malloc (sizeof (struct socket_client));
  if (!cl)
    SOMA_FATAL ("Error memory.");

  memset (cl, 0, sizeof (struct socket_client));
  cl->fd = fd;

  if (pthread_create (&cl->th, NULL, soma_socket_thread, cl))
    SOMA_FATAL ("Threading error.");

  if (pthread_detach (cl->th))
    SOMA_FATAL ("Threading error.");
}

static void *
soma_socket_thread (void *dummy)
{
  struct socket_client *c = (struct socket_client *) dummy;
  char buf[SOMA_MAX_BUFF];
  int cmd, ret = 0;

  if (conf->Background)
    signal_off ();
  else
    signal_small_off ();

  /* Description: */
  cmd = snprintf (buf, sizeof (buf), "%s %s %s\n", PACKAGE, VERSION,
#ifdef SOMA_USE_OPENSSL
		  conf->Ssl ? "SSL" : "NO_SSL"
#else
		  "NO_SSL"
#endif
    );

  write (c->fd, buf, cmd);

  /* Get name and other info: */
  if (get_line_fd (c->fd, buf, sizeof (buf)))
    {
      SOMA_NO_LOG_ALL ("Connection closed.");
      close (c->fd);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  /* Check header */
  buf[3] = 0;
  cmd = atoi (buf);

  if (cmd != SOMA_CODE_OK)
    {
      SOMA_NO_LOG_ALL ("Connection closed.");
      close (c->fd);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

#ifdef SOMA_USE_OPENSSL
  if (conf->Ssl)
    {
      c->ssl = SSL_new (ctx);
      if (!c->ssl)
	{
	  SOMA_FATAL ("SSL create error.");
	}

      SSL_set_fd (c->ssl, c->fd);

      if (SSL_accept (c->ssl) <= 0)
	{
	  SOMA_NO_LOG_INFO ("SSL client error.");

	  shutdown (c->fd, SHUT_RDWR);
	  close (c->fd);

	  c->flag = 1;
	  free (c);
	  pthread_exit (NULL);
	  return 0;
	}

      SOMA_NO_LOG_ALL ("New connect with SSL: %s.", SSL_get_cipher (c->ssl));
    }
#endif

  /* 1 -> SOMA_WELCOME */
  soma_write (c, SOMA_STR_WELCOME, strlen (SOMA_STR_WELCOME));

  SOMA_NO_LOG_ALL ("Password checking...");

  /* Password check */
  if (get_line (c, buf, sizeof (buf)))
    {
      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  if (conf->Password)
    {
      int len = strlen (conf->Password);

      if (strncmp
	  (buf, conf->Password, len > SOMA_PASS_LEN ? SOMA_PASS_LEN : len))
	{
	  SOMA_NO_LOG_ALL ("Password error!");
	  soma_write (c, SOMA_STR_ERR_PASS, strlen (SOMA_STR_ERR_PASS));

	  soma_close (c);

	  c->flag = 1;
	  free (c);
	  pthread_exit (NULL);
	  return 0;
	}
    }
  else if (*buf != 0)
    {
      SOMA_NO_LOG_ALL ("Password error!");
      soma_write (c, SOMA_STR_ERR_PASS, strlen (SOMA_STR_ERR_PASS));

      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  /* Correct password */
  soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));

  SOMA_NO_LOG_ALL ("Getting Command...");
  /* Get the command */
  if (get_line (c, buf, sizeof (buf)))
    {
      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  /* Check commands */
  buf[3] = 0;
  cmd = atoi (buf);

  /* Reload the palinsesto file */
  if (cmd == SOMA_CODE_READ_PL)
    {
      SOMA_NO_LOG_ALL ("Cmd: Read Palinsesto");

      if (LOCK ())
	SOMA_FATAL ("lock error.");

      ret = build_palinsesto ();

      UNLOCK ();

      if (ret)
	soma_write (c, SOMA_STR_ERR, strlen (SOMA_STR_ERR));
      else
	soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));

      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  /* Set the init palinsesto */
  if (cmd == SOMA_CODE_OLD_PL)
    {
      SOMA_NO_LOG_ALL ("Cmd: Old Palinsesto");

      if (LOCK ())
	SOMA_FATAL ("lock error.");

      if (conf->Old_Palinsesto)
	{
	  free (conf->Palinsesto_fl);

	  if (!(conf->Palinsesto_fl = strdup (conf->Old_Palinsesto)))
	    SOMA_FATAL ("Error: memory.");

	  ret = build_palinsesto ();
	}

      UNLOCK ();

      if (ret)
	soma_write (c, SOMA_STR_ERR, strlen (SOMA_STR_ERR));
      else
	soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));

      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  /* Reload the spot file */
  if (cmd == SOMA_CODE_READ_SPOT)
    {
      SOMA_NO_LOG_ALL ("Cmd: Read Spot");

      if (LOCK ())
	SOMA_FATAL ("lock error.");

      ret = build_spot ();
      UNLOCK ();

      if (ret)
	soma_write (c, SOMA_STR_ERR, strlen (SOMA_STR_ERR));
      else
	soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));

      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  /* Set the init spot */
  if (cmd == SOMA_CODE_OLD_SPOT)
    {
      SOMA_NO_LOG_ALL ("Cmd: Old Spot");

      if (LOCK ())
	SOMA_FATAL ("lock error.");

      if (conf->Old_Spot)
	{
	  free (conf->Spot_fl);

	  if (!(conf->Spot_fl = strdup (conf->Old_Spot)))
	    SOMA_FATAL ("Error: memory.");

	  ret = build_spot ();
	}

      UNLOCK ();

      if (ret)
	soma_write (c, SOMA_STR_ERR, strlen (SOMA_STR_ERR));
      else
	soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));

      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  /* Reload the tree of directory */
  if (cmd == SOMA_CODE_READ_DIR)
    {
      SOMA_NO_LOG_ALL ("Cmd: Read Directories");

      /* No lock... */
      if (build_data ())
	soma_write (c, SOMA_STR_ERR, strlen (SOMA_STR_ERR));
      else
	soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));

      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  /* Check the password */
  if (cmd == SOMA_CODE_PASSWORD)
    {
      SOMA_NO_LOG_ALL ("Cmd: Check Password");
      soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));

      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  /* Get the status */
  if (cmd == SOMA_CODE_STATUS)
    {
      char *status;
      SOMA_NO_LOG_ALL ("Cmd: Status");

      soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));

      LOCK ();
      status = conf->Status ? strdup (conf->Status) : NULL;
      UNLOCK ();

      if (status)
	{
	  soma_write (c, status, strlen (status));
	  free (status);
	}

      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  /* Get the time */
  if (cmd == SOMA_CODE_TIME)
    {
      char s[SOMA_MAX_BUFF];
      int len;

      SOMA_NO_LOG_ALL ("Cmd: Time");

      soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));

      len = snprintf (s, sizeof (s), "%ld\n", time (NULL));
      soma_write (c, s, len);

      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  if (cmd == SOMA_CODE_NEXT_ITEM_GET_SPOT)
    {
      SOMA_NO_LOG_ALL ("Cmd: NextItem Get Spot");

      if (conf->nextitem_spot)
	soma_write (c, SOMA_STR_ERR, strlen (SOMA_STR_ERR));
      else
	soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));

      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  /* Next Item Set Mix */
  if (cmd == SOMA_CODE_NEXT_ITEM_SET_SPOT)
    {
      int k = 0;
      char fl_name[SOMA_MAX_BUFF];

      SOMA_NO_LOG_ALL ("Cmd: NextItem Set Spot");

      soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));

      while (soma_read (c, fl_name + k, 1) == 1)
	{
	  if (*(fl_name + k) == '\n')
	    break;

	  k++;
	  if (k == SOMA_MAX_BUFF - 1)
	    break;
	}

      *(fl_name + k) = 0;

      if (!strcmp (fl_name, "YES"))
	nextitem_set_mix (1);
      else
	nextitem_set_mix (0);

      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  /* Next Item Set */
  if (cmd == SOMA_CODE_NEXT_ITEM_SET)
    {
      int k = 0;
      char fl_name[SOMA_MAX_BUFF];

      SOMA_NO_LOG_ALL ("Cmd: NextItem Set");

      soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));

      while (soma_read (c, fl_name + k, 1) == 1)
	{
	  if (*(fl_name + k) == '\n')
	    break;

	  k++;
	  if (k == SOMA_MAX_BUFF - 1)
	    break;
	}

      *(fl_name + k) = 0;

      nextitem_add (fl_name);
      log_item_next_empty (0);

      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  /* Next Item Set Distribuited path */
  if (cmd == SOMA_CODE_NEXT_ITEM_PATH_SET)
    {
      int k = 0;
      char fl_name[SOMA_MAX_BUFF];

      SOMA_NO_LOG_ALL ("Cmd: NextItem Path Set");

      soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));

      while (soma_read (c, fl_name + k, 1) == 1)
	{
	  if (*(fl_name + k) == '\n')
	    break;

	  k++;
	  if (k == SOMA_MAX_BUFF - 1)
	    break;
	}

      *(fl_name + k) = 0;

      nextitem_path_add (fl_name);
      log_item_next_empty (0);

      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  if (cmd == SOMA_CODE_NEXT_ITEM_LIST)
    {
      struct nextitem *ns;
      string_t *str;

      SOMA_NO_LOG_ALL ("Cmd: NextItem List");

      soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));

      if (LOCK ())
	SOMA_FATAL ("Lock error.");

      str = string_new ();

      ns = conf->next_item;
      while (ns)
	{
	  string_add (str, ns->item, strlen (ns->item));
	  string_add (str, "\n", 1);
	  ns = ns->next;
	}

      UNLOCK ();

      if (str->size)
	soma_write (c, str->string, str->size);

      string_destroy (str);

      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  if (cmd == SOMA_CODE_NEXT_ITEM_REMOVE)
    {
      int k = 0;
      char cmd[SOMA_MAX_BUFF];

      SOMA_NO_LOG_ALL ("Cmd: NextItem Remove");

      soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));

      while (soma_read (c, cmd + k, 1) == 1)
	{
	  if (*(cmd + k) == '\n')
	    break;

	  k++;
	  if (k == SOMA_MAX_BUFF - 1)
	    break;
	}

      *(cmd + k) = 0;

      if (!strcasecmp (cmd, "last"))
	nextitem_remove_last ();
      else if (!strcasecmp (cmd, "first"))
	nextitem_remove_first ();
      else
	nextitem_remove_n (atoi (cmd));

      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  /* Path Request */
  if (cmd == SOMA_CODE_GET_PATH)
    {
      int k = 0;
      char cmd[SOMA_MAX_BUFF];

      SOMA_NO_LOG_ALL ("Cmd: Get Path");

      if (LOCK ())
	SOMA_FATAL ("Lock error.");

      if (!conf->DistribuitedPath)
	{
	  UNLOCK ();
	  soma_write (c, SOMA_STR_ERR_DATA, strlen (SOMA_STR_ERR_DATA));
	}

      else
	{
	  string_t *str;

	  UNLOCK ();

	  soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));

	  while (soma_read (c, cmd + k, 1) == 1)
	    {
	      if (*(cmd + k) == '\n')
		break;

	      k++;
	      if (k == SOMA_MAX_BUFF - 1)
		break;
	    }

	  *(cmd + k) = 0;

	  LOCK ();
	  str = distribuited_write (cmd);
	  UNLOCK ();

	  if (str && str->size)
	    soma_write (c, str->string, str->size);

	  string_destroy (str);
	}

      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  /* New palinsesto */
  if (cmd == SOMA_CODE_NEW_PL)
    {
      int fl;
      char *fl_name;
      char *fl_old;

      SOMA_NO_LOG_ALL ("Cmd: New Palinsesto");

      if (!(fl_name = soma_mktemp ()))
	soma_write (c, SOMA_STR_ERR, strlen (SOMA_STR_ERR));

      else if ((fl = open (fl_name, O_WRONLY)) < 0)
	{
	  free (fl_name);
	  soma_write (c, SOMA_STR_ERR, strlen (SOMA_STR_ERR));
	}

      else
	{
	  unsigned int k = 0;

	  /* Write the new palinsesto file */
	  soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));

	  while (soma_read (c, buf, 1) == 1)
	    {
	      write (fl, buf, 1);

	      k++;
	      if (k == 307200)
		break;
	    }

	  close (fl);
	  chmod (fl_name, 0640);

	  if (LOCK ())
	    SOMA_FATAL ("Lock error.");

	  fl_old = conf->Palinsesto_fl;
	  conf->Palinsesto_fl = fl_name;

	  if (build_palinsesto ())
	    {
	      conf->Palinsesto_fl = fl_old;
	      free (fl_name);
	    }

	  else
	    free (fl_old);

	  UNLOCK ();
	}

      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  /* Set current palinsesto as default */
  if (cmd == SOMA_CODE_DEFAULT_PL)
    {
      SOMA_NO_LOG_ALL ("Cmd: Set Default Palinsesto");

      if (LOCK ())
	SOMA_FATAL ("Lock error.");

      if (conf->Palinsesto_fl && conf->Old_Palinsesto
	  && !strcmp (conf->Palinsesto_fl, conf->Old_Palinsesto))
	{
	  UNLOCK ();
	  soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));
	}
      else
	{
	  int f1, f2;
	  char ch;

	  unlink (conf->Old_Palinsesto);

	  if ((f1 = open (conf->Old_Palinsesto, O_WRONLY | O_CREAT)) < 0)
	    {
	      UNLOCK ();

	      soma_write (c, SOMA_STR_ERR, strlen (SOMA_STR_ERR));
	      soma_close (c);

	      c->flag = 1;
	      free (c);
	      pthread_exit (NULL);
	      return 0;
	    }

	  if ((f2 = open (conf->Palinsesto_fl, O_RDONLY)) < 0)
	    {
	      UNLOCK ();

	      soma_write (c, SOMA_STR_ERR, strlen (SOMA_STR_ERR));
	      close (f1);

	      soma_close (c);

	      c->flag = 1;
	      free (c);
	      pthread_exit (NULL);
	      return 0;
	    }

	  while (read (f2, &ch, 1) == 1)
	    write (f1, &ch, 1);

	  close (f1);
	  close (f2);

	  chmod (conf->Old_Palinsesto, 0640);

	  UNLOCK ();
	  soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));

	  SOMA_NO_LOG_INFO ("somad set default palinsesto.");
	}

      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  /* Get the palinsesto */
  if (cmd == SOMA_CODE_GET_PL)
    {
      struct stat st;

      SOMA_NO_LOG_ALL ("Cmd: Get Palinsesto");

      if (LOCK ())
	SOMA_FATAL ("Lock error.");

      if (!conf->Palinsesto || lstat (conf->Palinsesto_fl, &st))
	{
	  UNLOCK ();
	  soma_write (c, SOMA_STR_ERR_DATA, strlen (SOMA_STR_ERR_DATA));
	}

      else
	{
	  int f;
	  char ch;

	  if ((f = open (conf->Palinsesto_fl, O_RDONLY)) < 0)
	    {
	      UNLOCK ();
	      soma_write (c, SOMA_STR_ERR, strlen (SOMA_STR_ERR));
	    }

	  else
	    {
	      string_t *str;

	      str = string_new ();

	      while (read (f, &ch, 1) == 1)
		string_add (str, &ch, 1);

	      close (f);

	      UNLOCK ();

	      soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));

	      if (str->size)
		soma_write (c, str->string, str->size);

	      string_destroy (str);
	    }
	}

      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  /* Get the old palinsesto */
  if (cmd == SOMA_CODE_GET_OLD_PL)
    {
      struct stat st;

      SOMA_NO_LOG_ALL ("Cmd: Get Old Palinsesto");

      if (LOCK ())
	SOMA_FATAL ("Lock error.");

      if (!conf->Old_Palinsesto || lstat (conf->Old_Palinsesto, &st))
	{
	  UNLOCK ();
	  soma_write (c, SOMA_STR_ERR_DATA, strlen (SOMA_STR_ERR_DATA));
	}

      else
	{
	  int f;
	  char ch;

	  if ((f = open (conf->Old_Palinsesto, O_RDONLY)) < 0)
	    {
	      UNLOCK ();
	      soma_write (c, SOMA_STR_ERR, strlen (SOMA_STR_ERR));
	    }

	  else
	    {
	      string_t *str;

	      str = string_new ();

	      while (read (f, &ch, 1) == 1)
		string_add (str, &ch, 1);

	      close (f);
	      UNLOCK ();

	      soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));

	      if (str->size)
		soma_write (c, str->string, str->size);

	      string_destroy (str);
	    }
	}

      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  /* New spot */
  if (cmd == SOMA_CODE_NEW_SPOT)
    {
      int fl;
      char *fl_name;
      char *fl_old;

      SOMA_NO_LOG_ALL ("Cmd: New Spot");

      if (!(fl_name = soma_mktemp ()))
	soma_write (c, SOMA_STR_ERR, strlen (SOMA_STR_ERR));

      else if ((fl = open (fl_name, O_WRONLY)) < 0)
	{
	  free (fl_name);
	  soma_write (c, SOMA_STR_ERR, strlen (SOMA_STR_ERR));
	}

      else
	{
	  unsigned int k = 0;

	  /* Write the new spot file */
	  soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));

	  while (soma_read (c, buf, 1) == 1)
	    {
	      write (fl, buf, 1);

	      k++;
	      if (k == 307200)
		break;
	    }

	  close (fl);
	  chmod (fl_name, 0640);

	  if (LOCK ())
	    SOMA_FATAL ("Lock error.");

	  fl_old = conf->Spot_fl;
	  conf->Spot_fl = fl_name;

	  if (build_spot ())
	    {
	      conf->Spot_fl = fl_old;
	      free (fl_name);
	    }

	  else
	    free (fl_old);

	  UNLOCK ();
	}

      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  /* Set current spot file as default */
  if (cmd == SOMA_CODE_DEFAULT_SPOT)
    {
      SOMA_NO_LOG_ALL ("Cmd: Set Default Spot");

      if (LOCK ())
	SOMA_FATAL ("Lock error.");

      if (conf->Spot_fl && conf->Old_Spot
	  && !strcmp (conf->Spot_fl, conf->Old_Spot))
	{
	  UNLOCK ();
	  soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));
	}
      else
	{
	  int f1, f2;
	  char ch;

	  unlink (conf->Old_Spot);

	  if ((f1 = open (conf->Old_Spot, O_WRONLY | O_CREAT)) < 0)
	    {
	      UNLOCK ();

	      soma_write (c, SOMA_STR_ERR, strlen (SOMA_STR_ERR));
	      soma_close (c);

	      c->flag = 1;
	      free (c);
	      pthread_exit (NULL);
	      return 0;
	    }

	  if ((f2 = open (conf->Spot_fl, O_RDONLY)) < 0)
	    {
	      UNLOCK ();

	      close (f1);
	      soma_write (c, SOMA_STR_ERR, strlen (SOMA_STR_ERR));
	      soma_close (c);

	      c->flag = 1;
	      free (c);
	      pthread_exit (NULL);
	      return 0;
	    }

	  while (read (f2, &ch, 1) == 1)
	    write (f1, &ch, 1);

	  close (f1);
	  close (f2);

	  chmod (conf->Old_Spot, 0640);

	  UNLOCK ();

	  soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));

	  SOMA_NO_LOG_INFO ("somad set default spot file.");
	}

      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  /* Get the spot file */
  if (cmd == SOMA_CODE_GET_SPOT)
    {
      struct stat st;

      SOMA_NO_LOG_ALL ("Cmd: Get Spot");

      if (LOCK ())
	SOMA_FATAL ("Lock error.");

      if (!conf->Spot_fl || lstat (conf->Spot_fl, &st))
	{
	  UNLOCK ();
	  soma_write (c, SOMA_STR_ERR_DATA, strlen (SOMA_STR_ERR_DATA));
	}

      else
	{
	  int f;
	  char ch;

	  if ((f = open (conf->Spot_fl, O_RDONLY)) < 0)
	    {
	      UNLOCK ();
	      soma_write (c, SOMA_STR_ERR, strlen (SOMA_STR_ERR));
	    }

	  else
	    {
	      string_t *str;

	      str = string_new ();

	      while (read (f, &ch, 1) == 1)
		string_add (str, &ch, 1);

	      close (f);

	      UNLOCK ();

	      soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));

	      if (str->size)
		soma_write (c, str->string, str->size);
	    }
	}

      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  /* Get the old palinsesto */
  if (cmd == SOMA_CODE_GET_OLD_SPOT)
    {
      struct stat st;

      SOMA_NO_LOG_ALL ("Cmd: Get Old Spot");

      if (LOCK ())
	SOMA_FATAL ("Lock error.");

      if (!conf->Old_Spot || lstat (conf->Old_Spot, &st))
	{
	  UNLOCK ();
	  soma_write (c, SOMA_STR_ERR_DATA, strlen (SOMA_STR_ERR_DATA));
	}

      else
	{
	  int f;
	  char ch;

	  if ((f = open (conf->Old_Spot, O_RDONLY)) < 0)
	    {
	      UNLOCK ();
	      soma_write (c, SOMA_STR_ERR, strlen (SOMA_STR_ERR));
	    }

	  else
	    {
	      string_t *str;

	      str = string_new ();

	      while (read (f, &ch, 1) == 1)
		string_add (str, &ch, 1);

	      close (f);
	      UNLOCK ();

	      soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));

	      if (str->size)
		soma_write (c, str->string, str->size);

	      string_destroy (str);
	    }
	}

      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  /* Exit */
  if (cmd == SOMA_CODE_QUIT)
    {
      SOMA_NO_LOG_ALL ("Cmd: Quit");

      set_quit = 1;

      soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));

      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  /* Set the Skip */
  if (cmd == SOMA_CODE_SKIP)
    {
      SOMA_NO_LOG_ALL ("Cmd: Skip.");

      if (!set_stop)
	set_skip = 1;

      soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));

      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  /* Set the Skip next */
  if (cmd == SOMA_CODE_SKIP_FOLLOW_ITEM)
    {
      SOMA_NO_LOG_ALL ("Cmd: Skip follow item.");

      soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));

      log_item_next_empty (1);
      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  /* Set the Start */
  if (cmd == SOMA_CODE_START)
    {
      SOMA_NO_LOG_ALL ("Cmd: Start.");

      if (set_stop)
	SOMA_LOG_INFO ("somad started.");

      set_stop = 0;
      id_item++;
      id_spot++;

      soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));

      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  /* Stop */
  if (cmd == SOMA_CODE_STOP)
    {
      int k = 0;
      char t[SOMA_MAX_BUFF];

      SOMA_NO_LOG_ALL ("Cmd: Stop.");

      if (!set_stop)
	SOMA_LOG_INFO ("somad stopped.");

      soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));

      while (soma_read (c, t + k, 1) == 1)
	{
	  if (*(t + k) == '\n')
	    break;

	  k++;
	  if (k == SOMA_MAX_BUFF - 1)
	    break;
	}

      *(t + k) = 0;

      set_stop = atoi (t);

      if (set_stop)
	soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));
      else
	soma_write (c, SOMA_STR_ERR, strlen (SOMA_STR_ERR));

      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  /* RUNNING */
  if (cmd == SOMA_CODE_RUNNING)
    {
      SOMA_NO_LOG_ALL ("Cmd: Get Stop.");

      if(set_stop || set_in_pause)
        soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));
      else
        soma_write (c, SOMA_STR_ERR, strlen (SOMA_STR_ERR));

      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  /* get Stop */
  if (cmd == SOMA_CODE_GET_STOP)
    {
      SOMA_NO_LOG_ALL ("Cmd: Get Stop.");

      soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));

      soma_write (c, set_stop ? "1\n" : "0\n", 1);

      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  if (cmd == SOMA_CODE_GET_ITEM)
    {
      char *item;

      SOMA_NO_LOG_ALL ("Cmd: Get Item");

      soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));

      LOCK ();
      item = *conf->Item ? strdup (conf->Item) : NULL;
      UNLOCK ();

      if (item)
	{
	  soma_write (c, item, strlen (item));
	  free (item);
	}

      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  if (cmd == SOMA_CODE_GET_FOLLOW_ITEM)
    {
      char *item;

      SOMA_NO_LOG_ALL ("Cmd: Get Follow Item");

      soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));

      LOCK ();
      item = conf->Item_next ? strdup (conf->Item_next) : NULL;
      UNLOCK ();

      if (item)
	{
	  soma_write (c, item, strlen (item));
	  free (item);
	}

      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  if (cmd == SOMA_CODE_GET_STAT)
    {
      int k = 0;
      char fl_name[SOMA_MAX_BUFF];
      string_t *str;

      SOMA_NO_LOG_ALL ("Cmd: Get Stat");

      soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));

      while (soma_read (c, fl_name + k, 1) == 1)
	{
	  if (*(fl_name + k) == '\n')
	    break;

	  k++;
	  if (k == SOMA_MAX_BUFF - 1)
	    break;
	}

      *(fl_name + k) = 0;

      if ((str = stat_normal (fl_name)))
	{
	  if (str->size)
	    soma_write (c, str->string, str->size);

	  string_destroy (str);
	}

      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  if (cmd == SOMA_CODE_GET_STAT_DIR)
    {
      int k = 0;
      char fl_name[SOMA_MAX_BUFF];
      string_t *str;

      SOMA_NO_LOG_ALL ("Cmd: Get Stat Dir");

      soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));

      while (soma_read (c, fl_name + k, 1) == 1)
	{
	  if (*(fl_name + k) == '\n')
	    break;

	  k++;
	  if (k == SOMA_MAX_BUFF - 1)
	    break;
	}

      *(fl_name + k) = 0;

      if ((str = stat_normal_dir (fl_name)))
	{
	  if (str->size)
	    soma_write (c, str->string, str->size);

	  string_destroy (str);
	}

      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  if (cmd == SOMA_CODE_GET_STAT_PATH)
    {
      int k = 0;
      char fl_name[SOMA_MAX_BUFF];
      string_t *str;

      SOMA_NO_LOG_ALL ("Cmd: Get Stat Path");

      soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));

      while (soma_read (c, fl_name + k, 1) == 1)
	{
	  if (*(fl_name + k) == '\n')
	    break;

	  k++;
	  if (k == SOMA_MAX_BUFF - 1)
	    break;
	}

      *(fl_name + k) = 0;

      if ((str = distribuited_stat (fl_name)))
	{
	  if (str->size)
	    soma_write (c, str->string, str->size);

	  string_destroy (str);
	}

      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  if (cmd == SOMA_CODE_GET_STAT_DIR_PATH)
    {
      int k = 0;
      char fl_name[SOMA_MAX_BUFF];
      string_t *str;

      SOMA_NO_LOG_ALL ("Cmd: Get Stat Dir Path");

      soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));

      while (soma_read (c, fl_name + k, 1) == 1)
	{
	  if (*(fl_name + k) == '\n')
	    break;

	  k++;
	  if (k == SOMA_MAX_BUFF - 1)
	    break;
	}

      *(fl_name + k) = 0;

      if ((str = distribuited_stat_dir (fl_name)))
	{
	  if (str->size)
	    soma_write (c, str->string, str->size);

	  string_destroy (str);
	}

      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  if (cmd == SOMA_CODE_GET_ITEM_LIST)
    {
      item *item;
      string_t *str;

      SOMA_NO_LOG_ALL ("Cmd: Get Item List");

      soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));

      if (LOCK ())
	SOMA_FATAL ("Lock error.");

      str = string_new ();

      item = conf->item;
      while (item)
	{
	  string_add (str, item->name, strlen (item->name));
	  string_add (str, "\n", 1);

	  item = item->next;
	}

      UNLOCK ();

      if (str->size)
	soma_write (c, str->string, str->size);

      string_destroy (str);

      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  if (cmd == SOMA_CODE_GET_SPOT_LIST)
    {
      item *spot;
      string_t *str;

      SOMA_NO_LOG_ALL ("Cmd: Get Spot List");

      soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));

      if (LOCK ())
	SOMA_FATAL ("Lock error.");

      str = string_new ();

      spot = conf->spot;
      while (spot)
	{
	  string_add (str, spot->name, strlen (spot->name));
	  string_add (str, "\n", 1);

	  spot = spot->next;
	}

      UNLOCK ();

      if (str->size)
	soma_write (c, str->string, str->size);

      string_destroy (str);

      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  if (cmd == SOMA_CODE_GET_PL_NAME)
    {
      char *description = NULL;

      SOMA_NO_LOG_ALL ("Cmd: Get Palinsesto Name");

      soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));

      if (LOCK ())
	SOMA_FATAL ("Lock error.");

      if (palinsesto && palinsesto->Description)
	description = strdup (palinsesto->Description);

      UNLOCK ();

      if (description)
	{
	  soma_write (c, description, strlen (description));
	  free (description);
	}

      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  if (cmd == SOMA_CODE_GET_TIME_PLAY)
    {
      char s[SOMA_MAX_BUFF];
      int len;

      SOMA_NO_LOG_ALL ("Cmd: Get Time Play");

      soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));

      if (LOCK ())
	SOMA_FATAL ("Lock error.");

      len = snprintf (s, sizeof (s), "%ld\n", conf->time_play);

      UNLOCK ();

      soma_write (c, s, len);

      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  if (cmd == SOMA_CODE_REMOVE_ITEM)
    {
      int k = 0;
      char cmd[SOMA_MAX_BUFF];

      SOMA_NO_LOG_ALL ("Cmd: Remove Item");

      soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));

      while (soma_read (c, cmd + k, 1) == 1)
	{
	  if (*(cmd + k) == '\n')
	    break;

	  k++;
	  if (k == SOMA_MAX_BUFF - 1)
	    break;
	}

      *(cmd + k) = 0;

      remove_item (0, atoi (cmd));

      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  if (cmd == SOMA_CODE_REMOVE_SPOT)
    {
      int k = 0;
      char cmd[SOMA_MAX_BUFF];

      SOMA_NO_LOG_ALL ("Cmd: Remove Spot");

      soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));

      while (soma_read (c, cmd + k, 1) == 1)
	{
	  if (*(cmd + k) == '\n')
	    break;

	  k++;
	  if (k == SOMA_MAX_BUFF - 1)
	    break;
	}

      *(cmd + k) = 0;

      remove_item (1, atoi (cmd));

      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  /* Set pause */
  if (cmd == SOMA_CODE_PAUSE)
    {
      SOMA_NO_LOG_ALL ("Cmd: Pause.");

      if (!set_pause)
	set_pause = 1;

      soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));

      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  /* Set UnPause */
  if (cmd == SOMA_CODE_UNPAUSE)
    {
      SOMA_NO_LOG_ALL ("Cmd: UnPause.");

      set_pause = 0;

      soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));

      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  /* Set UnPause */
  if (cmd == SOMA_CODE_GET_PAUSE)
    {
      SOMA_NO_LOG_ALL ("Cmd: Get Pause.");

      soma_write (c, SOMA_STR_OK, strlen (SOMA_STR_OK));

      soma_write (c, set_pause ? "1\n" : "0\n", 1);

      soma_close (c);

      c->flag = 1;
      free (c);
      pthread_exit (NULL);
      return 0;
    }

  /* Other */
  SOMA_NO_LOG_ALL ("Cmd: Error!");
  soma_write (c, SOMA_STR_ERR_CMD, strlen (SOMA_STR_ERR_CMD));

  soma_close (c);

  c->flag = 1;
  free (c);
  pthread_exit (NULL);
  return 0;
}

static int
get_line (struct socket_client *c, char *buf, size_t size)
{
  char ch;
  int i = 0;

  while (soma_read (c, &ch, 1) == 1)
    {
      if (i == size - 1)
	{
	  buf[i] = 0;
	  while (soma_read (c, &ch, 1) == 1 && ch != '\n');
	  return 0;
	}

      if (ch != '\n')
	buf[i] = ch;
      else
	{
	  buf[i] = 0;
	  return 0;
	}

      i++;
    }

  return 1;
}

static int
get_line_fd (int fd, char *buf, size_t size)
{
  char ch;
  int i = 0;

  while (read (fd, &ch, 1) == 1)
    {
      if (i == size - 1)
	{
	  buf[i] = 0;
	  while (read (fd, &ch, 1) == 1 && ch != '\n');
	  return 0;
	}

      if (ch != '\n')
	buf[i] = ch;
      else
	{
	  buf[i] = 0;
	  return 0;
	}

      i++;
    }

  return 1;
}

/* EOF */
