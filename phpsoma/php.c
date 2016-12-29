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
#include "config.h"
#endif

#include "php.h"

#ifndef PHPSOMA_INTERNAL
#  include <soma/commons.h>
#  include <soma/controller.h>
#  include <soma/local.h>
#else
#  include "commons.h"
#  include "controller.h"
#  include "local.h"
#endif

#define PHP_SOMA_VERSION "1.0"
#define PHP_SOMA_EXTNAME "soma"

PHP_FUNCTION(soma_open_unix);
PHP_FUNCTION(soma_open_tcp);

PHP_FUNCTION(soma_free);

PHP_FUNCTION(soma_status);

PHP_FUNCTION(soma_read_palinsesto);
PHP_FUNCTION(soma_read_spot);
PHP_FUNCTION(soma_read_directory);

PHP_FUNCTION(soma_new_palinsesto_file);
PHP_FUNCTION(soma_new_palinsesto_buffer);
PHP_FUNCTION(soma_get_palinsesto);
PHP_FUNCTION(soma_get_old_palinsesto);
PHP_FUNCTION(soma_old_palinsesto);
PHP_FUNCTION(soma_set_default_palinsesto);

PHP_FUNCTION(soma_new_spot_file);
PHP_FUNCTION(soma_new_spot_buffer);
PHP_FUNCTION(soma_get_spot);
PHP_FUNCTION(soma_get_old_spot);
PHP_FUNCTION(soma_old_spot);
PHP_FUNCTION(soma_set_default_spot);

PHP_FUNCTION(soma_check_password);

PHP_FUNCTION(soma_quit);
PHP_FUNCTION(soma_skip);
PHP_FUNCTION(soma_skip_next);
PHP_FUNCTION(soma_stop);
PHP_FUNCTION(soma_start);
PHP_FUNCTION(soma_running);
PHP_FUNCTION(soma_time);
PHP_FUNCTION(soma_get_time_play);

PHP_FUNCTION(soma_nextitem_set);
PHP_FUNCTION(soma_nextitem_path_set);
PHP_FUNCTION(soma_nextitem_set_spot);
PHP_FUNCTION(soma_nextitem_get_spot);
PHP_FUNCTION(soma_nextitem_list);
PHP_FUNCTION(soma_nextitem_remove);

PHP_FUNCTION(soma_get_path);

PHP_FUNCTION(soma_get_item);
PHP_FUNCTION(soma_get_item_next);

PHP_FUNCTION(soma_get_stat);
PHP_FUNCTION(soma_get_stat_dir);

PHP_FUNCTION(soma_get_stat_path);
PHP_FUNCTION(soma_get_stat_dir_path);

PHP_FUNCTION(soma_local_stat);
PHP_FUNCTION(soma_local_stat_dir);

PHP_FUNCTION(soma_error);
PHP_FUNCTION(soma_error_string);

PHP_FUNCTION(soma_get_item_list);
PHP_FUNCTION(soma_get_spot_list);

PHP_FUNCTION(soma_palinsesto_name);

PHP_FUNCTION(soma_remove_item);
PHP_FUNCTION(soma_remove_spot);

PHP_FUNCTION(soma_set_pause);
PHP_FUNCTION(soma_set_unpause);
PHP_FUNCTION(soma_get_pause);

PHP_FUNCTION(soma_get_stop);

zend_module_entry soma_module_entry;

