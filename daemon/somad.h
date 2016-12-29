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

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <libxml/parser.h>

#ifdef ENABLE_FFMPEG
#  include <ffmpeg/avcodec.h>
#  include <ffmpeg/avformat.h>
#else
#  include "avcodec.h"
#  include "avformat.h"
#endif

#ifdef ENABLE_SM
#  include "../sm/sm.h"		/* Simple Mutex library */
#endif

#include <pthread.h>
#include <signal.h>

#ifdef ENABLE_CONFUSE
#  include "../confuse/confuse.h"		/* Libconfuse */
#else
#  include <confuse.h>
#endif

#include "../ghthash/ght_hash_table.h"	/* Libghthash */
#include <sys/wait.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <fcntl.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <termios.h>
#include <sched.h>
#include <errno.h>
#include <dlfcn.h>

#ifdef SOMA_USE_OPENSSL
#  include <openssl/rsa.h>
#  include <openssl/crypto.h>
#  include <openssl/ssl.h>
#  include <openssl/err.h>
#  include <openssl/x509.h>
#  include <openssl/pem.h>
#endif

#include "../include/soma.h"

#define getrandom(min, max) ((rand() % (int)(((max)+1) - (min))) + (min))

#define SOMA_DEV_NULL "/dev/null"

/* Cond variable */
#ifdef ENABLE_SM
extern sm mutex;
extern sm log_mutex;
extern sm hash_mutex;
#else
extern pthread_mutex_t mutex;
extern pthread_mutex_t log_mutex;
extern pthread_mutex_t hash_mutex;
#endif

typedef struct string_t string_t;
struct string_t  {
  char *string;
  size_t size;
};

extern cfg_t *cfg;
extern char **env, **arg;
extern soma_data *conf;
extern int id_item, id_spot, pid;
extern char set_quit, set_time_stop, set_skip;
extern int set_pause, set_stop, set_in_pause;
extern soma_pl *palinsesto;
extern void *module;
extern int (*internal_module_init) (char *);
extern char *(*internal_module_run) (char *);
extern char **(*internal_module_exec) (char *);
extern soma_item *(*internal_module_list) (char *);
extern int (*internal_module_quit) (char *);
extern char *(*internal_module_name) (void);
extern int (*internal_module_time_stop) (char *);

/* play.c */
int play (int, char *, int);
int play_exec (int, char **);

/* signal.c */
void signal_off (void);
void signal_on (void);
void signal_small_off (void);
void signal_small_on (void);

/* data.c */
int add_item (int, char *);
int add_file (int, char *);
int add_directory (int, char *);
void remove_item (int, int);
int search_item (int, int, int);

/* playlist.c */
int playlist_is(char *);
int add_playlist(int, char *);

/* build.c */
int build_data (void);
int build (void);
int delete_pl (void);
int build_pl (void);
void build_tree (soma_item *, int);

/* palinsesto.c */
int build_palinsesto (void);

/* list_dir.c */
void soma_item_add (int, char *);

/* options.c */
void soma_options (int, char *);
char *soma_search_path(char *);

/* list_pl.c */
soma_pl *soma_pl_add (char *, char *, int, char *, int, int, int, int, int, 
		int, char *, char *, char *, char *, char *, int, char *, 
		char *);
void soma_item_pl_add (soma_pl *, int, char *);
soma_time *soma_parse_time (char *, char *, int);

/* start.c */
void start (void);
int start_check (soma_time *);

/* socket.c */
void *soma_socket (void *);

/* pid.c */
void write_pid (void);
void check_pid (void);
void close_pid (void);

/* file.c */
char *soma_mktemp (void);
int remote_file (char *);
int distribuited_file (char *);

/* echo.c */
void echo_off (void);
void echo_on (void);

/* module.c */
void *check_module (char *str);
void clear_module (void);

/* log.c */
void log_init (void);
void log_quit (void);
void log_write (int, char *, ...);
char * soma_trim (char *);
void log_restart (int);
void log_item_set(char *);
void log_item_empty(void);
char *log_item_next_set(int *, char *, int);
void log_item_next_empty(int);
void log_item_next_empty_nl(int);

