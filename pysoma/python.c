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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <dirent.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#ifdef PYSOMA25
#  include <python2.5/Python.h>
#elif PYSOMA24
#  include <python2.4/Python.h>
#elif PYSOMA23
#  include <python2.3/Python.h>
#else
#  include <python2.4/Python.h>
#endif

#if PYSOMA_INTERNAL == 0
#  include <soma/code.h>
#  include <soma/commons.h>
#  include <soma/controller.h>
#  include <soma/local.h>
#  include <soma/code.h>
#else
#  include "code.h"
#  include "commons.h"
#  include "controller.h"
#  include "local.h"
#  include "code.h"
#endif

typedef struct
{
  PyObject_HEAD
  soma_controller *controller;
} somaobject;

static PyObject *SomaError;
static PyTypeObject Somatype;

static int
check_error (int error)
{
  switch (error)
    {
    case SOMA_ERR_SSL:
      PyErr_SetString (SomaError, "SSL error");
      return 1;
    case SOMA_ERR_CONNECT:
      PyErr_SetString (SomaError, "Connect error");
      return 1;
    case SOMA_ERR_HOST:
      PyErr_SetString (SomaError, "Host unknown");
      return 1;
    case SOMA_ERR_PROTOCOL:
      PyErr_SetString (SomaError, "Protocol error");
      return 1;
    case SOMA_ERR_PASSWORD:
      PyErr_SetString (SomaError, "Password error");
      return 1;
    case SOMA_ERR_POSIX:
      PyErr_SetString (SomaError, "System error");
      return 1;
    case SOMA_ERR_USER:
      PyErr_SetString (SomaError, "No correct data");
      return 1;
    case SOMA_ERR_SSL_REQUEST:
      PyErr_SetString (SomaError, "SSL admin interface without correct flag");
      return 1;
    case SOMA_ERR_NO_SSL_REQUEST:
      PyErr_SetString (SomaError,
		       "No SSL admin interface without correct flag");
      return 1;
    default:
      return 0;
    }
}

static PyObject *
open_unix (PyObject * self, PyObject * args)
{
  char *unixpath;
  char *password;
  int ssl;
  soma_controller *c;
  somaobject *ob;

  if (!PyArg_ParseTuple (args, "ssi:open_unix", &unixpath, &password, &ssl))
    return NULL;

  if (!(c = soma_open_unix (unixpath, password, ssl)))
    {
      PyErr_SetString (SomaError, "Error memory.");
      return NULL;
    }

  if (check_error (soma_error (c)))
    {
      soma_free (c);
      return NULL;
    }

  if (!(ob = PyObject_New (somaobject, &Somatype)))
    {
      PyErr_SetString (SomaError, "Error memory.");
      soma_free (c);
      return NULL;
    }

  ob->controller = c;
  return (PyObject *) ob;
}

static PyObject *
open_tcp (PyObject * self, PyObject * args)
{
  char *server;
  char *password;
  int ssl;
  int port;
  soma_controller *c;
  somaobject *ob;

  if (!PyArg_ParseTuple
      (args, "sisi:open_tcp", &server, &port, &password, &ssl))
    return NULL;

  if (!(c = soma_open_tcp (server, port, password, ssl)))
    {
      PyErr_SetString (SomaError, "Error memory.");
      return NULL;
    }

  if (check_error (soma_error (c)))
    {
      soma_free (c);
      return NULL;
    }

  if (!(ob = PyObject_New (somaobject, &Somatype)))
    {
      PyErr_SetString (SomaError, "Error memory.");
      soma_free (c);
      return NULL;
    }

  ob->controller = c;
  return (PyObject *) ob;
}

static PyObject *
status (PyObject * self)
{
  somaobject *ob = (somaobject *) self;
  PyObject *ret;
  char *a;

  a = soma_status (ob->controller);
  if (check_error (soma_error (ob->controller)))
    return NULL;

  if(!a)
    return Py_None;

  ret=PyString_FromString (a);
  free(a);

  return ret;
}

static PyObject *
palinsesto_name (PyObject * self)
{
  somaobject *ob = (somaobject *) self;
  PyObject *ret;
  char *a;

  a = soma_palinsesto_name (ob->controller);
  if (check_error (soma_error (ob->controller)))
    return NULL;

  if(!a)
    return Py_None;

  ret=PyString_FromString (a);
  free(a);

  return ret;
}

static PyObject *
get_item (PyObject * self)
{
  somaobject *ob = (somaobject *) self;
  char *a;
  PyObject *ret;

  a = soma_get_item (ob->controller);
  if (check_error (soma_error (ob->controller)))
    return NULL;

  if(!a)
    return Py_None;

  ret= PyString_FromString (a);
  free(a);

  return ret;
}

static PyObject *
get_item_next (PyObject * self)
{
  somaobject *ob = (somaobject *) self;
  char *a;
  PyObject *ret;

  a = soma_get_item_next (ob->controller);
  if (check_error (soma_error (ob->controller)))
    return NULL;

  if(!a)
    return Py_None;

  ret= PyString_FromString (a);
  free(a);

  return ret;
}

static PyObject *
read_palinsesto (PyObject * self)
{
  somaobject *ob = (somaobject *) self;

  soma_read_palinsesto (ob->controller);
  if (check_error (soma_error (ob->controller)))
    return NULL;

  return Py_None;
}

static PyObject *
read_spot (PyObject * self)
{
  somaobject *ob = (somaobject *) self;

  soma_read_spot (ob->controller);
  if (check_error (soma_error (ob->controller)))
    return NULL;

  return Py_None;
}

