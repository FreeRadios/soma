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

#ifndef __SOMA_COMMON_H__
#define __SOMA_COMMON_H__

#include <sys/types.h>
#include <unistd.h>

#define SOMA_VERSION 	"2.4"
#define SOMA_AUTHOR 	"Andrea Marchesini"
#define SOMA_AUTHOR_EMAIL "bakunin <bakunin@autistici.org>"
#define SOMA_COPYRIGHT 	"Copyright (C) 2003-7"

#define SOMA_FILE_CFG	"/etc/somad/soma.cfg"
#define SOMA_FILE_PID	"/var/run/somad.pid"
#define SOMA_TMP_DIR	"/tmp"
#define SOMA_PATH_MOD	"/usr/share/soma"
#define SOMA_UNIX_SOCK	"/tmp/somad.sock"
#define SOMA_PASS_LEN	20
#define SOMA_PROC_DIR	"/proc"

#define SOMA_PORT    	12521
#define SOMA_LISTEN    	5
#define SOMA_MAX_BUFF  	1024

#define SOMA_HOST_DENY		0
#define SOMA_HOST_ALLOW		1

#define SOMA_DENYALLOW_NUMBER		0
#define SOMA_DENYALLOW_CONDITION	1
#define SOMA_DENYALLOW_NETMASK		2

#define SOMA_PATH_ITEM		0
#define SOMA_PATH_SPOT		1
#define SOMA_DISTRIBUITED_PATH	2

/** 
 * If the first char of a item is DISTRIBUTED_CHAR it is part of the
 * distribuited filesystem
 */
#define DISTRIBUITED_CHAR	':'

#define SOMA_TIME_BASE		1000000

#define SOMA_CONFIG_STRUCT \
    CFG_STR ("User", NULL, CFGF_NONE), \
    CFG_STR ("Group", NULL, CFGF_NONE), \
    CFG_BOOL ("Background", cfg_false, CFGF_NONE), \
    \
    CFG_BOOL ("Ssl", cfg_false, CFGF_NONE), \
    CFG_STR ("Certificate", NULL, CFGF_NONE), \
    CFG_STR ("PrivateKey", NULL, CFGF_NONE), \
    \
    CFG_BOOL ("UnixSocket", cfg_false, CFGF_NONE), \
    CFG_STR ("UnixPath", NULL, CFGF_NONE), \
    \
    CFG_STR ("ServerName", NULL, CFGF_NONE), \
    CFG_INT ("Port", SOMA_PORT, CFGF_NONE), \
    CFG_INT ("Listen", SOMA_LISTEN, CFGF_NONE), \
    \
    CFG_STR ("Password", NULL, CFGF_NONE), \
    \
    CFG_INT ("Debug", 3, CFGF_NONE), \
    \
    CFG_STR ("LogFile", NULL, CFGF_NONE), \
    CFG_STR ("PidFile", NULL, CFGF_NONE), \
    \
    CFG_STR_LIST ("PathItem", "{}", CFGF_NONE), \
    CFG_STR_LIST ("PathSpot", "{}", CFGF_NONE), \
    \
    CFG_STR ("ProgramItem", NULL, CFGF_NONE), \
    CFG_STR ("OptionsItem", NULL, CFGF_NONE), \
    CFG_STR ("ProgramStream", NULL, CFGF_NONE), \
    CFG_STR ("OptionsStream", NULL, CFGF_NONE), \
    \
    CFG_STR ("Palinsesto", NULL, CFGF_NONE), \
    CFG_STR ("Spot", NULL, CFGF_NONE), \
    \
    CFG_STR ("PathModules", NULL, CFGF_NONE), \
    \
    CFG_STR_LIST ("DistribuitedPath", NULL, CFGF_NONE), \
    \
    CFG_BOOL ("XmlSyntax", cfg_true, CFGF_NONE), \
    \
    CFG_BOOL ("SymLinks", cfg_false, CFGF_NONE), \
    \
    CFG_STR_LIST ("HostDeny", "{}", CFGF_NONE), \
    CFG_STR_LIST ("HostAllow", "{}", CFGF_NONE), \
    \
    CFG_END ()

/* Directories/Item of audio file */
typedef struct soma_item soma_item;
struct soma_item
{
  char *item;
  struct soma_item *next;
};

typedef struct soma_stat_stream soma_stat_stream;
struct soma_stat_stream
{
  char stream[1024];
  struct soma_stat_stream *next;
};

typedef struct soma_stat soma_stat;
struct soma_stat
{
  char filename[1024];
  char title[1024];
  char author[1024];
  char copyright[1024];
  char comment[1024];
  char album[1024];
  int year;
  int track;
  char genre[1024];
  int64_t start_time;
  int64_t duration;
  int bitrate;
  soma_stat_stream *streams;
};

typedef struct soma_stat_list soma_stat_list;
struct soma_stat_list
{
  soma_stat *stat;
  struct soma_stat_list *next;
};

typedef struct soma_stat_dir soma_stat_dir;
struct soma_stat_dir
{
  char dirname[1024];
  int64_t duration;
  soma_stat_list *list;
};

#endif

/* EOF */
