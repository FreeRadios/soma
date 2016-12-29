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

static int play_run (char **, int softstop);
static int play_check_status (int, char *);

/* Play the audio file. */
int
play (int softstop, char *s, int from_ns)
{
  /* List of elements... */
  char **argv;
  soma_opt *tmp;
  char *path;
  int i;
  struct stat st;
  int k;
  char file[SOMA_MAX_BUFF];

  /* Cache the item: */
  if (!(s = log_item_next_set (&softstop, s, from_ns)))
    return 0;

  SOMA_LOG_ALL ("Play item: %s", s);

  log_item_set (s);

  file[0] = 0;

  if (remote_file (s))
    {
      k = SOMA_PLAY_STREAM;
      strncpy (file, s, sizeof (file));
    }

  else if (distribuited_file (s) && (path = distribuited_get_path (s + 1)))
    {
      k = SOMA_PLAY_FILES;
      strncpy (file, path, sizeof (file));
      free (path);
    }

  else
    {
      k = SOMA_PLAY_FILES;
      strncpy (file, s, sizeof (file));
    }

  if (!file[0])
    {
      log_item_empty ();
      free (s);
      return 1;
    }

  SOMA_LOG_INFO ("Play: %s.", file);
  if (!k)
    {
      if (lstat (file, &st))
	{
	  SOMA_LOG_INFO ("%s does not exist!", file);
	  log_item_set (s);
	  free (s);
	  return 1;
	}
    }

  if (k == SOMA_PLAY_FILES)
    {
      if (!
	  (argv =
	   (char **) malloc (sizeof (char *) * (conf->n_OptionsItem + 3))))
	SOMA_FATAL ("Error: memory.");

      argv[0] = conf->ProgramItem;
      tmp = conf->OptionsItem;
    }

  else
    {
      if (!
	  (argv =
	   (char **) malloc (sizeof (char *) * (conf->n_OptionsStream + 3))))
	SOMA_FATAL ("Error: memory.");

      argv[0] = conf->ProgramStream;
      tmp = conf->OptionsStream;
    }

  i = 1;

  while (tmp)
    {
      argv[i] = tmp->opt;
      i++;

      tmp = tmp->next;
    }

  argv[i++] = file;
  argv[i] = NULL;

  i = play_run (argv, softstop);
  free (argv);

  log_item_empty ();

  free (s);
  return i;
}

int
play_exec (int softstop, char **s)
{
  /* List of elements... */
  char **argv;
  int i;
  char **tmp;

  SOMA_LOG_ALL ("Play Exec");

  if (!s)
    return 0;

  i = 0;
  tmp = s;
  while (*tmp)
    {
      tmp++;
      i++;
    }

  if (!(argv = (char **) malloc (sizeof (char *) * (i + 1))))
    SOMA_FATAL ("Error: memory.");

  SOMA_LOG_INFO ("Exec program: %s", *s);

  i = 0;
  tmp = s;
  while (*tmp)
    {
      argv[i++] = *tmp;
      tmp++;
    }

  argv[i] = NULL;

  i = play_run (argv, softstop);
  free (argv);

  return i;
}

static int
play_run (char **argv, int softstop)
{
  time_t start_time;

  SOMA_LOG_ALL ("Play Run");

  /* mov ax,2 and int 80h. */
  if ((pid = fork ()) < 0)
    SOMA_FATAL ("Fork error!");

  /* Father */
  if (pid)
    {

      /* Get the time of starting */
      start_time = time (NULL);

      /* This loop, check when (and how) the child returns */
      while (1)
	{
	  struct tm *w;
	  time_t j;
	  int t, p, status;

	  /* waitpid */
	  p = waitpid (pid, &status, WNOHANG);

	  conf->time_play = time (NULL) - start_time;

	  if (p == pid || (p == -1 && errno == ECHILD))
	    {

	      /* Read the note note of wait(2) about Linux kernel */
	      if (p == -1)
		kill (pid, SIGQUIT);

	      echo_on ();

	      conf->time_play = 0;
	      return play_check_status (status, argv[0]);
	    }

	  /* If... */
	  if (set_quit || set_stop || set_skip)
	    {
	      kill (pid, SIGQUIT);

	      p = 10;

	      do
		{
		  if (waitpid (pid, &status, WNOHANG) == pid)
		    break;

		  usleep (100000);
		  p--;

		}
	      while (p);

	      echo_on ();

	      set_skip = 0;
	      conf->time_play = 0;

	      return 0;
	    }

	  /* If softstop is off, i must kill! */
	  if (!softstop)
	    {
	      /* Now... */
	      j = time (NULL);
	      w = localtime (&j);
	      t = w->tm_hour * 100 + w->tm_min;

	      if (LOCK ())
		{
		  echo_on ();

		  SOMA_LOG_WARN ("Lock error.");
		  continue;
		}

	      /* Check of palinsesto or quit event and kill */
	      if ((set_time_stop && t >= set_time_stop)
		  || (palinsesto && start_check (palinsesto->timer)))
		{
		  kill (pid, SIGQUIT);

		  p = 10;

		  do
		    {
		      if (waitpid (pid, &status, WNOHANG) == pid)
			break;
		      usleep (100000);
		      p--;

		    }
		  while (p);

		  echo_on ();

		  conf->time_play = 0;

		  UNLOCK ();

		  return 0;
		}

	      UNLOCK ();
	    }

	  /* Sleep... */
	  usleep (50000);
	}
    }
  else
    {
      /* Change the file descriptor */

      if (conf->Background)
	signal_on ();
      else
	signal_small_on ();

      echo_off ();

      execve (argv[0], argv, env);

      /* If execve error... */
      echo_on ();

      exit (0);
    }

  /* stop */
  echo_on ();
  conf->time_play = 0;

  return 0;
}

static int
play_check_status (int status, char *file)
{
  SOMA_LOG_ALL ("Play Check Status");

  if (WEXITSTATUS (status))
    {
      SOMA_LOG_WARN ("%s exit with value %d.", file, WEXITSTATUS (status));
      return 1;
    }

  else if (WIFSIGNALED (status))
    {
      SOMA_LOG_WARN ("%s killed with signal %d.", file, WTERMSIG (status));
      return 1;
    }

  else if (WIFSTOPPED (status))
    {
      SOMA_LOG_WARN ("%s stopped with signal %d.", file, WSTOPSIG (status));
      return 1;
    }

  return 0;
}

/* EOF */
