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

static char *soma_trim (char *);
static int soma_parse_hourmin (char *, int *, int *);
static int soma_parse_day (char *, int *);
static int soma_parse_data (char *, int *, int *, int *);

/* Trim function */
static char *
soma_trim (char *tmp)
{
  int i = 0;
  while (tmp[i] == ' ' || tmp[i] == '\t' || tmp[i] == '\r' || tmp[i] == '\n')
    tmp++;

  i = strlen (tmp);
  i--;

  while (tmp[i] == ' ' || tmp[i] == '\t' || tmp[i] == '\r' || tmp[i] == '\n')
    i--;

  tmp[i + 1] = 0;

  return tmp;
}

/* This function parse the time string format and sets the correct
 * value in the palinsesto struct */
int
check_time (char *start, char *stop)
{
  int len;
  char *s1, *s2, *s3;
  soma_time *timer;

  if (!start || !stop)
    return 1;

  if (!(timer = (soma_time *) malloc (sizeof (soma_time))))
    fatal ("Error memory.");

  if (strlen (start) > strlen (stop))
    len = strlen (start);
  else
    len = strlen (stop);

  if (!(s1 = (char *) malloc (sizeof (char) * len)))
    fatal ("Memory.");
  if (!(s2 = (char *) malloc (sizeof (char) * len)))
    fatal ("Memory.");
  if (!(s3 = (char *) malloc (sizeof (char) * len)))
    fatal ("Memory.");

  start = soma_trim (start);
  len = sscanf (start, "%s %s %s", s1, s2, s3);

  switch (len)
    {
    case 0:
      free (s1);
      free (s2);
      free (s3);
      free (timer);

      return 1;

    case 1:
      if (soma_parse_hourmin (s1, &timer->start_hour, &timer->start_min))
	{
	  free (s1);
	  free (s2);
	  free (s3);

	  free (timer);
	  return 1;
	}

      timer->start_wday = -1;
      timer->start_mday = -1;
      timer->start_month = -1;
      timer->start_year = -1;
      break;

    case 2:
      if (soma_parse_hourmin (s2, &timer->start_hour, &timer->start_min))
	{
	  free (s1);
	  free (s2);
	  free (s3);

	  free (timer);
	  return 1;
	}

      if (soma_parse_day (s1, &timer->start_wday))
	{
	  free (s1);
	  free (s2);
	  free (s3);

	  free (timer);
	  return 1;
	}

      timer->start_mday = -1;
      timer->start_month = -1;
      timer->start_year = -1;
      break;

    case 3:
      if (soma_parse_hourmin (s3, &timer->start_hour, &timer->start_min))
	{
	  free (s1);
	  free (s2);
	  free (s3);

	  free (timer);
	  return 1;
	}

      if (soma_parse_day (s2, &timer->start_wday))
	{
	  free (s1);
	  free (s2);
	  free (s3);

	  free (timer);
	  return 1;
	}

      if (soma_parse_data
	  (s1, &timer->start_year, &timer->start_month, &timer->start_mday))
	{
	  free (s1);
	  free (s2);
	  free (s3);

	  free (timer);
	  return 1;
	}

      break;
    }

  stop = soma_trim (stop);
  len = sscanf (stop, "%s %s %s", s1, s2, s3);

  switch (len)
    {
    case 0:
      free (s1);
      free (s2);
      free (s3);
      return 1;

    case 1:
      if (soma_parse_hourmin (s1, &timer->stop_hour, &timer->stop_min))
	{
	  free (s1);
	  free (s2);
	  free (s3);

	  free (timer);
	  return 1;
	}

      timer->stop_wday = -1;
      timer->stop_mday = -1;
      timer->stop_month = -1;
      timer->stop_year = -1;
      break;

    case 2:
      if (soma_parse_hourmin (s2, &timer->stop_hour, &timer->stop_min))
	{
	  free (s1);
	  free (s2);
	  free (s3);

	  free (timer);
	  return 1;
	}

      if (soma_parse_day (s1, &timer->stop_wday))
	{
	  free (s1);
	  free (s2);
	  free (s3);

	  free (timer);
	  return 1;
	}

      timer->stop_mday = -1;
      timer->stop_month = -1;
      timer->stop_year = -1;
      break;

    case 3:
      if (soma_parse_hourmin (s3, &timer->stop_hour, &timer->stop_min))
	{
	  free (s1);
	  free (s2);
	  free (s3);

	  free (timer);
	  return 1;
	}

      if (soma_parse_day (s2, &timer->stop_wday))
	{
	  free (s1);
	  free (s2);
	  free (s3);

	  free (timer);
	  return 1;
	}

      if (soma_parse_data
	  (s1, &timer->stop_year, &timer->stop_month, &timer->stop_mday))
	{
	  free (s1);
	  free (s2);
	  free (s3);

	  free (timer);
	  return 1;
	}

      break;
    }

  free (s1);
  free (s2);
  free (s3);

  free (timer);

  return 0;
}