/* nextitem.c */
void nextitem_add (char *);
void nextitem_set_mix(int);
void nextitem_path_add (char *);
void nextitem_remove (void);
void nextitem_remove_last (void);
void nextitem_remove_first (void);
void nextitem_remove_n (int);

/* distribuited.c */
char *distribuited_get_path(char *);
char *distribuited_trim(char *);
string_t *distribuited_write (char *);
string_t *distribuited_stat (char *);
string_t *distribuited_stat_dir (char *);

/* spot.c */
spot_data *spot_data_add (char *, char *, char *, int, int);
void spot_data_dir_add (spot_data *, char *);
void spot_controller(int, int);
int build_spot (void);

/* denyallow.c */
int denyallow_parser(int);
void denyallow_add(int, char *ip);

/* stat.c */
void soma_stat_init(void);
soma_stat *soma_stat_get (char *);
void soma_stat_free (soma_stat *);
void soma_stat_dir_free (soma_stat_dir *);
soma_stat_dir *soma_stat_get_dir(char *, int);
string_t *stat_normal (char *file);
string_t *stat_normal_dir (char *file);
string_t *stat_string (soma_stat * stat);
string_t *stat_string_dir(soma_stat_dir *stat);

/* string.c */
string_t *string_new (void);
void string_destroy(string_t *str);
int string_add(string_t *str, char *what, size_t size);
int string_printf(string_t *str,  char *what, ...);

#ifdef ENABLE_SM
#  define INIT()   sm_init (&mutex)
#  define LOCK()   sm_lock (&mutex)
#  define UNLOCK() sm_unlock (&mutex)
#else
#  define INIT()   pthread_mutex_init (&mutex, NULL)
#  define LOCK()   pthread_mutex_lock (&mutex)
#  define UNLOCK() pthread_mutex_unlock (&mutex)
#endif

#ifdef ENABLE_SM
#  define LOG_INIT()   sm_init (&log_mutex)
#  define LOG_LOCK()   sm_lock (&log_mutex)
#  define LOG_UNLOCK() sm_unlock (&log_mutex)
#else
#  define LOG_INIT()   pthread_mutex_init (&log_mutex, NULL)
#  define LOG_LOCK()   pthread_mutex_lock (&log_mutex)
#  define LOG_UNLOCK() pthread_mutex_unlock (&log_mutex)
#endif

#ifdef ENABLE_SM
#  define HASH_INIT()   sm_init (&hash_mutex)
#  define HASH_LOCK()   sm_lock (&hash_mutex)
#  define HASH_UNLOCK() sm_unlock (&hash_mutex)
#else
#  define HASH_INIT()   pthread_mutex_init (&hash_mutex, NULL)
#  define HASH_LOCK()   pthread_mutex_lock (&hash_mutex)
#  define HASH_UNLOCK() pthread_mutex_unlock (&hash_mutex)
#endif

/* Debug && Log */
#define SOMA_LOG_ALL(A...) log_write(SOMA_DEBUG_ALL, A)

#define SOMA_LOG_INFO(A...) log_write(SOMA_DEBUG_INFO, A)

#define SOMA_LOG_WARN(A...) log_write(SOMA_DEBUG_WARN, A)

#define SOMA_LOG_ERR(A...) log_write(SOMA_DEBUG_ERR, A)

#define SOMA_NO_LOG_ALL(A...) log_write(SOMA_DEBUG_NO_ALL, A)

#define SOMA_NO_LOG_INFO(A...) log_write(SOMA_DEBUG_NO_INFO, A)

#define SOMA_NO_LOG_WARN(A...) log_write(SOMA_DEBUG_NO_WARN, A)

#define SOMA_NO_LOG_ERR(A...) log_write(SOMA_DEBUG_NO_ERR, A)

#define SOMA_FATAL(A...) { \
  log_write(0, A); \
  if (pid) \
    { \
      kill (pid, SIGQUIT); \
    } \
  exit (1); \
}

/* EOF */