static PyObject *
read_directory (PyObject * self)
{
  somaobject *ob = (somaobject *) self;

  soma_read_directory (ob->controller);
  if (check_error (soma_error (ob->controller)))
    return NULL;

  return Py_None;
}

static PyObject *
new_palinsesto (PyObject * self, PyObject * args)
{
  somaobject *ob = (somaobject *) self;
  char *pl;

  if (!PyArg_ParseTuple (args, "s:new_palinsesto", &pl))
    return NULL;

  soma_new_palinsesto (ob->controller, pl);
  if (check_error (soma_error (ob->controller)))
    return NULL;

  return Py_None;
}

static PyObject *
get_palinsesto (PyObject * self)
{
  somaobject *ob = (somaobject *) self;
  char *pl;
  PyObject *ret;

  pl = soma_get_palinsesto (ob->controller);
  if (check_error (soma_error (ob->controller)))
    return NULL;

  if(!pl)
    return Py_None;

  ret=PyString_FromString (pl);
  free(pl);

  return ret;
}

static PyObject *
get_old_palinsesto (PyObject * self)
{
  somaobject *ob = (somaobject *) self;
  char *pl;
  PyObject *ret;

  pl = soma_get_old_palinsesto (ob->controller);
  if (check_error (soma_error (ob->controller)))
    return NULL;

  if(!pl)
    return Py_None;

  ret=PyString_FromString (pl);
  free(pl);

  return ret;
}

static PyObject *
old_palinsesto (PyObject * self)
{
  somaobject *ob = (somaobject *) self;

  soma_old_palinsesto (ob->controller);
  if (check_error (soma_error (ob->controller)))
    return NULL;

  return Py_None;
}

static PyObject *
set_default_palinsesto (PyObject * self)
{
  somaobject *ob = (somaobject *) self;

  soma_set_default_palinsesto (ob->controller);
  if (check_error (soma_error (ob->controller)))
    return NULL;

  return Py_None;
}

static PyObject *
new_spot (PyObject * self, PyObject * args)
{
  somaobject *ob = (somaobject *) self;
  char *pl;

  if (!PyArg_ParseTuple (args, "s:new_spot", &pl))
    return NULL;

  soma_new_spot (ob->controller, pl);
  if (check_error (soma_error (ob->controller)))
    return NULL;

  return Py_None;
}

static PyObject *
get_spot (PyObject * self)
{
  somaobject *ob = (somaobject *) self;
  char *pl;
  PyObject *ret;

  pl = soma_get_spot (ob->controller);
  if (check_error (soma_error (ob->controller)))
    return NULL;

  if(!pl)
    return Py_None;

  ret=PyString_FromString (pl);
  free(pl);

  return ret;
}

static PyObject *
get_old_spot (PyObject * self)
{
  somaobject *ob = (somaobject *) self;
  char *pl;
  PyObject *ret;

  pl = soma_get_old_spot (ob->controller);
  if (check_error (soma_error (ob->controller)))
    return NULL;

  if(!pl)
    return Py_None;

  ret=PyString_FromString (pl);
  free(pl);

  return ret;
}

static PyObject *
old_spot (PyObject * self)
{
  somaobject *ob = (somaobject *) self;

  soma_old_spot (ob->controller);
  if (check_error (soma_error (ob->controller)))
    return NULL;

  return Py_None;
}

static PyObject *
set_default_spot (PyObject * self)
{
  somaobject *ob = (somaobject *) self;

  soma_set_default_spot (ob->controller);
  if (check_error (soma_error (ob->controller)))
    return NULL;

  return Py_None;
}

static PyObject *
check_password (PyObject * self)
{
  somaobject *ob = (somaobject *) self;

  soma_check_password (ob->controller);
  if (check_error (soma_error (ob->controller)))
    return NULL;

  return Py_None;
}

static PyObject *
quit (PyObject * self)
{
  somaobject *ob = (somaobject *) self;

  soma_quit (ob->controller);
  if (check_error (soma_error (ob->controller)))
    return NULL;

  return Py_None;
}

static PyObject *
skip (PyObject * self)
{
  somaobject *ob = (somaobject *) self;

  soma_skip (ob->controller);
  if (check_error (soma_error (ob->controller)))
    return NULL;

  return Py_None;
}

static PyObject *
skip_next (PyObject * self)
{
  somaobject *ob = (somaobject *) self;

  soma_skip_next (ob->controller);
  if (check_error (soma_error (ob->controller)))
    return NULL;

  return Py_None;
}

static PyObject *
cmd_stop (PyObject * self, PyObject * args)
{
  somaobject *ob = (somaobject *) self;
  int s;

  if (!PyArg_ParseTuple (args, "i:stop", &s))
    return NULL;

  if (!s)
    s = -1;

  soma_stop (ob->controller, s);
  if (check_error (soma_error (ob->controller)))
    return NULL;

  return Py_None;
}

static PyObject *
cmd_start (PyObject * self)
{
  somaobject *ob = (somaobject *) self;

  soma_start (ob->controller);
  if (check_error (soma_error (ob->controller)))
    return NULL;

  return Py_None;
}

static PyObject *
running (PyObject * self)
{
  somaobject *ob = (somaobject *) self;
  int i;

  i = soma_running (ob->controller);
  if (check_error (soma_error (ob->controller)))
    return NULL;

  return PyBool_FromLong ((long) i);
}

static PyObject *
s_time (PyObject * self)
{
  somaobject *ob = (somaobject *) self;
  int i;

  i = soma_time (ob->controller);
  if (check_error (soma_error (ob->controller)))
    return NULL;

  return PyInt_FromLong ((long) i);
}

