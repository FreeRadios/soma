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

#include <stdio.h>
#include <string.h>

int
usage (void)
{
  fprintf (stderr, "Usage: soma-config [OPTIONS]\nOptions:\n");
  fprintf (stderr, "\t[--prefix]\n");
  fprintf (stderr, "\t[--version]\n");
  fprintf (stderr, "\t[--libs]\n");
  fprintf (stderr, "\t[--cflags]\n");
  fprintf (stderr, "\t[--ssl]\n");

  return 1;
}

int
main (int argc, char *argv[])
{
  register int i;

  if (argc < 2)
    return usage ();

  for (i = 1; i < argc; i++)
    {
      if (strcmp (argv[i], "--prefix") &&
	  strcmp (argv[i], "--version") &&
	  strcmp (argv[i], "--libs") &&
	  strcmp (argv[i], "--cflags") && 
	  strcmp (argv[i], "--ssl"))
	return usage ();
    }

  for (i = 1; i < argc; i++)
    {
      if (!strcmp (argv[i], "--prefix"))
	fprintf (stdout, "/usr");

      else if (!strcmp (argv[i], "--version"))
	fprintf (stdout, "2.4");

      else if (!strcmp (argv[i], "--libs"))
	fprintf (stdout, " -lxml2 -lz -lm  " " -lhistory -lreadline -lncurses -ldl -lpthread  -lconfuse -lssl -lcrypto  ");

      else if (!strcmp (argv[i], "--cflags"))
	fprintf (stdout, "-g -O2   -DSOMA_USE_OPENSSL -I/usr/include/libxml2   -Wall");

      else if (!strcmp (argv[i], "--ssl"))
#ifdef SOMA_USE_OPENSSL
	fprintf (stdout, "1")
#else
	fprintf (stdout, "0")
#endif
	  ;
    }

  fprintf (stdout, "\n");

  return 0;
}

/* EOF */
