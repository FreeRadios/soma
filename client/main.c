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

#include "client.h"

int set_quit = 0;
int set_shell = 0;
int set_code = 0;
int set_ssl = 0;
int unix_sock = 0;

struct termios stored;

soma_controller *controller;

int
main (int argc, char *argv[], char **arge)
{
  char *server = NULL;
  char *pass = NULL;
  char buff[1024];
  int port = 0;
  int a;
  int s_cmd = 0;
  char *arg = NULL;
  int stop = 0;
  char set_spot = 0;

  if (argc > 1)
    {

      for (a = 1; a < argc; a++)
	{
#ifdef SOMA_USE_OPENSSL
	  if (!strcmp (argv[a], "-ssl"))
	    {
	      set_ssl = 1;
	    }

	  else
#endif
	  if (!strcmp (argv[a], "-S") || !strcmp (argv[a], "--server"))
	    {
	      a++;
	      if (argv[a])
		server = strdup (argv[a]);
	      else
		fatal (1);

	      unix_sock = 0;
	    }

	  else if (!strcmp (argv[a], "-U") || !strcmp (argv[a], "--unix"))
	    {
	      a++;
	      if (argv[a])
		server = argv[a];
	      else
		fatal (1);

	      unix_sock = 1;
	    }

	  else if (!strcmp (argv[a], "-P") || !strcmp (argv[a], "--port"))
	    {
	      a++;
	      if (argv[a])
		port = atoi (argv[a]);
	      else
		fatal (1);
	    }

	  else if (!strcmp (argv[a], "-ns")
		   || !strcmp (argv[a], "--nextitem"))
	    {
	      a++;
	      if (argv[a])
		arg = argv[a];
	      else
		fatal (1);

	      s_cmd = SOMA_CODE_NEXT_ITEM_SET;
	    }

	  else if (!strcmp (argv[a], "-nsgs")
		   || !strcmp (argv[a], "--nextitemgetspot"))
	    {
	      s_cmd = SOMA_CODE_NEXT_ITEM_GET_SPOT;
	    }

	  else if (!strcmp (argv[a], "-nsss")
		   || !strcmp (argv[a], "--nextitemsetspot"))
	    {
	      s_cmd = SOMA_CODE_NEXT_ITEM_SET_SPOT;

	      a++;
	      if (argv[a])
		set_spot = atoi (argv[a]);

	      else
		{
		  set_spot = 1;
		  a--;
		}
	    }

	  else if (!strcmp (argv[a], "-nsl")
		   || !strcmp (argv[a], "--nextitemlist"))
	    {
	      s_cmd = SOMA_CODE_NEXT_ITEM_LIST;
	    }

	  else if (!strcmp (argv[a], "-nsr")
		   || !strcmp (argv[a], "--nextitemremove"))
	    {

	      a++;
	      if (argv[a])
		arg = argv[a];
	      else
		fatal (1);

	      s_cmd = SOMA_CODE_NEXT_ITEM_REMOVE;
	    }

	  else if (!strcmp (argv[a], "-nsgp")
		   || !strcmp (argv[a], "--nextitemgetpath"))
	    {

	      a++;
	      if (argv[a])
		arg = argv[a];
	      else
		fatal (1);

	      s_cmd = SOMA_CODE_GET_PATH;
	    }

	  else if (!strcmp (argv[a], "-fp")
		   || !strcmp (argv[a], "--filepalinsesto"))
	    {
	      a++;
	      if (argv[a])
		arg = argv[a];
	      else
		fatal (1);

	      s_cmd = SOMA_CODE_NEW_PL;
	    }

	  else if (!strcmp (argv[a], "-fs")
		   || !strcmp (argv[a], "--filespot"))
	    {
	      a++;
	      if (argv[a])
		arg = argv[a];
	      else
		fatal (1);

	      s_cmd = SOMA_CODE_NEW_SPOT;
	    }

	  else if (!strcmp (argv[a], "-rd")
		   || !strcmp (argv[a], "--readdirectory"))
	    {
	      s_cmd = SOMA_CODE_READ_DIR;
	    }

	  else if (!strcmp (argv[a], "-rp")
		   || !strcmp (argv[a], "--readpalinsesto"))
	    {
	      s_cmd = SOMA_CODE_READ_PL;
	    }

	  else if (!strcmp (argv[a], "-rs")
		   || !strcmp (argv[a], "--readspot"))
	    {
	      s_cmd = SOMA_CODE_READ_SPOT;
	    }

	  else if (!strcmp (argv[a], "-op")
		   || !strcmp (argv[a], "--oldpalinsesto"))
	    {
	      s_cmd = SOMA_CODE_OLD_PL;
	    }

	  else if (!strcmp (argv[a], "-gp")
		   || !strcmp (argv[a], "--getpalinsesto"))
	    {
	      s_cmd = SOMA_CODE_GET_PL;
	    }

	  else if (!strcmp (argv[a], "-gop")
		   || !strcmp (argv[a], "--getoldpalinsesto"))
	    {
	      s_cmd = SOMA_CODE_GET_OLD_PL;
	    }

	  else if (!strcmp (argv[a], "-dp")
		   || !strcmp (argv[a], "--setdefaultpalinsesto"))
	    {
	      s_cmd = SOMA_CODE_DEFAULT_PL;
	    }

	  else if (!strcmp (argv[a], "-os") || !strcmp (argv[a], "--oldspot"))
	    {
	      s_cmd = SOMA_CODE_OLD_SPOT;
	    }

	  else if (!strcmp (argv[a], "-gs") || !strcmp (argv[a], "--getspot"))
	    {
	      s_cmd = SOMA_CODE_GET_SPOT;
	    }

	  else if (!strcmp (argv[a], "-gos")
		   || !strcmp (argv[a], "--getoldspot"))
	    {
	      s_cmd = SOMA_CODE_GET_OLD_SPOT;
	    }

	  else if (!strcmp (argv[a], "-ds")
		   || !strcmp (argv[a], "--setdefaultspot"))
	    {
	      s_cmd = SOMA_CODE_DEFAULT_SPOT;
	    }

	  else if (!strcmp (argv[a], "-pw")
		   || !strcmp (argv[a], "--password"))
	    {
	      s_cmd = SOMA_CODE_PASSWORD;
	    }

	  else if (!strcmp (argv[a], "-tm") || !strcmp (argv[a], "--time"))
	    {
	      s_cmd = SOMA_CODE_TIME;
	    }

	  else if (!strcmp (argv[a], "-st") || !strcmp (argv[a], "--status"))
	    {
	      s_cmd = SOMA_CODE_STATUS;
	    }

	  else if (!strcmp (argv[a], "-gi")
		   || !strcmp (argv[a], "--get_item"))
	    {
	      s_cmd = SOMA_CODE_GET_ITEM;
	    }

	  else if (!strcmp (argv[a], "-gin")
		   || !strcmp (argv[a], "--get_item_next"))
	    {
	      s_cmd = SOMA_CODE_GET_FOLLOW_ITEM;
	    }

	  else if (!strcmp (argv[a], "-q") || !strcmp (argv[a], "--quit"))
	    {
	      s_cmd = SOMA_CODE_QUIT;
	    }

	  else if (!strcmp (argv[a], "-sk") || !strcmp (argv[a], "--skip"))
	    {
	      s_cmd = SOMA_CODE_SKIP;
	    }

	  else if (!strcmp (argv[a], "-skn")
		   || !strcmp (argv[a], "--skipnext"))
	    {
	      s_cmd = SOMA_CODE_SKIP_FOLLOW_ITEM;
	    }

	  else if (!strcmp (argv[a], "-1") || !strcmp (argv[a], "--stop"))
	    {
	      {
		s_cmd = SOMA_CODE_STOP;

		a++;
		if (argv[a] && atoi (argv[a]))
		  stop = check_time (argv[a]);

		else
		  {
		    stop = -1;
		    a--;
		  }
	      }

	    }
	  else if (!strcmp (argv[a], "-0") || !strcmp (argv[a], "--start"))
	    {
	      s_cmd = SOMA_CODE_START;
	    }

	  else if (!strcmp (argv[a], "-r") || !strcmp (argv[a], "--running"))
	    {
	      s_cmd = SOMA_CODE_RUNNING;
	    }

	  else if (!strcmp (argv[a], "-c") || !strcmp (argv[a], "--code"))
	    {
	      set_code = 1;
	    }

	  else if (!strcmp (argv[a], "-pn")
		   || !strcmp (argv[a], "--palinsestoname"))
	    {
	      s_cmd = SOMA_CODE_GET_PL_NAME;
	    }

	  else if (!strcmp (argv[a], "-lso")
		   || !strcmp (argv[a], "--listitem"))
	    {
	      s_cmd = SOMA_CODE_GET_ITEM_LIST;
	    }

	  else if (!strcmp (argv[a], "-lsp")
		   || !strcmp (argv[a], "--listspot"))
	    {
	      s_cmd = SOMA_CODE_GET_SPOT_LIST;
	    }

	  else if (!strcmp (argv[a], "-rso")
		   || !strcmp (argv[a], "--removeitem"))
	    {

	      a++;
	      if (argv[a])
		arg = argv[a];
	      else
		fatal (1);

	      s_cmd = SOMA_CODE_REMOVE_ITEM;
	    }

	  else if (!strcmp (argv[a], "-rsp")
		   || !strcmp (argv[a], "--removespot"))
	    {

	      a++;
	      if (argv[a])
		arg = argv[a];
	      else
		fatal (1);

	      s_cmd = SOMA_CODE_REMOVE_SPOT;
	    }

	  else if (!strcmp (argv[a], "-sp")
		   || !strcmp (argv[a], "--setpause"))
	    {
	      s_cmd = SOMA_CODE_PAUSE;
	    }

	  else if (!strcmp (argv[a], "-up")
		   || !strcmp (argv[a], "--unsetpause"))
	    {
	      s_cmd = SOMA_CODE_UNPAUSE;
	    }

	  else if (!strcmp (argv[a], "-gps")
		   || !strcmp (argv[a], "--getpause"))
	    {
	      s_cmd = SOMA_CODE_GET_PAUSE;
	    }

	  else if (!strcmp (argv[a], "-gss")
		   || !strcmp (argv[a], "--getstop"))
	    {
	      s_cmd = SOMA_CODE_GET_STOP;
	    }

	  else if (!strcmp (argv[a], "-h") || !strcmp (argv[a], "--help"))
	    {
	      usage (argv[0]);
	      return 0;
	    }

	  else if (!strncmp (argv[a], "-p", 2))
	    {
	      strcpy (buff, argv[a] + 2);
#ifdef HAVE_WRITABLE_ARGV
	      strncpy (argv[a], "-p*****", strlen (argv[a]));
#endif

	      pass = buff;
	    }

	  else
	    fatal (PARAMETER_ERROR);
	}
    }

  if (!server)
    server = "localhost";

  /* Activing shell mode, if no single command */
  if (!s_cmd)
    set_shell = 1;

  if (!set_code)
    fprintf (stdout, "Soma %s - %s - %s\n\n", SOMA_VERSION, SOMA_COPYRIGHT,
	     SOMA_AUTHOR_EMAIL);

  /* If no password (with -p option), i ask it */
  if (!pass)
    {
      get_passwd (buff);
      pass = buff;
    }

  if (unix_sock)
    controller = soma_open_unix (server, pass, set_ssl);
  else
    controller = soma_open_tcp (server, port, pass, set_ssl);

  CHECK_INT (soma_error (controller));

  /* Activing shell */
  if (set_shell)
    {
      if (soma_check_password (controller))
	fatal (PASSWORD_ERROR);
      shell ();
      return 0;
    }

  switch (s_cmd)
    {
    case SOMA_CODE_READ_PL:
      CHECK_INT (soma_read_palinsesto (controller));
      break;

    case SOMA_CODE_READ_SPOT:
      CHECK_INT (soma_read_spot (controller));
      break;

    case SOMA_CODE_READ_DIR:
      CHECK_INT (soma_read_directory (controller));
      break;

    case SOMA_CODE_NEW_PL:
      CHECK_INT (soma_new_palinsesto_file (controller, arg));
      break;

    case SOMA_CODE_OLD_PL:
      CHECK_INT (soma_old_palinsesto (controller));
      break;

    case SOMA_CODE_GET_PL:
      {
	char *buf = soma_get_palinsesto (controller);
	if (buf)
	  puts (buf);
      }
      break;

    case SOMA_CODE_GET_OLD_PL:
      {
	char *buf = soma_get_old_palinsesto (controller);
	if (buf)
	  puts (buf);
      }
      break;

    case SOMA_CODE_DEFAULT_PL:
      CHECK_INT (soma_set_default_palinsesto (controller));
      break;

    case SOMA_CODE_NEW_SPOT:
      CHECK_INT (soma_new_spot_file (controller, arg));
      break;

    case SOMA_CODE_OLD_SPOT:
      CHECK_INT (soma_old_spot (controller));
      break;

    case SOMA_CODE_GET_SPOT:
      {
	char *buf = soma_get_spot (controller);
	if (buf)
	  puts (buf);
      }
      break;

    case SOMA_CODE_GET_OLD_SPOT:
      {
	char *buf = soma_get_old_spot (controller);
	if (buf)
	  puts (buf);
      }
      break;

    case SOMA_CODE_DEFAULT_SPOT:
      CHECK_INT (soma_set_default_spot (controller));
      break;

    case SOMA_CODE_PASSWORD:
      /* Already checked! */
      break;

    case SOMA_CODE_STATUS:
      {
	char *buf = soma_status (controller);
	if (buf)
	  puts (buf);
      }
      break;

    case SOMA_CODE_GET_ITEM:
      {
	char *buf = soma_get_item (controller);
	if (buf)
	  puts (buf);
      }
      break;

    case SOMA_CODE_GET_FOLLOW_ITEM:
      {
	char *buf = soma_get_item_next (controller);
	if (buf)
	  puts (buf);
      }
      break;

    case SOMA_CODE_TIME:
      {
	time_t t = soma_time (controller);
	if (t)
	  printf ("%ld\n", t);
      }
      break;

    case SOMA_CODE_QUIT:
      CHECK_INT (soma_quit (controller));
      break;

    case SOMA_CODE_SKIP:
      CHECK_INT (soma_skip (controller));
      break;

    case SOMA_CODE_SKIP_FOLLOW_ITEM:
      CHECK_INT (soma_skip_next (controller));
      break;

    case SOMA_CODE_STOP:
      CHECK_INT (soma_stop (controller, stop));
      break;

    case SOMA_CODE_START:
      CHECK_INT (soma_start (controller));
      break;

    case SOMA_CODE_RUNNING:
      if (soma_running (controller))
	message (YES_RUNNING);
      else
	message (NO_STOPPED);
      break;

    case SOMA_CODE_NEXT_ITEM_GET_SPOT:
      if (soma_nextitem_get_spot (controller))
	message (YES_NEXTITEM_SPOT);
      else
	message (NO_NEXTITEM_SPOT);
      break;

    case SOMA_CODE_NEXT_ITEM_SET_SPOT:
      CHECK_INT (soma_nextitem_set_spot (controller, set_spot));
      break;

    case SOMA_CODE_NEXT_ITEM_SET:
      {
	char s[1024];

	if (*arg != '/' && *arg != ':')
	  {
	    getcwd (s, sizeof (s));
	    strncat (s, "/", sizeof (s));
	    strncat (s, arg, sizeof (s));
	    CHECK_INT (soma_nextitem_set (controller, s));
	  }
	else
	  {
	    CHECK_INT (soma_nextitem_set (controller, arg));
	  }
      }
      break;

    case SOMA_CODE_NEXT_ITEM_REMOVE:
      {
	int id;

	if (!strcmp (arg, "last"))
	  id = -1;
	else if (!strcmp (arg, "first"))
	  id = 0;
	else
	  id = atoi (arg);

	CHECK_INT (soma_nextitem_remove (controller, id));
      }
      break;

    case SOMA_CODE_NEXT_ITEM_LIST:
      {
	char **list = soma_nextitem_list (controller);

	if (list)
	  {
	    char **old = list;

	    while (*list)
	      {
		puts (*list);
		list++;
	      }

	    soma_nextitem_list_free (old);
	  }
      }
      break;

    case SOMA_CODE_GET_PATH:
      {
	char **buf, **old;

	buf = old = soma_get_path (controller, arg);

	if (buf)
	  {
	    while (*buf)
	      {
		puts (*buf);
		buf++;
	      }
	  }

	soma_get_path_free (old);
      }
      break;

    case SOMA_CODE_GET_PL_NAME:
      {
	char *buf = soma_palinsesto_name (controller);
	if (buf)
	  puts (buf);
      }
      break;

    case SOMA_CODE_GET_ITEM_LIST:
      {
	char **list;
	list = soma_get_item_list (controller);

	if (!list)
	  break;

	while (*list)
	  {
	    puts (*list);
	    list++;
	  }
      }
      break;

    case SOMA_CODE_GET_SPOT_LIST:
      {
	char **list;
	list = soma_get_spot_list (controller);

	if (!list)
	  break;

	while (*list)
	  {
	    puts (*list);
	    list++;
	  }
      }
      break;

    case SOMA_CODE_REMOVE_ITEM:
      CHECK_INT (soma_remove_item (controller, atoi (arg)));
      break;

    case SOMA_CODE_REMOVE_SPOT:
      CHECK_INT (soma_remove_spot (controller, atoi (arg)));
      break;

    case SOMA_CODE_PAUSE:
      CHECK_INT (soma_set_pause (controller));
      break;

    case SOMA_CODE_UNPAUSE:
      CHECK_INT (soma_set_unpause (controller));
      break;

    case SOMA_CODE_GET_PAUSE:
      {
	int p;
	CHECK_INT (soma_get_pause (controller, &p));
	printf ("%s\n", p ? "true" : "false");
      }
      break;

    case SOMA_CODE_GET_STOP:
      {
	int p;
	CHECK_INT (soma_get_stop (controller, &p));
	printf ("%s\n", p ? "true" : "false");
      }
      break;

    default:
      message (INTERNAL_ERROR);
      break;
    }

  message (DONE);

  return 0;
}