PHP_MINIT_FUNCTION(soma) {
REGISTER_LONG_CONSTANT("SOMA_ERR_SSL_REQUEST", SOMA_ERR_SSL_REQUEST, CONST_CS | CONST_PERSISTENT);
REGISTER_LONG_CONSTANT("SOMA_ERR_NO_SSL_REQUEST", SOMA_ERR_NO_SSL_REQUEST , CONST_CS | CONST_PERSISTENT);
REGISTER_LONG_CONSTANT("SOMA_ERR_SSL", SOMA_ERR_SSL, CONST_CS | CONST_PERSISTENT);
REGISTER_LONG_CONSTANT("SOMA_ERR_CONNECT", SOMA_ERR_CONNECT, CONST_CS | CONST_PERSISTENT);
REGISTER_LONG_CONSTANT("SOMA_ERR_HOST", SOMA_ERR_HOST, CONST_CS | CONST_PERSISTENT);
REGISTER_LONG_CONSTANT("SOMA_ERR_PROTOCOL", SOMA_ERR_PROTOCOL, CONST_CS | CONST_PERSISTENT);
REGISTER_LONG_CONSTANT("SOMA_ERR_PASSWORD",SOMA_ERR_PASSWORD , CONST_CS | CONST_PERSISTENT);
REGISTER_LONG_CONSTANT("SOMA_ERR_POSIX", SOMA_ERR_POSIX, CONST_CS | CONST_PERSISTENT);
REGISTER_LONG_CONSTANT("SOMA_ERR_OK", SOMA_ERR_OK, CONST_CS | CONST_PERSISTENT);
REGISTER_LONG_CONSTANT("SOMA_ERR_USER", SOMA_ERR_USER, CONST_CS | CONST_PERSISTENT);

return SUCCESS;
}

static function_entry soma_functions[] = {
  PHP_FE (soma_open_unix, NULL) 
  PHP_FE (soma_open_tcp, NULL) 

  PHP_FE (soma_free, NULL) 

  PHP_FE (soma_status, NULL)

  PHP_FE (soma_read_palinsesto, NULL)
  PHP_FE (soma_read_spot, NULL)
  PHP_FE (soma_read_directory, NULL)

  PHP_FE (soma_new_palinsesto_file, NULL)
  PHP_FE (soma_new_palinsesto_buffer, NULL)
  PHP_FE (soma_get_palinsesto, NULL)
  PHP_FE (soma_get_old_palinsesto, NULL)
  PHP_FE (soma_old_palinsesto, NULL)
  PHP_FE (soma_set_default_palinsesto, NULL)

  PHP_FE (soma_new_spot_file, NULL)
  PHP_FE (soma_new_spot_buffer, NULL)
  PHP_FE (soma_get_spot, NULL)
  PHP_FE (soma_get_old_spot, NULL)
  PHP_FE (soma_old_spot, NULL)
  PHP_FE (soma_set_default_spot, NULL)

  PHP_FE (soma_check_password, NULL)

  PHP_FE (soma_quit, NULL)
  PHP_FE (soma_skip, NULL)
  PHP_FE (soma_skip_next, NULL)
  PHP_FE (soma_stop, NULL)
  PHP_FE (soma_start, NULL)

  PHP_FE (soma_running, NULL)

  PHP_FE (soma_time, NULL)
  PHP_FE (soma_get_time_play, NULL)

  PHP_FE (soma_nextitem_set, NULL)
  PHP_FE (soma_nextitem_path_set, NULL)
  PHP_FE (soma_nextitem_set_spot, NULL)
  PHP_FE (soma_nextitem_get_spot, NULL)
  PHP_FE (soma_nextitem_list, NULL)
  PHP_FE (soma_nextitem_remove, NULL)

  PHP_FE (soma_get_path, NULL)
  
  PHP_FE (soma_get_item, NULL)
  PHP_FE (soma_get_item_next, NULL)

  PHP_FE (soma_get_stat, NULL)
  PHP_FE (soma_get_stat_dir, NULL)

  PHP_FE (soma_get_stat_path, NULL)
  PHP_FE (soma_get_stat_dir_path, NULL)

  PHP_FE (soma_local_stat, NULL)
  PHP_FE (soma_local_stat_dir, NULL)

  PHP_FE (soma_error, NULL)
  PHP_FE (soma_error_string, NULL)

  PHP_FE (soma_get_item_list, NULL)
  PHP_FE (soma_get_spot_list, NULL)

  PHP_FE (soma_palinsesto_name, NULL)

  PHP_FE (soma_remove_item, NULL)
  PHP_FE (soma_remove_spot, NULL)

  PHP_FE (soma_set_pause, NULL)
  PHP_FE (soma_set_unpause, NULL)
  PHP_FE (soma_get_pause, NULL)

  PHP_FE (soma_get_stop, NULL)
  
  {NULL, NULL, NULL}

};

zend_module_entry soma_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
  STANDARD_MODULE_HEADER,
#endif
  PHP_SOMA_EXTNAME,
  soma_functions,
  PHP_MINIT(soma),
  NULL,
  NULL,
  NULL,
  NULL,
