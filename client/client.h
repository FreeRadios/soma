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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/socket.h>
#include <netdb.h>
#include <termios.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#include "../include/commons.h"
#include "../include/controller.h"
#include "../include/code.h"

#define GENERIC_ERROR   0
#define PARAMETER_ERROR 1
#define SOCKET_ERROR	2
#define UNKNOW_HOST	3
#define CONNECT_ERROR	4
#define PROTOCOL_ERROR	5
#define PASSWORD_ERROR	6
#define DONE		7
#define INTERNAL_ERROR	8
#define LOCK_ERROR	9
#define YES_RUNNING	10
#define NO_STOPPED	11
#define YES_NEXTITEM_SPOT 12
#define NO_NEXTITEM_SPOT 13

#define CHECK_INT( a ) { \
	if (a) { \
	  switch(soma_error(controller)) { \
	    case SOMA_ERR_POSIX: puts("Posix error."); exit(1); \
	    case SOMA_ERR_USER: puts("User error."); exit(1); \
	    case SOMA_ERR_PASSWORD: puts("Password error."); exit(1); \
	    case SOMA_ERR_HOST: puts("Host error."); exit(1); \
	    case SOMA_ERR_CONNECT: puts("Connect error."); exit(1); \
	    case SOMA_ERR_PROTOCOL: puts("Protocol error."); exit(1); \
	    case SOMA_ERR_SSL_REQUEST: puts("Somad is running with ssl. Use -ssl flag."); exit(1); \
	    case SOMA_ERR_NO_SSL_REQUEST: puts("Somad is not runnig with ssl. Don't use -ssl flag."); exit(1); \
            default: puts("Internal error."); exit(1); \
	  } \
	} \
     }

/* ReadLine docet: */
typedef struct
{
  char *name;
  rl_icpfunc_t *func;
  char *doc;
}
soma_cmd;

/* Output messages */
typedef struct
{
  int code;
  char *str;
}
code_str;

void usage (char *a);

char *trim (char *tmp);
char **soma_completion (const char *, int, int);
char *command_generator (const char *, int);
soma_cmd *find_cmds (char *);
void echo_off (void);
void echo_on (void);
void fatal (int);
void message (int);
void soma_socket (void);
void shell (void);
int execute (char *);
void get_passwd (char *);
int check_time (char *);

int cmd_exit (char *);
int cmd_shutdown (char *);
int cmd_help (char *);
int cmd_cd (char *);
int cmd_ls (char *);
int cmd_pwd (char *);
int cmd_nextitem(char *);
int cmd_readdirectory(char *);
int cmd_readpalinsesto(char *);
int cmd_readspot(char *);

int cmd_filepalinsesto(char *);
int cmd_getpalinsesto(char *);
int cmd_getoldpalinsesto(char *);
int cmd_defaultpalinsesto(char *);
int cmd_oldpalinsesto(char *);

int cmd_filespot(char *);
int cmd_getspot(char *);
int cmd_getoldspot(char *);
int cmd_defaultspot(char *);
int cmd_oldspot(char *);

int cmd_status(char *);
int cmd_pl_name(char *);
int cmd_get_item(char *);
int cmd_get_item_next(char *);
int cmd_time(char *);
int cmd_quit(char *);
int cmd_skip(char *);
int cmd_skip_next(char *);
int cmd_start(char *);
int cmd_stop(char *);
int cmd_running(char *);

int cmd_remove_item(char *);
int cmd_remove_spot(char *);

int cmd_item_list(char *);
int cmd_spot_list(char *);

int cmd_pause(char *);
int cmd_unpause(char *);
int cmd_get_pause(char *);

int cmd_get_stop(char *);

extern int set_quit;
extern int set_shell;
extern int set_code;
extern struct termios stored;
extern soma_controller *controller;

/* EOF */
