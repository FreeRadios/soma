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

/* Support commands: */
soma_cmd soma_cmds[] = {
  {"exit", cmd_exit, "Exit from somaclient"},
  {"quit", cmd_exit, "Exit from somaclient"},

  {"help", cmd_help, "Help about the commands"},
  {"?", cmd_help, "Help about the commands"},

  {"cd", cmd_cd, "Change to directory DIR"},
  {"pwd", cmd_pwd, "Print the current working directory"},

  {"ls", cmd_ls, "List files in DIR"},
  {"list", cmd_ls, "List files in DIR"},

  {"nextitem", cmd_nextitem,
   "Admin the nextitem list. (set - remove - list - set_spot - get_spot)"},
  {"readdirectory", cmd_readdirectory,
   "Reload the audio file in the directories"},
  {"readpalinsesto", cmd_readpalinsesto, "Reload the palinsesto file"},
  {"readspot", cmd_readspot, "Reload the spot file"},

  {"filepalinsesto", cmd_filepalinsesto,
   "Change the palinsesto file with another one."},
  {"oldpalinsesto", cmd_oldpalinsesto, "Return to the first palinsesto file"},
  {"getpalinsesto", cmd_getpalinsesto, "Show the palinsesto file"},
  {"getoldpalinsesto", cmd_getoldpalinsesto, "Show the old palinsesto file"},
  {"setdefaultpalinsesto", cmd_defaultpalinsesto,
   "Set current palinsesto file as default"},

  {"filespot", cmd_filespot, "Change the spot file with another one."},
  {"oldspot", cmd_oldspot, "Return to the first spot file"},
  {"getspot", cmd_getspot, "Show the spot file"},
  {"getoldspot", cmd_getoldspot, "Show the old spot file"},
  {"setdefaultspot", cmd_defaultspot, "Set current spot file as default"},

  {"status", cmd_status, "Show the status"},
  {"getitem", cmd_get_item, "Show the item"},
  {"getitemnext", cmd_get_item_next, "Show the follow item"},
  {"time", cmd_time, "Show the time of server"},
  {"quit", cmd_quit, "Somad quit"},
  {"skip", cmd_skip, "Skip to the next item."},
  {"skipnext", cmd_skip_next, "Skip the follow item."},
  {"stop", cmd_stop,
   "Somad stop. If you set a \"num\", somad stops for \"num\" seconds"},
  {"start", cmd_start, "Somad starts"},
  {"running", cmd_running, "Show the run status of somad"},

  {"shutdown", cmd_shutdown, "Shutdown somad"},

  {"palinsestoname", cmd_pl_name, "Show the name of current palinsesto"},

  {"removeitem", cmd_remove_item, "Remove a ID from the list of item"},
  {"removespot", cmd_remove_spot, "Remove a ID from the list of spot"},

  {"list_item", cmd_item_list, "Show the list of item"},
  {"list_spot", cmd_spot_list, "Show the list of spot"},

  {"pause", cmd_pause, "Pause"},
  {"unsetpause", cmd_unpause, "Remove the pause flags"},
  {"getpause", cmd_get_pause, "Print the pause flags"},
  {"getstop", cmd_get_stop, "Print the stop flags"},

  {NULL, NULL, NULL}
};

/* Questa funzione riceve il comando scritto dall'utente, lo divide in
 * comando e argomenti, ricerca nella struttura se e' un comando vero
 * e se si' ne lancia la funzione associata */
int
execute (char *line)
{
  register int i;
  soma_cmd *cmd;
  char *word;

  i = 0;
  while (line[i] && whitespace (line[i]))
    i++;
  word = line + i;

  while (line[i] && !whitespace (line[i]))
    i++;

  if (line[i])
    line[i++] = '\0';

  cmd = find_cmds (word);

  if (!cmd)
    {
      fprintf (stderr, "%s: No such command for somaclient.\n", word);
      return -1;
    }

  while (whitespace (line[i]))
    i++;

  word = line + i;

  return (*(cmd->func)) (word);
}

/* Trimmo il comando */
char *
trim (char *tmp)
{
  int i = 0;
  while (tmp[i] == ' ' || tmp[i] == '\t' || tmp[i] == '\n')
    tmp++;

  i = strlen (tmp);
  i--;

  while (tmp[i] == ' ' || tmp[i] == '\t' || tmp[i] == '\n')
    i--;

  tmp[i + 1] = 0;

  return tmp;
}

/* Funzione utile per le readline e atta al completamento dei comandi
 * attraverso il tab.  Quando si preme tab viene lanciata una funzione
 * chiamata command_generator. */
char **
soma_completion (const char *text, int start, int end)
{
  char **matches = (char **) NULL;

  if (start == 0)
    matches = rl_completion_matches (text, command_generator);

  return matches;
}

/* Controlla in tutta la struttura un comando che inizia per quello
 * richiesto e completa */
char *
command_generator (const char *text, int state)
{
  static int id, len;
  char *cmd;

  if (!state)
    {
      id = 0;
      len = strlen (text);
    }

  while ((cmd = soma_cmds[id].name))
    {
      id++;

      if (!strncmp (cmd, text, len))
	{
	  char *tmp;

	  if (!(tmp = (char *) malloc ((strlen (cmd) + 1) * sizeof (char))))
	    fatal (INTERNAL_ERROR);

	  strcpy (tmp, cmd);

	  return tmp;
	}
    }

  return NULL;
}

/* Ricerca un comando dentro la struttura comandi */
soma_cmd *
find_cmds (char *name)
{
  int i;

  for (i = 0; soma_cmds[i].name; i++)
    if (!strcmp (soma_cmds[i].name, name))
      return &soma_cmds[i];

  return NULL;
}

/* Help dei comandi */
int
cmd_help (char *arg)
{
  register int i;
  int j = 0;
  int k;
  int len;

  for (i = 0; soma_cmds[i].name; i++)
    {
      if (!*arg || (strcmp (arg, soma_cmds[i].name) == 0))
	{
	  len = strlen (soma_cmds[i].name);

	  fprintf (stdout, "%s", soma_cmds[i].name);
	  for (k = len; k < 21; k++)
	    fprintf (stdout, " ");
	  fprintf (stdout, "%s.\n", soma_cmds[i].doc);
	  j++;
	}
    }

  if (!j)
    {
      fprintf (stdout, "No commands match `%s'.  Possibilties are:\n", arg);

      for (i = 0; soma_cmds[i].name; i++)
	{
	  /* Print in six columns. */
	  if (j == 6)
	    {
	      j = 0;
	      fprintf (stdout, "\n");
	    }

	  fprintf (stdout, "%s\t", soma_cmds[i].name);
	  j++;
	}

      if (j)
	fprintf (stdout, "\n");
    }
  return (0);
}

/* EOF */