#if ZEND_MODULE_API_NO >= 20010901
  PHP_SOMA_VERSION,
#endif
  STANDARD_MODULE_PROPERTIES
};

ZEND_GET_MODULE (soma);

PHP_FUNCTION (soma_open_unix)
{
  char *unixpath;
  int unixpath_len;
  char *password;
  int password_len;
  int ssl;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssl", &unixpath, &unixpath_len, &password,&password_len, &ssl) == FAILURE)
     return;

  RETURN_LONG((long)soma_open_unix(unixpath, password, ssl))
}

PHP_FUNCTION (soma_open_tcp)
{
  char *server;
  int server_len;
  int port;
  char *password;
  int password_len;
  int ssl;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "slsl", &server, &server_len, &port, &password,&password_len, &ssl) == FAILURE)
     return;

  RETURN_LONG((long)soma_open_tcp(server, port, password, ssl))
}

PHP_FUNCTION (soma_free)
{
  long i;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &i) == FAILURE)
     return;

  if(soma_free((soma_controller *)i))
    RETURN_TRUE

  RETURN_FALSE
}

PHP_FUNCTION (soma_status)
{
  long i;
  char *c;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &i) == FAILURE)
     return;

  c=soma_status((soma_controller *)i);

  if(c)
    RETURN_STRING(c, 1)

  RETURN_STRING("", 1)
}

PHP_FUNCTION (soma_palinsesto_name)
{
  long i;
  char *c;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &i) == FAILURE)
     return;

  c=soma_palinsesto_name((soma_controller *)i);

  if(c)
    RETURN_STRING(c, 1)

  RETURN_STRING("", 1)
}

PHP_FUNCTION (soma_read_palinsesto)
{
  long i;
  int ret;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &i) == FAILURE)
     return;

  ret=soma_read_palinsesto((soma_controller *)i);

  if(ret)
    RETURN_TRUE

  RETURN_FALSE
}

PHP_FUNCTION (soma_read_spot)
{
  long i;
  int ret;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &i) == FAILURE)
     return;

  ret=soma_read_spot((soma_controller *)i);

  if(ret)
    RETURN_TRUE

  RETURN_FALSE
}

PHP_FUNCTION (soma_read_directory)
{
  long i;
  int ret;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &i) == FAILURE)
     return;

  ret=soma_read_directory((soma_controller *)i);

  if(ret)
    RETURN_TRUE

  RETURN_FALSE
}

PHP_FUNCTION (soma_new_palinsesto_file)
{
  long i;
  char *file;
  int file_len;
  int ret;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &i,&file, &file_len) == FAILURE)
     return;

  ret=soma_new_palinsesto_file((soma_controller *)i, file);

  if(ret)
    RETURN_TRUE

  RETURN_FALSE
}

PHP_FUNCTION (soma_new_palinsesto_buffer)
{
  long i;
  char *file;
  int file_len;
  int ret;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &i,&file, &file_len) == FAILURE)
     return;

  ret=soma_new_palinsesto_buffer((soma_controller *)i, file, file_len);

  if(ret)
    RETURN_TRUE

  RETURN_FALSE
}

PHP_FUNCTION (soma_get_palinsesto)
{
  long i;
  char *c;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &i) == FAILURE)
     return;

  c=soma_get_palinsesto((soma_controller *)i);

  if(c)
    RETURN_STRING(c, 1)

  RETURN_STRING("", 1)
}

PHP_FUNCTION (soma_get_old_palinsesto)
{
  long i;
  char *c;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &i) == FAILURE)
     return;

  c=soma_get_old_palinsesto((soma_controller *)i);

  if(c)
    RETURN_STRING(c, 1)

  RETURN_STRING("", 1)
}

PHP_FUNCTION (soma_old_palinsesto)
{
  long i;
  int ret;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &i) == FAILURE)
     return;

  ret=soma_old_palinsesto((soma_controller *)i);

  if(ret)
    RETURN_TRUE
  else
    RETURN_FALSE
}

PHP_FUNCTION (soma_set_default_palinsesto)
{
  long i;
  int ret;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &i) == FAILURE)
     return;

  ret=soma_set_default_palinsesto((soma_controller *)i);

  if(ret)
    RETURN_TRUE

  RETURN_FALSE
}