static PyObject *
get_time_play (PyObject * self)
{
  somaobject *ob = (somaobject *) self;
  int i;

  i = soma_get_time_play (ob->controller);
  if (check_error (soma_error (ob->controller)))
    return NULL;

  return PyInt_FromLong ((long) i);
}

static PyObject *
nextitem_set (PyObject * self, PyObject * args)
{
  somaobject *ob = (somaobject *) self;
  char *s;

  if (!PyArg_ParseTuple (args, "s:nextitem_set", &s))
    return NULL;

  soma_nextitem_set (ob->controller, s);
  if (check_error (soma_error (ob->controller)))
    return NULL;

  return Py_None;
}

static PyObject *
nextitem_path_set (PyObject * self, PyObject * args)
{
  somaobject *ob = (somaobject *) self;
  char *s;

  if (!PyArg_ParseTuple (args, "s:nextitem_set", &s))
    return NULL;

  soma_nextitem_path_set (ob->controller, s);
  if (check_error (soma_error (ob->controller)))
    return NULL;

  return Py_None;
}

static PyObject *
nextitem_set_spot (PyObject * self, PyObject * args)
{
  somaobject *ob = (somaobject *) self;
  int s;

  if (!PyArg_ParseTuple (args, "i:nextitem_set_spot", &s))
    return NULL;

  soma_nextitem_set_spot (ob->controller, s ? 1 : 0);
  if (check_error (soma_error (ob->controller)))
    return NULL;

  return Py_None;
}

static PyObject *
nextitem_get_spot (PyObject * self)
{
  somaobject *ob = (somaobject *) self;
  int s;

  s = soma_nextitem_get_spot (ob->controller);
  if (check_error (soma_error (ob->controller)))
    return NULL;

  return PyBool_FromLong ((long) s);
}

static PyObject *
nextitem_list (PyObject * self)
{
  somaobject *ob = (somaobject *) self;
  char **list, **old;
  PyObject *ol, *str;

  old=list = soma_nextitem_list (ob->controller);
  if (check_error (soma_error (ob->controller)))
    return NULL;

  if(!old)
    return Py_None;

  ol = PyList_New (0);

  while (*list)
    {
      str = PyString_FromString (*list);
      PyList_Append (ol, str);
      list++;
    }

  soma_nextitem_list_free(old);

  return ol;
}

static PyObject *
nextitem_remove (PyObject * self, PyObject * args)
{
  somaobject *ob = (somaobject *) self;
  int s;

  if (!PyArg_ParseTuple (args, "i:nextitem_remove", &s))
    return NULL;

  soma_nextitem_remove (ob->controller, s);
  if (check_error (soma_error (ob->controller)))
    return NULL;

  return Py_None;
}

static PyObject *
get_path (PyObject * self, PyObject * args)
{
  somaobject *ob = (somaobject *) self;
  char **l, **old;
  char *s;
  PyObject *ol, *str;

  if (!PyArg_ParseTuple (args, "s:get_path", &s))
    return NULL;

  old = l = soma_get_path (ob->controller, s);
  if (check_error (soma_error (ob->controller)))
    return NULL;

  if(!l)
    return Py_None;

  ol = PyList_New (0);

  while (*l)
    {
      str = PyString_FromString (*l);
      PyList_Append (ol, str);
      l++;
    }

  soma_get_path_free(old);

  return ol;
}

static PyObject *
get_stat (PyObject * self, PyObject * args)
{
  somaobject *ob = (somaobject *) self;
  soma_stat *stat;
  soma_stat_stream *stream;
  char *s;
  PyObject *od, *tmp, *list;

  if (!PyArg_ParseTuple (args, "s:get_stat", &s))
    return NULL;

  stat = soma_get_stat (ob->controller, s);
  if (check_error (soma_error (ob->controller)))
    return NULL;

  if(!stat)
    return Py_None;

  od = PyDict_New ();

  tmp = PyString_FromString (stat->filename);
  PyDict_SetItemString (od, "filename", tmp);

  tmp = PyString_FromString (stat->title);
  PyDict_SetItemString (od, "title", tmp);

  tmp = PyString_FromString (stat->author);
  PyDict_SetItemString (od, "author", tmp);

  tmp = PyString_FromString (stat->copyright);
  PyDict_SetItemString (od, "copyright", tmp);

  tmp = PyString_FromString (stat->comment);
  PyDict_SetItemString (od, "comment", tmp);

  tmp = PyString_FromString (stat->album);
  PyDict_SetItemString (od, "album", tmp);

  tmp = PyInt_FromLong (stat->year);
  PyDict_SetItemString (od, "year", tmp);

  tmp = PyInt_FromLong (stat->track);
  PyDict_SetItemString (od, "track", tmp);

  tmp = PyString_FromString (stat->genre);
  PyDict_SetItemString (od, "genre", tmp);

  tmp = PyLong_FromLongLong (stat->start_time);
  PyDict_SetItemString (od, "start_time", tmp);

  tmp = PyLong_FromLongLong (stat->duration);
  PyDict_SetItemString (od, "duration", tmp);

  list = PyList_New (0);

  stream = stat->streams;
  while(stream) {
    tmp = PyString_FromString (stream->stream);
    PyList_Append (list, tmp);

    stream=stream->next;
  }

  PyDict_SetItemString (od, "streams", list);
  
  soma_stat_free(stat);

  return od;
}

