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

static FILE *log_fd = NULL;

static void puts_time (FILE *);

void
log_init (void)
{
  if (!conf->LogFile)
    {
      fprintf (stderr,
	       "Error: you must set your log file. Check your config file.\n");
      exit (1);
    }

  LOG_LOCK ();

  if (!(log_fd = fopen (conf->LogFile, "ab")))
    {
      fprintf (stderr, "Open %s log file error.\n", conf->LogFile);
      exit (1);
    }

  LOG_UNLOCK ();
}

void
log_restart (int dummy /* signal */ )
{
  SOMA_LOG_ERR ("SIGHUP received.  Attempting to restart.");

  LOG_LOCK ();

  if (log_fd)
    fclose (log_fd);

  if (!(log_fd = fopen (conf->LogFile, "ab")))
    {
      fprintf (stderr, "Open %s log file error.\n", conf->LogFile);
      exit (1);
    }

  LOG_UNLOCK ();

  SOMA_LOG_ERR ("Somad log - restart -");
}

void
log_quit (void)
{
  LOG_LOCK ();

  if (log_fd)
    fclose (log_fd);

  log_fd = 0;

  LOG_UNLOCK ();
}

static void
puts_time (FILE * fd)
{
  time_t j;
  struct tm *k;

  static char *day[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

  j = time (NULL);
  k = localtime (&j);

  fprintf (fd, "[%.2d %s %.2d:%.2d:%.2d] ", k->tm_mday, day[k->tm_wday],
	   k->tm_hour, k->tm_min, k->tm_sec);
}

void
log_write (int type, char *str, ...)
{
  va_list va;
  static char prev[SOMA_MAX_BUFF];
  char s[SOMA_MAX_BUFF];

  va_start (va, str);
  vsnprintf (s, SOMA_MAX_BUFF, str, va);
  va_end (va);

  if (abs (type) < SOMA_DEBUG_ALL && type > -1)
    snprintf (conf->Status, SOMA_MAX_BUFF, s);

  if (conf->Debug < abs (type))
    return;

  if (strcmp (s, prev))
    {
      if (!conf->Background)
	{
	  puts_time (stderr);
	  fprintf (stderr, "%s\n", s);
	}

      LOG_LOCK ();

      if (log_fd)
	{
	  puts_time (log_fd);
	  fprintf (log_fd, "%s\n", s);
	  fflush (log_fd);
	  fsync (fileno (log_fd));
	}

      LOG_UNLOCK ();

      strcpy (prev, s);
    }
}

/* Trim function */
char *
soma_trim (char *tmp)
{
  SOMA_LOG_ALL ("Soma - Trim - Started with %s", tmp);

  int i = 0;
  while (tmp[i] == ' ' || tmp[i] == '\t' || tmp[i] == '\r' || tmp[i] == '\n')
    tmp++;

  i = strlen (tmp);
  i--;

  while (tmp[i] == ' ' || tmp[i] == '\t' || tmp[i] == '\r' || tmp[i] == '\n')
    i--;

  tmp[i + 1] = 0;

  SOMA_LOG_ALL ("Soma - Trim - Exit with %s", tmp);

  return tmp;
}

void
log_item_set (char *s)
{
  int len;
  int last;

  LOCK ();

  if (!s)
    {
      conf->Item[0] = 0;
      UNLOCK ();
      return;
    }

  len = strlen (s);
  last = len > sizeof (conf->Item) ? sizeof (conf->Item) : len;
  strncpy (conf->Item, s, last);
  conf->Item[last] = 0;

  UNLOCK ();
}

void
log_item_empty (void)
{
  LOCK ();
  conf->Item[0] = 0;
  UNLOCK ();
}

/* Cache the next item: */
char *
log_item_next_set (int *softstop, char *s, int from_ns)
{
  char *ret = NULL;

  LOCK ();

  if (conf->Item_next)
    {
      ret = conf->Item_next;
      *softstop = conf->Item_next_softstop;
    }

  SOMA_LOG_INFO ("Cache the item: %s.", s);

  conf->Item_next = strdup (s);
  conf->Item_next_softstop = *softstop;
  conf->Item_next_from_ns = from_ns;

  UNLOCK ();
  return ret;
}

void
log_item_next_empty_nl (int force)
{
  if (!force && conf->Item_next_from_ns)
    return;

  if (conf->Item_next)
    {
      free (conf->Item_next);
      conf->Item_next = NULL;
    }
}

void
log_item_next_empty (int force)
{
  LOCK ();
  log_item_next_empty_nl (force);
  UNLOCK ();
}

/* EOF */