PHP_FUNCTION (soma_new_spot_file)
{
  long i;
  char *file;
  int file_len;
  int ret;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &i,&file, &file_len) == FAILURE)
     return;

  ret=soma_new_spot_file((soma_controller *)i, file);

  if(ret)
    RETURN_TRUE

  RETURN_FALSE
}

PHP_FUNCTION (soma_new_spot_buffer)
{
  long i;
  char *file;
  int file_len;
  int ret;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &i,&file, &file_len) == FAILURE)
     return;

  ret=soma_new_spot_buffer((soma_controller *)i, file, file_len);

  if(ret)
    RETURN_TRUE

  RETURN_FALSE
}

PHP_FUNCTION (soma_get_spot)
{
  long i;
  char *c;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &i) == FAILURE)
     return;

  c=soma_get_spot((soma_controller *)i);

  if(c)
    RETURN_STRING(c, 1)
	    
  RETURN_STRING("", 1)
}

PHP_FUNCTION (soma_get_old_spot)
{
  long i;
  char *c;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &i) == FAILURE)
     return;

  c=soma_get_old_spot((soma_controller *)i);

  if(c)
    RETURN_STRING(c, 1)

  RETURN_STRING("", 1)
}

PHP_FUNCTION (soma_old_spot)
{
  long i;
  int ret;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &i) == FAILURE)
     return;

  ret=soma_old_spot((soma_controller *)i);

  if(ret)
    RETURN_TRUE

  RETURN_FALSE
}

PHP_FUNCTION (soma_set_default_spot)
{
  long i;
  int ret;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &i) == FAILURE)
     return;

  ret=soma_set_default_spot((soma_controller *)i);

  if(ret)
    RETURN_TRUE

  RETURN_FALSE
}

PHP_FUNCTION (soma_check_password)
{
  long i;
  int ret;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &i) == FAILURE)
     return;

  ret=soma_check_password((soma_controller *)i);

  if(ret)
    RETURN_TRUE

  RETURN_FALSE
}

PHP_FUNCTION (soma_quit)
{
  long i;
  int ret;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &i) == FAILURE)
     return;

  ret=soma_quit((soma_controller *)i);

  if(ret)
    RETURN_TRUE

  RETURN_FALSE
}

PHP_FUNCTION (soma_skip)
{
  long i;
  int ret;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &i) == FAILURE)
     return;

  ret=soma_skip((soma_controller *)i);

  if(ret)
    RETURN_TRUE

  RETURN_FALSE
}

PHP_FUNCTION (soma_skip_next)
{
  long i;
  int ret;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &i) == FAILURE)
     return;

  ret=soma_skip_next((soma_controller *)i);

  if(ret)
    RETURN_TRUE

  RETURN_FALSE
}

PHP_FUNCTION (soma_start)
{
  long i;
  int ret;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &i) == FAILURE)
     return;

  ret=soma_start((soma_controller *)i);

  if(ret)
    RETURN_TRUE

  RETURN_FALSE
}

PHP_FUNCTION (soma_stop)
{
  long i;
  int ret;
  int st;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &i, &st) == FAILURE)
     return;

  ret=soma_stop((soma_controller *)i, st);

  if(ret)
    RETURN_TRUE

  RETURN_FALSE
}

PHP_FUNCTION (soma_running)
{
  long i;
  int ret;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &i) == FAILURE)
     return;

  ret=soma_running((soma_controller *)i);

  if(ret)
    RETURN_TRUE

  RETURN_FALSE
}

PHP_FUNCTION (soma_time)
{
  long i;
  time_t t;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &i) == FAILURE)
     return;

  t=soma_time((soma_controller *)i);

  RETURN_LONG((long)t)
}

PHP_FUNCTION (soma_get_time_play)
{
  long i;
  time_t t;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &i) == FAILURE)
     return;

  t=soma_get_time_play((soma_controller *)i);

  RETURN_LONG((long)t)
}

PHP_FUNCTION (soma_nextitem_set)
{
  long i;
  char *c;
  int c_len;
  int ret;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &i,&c, &c_len) == FAILURE)
     return;

  ret=soma_nextitem_set((soma_controller *)i, c);

  if(ret)
    RETURN_TRUE

  RETURN_FALSE
}

