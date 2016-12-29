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

code_str messages[] = {
  {GENERIC_ERROR, "Generic error.\n"}
  ,
  {PARAMETER_ERROR, "Parameter error.\nPlease, try --help\n"}
  ,
  {SOCKET_ERROR, "Socket error!\n"}
  ,
  {UNKNOW_HOST, "Error: unknow host.\n"}
  ,
  {CONNECT_ERROR, "Connect error.\n"}
  ,
  {PROTOCOL_ERROR, "Protocol error.\n"}
  ,
  {PASSWORD_ERROR, "Password error!\n"}
  ,
  {DONE, "Done.\n"}
  ,
  {INTERNAL_ERROR, "Internal error!!!\n"}
  ,
  {LOCK_ERROR, "Lock error.\n"}
  ,
  {YES_RUNNING, "Yes! Somad running.\n"}
  ,
  {NO_STOPPED, "No! Somad stopped\n"}
  ,
  {YES_NEXTITEM_SPOT, "Yes! NextItem mix with Spot.\n"}
  ,
  {NO_NEXTITEM_SPOT, "No! NextItem no mix with Spot.\n"}
  ,
  {0, NULL}
};

/* ECHO OFF */
void
echo_off (void)
{
  struct termios new;

  tcgetattr (0, &stored);

  new = stored;
  new.c_lflag &= (~ECHO);

  tcsetattr (0, TCSANOW, &new);
}

/* ECHO ON */
void
echo_on (void)
{
  tcsetattr (0, TCSANOW, &stored);
}

/* Fatal function. Write a message and quit. */
void
fatal (int code)
{
  if (set_code)
    fprintf (stdout, "%X\n", code);
  else
    fprintf (stdout, "%s", messages[code].str);

  exit (1);
}

/* Write a message */
void
message (int code)
{
  if (set_code)
    fprintf (stdout, "%X\n", code);
  else
    fprintf (stdout, "%s", messages[code].str);
}

/* Ask the password */
void
get_passwd (char *buff)
{
  int a;

  fprintf (stdout, "Soma's password: ");
  fflush (stdout);

  echo_off ();

  a = read (STDIN_FILENO, buff, 1024);
  buff[a - 1] = 0;

  echo_on ();

  fprintf (stdout, "\n");
}

/* EOF */