static PyObject *
get_stat_dir (PyObject * self, PyObject * args)
{
  somaobject *ob = (somaobject *) self;
  soma_stat_dir *dir;
  soma_stat *stat;
  soma_stat_stream *stream;
  soma_stat_list *list;
  char *s;
  PyObject *od, *tmp, *l, *obj, *od_stat;

  if (!PyArg_ParseTuple (args, "s:get_stat_dir", &s))
    return NULL;

  dir = soma_get_stat_dir (ob->controller, s);
  if (check_error (soma_error (ob->controller)))
    return NULL;

  if(!dir)
    return Py_None;

  od = PyDict_New ();

  tmp = PyString_FromString (dir->dirname);
  PyDict_SetItemString (od, "dirname", tmp);

  tmp = PyLong_FromLongLong (dir->duration);
  PyDict_SetItemString (od, "duration", tmp);

  l = PyList_New (0);

  list = dir->list;
  while(list) {
    stat=list->stat;

    od_stat = PyDict_New ();
  
    tmp = PyString_FromString (stat->filename);
    PyDict_SetItemString (od_stat, "filename", tmp);
  
    tmp = PyString_FromString (stat->title);
    PyDict_SetItemString (od_stat, "title", tmp);
  
    tmp = PyString_FromString (stat->author);
    PyDict_SetItemString (od_stat, "author", tmp);
  
    tmp = PyString_FromString (stat->copyright);
    PyDict_SetItemString (od_stat, "copyright", tmp);
  
    tmp = PyString_FromString (stat->comment);
    PyDict_SetItemString (od_stat, "comment", tmp);
  
    tmp = PyString_FromString (stat->album);
    PyDict_SetItemString (od_stat, "album", tmp);
  
    tmp = PyInt_FromLong (stat->year);
    PyDict_SetItemString (od_stat, "year", tmp);
  
    tmp = PyInt_FromLong (stat->track);
    PyDict_SetItemString (od_stat, "track", tmp);
  
    tmp = PyString_FromString (stat->genre);
    PyDict_SetItemString (od_stat, "genre", tmp);
  
    tmp = PyLong_FromLongLong (stat->start_time);
    PyDict_SetItemString (od_stat, "start_time", tmp);
  
    tmp = PyLong_FromLongLong (stat->duration);
    PyDict_SetItemString (od_stat, "duration", tmp);
  
    obj = PyList_New (0);
  
    stream = stat->streams;
    while(stream) {
      tmp = PyString_FromString (stream->stream);
      PyList_Append (obj, tmp);
  
      stream=stream->next;
    }
  
    PyDict_SetItemString (od_stat, "streams", obj);
  
    PyList_Append (l, od_stat);

    list=list->next;
  }

  PyDict_SetItemString (od, "stats", l);

  soma_stat_dir_free(dir);

  return od;
}

static PyObject *
get_stat_path (PyObject * self, PyObject * args)
{
  somaobject *ob = (somaobject *) self;
  soma_stat *stat;
  soma_stat_stream *stream;
  char *s;
  PyObject *od, *tmp, *list;

  if (!PyArg_ParseTuple (args, "s:get_stat_path", &s))
    return NULL;

  stat = soma_get_stat_path (ob->controller, s);
  if (check_error (soma_error (ob->controller)))
    return NULL;

  if(!stat)
    return Py_None;

  od = PyDict_New ();

  tmp = PyString_FromString (stat->filename);
  PyDict_SetItemString (od, "filename", tmp);

  tmp = PyString_FromString (stat->title);
  PyDict_SetItemString (od, "title", tmp);

  tmp = PyString_FromString (stat->author);
  PyDict_SetItemString (od, "author", tmp);

  tmp = PyString_FromString (stat->copyright);
  PyDict_SetItemString (od, "copyright", tmp);

  tmp = PyString_FromString (stat->comment);
  PyDict_SetItemString (od, "comment", tmp);

  tmp = PyString_FromString (stat->album);
  PyDict_SetItemString (od, "album", tmp);

  tmp = PyInt_FromLong (stat->year);
  PyDict_SetItemString (od, "year", tmp);

  tmp = PyInt_FromLong (stat->track);
  PyDict_SetItemString (od, "track", tmp);

  tmp = PyString_FromString (stat->genre);
  PyDict_SetItemString (od, "genre", tmp);

  tmp = PyLong_FromLongLong (stat->start_time);
  PyDict_SetItemString (od, "start_time", tmp);

  tmp = PyLong_FromLongLong (stat->duration);
  PyDict_SetItemString (od, "duration", tmp);

  list = PyList_New (0);

  stream = stat->streams;
  while(stream) {
    tmp = PyString_FromString (stream->stream);
    PyList_Append (list, tmp);

    stream=stream->next;
  }

  PyDict_SetItemString (od, "streams", list);
  
  soma_stat_free(stat);

  return od;
}