PHP_FUNCTION (soma_nextitem_path_set)
{
  long i;
  char *c;
  int c_len;
  int ret;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &i,&c, &c_len) == FAILURE)
     return;

  ret=soma_nextitem_path_set((soma_controller *)i, c);

  if(ret)
    RETURN_TRUE

  RETURN_FALSE
}

PHP_FUNCTION (soma_nextitem_set_spot)
{
  long i;
  long t;
  int ret;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &i,&t) == FAILURE)
     return;

  ret=soma_nextitem_set_spot((soma_controller *)i, t);

  if(ret)
    RETURN_TRUE

  RETURN_FALSE
}

PHP_FUNCTION (soma_nextitem_get_spot)
{
  long i;
  int ret;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &i) == FAILURE)
     return;

  ret=soma_nextitem_get_spot((soma_controller *)i);

  if(ret)
    RETURN_TRUE

  RETURN_FALSE
}

PHP_FUNCTION (soma_nextitem_list)
{
  long i;
  char **a, **old;
  int k;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &i) == FAILURE)
     return;

  if(!(a=soma_nextitem_list((soma_controller *)i)))
	  RETURN_FALSE;

  old=a;

  array_init(return_value);
  
  k=0;

  while(*a) {
    add_index_string(return_value, k++, *a, 1);
    a++;
  }

  soma_nextitem_list_free(old);
}

PHP_FUNCTION (soma_nextitem_remove)
{
  long i;
  int ret;
  int k;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &i,&k) == FAILURE)
     return;

  ret=soma_nextitem_remove((soma_controller *)i, k);

  if(ret)
    RETURN_TRUE

  RETURN_FALSE
}

PHP_FUNCTION (soma_get_path)
{
  long i;
  char **a, **old;
  char *c;
  int c_len;
  int k;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &i, &c, &c_len) == FAILURE)
     return;

  if(!(a=soma_get_path((soma_controller *)i, c)))
	  RETURN_FALSE;

  old=a;

  array_init(return_value);
  
  k=0;

  while(*a) {
    add_index_string(return_value, k++, *a, 1);
    a++;
  }

  soma_get_path_free(old);
}

PHP_FUNCTION (soma_get_item)
{
  long i;
  char *c;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &i) == FAILURE)
     return;

  c=soma_get_item((soma_controller *)i);

  if(c)
    RETURN_STRING(c, 1)

  RETURN_STRING("", 1)
}

PHP_FUNCTION (soma_get_item_next)
{
  long i;
  char *c;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &i) == FAILURE)
     return;

  c=soma_get_item_next((soma_controller *)i);

  if(c)
    RETURN_STRING(c, 1)

  RETURN_STRING("", 1)
}

PHP_FUNCTION (soma_get_stat)
{
  long i;
  soma_stat *a;
  soma_stat_stream *s;
  char *c;
  int c_len;
  zval *streams;
  int k;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &i, &c, &c_len) == FAILURE)
     return;

  if(!(a=soma_get_stat((soma_controller *)i, c)))
	  RETURN_FALSE;

  array_init(return_value);
  
  add_assoc_string(return_value, "filename", a->filename, 1);
  add_assoc_string(return_value, "title", a->title, 1);
  add_assoc_string(return_value, "author", a->author, 1);
  add_assoc_string(return_value, "copyright", a->copyright, 1);
  add_assoc_string(return_value, "comment", a->comment, 1);
  add_assoc_string(return_value, "album", a->album, 1);
  add_assoc_long(return_value, "year", (long)a->year);
  add_assoc_long(return_value, "track", (long)a->track);
  add_assoc_string(return_value, "genre", a->genre, 1);
  add_assoc_double(return_value, "start_time", (double)a->start_time);
  add_assoc_double(return_value, "duration", (double)a->duration);
  add_assoc_long(return_value, "bitrate", (long)a->bitrate);

  MAKE_STD_ZVAL(streams);
  array_init(streams);

  k=0;
  s=a->streams;
  while(s) {
    add_index_string(streams, k++,s->stream, 1);
    s=s->next;
  }

  add_assoc_zval(return_value, "streams", streams);
	
  soma_stat_free(a);
}