void
usage (char *a)
{
  fprintf
    (stdout,
     "Usage:\n\t%s [-S server] [options]\n\n"
     "-S    or --server          Host of somad.\n"
     "-P    or --port            Set the port. Default %d\n\n"
     "-U    or --unix            Unix Socket Path of somad.\n"
#ifdef SOMA_USE_OPENSSL
     "-ssl                       Active ssl connection\n"
#endif
     "-pw   or --password        Only check of password.\n"
     "-rd   or --readdirectory   Scans the directories of palinsesto.\n"
     "-rp   or --readpalinsesto  Reloads the palinsesto file.\n"
     "-rs   or --readspot        Reloads the spot file.\n"
     "-fp   or --filepalinsesto  Uploads the new palinsesto file.\n"
     "-op   or --oldpalinsesto   Reload the old palinsesto file.\n"
     "-gp   or --getpalinsesto   Shows the palinsesto.\n"
     "-gop  or --getoldpalinsesto Shows the old palinsesto.\n"
     "-dp   or --setdefaultpalinsesto Set current palinsesto file as default.\n"
     "-fs   or --filespot        Uploads the new spot file.\n"
     "-os   or --oldspot         Reload the old spot file.\n"
     "-gs   or --getspot         Shows the spot file.\n"
     "-gos  or --getoldspot      Shows the old spot file.\n"
     "-ds   or --setdefaultspot  Set current spot file as default.\n"
     "-st   or --status          Gets status of somad.\n"
     "-gi   or --get_item        Gets the item of somad.\n"
     "-gin  or --get_item_next   Gets the following item of somad.\n"
     "-ns   or --nextitem        Sets next item. Somad saves the input in a list\n"
     "                           so you can make a playlist.\n"
     "-nsl  or --nextitemlist    Show the list of nextitem.\n"
     "-nsr  or --nextitemremove  Remove a item of nextitem list. You can use as\n"
     "                           argoment \"first\" and \"last\" for remove the\n"
     "                           first item or the last or a number [0 to max].\n"
     "-nsss or --nextitemsetspot Set the mix with spot struct the current\n"
     "                           palinesesto\n"
     "-nsgs or --nextitemgetspot Return if the nextitem struct has the mix spot\n"
     "-nsgp or --nextitemgetpath Return the list of files in a shared directory\n"
     "                           activated\n"
     "-pn   or --palinsestoname  Returns the name of current palinsesto\n"
     "-r    or --running         Checks if somad is running or not\n"
     "-q    or --quit            Quit somad\n"
     "-sk   or --skip            Skip to the next item\n"
     "-skn  or --skipnext        Skip the follow item\n"
     "-1    or --stop            Stops somad\n"
     "-0    or --start           Reactives somad (after a stop)\n"
     "-rso  or --removeitem      Removes a item by the numeric ID\n"
     "-rsp  or --removespot      Removes a spot by the numeric ID\n"
     "-sp   or --setpause        Set Pause\n"
     "-up   or --unsetpause      Remove the Pause flags\n"
     "-gps  or --getpause        Print the Pause flags\n"
     "-gss  or --getstop         Print the Stop flags\n"
     "-c    or --code            Shows output in code version\n"
     "-h    or --help            Shows this help\n\n"
     "You can set the password in the line command with flags: -pyour_password\n\n"
     "Without command, somaplayer start in shell mode.\n\n", a, SOMA_PORT);
}

/* EOF */
