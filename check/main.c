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

#include "check.h"

void config_parse (char *file);
void spot_parse (char *file);
void palinsesto_parse (char *file);

/* Check the config file or palinsesto file */
int
main (int argc, char *argv[], char **arge)
{
  char *pl = NULL, *cf = NULL, *sp = NULL;
  int a;

  fprintf (stdout, "Soma %s - %s - %s\n\n", SOMA_VERSION, SOMA_COPYRIGHT,
	   SOMA_AUTHOR_EMAIL);

  /* Read the parameter... */
  if (argc > 1)
    {

      for (a = 1; a < argc; a++)
	{
	  if (!strcmp (argv[a], "-p") || !strcmp (argv[a], "--palinsesto"))
	    {
	      a++;
	      if (argv[a])
		pl = argv[a];
	      else
		fatal ("Parameter error!\nPlease, try --help.\n");
	    }
	  else if (!strcmp (argv[a], "-c") || !strcmp (argv[a], "--config"))
	    {
	      a++;
	      if (argv[a])
		cf = argv[a];
	      else
		fatal ("Parameter error!\nPlease, try --help.\n");
	    }
	  else if (!strcmp (argv[a], "-s") || !strcmp (argv[a], "--spot"))
	    {
	      a++;
	      if (argv[a])
		sp = argv[a];
	      else
		fatal ("Parameter error!\nPlease, try --help.\n");
	    }
	  else if (!strcmp (argv[a], "-h") || !strcmp (argv[a], "--help"))
	    {

	      fprintf
		(stdout,
		 "Usare:\n\t%s [-h | --help]\n\n"
		 "-c o --config checks the config file for somad\n"
		 "-p o --palinsesto checks the palinsesto file for somad\n"
		 "-s o --spot checks the spot file for somad\n"
		 "-h o --help shows this help.\n\n", argv[0]);
	      return 0;
	    }
	  else
	    fatal ("Parameter error!\nPlease, try --help.\n");
	}
    }

  if (!pl && !cf && !sp)
    fatal ("Parameter error!\nPlease, try --help.\n");

  if (cf)
    config_parse (cf);

  else if (sp)
    spot_parse (sp);

  else
    palinsesto_parse (pl);

  fprintf (stdout, "Sintax OK.\n");

  return 0;
}

void
config_parse (char *file)
{
  cfg_t *cfg;
  struct passwd *pw;
  struct stat st;
  int a, b;

  cfg_opt_t opts[] = {
    SOMA_CONFIG_STRUCT
  };

  cfg = cfg_init (opts, CFGF_NOCASE);
  a = cfg_parse (cfg, file);

  if (a == CFG_FILE_ERROR)
    fatal ("Error in config file.\n");

  else if (a == CFG_PARSE_ERROR)
    fatal ("Error parser.\n");

  if (!cfg_getstr (cfg, "User"))
    msg ("No User flag.");

  else if (!(pw = getpwnam (cfg_getstr (cfg, "User"))))
    msg ("Invalid user for this system!");

  else if (pw && pw->pw_uid && cfg_getint (cfg, "Port") <= 1024)
    msg
      ("Attenction: many posix system does not allow to an user to open a port minor 1024.");

  if (!cfg_getstr (cfg, "Password"))
    msg ("No password flag found.");

  if (cfg_getint (cfg, "Port") < 0)
    msg ("The port cannot be under 0.");

  if (cfg_getint (cfg, "Listen") < 0)
    msg ("The listen cannot be under 0.");

  if (cfg_getbool (cfg, "Ssl")
      && (!cfg_getstr (cfg, "Certificate")
	  || !cfg_getstr (cfg, "PrivateKey")))
    fatal ("If you want use SSL insert Certificate and PrivateKey flags!");

  if (cfg_getbool (cfg, "Ssl"))
    {
      if (lstat (cfg_getstr (cfg, "Certificate"), &st))
	fatal ("Certificate file does not exist!");

      if (lstat (cfg_getstr (cfg, "PrivateKey"), &st))
	fatal ("PrivateKey file does not exist!");
    }

  if (cfg_getint (cfg, "Debug") < SOMA_DEBUG_NO
      || cfg_getint (cfg, "Debug") > SOMA_DEBUG_ALL)
    msg ("Debug flag not correct.");

  if (!cfg_getstr (cfg, "LogFile"))
    msg ("Error in logfile flag.");

  else if (lstat (cfg_getstr (cfg, "LogFile"), &st))
    msg
      ("LogFile does not exist: it will be created if somad have the correct permission to write it.");

  if (!cfg_getstr (cfg, "PidFile"))
    msg ("Error in pidfile flag.");

  else if (lstat (cfg_getstr (cfg, "PidFile"), &st))
    msg
      ("PidFile does not exist: it will be created if somad have the correct permission to write it.");

  if (!cfg_getstr (cfg, "Palinsesto"))
    msg ("Error in palinsesto flag.");

  else if (lstat (cfg_getstr (cfg, "Palinsesto"), &st))
    msg ("Palinsesto file does not exist.");

  if (!cfg_getstr (cfg, "Spot"))
    msg ("Error in palinsesto flag.");

  else if (lstat (cfg_getstr (cfg, "Spot"), &st))
    msg ("Spot file does not exist.");

  if (!cfg_getstr (cfg, "PathModules"))
    msg ("Error in pathmodules flag.");

  else if (lstat (cfg_getstr (cfg, "PathModules"), &st))
    msg ("PathModules file does not exist.");

  b = cfg_size (cfg, "DistribuitedPath");
  for (a = 0; a < b; a++)
    if (lstat (cfg_getnstr (cfg, "DistribuitedPath", a), &st))
      msg ("%s path in DistribuitedPath does not exist!",
	   cfg_getnstr (cfg, "DistribuitedPath", a));

  b = cfg_size (cfg, "PathSpot");
  for (a = 0; a < b; a++)
    if (lstat (cfg_getnstr (cfg, "PathSpot", a), &st))
      msg ("%s path in PathSpot does not exist!",
	   cfg_getnstr (cfg, "PathSpot", a));

  b = cfg_size (cfg, "PathItem");
  for (a = 0; a < b; a++)
    if (lstat (cfg_getnstr (cfg, "PathItem", a), &st))
      msg ("%s path in PathItem does not exist!",
	   cfg_getnstr (cfg, "PathItem", a));

  if (!cfg_getbool (cfg, "XmlSyntax"))
    msg ("For somax, use XmlSyntax=true");

  b = cfg_size (cfg, "HostAllow");
  for (a = 0; a < b; a++)
    if (host_check (cfg_getnstr (cfg, "HostAllow", a)))
      msg ("The ip '%s' is no a compatible syntax!",
	   cfg_getnstr (cfg, "HostAllow", a));

  b = cfg_size (cfg, "HostDeny");
  for (a = 0; a < b; a++)
    if (host_check (cfg_getnstr (cfg, "HostDeny", a)))
      msg ("The ip '%s' is no a compatible syntax!",
	   cfg_getnstr (cfg, "HostDeny", a));

}

void
palinsesto_parse (char *file)
{
  check_palinsesto (file);
}

void
spot_parse (char *file)
{
  check_spot (file);
}

/* EOF */