PHP_FUNCTION (soma_get_stat_dir)
{
  long i;
  soma_stat_dir *a;
  soma_stat_stream *s;
  soma_stat *item;
  soma_stat_list *list;
  char *c;
  int c_len;
  zval *streams, *z, *items;
  int k, j;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &i, &c, &c_len) == FAILURE)
     return;

  if(!(a=soma_get_stat_dir((soma_controller *)i, c)))
	  RETURN_FALSE;

  array_init(return_value);
  
  add_assoc_string(return_value, "dirname", a->dirname, 1);
  add_assoc_double(return_value, "duration", (double)a->duration);

  MAKE_STD_ZVAL(items);
  array_init(items);

  list=a->list;
  k=0;
  
  while(list) {
    item=list->stat;

    MAKE_STD_ZVAL(z);
    array_init(z);

    add_assoc_string(z, "filename", item->filename, 1);
    add_assoc_string(z, "title", item->title, 1);
    add_assoc_string(z, "author", item->author, 1);
    add_assoc_string(z, "copyright", item->copyright, 1);
    add_assoc_string(z, "comment", item->comment, 1);
    add_assoc_string(z, "album", item->album, 1);
    add_assoc_long(z, "year", (long)item->year);
    add_assoc_long(z, "track", (long)item->track);
    add_assoc_string(z, "genre", item->genre, 1);
    add_assoc_double(z, "start_time", (double)item->start_time);
    add_assoc_double(z, "duration", (double)item->duration);
    add_assoc_long(z, "bitrate", (long)item->bitrate);

    add_index_zval(items, k++, z);
  
    MAKE_STD_ZVAL(streams);
    array_init(streams);
  
    j=0;
    s=item->streams;
    while(s) {
      add_index_string(streams, j++,s->stream, 1);
      s=s->next;
    }

    add_assoc_zval(z, "streams", streams);

    list=list->next;
  }

  add_assoc_zval(return_value, "items", items);
  
  soma_stat_dir_free(a);
}

PHP_FUNCTION (soma_get_stat_path)
{
  long i;
  soma_stat *a;
  soma_stat_stream *s;
  char *c;
  int c_len;
  zval *streams;
  int k;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &i, &c, &c_len) == FAILURE)
     return;

  if(!(a=soma_get_stat_path((soma_controller *)i, c)))
	  RETURN_FALSE;

  array_init(return_value);
  
  add_assoc_string(return_value, "filename", a->filename, 1);
  add_assoc_string(return_value, "title", a->title, 1);
  add_assoc_string(return_value, "author", a->author, 1);
  add_assoc_string(return_value, "copyright", a->copyright, 1);
  add_assoc_string(return_value, "comment", a->comment, 1);
  add_assoc_string(return_value, "album", a->album, 1);
  add_assoc_long(return_value, "year", (long)a->year);
  add_assoc_long(return_value, "track", (long)a->track);
  add_assoc_string(return_value, "genre", a->genre, 1);
  add_assoc_double(return_value, "start_time", (double)a->start_time);
  add_assoc_double(return_value, "duration", (double)a->duration);
  add_assoc_long(return_value, "bitrate", (long)a->bitrate);

  MAKE_STD_ZVAL(streams);
  array_init(streams);

  k=0;
  s=a->streams;
  while(s) {
    add_index_string(streams, k++,s->stream, 1);
    s=s->next;
  }

  add_assoc_zval(return_value, "streams", streams);
	
  soma_stat_free(a);
}