static PyObject *
get_stat_dir_path (PyObject * self, PyObject * args)
{
  somaobject *ob = (somaobject *) self;
  soma_stat_dir *dir;
  soma_stat *stat;
  soma_stat_stream *stream;
  soma_stat_list *list;
  char *s;
  PyObject *od, *tmp, *l, *obj, *od_stat;

  if (!PyArg_ParseTuple (args, "s:get_stat_dir_path", &s))
    return NULL;

  dir = soma_get_stat_dir_path (ob->controller, s);
  if (check_error (soma_error (ob->controller)))
    return NULL;

  if(!dir)
    return Py_None;

  od = PyDict_New ();

  tmp = PyString_FromString (dir->dirname);
  PyDict_SetItemString (od, "dirname", tmp);

  tmp = PyLong_FromLongLong (dir->duration);
  PyDict_SetItemString (od, "duration", tmp);

  l = PyList_New (0);

  list = dir->list;
  while(list) {
    stat=list->stat;

    od_stat = PyDict_New ();
  
    tmp = PyString_FromString (stat->filename);
    PyDict_SetItemString (od_stat, "filename", tmp);
  
    tmp = PyString_FromString (stat->title);
    PyDict_SetItemString (od_stat, "title", tmp);
  
    tmp = PyString_FromString (stat->author);
    PyDict_SetItemString (od_stat, "author", tmp);
  
    tmp = PyString_FromString (stat->copyright);
    PyDict_SetItemString (od_stat, "copyright", tmp);
  
    tmp = PyString_FromString (stat->comment);
    PyDict_SetItemString (od_stat, "comment", tmp);
  
    tmp = PyString_FromString (stat->album);
    PyDict_SetItemString (od_stat, "album", tmp);
  
    tmp = PyInt_FromLong (stat->year);
    PyDict_SetItemString (od_stat, "year", tmp);
  
    tmp = PyInt_FromLong (stat->track);
    PyDict_SetItemString (od_stat, "track", tmp);
  
    tmp = PyString_FromString (stat->genre);
    PyDict_SetItemString (od_stat, "genre", tmp);
  
    tmp = PyLong_FromLongLong (stat->start_time);
    PyDict_SetItemString (od_stat, "start_time", tmp);
  
    tmp = PyLong_FromLongLong (stat->duration);
    PyDict_SetItemString (od_stat, "duration", tmp);
  
    obj = PyList_New (0);
  
    stream = stat->streams;
    while(stream) {
      tmp = PyString_FromString (stream->stream);
      PyList_Append (obj, tmp);
  
      stream=stream->next;
    }
  
    PyDict_SetItemString (od_stat, "streams", obj);
  
    PyList_Append (l, od_stat);

    list=list->next;
  }

  PyDict_SetItemString (od, "stats", l);

  soma_stat_dir_free(dir);

  return od;
}

static PyObject *
get_local_stat (PyObject * self, PyObject * args)
{
  soma_stat *stat;
  soma_stat_stream *stream;
  char *s;
  PyObject *od, *tmp, *list;

  if (!PyArg_ParseTuple (args, "s:get_stat", &s))
    return NULL;

  if(!(stat = soma_local_stat (s)))
    return Py_None;

  od = PyDict_New ();

  tmp = PyString_FromString (stat->filename);
  PyDict_SetItemString (od, "filename", tmp);

  tmp = PyString_FromString (stat->title);
  PyDict_SetItemString (od, "title", tmp);

  tmp = PyString_FromString (stat->author);
  PyDict_SetItemString (od, "author", tmp);

  tmp = PyString_FromString (stat->copyright);
  PyDict_SetItemString (od, "copyright", tmp);

  tmp = PyString_FromString (stat->comment);
  PyDict_SetItemString (od, "comment", tmp);

  tmp = PyString_FromString (stat->album);
  PyDict_SetItemString (od, "album", tmp);

  tmp = PyInt_FromLong (stat->year);
  PyDict_SetItemString (od, "year", tmp);

  tmp = PyInt_FromLong (stat->track);
  PyDict_SetItemString (od, "track", tmp);

  tmp = PyString_FromString (stat->genre);
  PyDict_SetItemString (od, "genre", tmp);

  tmp = PyLong_FromLongLong (stat->start_time);
  PyDict_SetItemString (od, "start_time", tmp);

  tmp = PyLong_FromLongLong (stat->duration);
  PyDict_SetItemString (od, "duration", tmp);

  list = PyList_New (0);

  stream = stat->streams;
  while(stream) {
    tmp = PyString_FromString (stream->stream);
    PyList_Append (list, tmp);

    stream=stream->next;
  }

  PyDict_SetItemString (od, "streams", list);
  
  soma_local_stat_free(stat);

  return od;
}

static PyObject *
get_local_stat_dir (PyObject * self, PyObject * args)
{
  soma_stat_dir *dir;
  soma_stat *stat;
  soma_stat_stream *stream;
  soma_stat_list *list;
  char *s;
  int i;
  PyObject *od, *tmp, *l, *obj, *od_stat;

  if (!PyArg_ParseTuple (args, "si:get_stat_dir", &s, &i))
    return NULL;

  if(!(dir = soma_local_stat_dir (s, i)))
    return Py_None;

  od = PyDict_New ();

  tmp = PyString_FromString (dir->dirname);
  PyDict_SetItemString (od, "dirname", tmp);

  tmp = PyLong_FromLongLong (dir->duration);
  PyDict_SetItemString (od, "duration", tmp);

  l = PyList_New (0);

  list = dir->list;
  while(list) {
    stat=list->stat;

    od_stat = PyDict_New ();
  
    tmp = PyString_FromString (stat->filename);
    PyDict_SetItemString (od_stat, "filename", tmp);
  
    tmp = PyString_FromString (stat->title);
    PyDict_SetItemString (od_stat, "title", tmp);
  
    tmp = PyString_FromString (stat->author);
    PyDict_SetItemString (od_stat, "author", tmp);
  
    tmp = PyString_FromString (stat->copyright);
    PyDict_SetItemString (od_stat, "copyright", tmp);
  
    tmp = PyString_FromString (stat->comment);
    PyDict_SetItemString (od_stat, "comment", tmp);
  
    tmp = PyString_FromString (stat->album);
    PyDict_SetItemString (od_stat, "album", tmp);
  
    tmp = PyInt_FromLong (stat->year);
    PyDict_SetItemString (od_stat, "year", tmp);
  
    tmp = PyInt_FromLong (stat->track);
    PyDict_SetItemString (od_stat, "track", tmp);
  
    tmp = PyString_FromString (stat->genre);
    PyDict_SetItemString (od_stat, "genre", tmp);
  
    tmp = PyLong_FromLongLong (stat->start_time);
    PyDict_SetItemString (od_stat, "start_time", tmp);
  
    tmp = PyLong_FromLongLong (stat->duration);
    PyDict_SetItemString (od_stat, "duration", tmp);
  
    obj = PyList_New (0);
  
    stream = stat->streams;
    while(stream) {
      tmp = PyString_FromString (stream->stream);
      PyList_Append (obj, tmp);
  
      stream=stream->next;
    }
  
    PyDict_SetItemString (od_stat, "streams", obj);
  
    PyList_Append (l, od_stat);

    list=list->next;
  }

  PyDict_SetItemString (od, "stats", l);

  soma_local_stat_dir_free(dir);

  return od;
}

