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

/* Remove the pid info */
void
close_pid ()
{
  char *proc;

  if (!conf->PidFile)
    proc = SOMA_FILE_PID;
  else
    proc = conf->PidFile;

  unlink (proc);
}

/* Write the pid info */
void
write_pid ()
{
  FILE *fl;
  char *proc;

  if (!conf->PidFile)
    proc = SOMA_FILE_PID;
  else
    proc = conf->PidFile;

  if (!(fl = fopen (proc, "wb")))
    SOMA_FATAL ("I can't open the pid file: %s", proc);

  fprintf (fl, "%d", getpid ());
  fclose (fl);
}

void
check_pid (void)
{
#ifdef HAVE_PROC
  FILE *fl;
  char *proc;
  pid_t pid;
  struct stat st;

  if (!conf->PidFile)
    proc = SOMA_FILE_PID;
  else
    proc = conf->PidFile;

  if (!(fl = fopen (proc, "rb")))
    return;

  fscanf (fl, "%d", &pid);
  fclose (fl);

  if (!
      (proc =
       (char *) malloc ((strlen (SOMA_PROC_DIR) + 13) * sizeof (char))))
    SOMA_FATAL ("Error memory.");

  sprintf (proc, "%s/%d", SOMA_PROC_DIR, pid);

  if (lstat (proc, &st))
    {
      free (proc);
      return;
    }

  free (proc);

  puts ("Error: somad yet running.");
  exit (1);
#endif
}

/* EOF */