PHP_FUNCTION (soma_get_stat_dir_path)
{
  long i;
  soma_stat_dir *a;
  soma_stat_stream *s;
  soma_stat *item;
  soma_stat_list *list;
  char *c;
  int c_len;
  zval *streams, *z, *items;
  int k, j;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &i, &c, &c_len) == FAILURE)
     return;

  if(!(a=soma_get_stat_dir_path((soma_controller *)i, c)))
	  RETURN_FALSE;

  array_init(return_value);
  
  add_assoc_string(return_value, "dirname", a->dirname, 1);
  add_assoc_double(return_value, "duration", (double)a->duration);

  MAKE_STD_ZVAL(items);
  array_init(items);

  list=a->list;
  k=0;
  
  while(list) {
    item=list->stat;

    MAKE_STD_ZVAL(z);
    array_init(z);

    add_assoc_string(z, "filename", item->filename, 1);
    add_assoc_string(z, "title", item->title, 1);
    add_assoc_string(z, "author", item->author, 1);
    add_assoc_string(z, "copyright", item->copyright, 1);
    add_assoc_string(z, "comment", item->comment, 1);
    add_assoc_string(z, "album", item->album, 1);
    add_assoc_long(z, "year", (long)item->year);
    add_assoc_long(z, "track", (long)item->track);
    add_assoc_string(z, "genre", item->genre, 1);
    add_assoc_double(z, "start_time", (double)item->start_time);
    add_assoc_double(z, "duration", (double)item->duration);
    add_assoc_long(z, "bitrate", (long)item->bitrate);

    add_index_zval(items, k++, z);
  
    MAKE_STD_ZVAL(streams);
    array_init(streams);
  
    j=0;
    s=item->streams;
    while(s) {
      add_index_string(streams, j++,s->stream, 1);
      s=s->next;
    }

    add_assoc_zval(z, "streams", streams);

    list=list->next;
  }

  add_assoc_zval(return_value, "items", items);
  
  soma_stat_dir_free(a);
}

PHP_FUNCTION (soma_local_stat)
{
  soma_stat *a;
  soma_stat_stream *s;
  char *c;
  int c_len;
  zval *streams;
  int k;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &c, &c_len) == FAILURE)
     return;

  if(!(a=soma_local_stat(c)))
    RETURN_FALSE;

  array_init(return_value);
  
  add_assoc_string(return_value, "filename", a->filename, 1);
  add_assoc_string(return_value, "title", a->title, 1);
  add_assoc_string(return_value, "author", a->author, 1);
  add_assoc_string(return_value, "copyright", a->copyright, 1);
  add_assoc_string(return_value, "comment", a->comment, 1);
  add_assoc_string(return_value, "album", a->album, 1);
  add_assoc_long(return_value, "year", (long)a->year);
  add_assoc_long(return_value, "track", (long)a->track);
  add_assoc_string(return_value, "genre", a->genre, 1);
  add_assoc_double(return_value, "start_time", (double)a->start_time);
  add_assoc_double(return_value, "duration", (double)a->duration);
  add_assoc_long(return_value, "bitrate", (long)a->bitrate);

  MAKE_STD_ZVAL(streams);
  array_init(streams);

  k=0;
  s=a->streams;
  while(s) {
    add_index_string(streams, k++,s->stream, 1);
    s=s->next;
  }

  add_assoc_zval(return_value, "streams", streams);
	
  soma_local_stat_free(a);
}

PHP_FUNCTION (soma_local_stat_dir)
{
  soma_stat_dir *a;
  soma_stat_stream *s;
  soma_stat *item;
  soma_stat_list *list;
  char *c;
  int c_len;
  zval *streams, *z, *items;
  int k, j;
  long l;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl", &c, &c_len, &l) == FAILURE)
     return;

  if(!(a=soma_local_stat_dir(c, l)))
	  RETURN_FALSE;

  array_init(return_value);
  
  add_assoc_string(return_value, "dirname", a->dirname, 1);
  add_assoc_double(return_value, "duration", (double)a->duration);

  MAKE_STD_ZVAL(items);
  array_init(items);

  list=a->list;
  k=0;
  
  while(list) {
    item=list->stat;

    MAKE_STD_ZVAL(z);
    array_init(z);

    add_assoc_string(z, "filename", item->filename, 1);
    add_assoc_string(z, "title", item->title, 1);
    add_assoc_string(z, "author", item->author, 1);
    add_assoc_string(z, "copyright", item->copyright, 1);
    add_assoc_string(z, "comment", item->comment, 1);
    add_assoc_string(z, "album", item->album, 1);
    add_assoc_long(z, "year", (long)item->year);
    add_assoc_long(z, "track", (long)item->track);
    add_assoc_string(z, "genre", item->genre, 1);
    add_assoc_double(z, "start_time", (double)item->start_time);
    add_assoc_double(z, "duration", (double)item->duration);
    add_assoc_long(z, "bitrate", (long)item->bitrate);

    add_index_zval(items, k++, z);
  
    MAKE_STD_ZVAL(streams);
    array_init(streams);
  
    j=0;
    s=item->streams;
    while(s) {
      add_index_string(streams, j++,s->stream, 1);
      s=s->next;
    }

    add_assoc_zval(z, "streams", streams);

    list=list->next;
  }

  add_assoc_zval(return_value, "items", items);
  
  soma_local_stat_dir_free(a);
}