static PyObject *
remove_item (PyObject * self, PyObject * args)
{
  somaobject *ob = (somaobject *) self;
  int s;

  if (!PyArg_ParseTuple (args, "i:nextitem_remove", &s))
    return NULL;

  soma_remove_item (ob->controller, s);
  if (check_error (soma_error (ob->controller)))
    return NULL;

  return Py_None;
}

static PyObject *
remove_spot (PyObject * self, PyObject * args)
{
  somaobject *ob = (somaobject *) self;
  int s;

  if (!PyArg_ParseTuple (args, "i:nextspot_remove", &s))
    return NULL;

  soma_remove_spot (ob->controller, s);
  if (check_error (soma_error (ob->controller)))
    return NULL;

  return Py_None;
}

static PyObject *
set_pause (PyObject * self, PyObject * args)
{
  somaobject *ob = (somaobject *) self;

  soma_set_pause (ob->controller);
  if (check_error (soma_error (ob->controller)))
    return NULL;

  return Py_None;
}

static PyObject *
set_unpause (PyObject * self, PyObject * args)
{
  somaobject *ob = (somaobject *) self;

  soma_set_unpause (ob->controller);
  if (check_error (soma_error (ob->controller)))
    return NULL;

  return Py_None;
}

static PyObject *
get_pause (PyObject * self, PyObject * args)
{
  somaobject *ob = (somaobject *) self;
  PyObject *od;
  int p;

  soma_get_pause (ob->controller, &p);
  if (check_error (soma_error (ob->controller)))
    return NULL;

  od = PyInt_FromLong (p);
  return od;
}

static PyObject *
get_stop (PyObject * self, PyObject * args)
{
  somaobject *ob = (somaobject *) self;
  PyObject *od;
  int p;

  soma_get_stop (ob->controller, &p);
  if (check_error (soma_error (ob->controller)))
    return NULL;

  od = PyInt_FromLong (p);
  return od;
}

static PyObject *
get_item_list (PyObject * self)
{
  somaobject *ob = (somaobject *) self;
  char **list, **old;
  PyObject *ol, *str;

  old=list = soma_get_item_list (ob->controller);
  if (check_error (soma_error (ob->controller)))
    return NULL;

  if(!old) return Py_None;

  ol = PyList_New (0);

  while (*list)
    {
      str = PyString_FromString (*list);
      PyList_Append (ol, str);
      list++;
    }

  soma_item_list_free(old);

  return ol;
}

static PyObject *
get_spot_list (PyObject * self)
{
  somaobject *ob = (somaobject *) self;
  char **list, **old;
  PyObject *ol, *str;

  old=list = soma_get_spot_list (ob->controller);
  if (check_error (soma_error (ob->controller)))
    return NULL;

  if(!old) return Py_None;

  ol = PyList_New (0);

  while (*list)
    {
      str = PyString_FromString (*list);
      PyList_Append (ol, str);
      list++;
    }

  soma_spot_list_free(old);

  return ol;
}

static PyObject *
error (PyObject * self)
{
  somaobject *ob = (somaobject *) self;
  return PyInt_FromLong ((long) soma_error (ob->controller));
}

static void
soma_dealloc (somaobject * self)
{
  if (self->controller != NULL)
    soma_free (self->controller);

  PyObject_Del(self);
}

static PyObject *soma_repr(somaobject *self) {
  return status((PyObject *)self);
}

PyDoc_STRVAR(open_unix__doc__, "open a connection with somad on UnixSocket.\n"
"This function needs unix_socket_path, password and ssl boolean value");

PyDoc_STRVAR(open_tcp__doc__, "open a connection with somad on TcpSocket.\n"
"This function needs server, port, password and ssl boolean value");

PyDoc_STRVAR(status__doc__,"return the status of somad");
PyDoc_STRVAR(palinsesto_name__doc__,"return the palinsesto name of somad");
PyDoc_STRVAR(get_item__doc__,"return the current item of somad");
PyDoc_STRVAR(get_item_next__doc__,"return the following item of somad");

PyDoc_STRVAR(read_palinsesto__doc__, "refresh the palinsesto data");
PyDoc_STRVAR(read_directory__doc__, "refresh the directory data");
PyDoc_STRVAR(read_spot__doc__, "refresh the spot data");

PyDoc_STRVAR(new_palinsesto_file__doc__, "send to somad a new palinsesto.\nWith argoment needs a path file");
PyDoc_STRVAR(new_palinsesto__doc__, "send to somad a new palinsesto.\nWith argoment needs a string contains the palinsesto");
PyDoc_STRVAR(get_palinsesto__doc__, "return the current palinsesto of somad");
PyDoc_STRVAR(get_old_palinsesto__doc__, "return the old palinsesto of somad");
PyDoc_STRVAR(old_palinsesto__doc__, "set as current palinsesto of somad the configured file");
PyDoc_STRVAR(set_default_palinsesto__doc__, "set the current palinsesto as default");

