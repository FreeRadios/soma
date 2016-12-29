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

#ifndef __SOMA_CONTROLLER_H__
#define __SOMA_CONTROLLER_H__

#include <sys/types.h>
#include <time.h>

#ifdef SOMA_USE_OPENSSL
#include <openssl/rsa.h>
#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#endif

#define SOMA_ERR_SSL_REQUEST	-8 /**< You try to connect to a SSL somad. */
#define SOMA_ERR_NO_SSL_REQUEST	-7 /**< You try to connect to a NO_SSL somad */
#define SOMA_ERR_SSL		-6 /**< SSL connection error */
#define SOMA_ERR_CONNECT	-5 /**< Connect error */
#define SOMA_ERR_HOST		-4 /**< Host unknown */
#define SOMA_ERR_PROTOCOL	-3 /**< Protocol error */
#define SOMA_ERR_PASSWORD	-2 /**< Password error */
#define SOMA_ERR_POSIX		-1 /**< POSIX error, use errno! */
#define SOMA_ERR_OK		0 /**< No error */
#define SOMA_ERR_USER		1 /**< Parameter error */

#ifdef __cplusplus
extern "C"
{
#endif

  /** 
   * This struct describes the connection between your software and
   * somad.
   */
  typedef struct
  {
#ifdef SOMA_USE_OPENSSL
    int activated_ssl; 		/**< This flag idicates if the connection
				     uses ssl or not*/
#endif

    int unixsocket;		/**< The connection is unixsocket or tcp ? */

    char *unixpath;		/**< The unix socket file */

    char *server;		/**< The server for TCP connection */
    int port;			/**< The port for TCP connection */

    char *password;		/**< Password of somad */

    int error;			/**< The error code */

  } soma_controller;

  /**
   * This function returns ALWAYS a soma_controller struct. If it returns
   * NULL there is a memory problem.
   *
   * This function connects your code to a somad server with a unix socket
   * connection.
   *
   * soma_error returns the error code of this function.
   *
   * \param unixpath the unixpath server. If it is NULL, libsoma use the
   * default unixsocket (/tmp/somad.sock)
   * \param password the password of somad. If it is NULL, libsoma try to
   * connect to somad without password.
   * \param ssl the connection must be with ssl or not? 1 with, 0 without ssl.
   * \return the soma_controller struct.
   */
  soma_controller *		soma_open_unix		(char *unixpath,
							 char *password,
							 int ssl); 
                                                         /* 0 no, 1 yes */

#define soma_open soma_open_tcp

  /**
   * This function returns ALWAYS a soma_controller struct. If it returns
   * NULL there is a memory problem.
   *
   * This function connects your code to a somad server with a TCP socket
   * connection.
   *
   * soma_error returns the error code of this function.
   *
   * \param server the IP or host. If it is NULL, somad try to connect 
   * to localhost server.
   * \param port the port for the TCP connection. If it is 0, libsoma uses 
   * the default port (12521)
   * \param password the password of somad. If it is NULL, libsoma try to
   * connect to somad without password.
   * \param ssl the connection must be with ssl or not? 1 with, 0 without ssl.
   * \return the soma_controller struct.
   */
  soma_controller *		soma_open_tcp		(char *server,
		  					 int port,
							 char *password,
							 int ssl);
                                                         /* 0 no, 1 yes */

  /**
   * This function frees a soma_controller struct and return a error code.
   *
   * \param c the soma_controller struct
   * \return a error code
   */
  int				soma_free		(soma_controller *c);

  /**
   * This function returns a string with the status of somad
   *
   * \param c the soma_controller struct
   * \return a string. You must free it after usage.
   */
  char *			soma_status		(soma_controller *c);

  /**
   * Somad re-parse the palinsesto file after this function and sends a 
   * error code.
   *
   * \param c the soma_controller struct
   * \return the error code.
   */
  int 				soma_read_palinsesto	(soma_controller *c);

  /**
   * Somad re-parse the spot ile after this function and sends a error code.
   *
   * \param c the soma_controller struct
   * \return the error code.
   */
  int 				soma_read_spot		(soma_controller *c);

  /**
   * Somad re-scan the directories of current palinsesto and sends a error code
   *
   * \param c the soma_controller struct
   * \return the error code.
   */
  int				soma_read_directory	(soma_controller *c);

#define				soma_new_palinsesto   soma_new_palinsesto_file

  /**
   * This function sends to somad a new palinsesto file from a local file.
   *
   * \param c the soma_controller struct
   * \param file a local file
   * \return the error code.
   */
  int				soma_new_palinsesto_file(soma_controller *c,
		  					 char *file);

  /**
   * This function sends to somad a new palinsesto file from a buffer.
   *
   * \param c the soma_controller struct
   * \param buffer the pointer to your buffer
   * \param size the size of your buffer. If size is <=0, libsoma use the
   * strlen function on your buffer.
   * \return the error code.
   */
  int				soma_new_palinsesto_buffer(soma_controller *c,
		  					 char *buffer,
							 size_t size);

  /**
   * This function returns to your code the current palinsesto of somad.
   *
   * \param c the soma_controller struct
   * \return a buffer with the palinsesto inside. You must free it after usage.
   */
  char *			soma_get_palinsesto	(soma_controller *c);

  /**
   * This function returns to your code the previews palinsesto of somad.
   *
   * \param c the soma_controller struct
   * \return a buffer with the palinsesto inside. You must free it after usage.
   */
  char *			soma_get_old_palinsesto	(soma_controller *c);

  /**
   * Somad uses the preview palinsesto if you exec this function.
   *
   * \param c the soma_controller struct
   * \return the error code
   */
  int				soma_old_palinsesto	(soma_controller *c);

  /**
   * Somad writes the current palinsesto in /etc/somad/palinsesto.cfg or
   * in the default palinsesto file.
   *
   * \param c the soma_controller struct
   * \return the error code
   */
  int				soma_set_default_palinsesto
	  						(soma_controller *c);

#define				soma_new_spot		soma_new_spot_file

  /**
   * This function sends to somad a new spot file from a local file.
   *
   * \param c the soma_controller struct
   * \param file a local file
   * \return the error code.
   */
  int				soma_new_spot_file	(soma_controller *c,
		  					 char *file);

  /**
   * This function sends to somad a new spot file from a buffer.
   *
   * \param c the soma_controller struct
   * \param buffer the pointer to your buffer
   * \param size the size of your buffer. If size is <=0, libsoma use the
   * strlen function on your buffer.
   * \return the error code.
   */
  int				soma_new_spot_buffer	(soma_controller *c,
		  					 char *buffer,
							 size_t size);

  /**
   * This function returns to your code the current spot file of somad.
   *
   * \param c the soma_controller struct
   * \return a buffer with the spot inside. You must free it after usage.
   */
  char *			soma_get_spot		(soma_controller *c);

  /**
   * This function returns to your code the previews spot file of somad.
   *
   * \param c the soma_controller struct
   * \return a buffer with the spot inside. You must free it after usage.
   */
  char *			soma_get_old_spot	(soma_controller *c);

  /**
   * Somad uses the preview spot file if you exec this function.
   *
   * \param c the soma_controller struct
   * \return the error code
   */
  int				soma_old_spot		(soma_controller *c);

  /**
   * Somad writes the current spot in /etc/somad/spot.cfg or
   * in the default spot file.
   *
   * \param c the soma_controller struct
   * \return the error code
   */
  int				soma_set_default_spot	(soma_controller *c);

  /**
   * This function return 0 if the password is correnct, else 1
   *
   * \param c the soma_controller struct
   * \return 0 is the password is OK, 1 it is't.
   */
  int				soma_check_password	(soma_controller *c);

  /**
   * Somad quits after this function.
   *
   * \param c the soma_controller struct
   * \return the error code
   */
  int				soma_quit		(soma_controller *c);

  /**
   * Somad skips the current item.
   *
   * \param c the soma_controller struct
   * \return the error code
   */
  int				soma_skip		(soma_controller *c);

  /**
   * Somad skips the follow item.
   *
   * \param c the soma_controller struct
   * \return the error code
   */
  int				soma_skip_next		(soma_controller *c);

  /**
   * Somad stops the scheduler for 'time' seconds.
   *
   * \param c the soma_controller struct
   * \param time the seconds. If it is 0, somad will be stopped until a 
   * soma_start(c)
   * \return the error code
   */
  int				soma_stop		(soma_controller *c,
		  					 int time);

  /**
   * Somad starts the scheduler. Somad does something only if it is stopped.
   *
   * \param c the soma_controller struct
   * \return the error code
   */
  int				soma_start		(soma_controller *c);

  /**
   * This function return 0 if somad is not stopped, 1 if somad is stopped.
   *
   * \param c the soma_controller struct
   * \return 0 if somad is not sopped, 1 if somad is stopped.
   */
  int				soma_running		(soma_controller *c);

  /**
   * This function return the time() in the server
   *
   * \param c the soma_controller struct
   * \return the output of time() on the server
   */
  time_t			soma_time		(soma_controller *c);

  /**
   * This function adds a item in the nextitem list of somad.
   *
   * \param c the soma_controller struct
   * \param item the item item
   * \return the error code
   */
  int				soma_nextitem_set	(soma_controller *c,
		  					 char *item);

  /**
   * This function adds a item from the distributed filesystem 
   * in the nextitem list of somad.
   *
   * \param c the soma_controller struct
   * \param item the item item from the dfs
   * \return the error code
   */
  int				soma_nextitem_path_set	(soma_controller *c,
		  					 char *item);

  /**
   * This function sets the iteration with the spot. If you set to YES the
   * iteration, somad schedules ratioitem nextitem items and after ratiospot
   * items from pathspot or from spotcontroller.
   *
   * \param c the soma_controller struct
   * \param yes_no 1 yes, 0 no
   * \return the error code
   */
  int				soma_nextitem_set_spot	(soma_controller *c,
		  					 int yes_no); 
  							/* 1=YES, 0=NO */

  /**
   * This function gets the iteration with the spot.
   *
   * \param c the soma_controller struct
   * \return 1 if there is iteration, 0 if there is not.
   */
  int				soma_nextitem_get_spot	(soma_controller *c);

  /**
   * This function returns the list of nextitem items. The last item of the
   * list is set to NULL.
   * You can use it like this:
   *
   * \code
   * [...]
   * char **list, **old;
   *
   * old=list=soma_nextitem_list(c);
   * 
   * if(!list)
   *   return;
   *
   * while(*list) {
   *   puts(*list);
   *   list++;
   * }
   *
   * soma_nextitem_list_free(old);
   * \endcode
   * 
   * \param c the soma_controller struct
   * \return a char **. You must free it with soma_nextitem_list_free function.
   */
  char **			soma_nextitem_list	(soma_controller *c);

  /**
   * This function frees a output of soma_nextitem_list.
   * \param output the list
   */
  void				soma_nextitem_list_free	(char **output);

  /**
   * This function removes a item from nextitem list. The ID is the number
   * of item of the list.
   *
   * \param c the soma_controller struct
   * \param id is the ID of the idem. If it is 0 you want remove the fist
   * if it is -1 you want remove the last.
   * \return the error code
   */
  int				soma_nextitem_remove	(soma_controller *c,
		  					 int id);
 							 /* 0 first, -1 last */


  /**
   * This function returns a list of files/directories of a remote path
   * If the path is "/", the functions shows the distribuited directories
   * (DistribuitedPath items). The last item of the list is set to NULL.
   * The items of the list are directory if the last char is "/" else are
   * files.
   *
   * \code
   * [...]
   * char **old, **list;
   *
   * list=old=soma_get_path(c, "/");
   *
   * if(!list)
   *   return;
   *
   * while(*list) {
   *
   *   if(*list[strlen(*list)-1]=='/')
   *     printf("Directory: %s\n", *list);    
   *   else
   *     printf("File: %s\n", *list);    
   *
   *   list++;
   * }
   *
   * soma_get_path_free(old);
   * \endcode
   *
   * \param c the soma_controller struct
   * \param path the path
   * \return the list of items
   */
  char **			soma_get_path		(soma_controller *c,
		  					 char *path);

  /**
   * This function frees the output of a soma_get_path
   *
   * \param output the list
   */
  void				soma_get_path_free	(char **output);


  /**
   * This function returns the name of current item item/spot.
   *
   * \param c the soma_controller struct
   * \return the name of the item. It must be free after usage.
   */
  char *			soma_get_item		(soma_controller *c);


  /**
   * This function returns the name of following item item/spot.
   *
   * \param c the soma_controller struct
   * \return the name of the item. It must be free after usage.
   */
  char *			soma_get_item_next	(soma_controller *c);


  /**
   * This function returns the seconds from the exec of the player with
   * the current item and now.
   *
   * \param c the soma_controller struct
   * \return the seconds.
   */
  time_t			soma_get_time_play	(soma_controller *c);

  /**
   * This function returns a soma_stat struct the the info about a remote
   * file. 
   *
   * \code
   * [...]
   * soma_stat *stat;
   *
   * stat=soma_get_stat(c, "/home/soma/mp3/file.mp3");
   *
   * if(!stat)
   *   return;
   *
   * puts(stat->title);
   * check_duration(stat);
   *
   * soma_stat_free(stat);
   * [...]
   *
   * void check_duration(soma_stat *stat) {
   *   int hours, mins, secs, us;
   *   secs = stat->duration / SOMA_TIME_BASE;
   *   us = stat->duration % SOMA_TIME_BASE;
   *   mins = secs / 60;
   *   secs %= 60;
   *   hours = mins / 60;
   *   mins %= 60;
   *   printf("duration: %02d:%02d:%02d.%01d\n",hours, mins, secs, 
   *           (10 * us) / SOMA_TIME_BASE);
   * }
   * \endcode
   *
   * \param c the soma_controller struct
   * \param file the file
   * \return a soma_stat struct. You must free it with soma_stat_free function.
   */
  soma_stat *			soma_get_stat		(soma_controller *c,
		  					 char *file);

  /**
   * This function returns a soma_stat_dir struct the the info about a remote
   * directory.
   *
   * \code
   * [...]
   * soma_stat_dir *stat;
   * soma_stat *t;
   *
   * stat=soma_get_stat(c, "/home/soma/mp3/");
   *
   * if(!stat)
   *   return;
   *
   * puts(stat->dirname);
   *
   * t=stat->list;
   * while(t) {
   *   puts(t->title);
   *   t=t->next;
   * }
   *
   * soma_stat_dir_free(stat);
   * \endcode
   *
   * \param c the soma_controller struct
   * \param directory the directory
   * \return a soma_stat_dir struct. You must free it after usage with
   * soma_stat_dir_free function
   */
  soma_stat_dir *		soma_get_stat_dir	(soma_controller *c,
		  					 char *directory);

  /**
   * This function returns a soma_stat struct the the info about a remote
   * file. It is like soma_get_stat but the file is from the distribuited
   * filesystem.
   *
   * \param c the soma_controller struct
   * \param file the file
   * \return a soma_stat struct. You must free it with soma_stat_free function.
   */
  soma_stat *			soma_get_stat_path	(soma_controller *c,
		  					 char *file);

  /**
   * This function returns a soma_stat_dir struct the the info about a remote
   * directory. It is like soma_get_stat but the directory is from the
   * distribuited filesystem.
   *
   * \param c the soma_controller struct
   * \param directory the directory
   * \return a soma_stat_dir struct. You must free it after usage with
   * soma_stat_dir_free function
   */
  soma_stat_dir *		soma_get_stat_dir_path	(soma_controller *c,
		  					 char *directory);

  /**
   * This function frees the soma_stat struct
   * 
   * \param stat the soma_stat struct
   */
  void				soma_stat_free		(soma_stat *stat);

  /**
   * This function frees the soma_stat_dir struct
   * 
   * \param stat the soma_stat_dir struct
   */
  void				soma_stat_dir_free	(soma_stat_dir *stat);

  /**
   * This function returns the list of items in the current palinsesto.
   * The list is composed with any file in the directories and subdirectories
   * of any element of PathItem of current palinsesto.
   *
   * \param c the soma_controller struct
   * \return the list. You must free it after usage with the
   * soma_item_list_free function
   */
  char **			soma_get_item_list	(soma_controller *c);

  /**
   * This function frees the memory of a soma_get_item_list.
   *
   * \param output the list
   */
  void				soma_item_list_free	(char **output);

  /**
   * This function returns the list of spot in the current palinsesto.
   * The list is composed with any file in the directories and subdirectories
   * of any element of PathSpot of current palinsesto.
   *
   * \param c the soma_controller struct
   * \return the list. You must free it after usage with the
   * soma_spot_list_free function
   */
  char **			soma_get_spot_list	(soma_controller *c);

  /**
   * This function frees the memory of a soma_get_spot_list.
   *
   * \param output the list
   */
  void				soma_spot_list_free	(char **output);

  /**
   * This function returns the name of the current palinsesto.
   *
   * \param c the soma_controller struct
   * \return the name of the current palinsesto. You must free it after usage
   */
  char *			soma_palinsesto_name	(soma_controller *c);

  /**
   * This function removes a item by a ID
   *
   * \param c the soma_controller struct
   * \param id the id of the item
   * \return the error code
   */
  int				soma_remove_item	(soma_controller *c,
		  					 int id);

  /**
   * This function removes a spot by a ID
   *
   * \param c the soma_controller struct
   * \param id the id of the spot
   * \return the error code
   */
  int				soma_remove_spot	(soma_controller *c,
		  					 int id);


  /**
   * This function sets the pause flag
   *
   * \param c the soma controller struct
   * \return the error code
   */
  int				soma_set_pause		(soma_controller *c);

  /**
   * This function sets the pause after the current item
   *
   * \param c the soma controller struct
   * \return the error code
   */
  int				soma_set_pause_after	(soma_controller *c);

  /**
   * This function unset the pause flags
   *
   * \param c the soma controller struct
   * \return the error code
   */
  int				soma_set_unpause	(soma_controller *c);

  /**
   * This function gets the pause status
   *
   * \param c the soma controller struct
   * \param pause pointer to pause integer
   * \return the error code
   */
  int				soma_get_pause		(soma_controller *c,
		  					 int *pause);

  /**
   * This function gets the stop status
   *
   * \param c the soma controller struct
   * \param stop pointer to stop integer
   * \return the error code
   */
  int				soma_get_stop		(soma_controller *c,
		  					 int *stop);

  /**
   * This function returns the error code from a soma_controller struct
   *
   * \param c the soma_controller struct
   * \return the error code
   */
  int				soma_error		(soma_controller *c);

#ifdef __cplusplus
}
#endif

#endif