PHP_FUNCTION (soma_get_item_list)
{
  long i;
  char **a, **old;
  int k;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &i) == FAILURE)
     return;

  if(!(a=soma_get_item_list((soma_controller *)i)))
	  RETURN_FALSE;

  old=a;

  array_init(return_value);
  
  k=0;

  while(*a) {
    add_index_string(return_value, k++, *a, 1);
    a++;
  }

  soma_item_list_free(old);
}

PHP_FUNCTION (soma_get_spot_list)
{
  long i;
  char **a, **old;
  int k;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &i) == FAILURE)
     return;

  if(!(a=soma_get_spot_list((soma_controller *)i)))
	  RETURN_FALSE;

  old=a;

  array_init(return_value);
  
  k=0;

  while(*a) {
    add_index_string(return_value, k++, *a, 1);
    a++;
  }

  soma_spot_list_free(old);
}

PHP_FUNCTION (soma_remove_item)
{
  long i;
  int ret;
  int k;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &i,&k) == FAILURE)
     return;

  ret=soma_remove_item((soma_controller *)i, k);

  if(ret)
    RETURN_TRUE

  RETURN_FALSE
}

PHP_FUNCTION (soma_remove_spot)
{
  long i;
  int ret;
  int k;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &i,&k) == FAILURE)
     return;

  ret=soma_remove_spot((soma_controller *)i, k);

  if(ret)
    RETURN_TRUE

  RETURN_FALSE
}

PHP_FUNCTION (soma_set_pause)
{
  long i;
  int ret;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &i) == FAILURE)
     return;

  ret=soma_set_pause((soma_controller *)i);

  if(ret)
    RETURN_TRUE

  RETURN_FALSE
}

PHP_FUNCTION (soma_set_unpause)
{
  long i;
  int ret;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &i) == FAILURE)
     return;

  ret=soma_set_unpause((soma_controller *)i);

  if(ret)
    RETURN_TRUE

  RETURN_FALSE
}

PHP_FUNCTION (soma_get_pause)
{
  long i;
  int ret;
  int p;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &i) == FAILURE)
     return;

  ret=soma_get_pause((soma_controller *)i, &p);

  if(ret)
    RETURN_TRUE

  RETURN_LONG((long)p)
}

PHP_FUNCTION (soma_get_stop)
{
  long i;
  int ret;
  int p;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &i) == FAILURE)
     return;

  ret=soma_get_stop((soma_controller *)i, &p);

  if(ret)
    RETURN_TRUE

  RETURN_LONG((long)p)
}

PHP_FUNCTION (soma_error)
{
  long i;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &i) == FAILURE)
     return;

  RETURN_LONG((long)soma_error((soma_controller *)i));
}

PHP_FUNCTION (soma_error_string)
{
  long i;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &i) == FAILURE)
     return;

  switch(soma_error((soma_controller *)i)) {
	case SOMA_ERR_SSL_REQUEST: RETURN_STRING("SSL request", 1); break;
	case SOMA_ERR_NO_SSL_REQUEST: RETURN_STRING("No SSL request", 1); break;
	case SOMA_ERR_SSL: RETURN_STRING("SSL error", 1); break;
	case SOMA_ERR_CONNECT: RETURN_STRING("Connect error", 1); break;
	case SOMA_ERR_HOST: RETURN_STRING("Host error", 1); break;
	case SOMA_ERR_PROTOCOL: RETURN_STRING("Protocol error", 1); break;
	case SOMA_ERR_PASSWORD: RETURN_STRING("Password error", 1); break;
	case SOMA_ERR_POSIX: RETURN_STRING("Posix error", 1); break;
	case SOMA_ERR_OK: RETURN_STRING("", 1); break;
	case SOMA_ERR_USER: RETURN_STRING("Data error", 1); break;
	default: RETURN_STRING("Unknow error",1);
  }
}

