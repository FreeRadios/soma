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

char **env = NULL, **arg = NULL;
soma_data *conf = NULL;
cfg_t *cfg = NULL;

/* The mutex! */
#ifdef ENABLE_SM
sm mutex;
sm log_mutex;
sm hash_mutex;
#else
pthread_mutex_t mutex;
pthread_mutex_t log_mutex;
pthread_mutex_t hash_mutex;
#endif

int id_item, id_spot, pid = 0;

char set_quit = 0, set_time_stop = 0, set_skip = 0;
int set_pause = 0, set_stop = 0, set_in_pause = 0;

void *module = NULL;
int (*internal_module_init) (char *) = NULL;
char *(*internal_module_run) (char *) = NULL;
char **(*internal_module_exec) (char *) = NULL;
soma_item *(*internal_module_list) (char *) = NULL;
int (*internal_module_quit) (char *) = NULL;
char *(*internal_module_name) (void) = NULL;
int (*internal_module_time_stop) (char *) = NULL;

static void usage (char *);
/* 
 * When ELF changes to process...
 * the mains are playing!
 * 
 * This function reads the parameters, actives the signals,
 * reads the config file, runs the start() func.
 */
int
main (int argc, char *argv[], char **arge)
{
  int a, b;
  pthread_t th;
  struct stat st;

  /* Config element (by libconfuse) */
  cfg_opt_t opts[] = {
    SOMA_CONFIG_STRUCT
  };

  fprintf (stderr, "Soma %s - %s - %s\n\n", SOMA_VERSION, SOMA_COPYRIGHT,
	   SOMA_AUTHOR_EMAIL);

  /* Alloc the conf struct and set it empty */
  if (!(conf = (soma_data *) malloc (sizeof (soma_data))))
    SOMA_FATAL ("Error memory.");

  memset ((void *) conf, 0, sizeof (soma_data));

  /* Config parameters */
  for (a = 1; a < argc; a++)
    {
      /* Change the conf file */
      if (!strcmp (argv[a], "-f") || !strcmp (argv[a], "--file"))
	{
	  a++;
	  if (argv[a])
	    conf->FileCfg = argv[a];
	  else
	    SOMA_FATAL ("The %s flag needs a parameter.", argv[a - 1]);
	}

      /* Background */
      else if (!strcmp (argv[a], "-b") || !strcmp (argv[a], "--background"))
	{
	  conf->Background = 1;
	}

      /* User */
      else if (!strcmp (argv[a], "-u") || !strcmp (argv[a], "--user"))
	{
	  a++;
	  if (argv[a])
	    conf->User = argv[a];
	  else
	    SOMA_FATAL ("The %s flag needs a parameter.", argv[a - 1]);
	}

      /* Group */
      else if (!strcmp (argv[a], "-g") || !strcmp (argv[a], "--group"))
	{
	  a++;
	  if (argv[a])
	    conf->Group = argv[a];
	  else
	    SOMA_FATAL ("The %s flag needs a parameter.", argv[a - 1]);
	}

      /* LogFile */
      else if (!strcmp (argv[a], "-l") || !strcmp (argv[a], "--logfile"))
	{
	  a++;
	  if (argv[a])
	    conf->LogFile = strdup (argv[a]);
	  else
	    SOMA_FATAL ("The %s flag needs a parameter.", argv[a - 1]);
	}

      /* PidFile */
      else if (!strcmp (argv[a], "-p") || !strcmp (argv[a], "--pidfile"))
	{
	  a++;
	  if (argv[a])
	    conf->PidFile = strdup (argv[a]);
	  else
	    SOMA_FATAL ("The %s flag needs a parameter.", argv[a - 1]);
	}

      /* Debug */
      else if (!strcmp (argv[a], "-d") || !strcmp (argv[a], "--debug"))
	{
	  a++;
	  if (argv[a])
	    conf->Debug = atoi (argv[a]);
	  else
	    SOMA_FATAL ("The %s flag needs a parameter.", argv[a - 1]);
	}

      /* Help */
      else if (!strcmp (argv[a], "-h") || !strcmp (argv[a], "--help"))
	{

	  usage (argv[0]);
	  return 0;
	}

      /* Other */
      else
	SOMA_FATAL ("Unknown flag %s.", argv[a]);
    }

  /* Conf file not set */
  if (!conf->FileCfg)
    conf->FileCfg = SOMA_FILE_CFG;

  /* Active the mutex */
  INIT ();
  LOG_INIT ();
  HASH_INIT ();

  /* Save the enviroment for the player */
  env = arge;
  arg = argv;

  /* libconfuse docet: */
  cfg = cfg_init (opts, CFGF_NOCASE);
  a = cfg_parse (cfg, conf->FileCfg);

  if (a == CFG_FILE_ERROR)
    {
      SOMA_FATAL ("Error: opening config file %s", conf->FileCfg);
    }
  else if (a == CFG_PARSE_ERROR)
    {
      SOMA_FATAL ("Config file: parser error.");
    }

  /**************** PARSE THE CONFIG FILE ********************/
  /* LogFile */
  if (!conf->LogFile)
    {
      conf->LogFile = cfg_getstr (cfg, "LogFile");
      if (!conf->LogFile || !*conf->LogFile)
	conf->LogFile = NULL;
    }

  log_init ();

  if (!conf->PidFile)
    {
      conf->PidFile = cfg_getstr (cfg, "PidFile");
      if (!conf->PidFile || !*conf->PidFile)
	conf->PidFile = NULL;
    }

  /* Ssl */
#ifdef SOMA_USE_OPENSSL
  conf->Ssl = cfg_getbool (cfg, "Ssl");

  conf->Certificate = cfg_getstr (cfg, "Certificate");
  if (!conf->Certificate || !*conf->Certificate)
    conf->Certificate = NULL;

  conf->PrivateKey = cfg_getstr (cfg, "PrivateKey");
  if (!conf->PrivateKey || !*conf->PrivateKey)
    conf->PrivateKey = NULL;

  if (conf->Ssl && (!conf->PrivateKey || !conf->Certificate))
    SOMA_FATAL ("SSL activated without certificate or private key flags.");
#endif

  conf->SymLinks = cfg_getbool (cfg, "SymLinks");

  /* UnixSocket */
  conf->UnixSocket = cfg_getbool (cfg, "UnixSocket");

  conf->UnixPath = cfg_getstr (cfg, "UnixPath");
  if (!conf->UnixPath || !*conf->UnixPath)
    conf->UnixPath = NULL;

  /* Port */
  conf->Port = cfg_getint (cfg, "Port");

  /* ServerName */
  conf->ServerName = cfg_getstr (cfg, "ServerName");
  if (!conf->ServerName || !*conf->ServerName)
    conf->ServerName = NULL;

  /* Listen */
  conf->Listen = cfg_getint (cfg, "Listen");

  /* Group */
  if (conf->Group || cfg_getstr (cfg, "Group"))
    {
      struct group *gr;

      if (!
	  (gr =
	   getgrnam (conf->Group ? conf->Group : cfg_getstr (cfg, "Group"))))
	SOMA_FATAL ("Group error.");

      setgid (gr->gr_gid);
    }

  /* User */
  if (conf->User || cfg_getstr (cfg, "User"))
    {
      struct passwd *pw;

      if (!
	  (pw =
	   getpwnam (conf->User ? conf->User : cfg_getstr (cfg, "User"))))
	SOMA_FATAL ("User error.");

      setuid (pw->pw_uid);
    }

  /* Password */
  conf->Password = cfg_getstr (cfg, "Password");
  if (!conf->Password || !*conf->Password)
    conf->Password = NULL;

  /* Background */
  if (!conf->Background)
    conf->Background = cfg_getbool (cfg, "Background");

  /* Debug */
  if (!conf->Debug)
    conf->Debug = cfg_getint (cfg, "Debug");

  if (conf->Debug > SOMA_DEBUG_ALL)
    conf->Debug = SOMA_DEBUG_ALL;

  /* PathModules */
  conf->PathModules = cfg_getstr (cfg, "PathModules");
  if (!conf->PathModules || !*conf->PathModules)
    conf->PathModules = SOMA_PATH_MOD;

  /* PathItem */
  b = cfg_size (cfg, "PathItem");
  for (b--; b >= 0; b--)
    soma_item_add (SOMA_PATH_ITEM, cfg_getnstr (cfg, "PathItem", b));

  /* PathSpot */
  b = cfg_size (cfg, "PathSpot");
  for (b--; b >= 0; b--)
    soma_item_add (SOMA_PATH_SPOT, cfg_getnstr (cfg, "PathSpot", b));

  /* Palinsesto */
  conf->Palinsesto_fl = cfg_getstr (cfg, "Palinsesto");
  if (!conf->Palinsesto_fl || !*conf->Palinsesto_fl)
    {
      SOMA_FATAL ("No Palinsesto tag.");
    }

  if (conf->Palinsesto_fl)
    conf->Old_Palinsesto = strdup (conf->Palinsesto_fl);

  /* Spot */
  conf->Spot_fl = cfg_getstr (cfg, "Spot");
  if (!conf->Spot_fl || !*conf->Spot_fl)
    {
      SOMA_FATAL ("No Spot tag.");
    }

  else if (!(conf->Spot_fl = strdup (conf->Spot_fl)))
    {
      SOMA_FATAL ("Error: memory.");
    }

  if (conf->Spot_fl)
    conf->Old_Spot = strdup (conf->Spot_fl);

  /* The Options for the programItem and ProgramStream are inserted in
   * a simple list called soma_opt. */

  /* ITEM: Program and Options */
  conf->ProgramItem = cfg_getstr (cfg, "ProgramItem");
  if (!conf->ProgramItem || !*conf->ProgramItem)
    SOMA_FATAL ("No ProgramItem tag.");

  if (cfg_getstr (cfg, "OptionsItem")
      && strlen (cfg_getstr (cfg, "OptionsItem")) > 0)
    soma_options (SOMA_OPT_ITEM, cfg_getstr (cfg, "OptionsItem"));

  if (lstat (conf->ProgramItem, &st))
    {
      char *c;
      if (!(c = soma_search_path (conf->ProgramItem)))
	SOMA_FATAL ("ProgramItem %s does not exist!", conf->ProgramItem);

      free (conf->ProgramItem);
      conf->ProgramItem = c;
    }

  /* STREAM: Program and Options */
  conf->ProgramStream = cfg_getstr (cfg, "ProgramStream");
  if (!conf->ProgramStream || !*conf->ProgramStream)
    conf->ProgramStream = conf->ProgramItem;

  if (lstat (conf->ProgramStream, &st))
    {
      char *c;
      if (!(c = soma_search_path (conf->ProgramStream)))
	SOMA_FATAL ("ProgramStream %s does not exist!", conf->ProgramStream);

      free (conf->ProgramStream);
      conf->ProgramStream = c;
    }

  if (cfg_getstr (cfg, "OptionsStream")
      && strlen (cfg_getstr (cfg, "OptionsStream")) > 0)
    soma_options (SOMA_OPT_STREAM, cfg_getstr (cfg, "OptionsStream"));
  else
    conf->OptionsStream = conf->OptionsItem;

  /* DistribuitedPath */
  b = cfg_size (cfg, "DistribuitedPath");
  for (a = 0; a < b; a++)
    {
      char *tmp;

      if (!(tmp = cfg_getnstr (cfg, "DistribuitedPath", a)))
	continue;

      if (!(tmp = distribuited_trim (tmp)))
	continue;

      soma_item_add (SOMA_DISTRIBUITED_PATH, strdup (tmp));
    }

  /* HostDeny */
  b = cfg_size (cfg, "HostDeny");
  for (a = 0; a < b; a++)
    denyallow_add (SOMA_HOST_DENY, cfg_getnstr (cfg, "HostDeny", a));

  /* HostAllow */
  b = cfg_size (cfg, "HostAllow");
  for (a = 0; a < b; a++)
    denyallow_add (SOMA_HOST_ALLOW, cfg_getnstr (cfg, "HostAllow", a));

  /* Check other somads */
  check_pid ();

  /* Write the somad.pid */
  write_pid ();

  /**************** BUILDS THE TREES **********************/
  build_data ();
  build_palinsesto ();

  /* Background */
  if (conf->Background)
    {
      signal_off ();
      setsid ();

      if (fork ())
	exit (0);
    }

  else
    signal_small_off ();

  /* ffmpeg init */
  soma_stat_init ();

  /* Run the socket thread */
  if (pthread_create (&th, NULL, soma_socket, NULL))
    SOMA_FATAL ("Threads error.");

  srand (time (NULL));

  SOMA_LOG_INFO ("Somad started.");

  /* Run start() */
  start ();

  /* Here only when start returns. Start() is stopped by a somaclient. */

  /* Socket Thread */
  pthread_join (th, NULL);

  if (conf->Background)
    signal_on ();
  else
    signal_small_on ();

  SOMA_LOG_INFO ("Somad stopped.");

  /* Close the log */
  log_quit ();

  /* Close pid */
  close_pid ();

  /* Free memory */
  cfg_free (cfg);
  free (conf);

  /* Bye! */
  return 0;
}

static void
usage (char *a)
{
  fprintf
    (stderr,
     "Usage:\n\t%s [options]\n\n"
     "-f o --file        <config file> set the config file. Default %s\n"
     "-b o --background  start in background.\n"
     "-d o --debug       <number> active the debug level.\n"
     "-l o --logfile     <file> set the log file.\n"
     "-p o --pidfile     <file> set the pid file.\n"
     "-u o --user        <user> set user permission.\n"
     "-h o --help        this help.\n\n", a, SOMA_FILE_CFG);
}

/* EOF */