/* Hour/Min */
static int
soma_parse_hourmin (char *str, int *h, int *m)
{
  char *sh, *sm;
  int len = strlen (str);

  if (!(sh = strtok (str, ":")))
    return 1;
  sm = str + strlen (sh) + 1;
  if (strlen (sh) >= len - 1)
    return 1;

  if (*sh == 'x' || *sh == 'X')
    *h = -1;
  else
    {
      *h = atoi (sh);

      if (*h > 23)
	{
	  *h = 0;
	  puts ("Hours are over 23.");
	}
      else if (*h < 0)
	{
	  *h = 0;
	  puts ("Hours are under 0.");
	}
    }

  if (*sm == 'x' || *sm == 'X')
    *m = -1;
  else
    {
      *m = atoi (sm);
      if (*m > 59)
	{
	  *m = 0;
	  puts ("Minuts are over 59.");
	}
      else if (*m < 0)
	{
	  *m = 0;
	  puts ("Minuts are under 0.");
	}
    }

  return 0;
}

/* Day */
static int
soma_parse_day (char *str, int *d)
{

  if (*str == 'x' || *str == 'X')
    {
      *d = -1;
      return 0;
    }
  if (!strcasecmp ("Sun", str) || !strcmp ("0", str))
    {
      *d = 0;
      return 0;
    }
  if (!strcasecmp ("Mon", str) || !strcmp ("1", str))
    {
      *d = 1;
      return 0;
    }
  if (!strcasecmp ("Tue", str) || !strcmp ("2", str))
    {
      *d = 2;
      return 0;
    }
  if (!strcasecmp ("Wed", str) || !strcmp ("3", str))
    {
      *d = 3;
      return 0;
    }
  if (!strcasecmp ("Thu", str) || !strcmp ("4", str))
    {
      *d = 4;
      return 0;
    }
  if (!strcasecmp ("Fri", str) || !strcmp ("5", str))
    {
      *d = 5;
      return 0;
    }
  if (!strcasecmp ("Sat", str) || !strcmp ("6", str))
    {
      *d = 6;
      return 0;
    }

  if (!strcasecmp ("Sun", str) || !strcmp ("7", str))
    {
      *d = 0;
      return 0;
    }

  return 1;
}

/* Date */
static int
soma_parse_data (char *str, int *y, int *m, int *d)
{
  char *sd;
  char *sm;
  char *sy;

  if (!(sy = strtok (str, "-")))
    {
      if (*sy == 'x' || *sy == 'X')
	*d = -1;
      else
	{
	  *d = atoi (sy);

	  if (*d > 30)
	    {
	      *d = 0;
	      puts ("Days are over 59.");
	    }
	  else if (*d < 0)
	    {
	      *d = 0;
	      puts ("Days are under 0.");
	    }
	}

      *m = -1;
      *y = -1;
      return 0;
    }

  if (!(sm = strtok (NULL, "-")))
    {
      if (*sy == 'x' || *sy == 'X')
	*m = -1;
      else
	{
	  *m = atoi (sy);

	  if (*m > 11)
	    {
	      *m = 0;
	      puts ("Months are over 11.");
	    }
	  else if (*m < 0)
	    {
	      *m = 0;
	      puts ("Months are under 0.");
	    }
	}

      if (*sm == 'x' || *sm == 'X')
	*d = -1;
      else
	{
	  *d = atoi (sm);

	  if (*d > 30)
	    {
	      *d = 0;
	      puts ("Days are over 59.");
	    }
	  else if (*d < 0)
	    {
	      *d = 0;
	      puts ("Days are under 0.");
	    }
	}

      *y = -1;
      return 1;
    }

  if (!(sd = strtok (NULL, "\0")))
    return 1;

  if (*sy == 'x' || *sy == 'X')
    *y = -1;
  else
    {
      *y = atoi (sy);

      if (*y > 3000)
	{
	  *y = 0;
	  puts ("Years are over 3000.");
	}
      else if (*y < 0)
	{
	  *y = 0;
	  puts ("Years are under 0.");
	}
    }

  if (*sm == 'x' || *sm == 'X')
    *m = -1;
  else
    {
      *m = atoi (sm);

      if (*m > 11)
	{
	  *m = 0;
	  puts ("Months are over 11.");
	}
      else if (*m < 0)
	{
	  *m = 0;
	  puts ("Months are under 0.");
	}
    }

  if (*sd == 'x' || *sd == 'X')
    *d = -1;
  else
    {
      *d = atoi (sd);

      if (*d > 30)
	{
	  *d = 0;
	  puts ("Days are over 59.");
	}
      else if (*d < 0)
	{
	  *d = 0;
	  puts ("Days are under 0.");
	}
    }

  return 0;
}

/* EOF */