PyDoc_STRVAR(new_spot_file__doc__, "send to somad a new spot.\nWith argoment needs a path file");
PyDoc_STRVAR(new_spot__doc__, "send to somad a new spot.\nWith argoment needs a string contains the spot");
PyDoc_STRVAR(get_spot__doc__, "return the current spot of somad");
PyDoc_STRVAR(get_old_spot__doc__, "return the old spot of somad");
PyDoc_STRVAR(old_spot__doc__, "set as current spot of somad the configured file");
PyDoc_STRVAR(set_default_spot__doc__, "set the current spot as default");

PyDoc_STRVAR(check_password__doc__, "check the password between you and somad. It realy stupid :)");

PyDoc_STRVAR(quit__doc__, "close somad");
PyDoc_STRVAR(skip__doc__, "skip the current item");
PyDoc_STRVAR(skip_next__doc__, "skip the follow item");
PyDoc_STRVAR(stop__doc__, "stop somad. If the argoment is != 0, you stop somad fro X seconds");
PyDoc_STRVAR(start__doc__, "start somad after a stop");

PyDoc_STRVAR(running__doc__, "return true if somad is running, else false");
PyDoc_STRVAR(time__doc__, "return the time on somad server");
PyDoc_STRVAR(get_time_play__doc__, "return the time on somad server");

PyDoc_STRVAR(nextitem_set__doc__, "Set a item on the nextitem list of somad. Need as argoment the file.");
PyDoc_STRVAR(nextitem_path_set__doc__, "Set a item on the nextitem list of somad.\n"
"Need as argoment the file from distribuited path");
PyDoc_STRVAR(nextitem_set_spot__doc__, "Set if somad must mix the nextitem with the spot. True or false");
PyDoc_STRVAR(nextitem_get_spot__doc__, "Return true if somad mixs the spot with nextitem list");
PyDoc_STRVAR(nextitem_list__doc__, "Return a list with the nextitem elements");
PyDoc_STRVAR(nextitem_remove__doc__, "Remove from nextitem list an element. The argoment is the ID");

PyDoc_STRVAR(get_path__doc__, "Return a list with the elements for the ask path");

PyDoc_STRVAR(get_stat__doc__, "Return a dict with the stats of a ask file");
PyDoc_STRVAR(get_stat_dir__doc__, "Return a dict with the stats of a ask directory");

PyDoc_STRVAR(get_stat_path__doc__, "Return a dict with the stats from the distributed filesystem of a ask file");
PyDoc_STRVAR(get_stat_dir_path__doc__, "Return a dict with the stats from the distributed filesystem of a ask directory");

PyDoc_STRVAR(get_local_stat__doc__, "Return a dict with the stats of a ask local file");
PyDoc_STRVAR(get_local_stat_dir__doc__, "Return a dict with the stats of a ask local directory");

PyDoc_STRVAR(remove_item__doc__, "Remove a item from the list. The argoment is the ID");
PyDoc_STRVAR(remove_spot__doc__, "Remove a spot from the list. The argoment is the ID");

PyDoc_STRVAR(get_item_list__doc__, "Return the list of items");
PyDoc_STRVAR(get_spot_list__doc__, "Return the list of spots");

PyDoc_STRVAR(set_pause__doc__, "Set the pause");
PyDoc_STRVAR(set_unpause__doc__, "Remove the pause flag");
PyDoc_STRVAR(get_pause__doc__, "Return the pause flag");

PyDoc_STRVAR(get_stop__doc__, "Return the stop flag");

PyDoc_STRVAR(error__doc__, "Return the current error for somad and you code");

static PyMethodDef SOMA_methods[] = {
  {"open_unix", (PyCFunction) open_unix, METH_VARARGS, open_unix__doc__},
  {"open_tcp", (PyCFunction) open_tcp, METH_VARARGS, open_tcp__doc__},

  {"status", (PyCFunction) status, METH_NOARGS, status__doc__},
  {"palinsesto_name", (PyCFunction) palinsesto_name, METH_NOARGS, palinsesto_name__doc__},
  {"get_item", (PyCFunction) get_item, METH_NOARGS, get_item__doc__},
  {"get_item_next", (PyCFunction) get_item_next, METH_NOARGS, get_item_next__doc__},

  {"read_palinsesto", (PyCFunction) read_palinsesto, METH_NOARGS, read_palinsesto__doc__},
  {"read_spot", (PyCFunction) read_spot, METH_NOARGS, read_spot__doc__},
  {"read_directory", (PyCFunction) read_directory, METH_NOARGS, read_directory__doc__},

  {"new_palinsesto_file", (PyCFunction) new_palinsesto, METH_VARARGS, new_palinsesto_file__doc__},
  {"new_palinsesto", (PyCFunction) new_palinsesto, METH_VARARGS, new_palinsesto__doc__},
  {"get_palinsesto", (PyCFunction) get_palinsesto, METH_NOARGS, get_palinsesto__doc__},
  {"get_old_palinsesto", (PyCFunction) get_old_palinsesto, METH_NOARGS, get_old_palinsesto__doc__},
  {"old_palinsesto", (PyCFunction) old_palinsesto, METH_NOARGS, old_palinsesto__doc__},
  {"set_default_palinsesto", (PyCFunction) set_default_palinsesto, METH_NOARGS, set_default_palinsesto__doc__},

  {"new_spot_file", (PyCFunction) new_spot, METH_VARARGS, new_spot_file__doc__},
  {"new_spot", (PyCFunction) new_spot, METH_VARARGS, new_spot__doc__},
  {"get_spot", (PyCFunction) get_spot, METH_NOARGS, get_spot__doc__},
  {"get_old_spot", (PyCFunction) get_old_spot, METH_NOARGS, get_old_spot__doc__},
  {"old_spot", (PyCFunction) old_spot, METH_NOARGS, old_spot__doc__},
  {"set_default_spot", (PyCFunction) set_default_spot, METH_NOARGS, set_default_spot__doc__},

  {"check_password", (PyCFunction) check_password, METH_NOARGS, check_password__doc__},

  {"quit", (PyCFunction) quit, METH_NOARGS, quit__doc__},
  {"skip", (PyCFunction) skip, METH_NOARGS, skip__doc__},
  {"skip_next", (PyCFunction) skip_next, METH_NOARGS, skip_next__doc__},

  {"stop", (PyCFunction) cmd_stop, METH_VARARGS, stop__doc__},
  {"start", (PyCFunction) cmd_start, METH_NOARGS, start__doc__},

  {"running", (PyCFunction) running, METH_NOARGS, running__doc__},
  {"time", (PyCFunction) s_time, METH_NOARGS, time__doc__},
  {"get_time_play", (PyCFunction) get_time_play, METH_NOARGS, get_time_play__doc__},

  {"nextitem_set", (PyCFunction) nextitem_set, METH_VARARGS, nextitem_set__doc__},
  {"nextitem_path_set", (PyCFunction) nextitem_path_set, METH_VARARGS, nextitem_path_set__doc__},
  {"nextitem_set_spot", (PyCFunction) nextitem_set_spot, METH_VARARGS, nextitem_set_spot__doc__},
  {"nextitem_get_spot", (PyCFunction) nextitem_get_spot, METH_NOARGS, nextitem_get_spot__doc__},

  {"nextitem_list", (PyCFunction) nextitem_list, METH_NOARGS, nextitem_list__doc__},
  {"nextitem_remove", (PyCFunction) nextitem_remove, METH_VARARGS, nextitem_remove__doc__},

  {"get_path", (PyCFunction) get_path, METH_VARARGS, get_path__doc__},

  {"get_stat", (PyCFunction) get_stat, METH_VARARGS, get_stat__doc__},
  {"get_stat_dir", (PyCFunction) get_stat_dir, METH_VARARGS, get_stat_dir__doc__},

  {"get_stat_path", (PyCFunction) get_stat_path, METH_VARARGS, get_stat_path__doc__},
  {"get_stat_dir_path", (PyCFunction) get_stat_dir_path, METH_VARARGS, get_stat_dir_path__doc__},

  {"get_local_stat", (PyCFunction) get_local_stat, METH_VARARGS, get_local_stat__doc__},
  {"get_local_stat_dir", (PyCFunction) get_local_stat_dir, METH_VARARGS, get_local_stat_dir__doc__},

  {"remove_item", (PyCFunction) remove_item, METH_VARARGS, remove_item__doc__},
  {"remove_spot", (PyCFunction) remove_spot, METH_VARARGS, remove_spot__doc__},

  {"get_item_list", (PyCFunction) get_item_list, METH_NOARGS, get_item_list__doc__},
  {"get_spot_list", (PyCFunction) get_spot_list, METH_NOARGS, get_spot_list__doc__},

  {"set_pause", (PyCFunction) set_pause, METH_VARARGS, set_pause__doc__},
  {"set_unpause", (PyCFunction) set_unpause, METH_VARARGS, set_unpause__doc__},
  {"get_pause", (PyCFunction) get_pause, METH_VARARGS, get_pause__doc__},

  {"get_stop", (PyCFunction) get_stop, METH_VARARGS, get_stop__doc__},
  
  {"error", (PyCFunction) error, METH_NOARGS, error__doc__},

  {NULL, NULL}
};

static PyObject *
soma_getattr (somaobject * self, char *name)
{
  return Py_FindMethod (SOMA_methods, (PyObject *) self, name);
}

PyDoc_STRVAR (soma_doc, "pysoma is a python extension for admin of somad.");

static PyTypeObject Somatype = {
  PyObject_HEAD_INIT (0)
  0,				/*ob_size */
  "soma.data",			/*tp_name */
  sizeof (somaobject),		/*tp_size */
  0,				/*tp_itemsize */
  (destructor) soma_dealloc,	/*tp_dealloc */
  0,				/*tp_print */
  (getattrfunc) soma_getattr,	/*tp_getattr */
  0,				/*tp_setattr */
  0,				/*tp_compare */
  (reprfunc) soma_repr,		/*tp_repr */
  0,				/*tp_as_number */
  0,				/*tp_as_sequence */
  0,				/*tp_as_mapping */
  0,				/*tp_hash */
  0,				/*tp_call */
  0,				/*tp_str */
  0,				/*tp_getattro */
  0,				/*tp_setattro */
  0,				/*tp_as_buffer */
  0,				/*tp_xxx4 */
  soma_doc,			/*tp_doc */
};

PyMODINIT_FUNC
initpysoma (void)
{
  PyObject *m;

  Somatype.ob_type = &PyType_Type;
  m = Py_InitModule ("pysoma", SOMA_methods);

  if(!(SomaError = PyErr_NewException ("soma.error", NULL, NULL))) return;

  Py_INCREF(SomaError);
  PyModule_AddObject(m, "error", SomaError);
}

/* EOF */
