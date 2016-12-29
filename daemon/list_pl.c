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

static int soma_time_to_string (soma_time *);
static int soma_parse_hourmin (char *, int *, int *);
static int soma_parse_day (char *, int *);
static int soma_parse_data (char *, int *, int *, int *);

/* Like soma_item_add, but for palinsesto struct */
void
soma_item_pl_add (soma_pl * pl, int k, char *arg)
{
  soma_item *tmp;

  SOMA_LOG_ALL ("Soma - Palinsesto Directory adding: %s", arg);

  if (!arg)
    return;

  if (!k)
    tmp = pl->PathItem;
  else
    tmp = pl->PathSpot;

  while (tmp)
    {
      if (!strcmp (tmp->item, arg))
	{
	  SOMA_LOG_WARN
	    ("%s: Directory already in the tree. Check your config file!",
	     tmp->item);
	  return;
	}
      tmp = tmp->next;
    }

  if (!(tmp = (soma_item *) malloc (sizeof (soma_item))))
    SOMA_FATAL ("Error memory.");

  if (!(tmp->item = strdup (arg)))
    SOMA_FATAL ("Error memory.");

  tmp->next = NULL;

  if (!k)
    {
      if (!pl->PathItem)
	pl->PathItem = pl->PathItemLast = tmp;

      else
	{
	  pl->PathItemLast->next = tmp;
	  pl->PathItemLast = tmp;
	}
    }
  else
    {
      if (!pl->PathSpot)
	pl->PathSpot = pl->PathSpotLast = tmp;

      else
	{
	  pl->PathSpotLast->next = tmp;
	  pl->PathSpotLast = tmp;
	}
    }
}

/* New palinsesto */
soma_pl *
soma_pl_add (char *start, char *stop, int timecontinued, char *desc,
	     int ritem, int rspot, int s, int ratioitem, int ratiospot,
	     int spotcontroller, char *type, char *stream, char *jingle,
	     char *module, char *moduledata, int priority, char *prespot,
	     char *postspot)
{
  soma_pl *tmp;

  SOMA_LOG_ALL ("Soma - Palinsesto adding: %s", desc);

  if (!(tmp = (soma_pl *) malloc (sizeof (soma_pl))))
    SOMA_FATAL ("Error memory.");
  memset (tmp, 0, sizeof (soma_pl));

  if (desc && *desc)
    tmp->Description = strdup (desc);
  else
    tmp->Description = strdup ("No description.");

  if (!tmp->Description)
    SOMA_FATAL ("Error memory.");

  /* Parse the time string format */
  if (!(tmp->timer = soma_parse_time (start, stop, timecontinued)))
    {
      SOMA_LOG_WARN ("Start/Stop syntax error.");

      free (tmp->Description);
      free (tmp);

      return NULL;
    }

  tmp->PathSpot = tmp->PathItem = NULL;
  tmp->RandomItem = ritem;
  tmp->RandomSpot = rspot;
  tmp->SoftStop = s;
  tmp->RatioItem = ratioitem;
  tmp->RatioSpot = ratiospot;
  tmp->SpotController = spotcontroller;
  tmp->Priority = priority;

  if (stream && *stream)
    tmp->Stream = strdup (stream);

  if (jingle && *jingle)
    tmp->Jingle = strdup (jingle);

  if (prespot && *prespot)
    tmp->PreSpot = strdup (prespot);

  if (postspot && *postspot)
    tmp->PostSpot = strdup (postspot);

  if (module && *module)
    {
      tmp->Module = strdup (module);
      if (moduledata && *moduledata)
	tmp->ModuleData = strdup (moduledata);
    }

  if (!type || !strcmp (type, SOMA_STR_FILES))
    {
      tmp->Type = SOMA_CODE_FILES;
      if (tmp->Stream)
	{
	  free (tmp->Stream);
	  tmp->Stream = NULL;
	}
      if (tmp->Module)
	{
	  free (tmp->Module);
	  tmp->Module = NULL;
	}
      if (tmp->ModuleData)
	{
	  free (tmp->ModuleData);
	  tmp->ModuleData = NULL;
	}
    }

  else if (!strcmp (type, SOMA_STR_STREAM))
    {
      tmp->Type = SOMA_CODE_STREAM;
      if (tmp->Module)
	{
	  free (tmp->Module);
	  tmp->Module = NULL;
	}
      if (tmp->ModuleData)
	{
	  free (tmp->ModuleData);
	  tmp->ModuleData = NULL;
	}
    }

  else if (!strcmp (type, SOMA_STR_MODULE))
    {
      tmp->Type = SOMA_CODE_MODULE;
      if (tmp->Stream)
	{
	  free (tmp->Stream);
	  tmp->Stream = NULL;
	}
    }

  else if (!strcmp (type, SOMA_STR_SILENCE))
    {
      if (tmp->Description)
	free (tmp->Description);

      if (tmp->Module)
	free (tmp->Module);

      if (tmp->ModuleData)
	free (tmp->ModuleData);

      if (tmp->Jingle)
	free (tmp->Jingle);

      if (tmp->Stream)
	free (tmp->Stream);

      free (tmp);
      return NULL;
    }
  else
    SOMA_FATAL ("Errore in palinsesto file. Type = %s.", type);

  tmp->next = conf->Palinsesto;

  if (conf->Palinsesto)
    conf->Palinsesto->prev = tmp;

  tmp->prev = NULL;
  conf->Palinsesto = tmp;

  return tmp;
}

/* This function parse the time format in a string */
static int
soma_time_to_string (soma_time * timer)
{
  char buffer[SOMA_MAX_BUFF];
  char item[5];

  SOMA_LOG_ALL ("Soma - Time to String");

  if (!timer)
    return 1;

  buffer[0] = 0;

  if (timer->start_year != -1)
    snprintf (item, sizeof (item), "%.4d", timer->start_year);
  else
    snprintf (item, sizeof (item), "xxxx");
  strncat (buffer, item, sizeof (buffer));
  strncat (buffer, "-", sizeof (buffer));

  if (timer->start_month != -1)
    snprintf (item, sizeof (item), "%.2d", timer->start_month);
  else
    snprintf (item, sizeof (item), "xx");
  strncat (buffer, item, sizeof (buffer));
  strncat (buffer, "-", sizeof (buffer));

  if (timer->start_mday != -1)
    snprintf (item, sizeof (item), "%.2d", timer->start_mday);
  else
    snprintf (item, sizeof (item), "xx");
  strncat (buffer, item, sizeof (buffer));
  strncat (buffer, " ", sizeof (buffer));

  if (timer->start_wday != -1)
    snprintf (item, sizeof (item), "%.1d", timer->start_mday);
  else
    snprintf (item, sizeof (item), "x");
  strncat (buffer, item, sizeof (buffer));
  strncat (buffer, " ", sizeof (buffer));

  if (timer->start_hour != -1)
    snprintf (item, sizeof (item), "%.2d", timer->start_hour);
  else
    snprintf (item, sizeof (item), "xx");
  strncat (buffer, item, sizeof (buffer));
  strncat (buffer, ":", sizeof (buffer));

  if (timer->start_min != -1)
    snprintf (item, sizeof (item), "%.2d", timer->start_min);
  else
    snprintf (item, sizeof (item), "xx");
  strncat (buffer, item, sizeof (buffer));

  if (!(timer->Start = strdup (buffer)))
    return 1;

  buffer[0] = 0;

  if (timer->stop_year != -1)
    snprintf (item, sizeof (item), "%.4d", timer->stop_year);
  else
    snprintf (item, sizeof (item), "xxxx");
  strncat (buffer, item, sizeof (buffer));
  strncat (buffer, "-", sizeof (buffer));

  if (timer->stop_month != -1)
    snprintf (item, sizeof (item), "%.2d", timer->stop_month);
  else
    snprintf (item, sizeof (item), "xx");
  strncat (buffer, item, sizeof (buffer));
  strncat (buffer, "-", sizeof (buffer));

  if (timer->stop_mday != -1)
    snprintf (item, sizeof (item), "%.2d", timer->stop_mday);
  else
    snprintf (item, sizeof (item), "xx");
  strncat (buffer, item, sizeof (buffer));
  strncat (buffer, " ", sizeof (buffer));

  if (timer->stop_wday != -1)
    snprintf (item, sizeof (item), "%.1d", timer->stop_mday);
  else
    snprintf (item, sizeof (item), "x");
  strncat (buffer, item, sizeof (buffer));
  strncat (buffer, " ", sizeof (buffer));

  if (timer->stop_hour != -1)
    snprintf (item, sizeof (item), "%.2d", timer->stop_hour);
  else
    snprintf (item, sizeof (item), "xx");
  strncat (buffer, item, sizeof (buffer));
  strncat (buffer, ":", sizeof (buffer));

  if (timer->stop_min != -1)
    snprintf (item, sizeof (item), "%.2d", timer->stop_min);
  else
    snprintf (item, sizeof (item), "xx");
  strncat (buffer, item, sizeof (buffer));

  if (!(timer->Stop = strdup (buffer)))
    return 1;

  return 0;
}

/* This function parse the time string format and sets the correct
 * value in the palinsesto struct */
soma_time *
soma_parse_time (char *start, char *stop, int timecontinued)
{
  int len;
  char *s1, *s2, *s3;
  soma_time *timer;

  SOMA_LOG_ALL ("Soma - Parse time");

  if (!start || !stop)
    return NULL;

  if (!(timer = (soma_time *) malloc (sizeof (soma_time))))
    SOMA_FATAL ("Memory.");

  if (strlen (start) > strlen (stop))
    len = strlen (start);
  else
    len = strlen (stop);

  if (!(s1 = (char *) malloc (sizeof (char) * len)))
    SOMA_FATAL ("Memory.");
  if (!(s2 = (char *) malloc (sizeof (char) * len)))
    SOMA_FATAL ("Memory.");
  if (!(s3 = (char *) malloc (sizeof (char) * len)))
    SOMA_FATAL ("Memory.");

  start = soma_trim (start);
  len = sscanf (start, "%s %s %s", s1, s2, s3);

  switch (len)
    {
    case 0:
      free (s1);
      free (s2);
      free (s3);
      free (timer);

      return NULL;

    case 1:
      if (soma_parse_hourmin (s1, &timer->start_hour, &timer->start_min))
	{
	  free (s1);
	  free (s2);
	  free (s3);

	  free (timer);
	  return NULL;
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
	  return NULL;
	}

      if (soma_parse_day (s1, &timer->start_wday))
	{
	  free (s1);
	  free (s2);
	  free (s3);

	  free (timer);
	  return NULL;
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
	  return NULL;
	}

      if (soma_parse_day (s2, &timer->start_wday))
	{
	  free (s1);
	  free (s2);
	  free (s3);

	  free (timer);
	  return NULL;
	}

      if (soma_parse_data
	  (s1, &timer->start_year, &timer->start_month, &timer->start_mday))
	{
	  free (s1);
	  free (s2);
	  free (s3);

	  free (timer);
	  return NULL;
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
      return NULL;

    case 1:
      if (soma_parse_hourmin (s1, &timer->stop_hour, &timer->stop_min))
	{
	  free (s1);
	  free (s2);
	  free (s3);

	  free (timer);
	  return NULL;
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
	  return NULL;
	}

      if (soma_parse_day (s1, &timer->stop_wday))
	{
	  free (s1);
	  free (s2);
	  free (s3);

	  free (timer);
	  return NULL;
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
	  return NULL;
	}

      if (soma_parse_day (s2, &timer->stop_wday))
	{
	  free (s1);
	  free (s2);
	  free (s3);

	  free (timer);
	  return NULL;
	}

      if (soma_parse_data
	  (s1, &timer->stop_year, &timer->stop_month, &timer->stop_mday))
	{
	  free (s1);
	  free (s2);
	  free (s3);

	  free (timer);
	  return NULL;
	}

      break;
    }

  free (s1);
  free (s2);
  free (s3);

  timer->TimeContinued = timecontinued;

  if (soma_time_to_string (timer))
    SOMA_FATAL ("Error memory.");

  return timer;
}

/* Hour/Min */
static int
soma_parse_hourmin (char *str, int *h, int *m)
{
  char *sh, *sm;
  int len = strlen (str);

  SOMA_LOG_ALL ("Soma - Parse hour/min");

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
	  SOMA_LOG_WARN ("Hours are over 23.");
	}
      else if (*h < 0)
	{
	  *h = 0;
	  SOMA_LOG_WARN ("Hours are under 0.");
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
	  SOMA_LOG_WARN ("Minuts are over 59.");
	}
      else if (*m < 0)
	{
	  *m = 0;
	  SOMA_LOG_WARN ("Minuts are under 0.");
	}
    }

  return 0;
}

/* Day */
static int
soma_parse_day (char *str, int *d)
{

  SOMA_LOG_ALL ("Soma - Parse day");

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

  SOMA_LOG_ALL ("Soma - Parse data");

  if (!(sy = strtok (str, "-")))
    {
      if (*sy == 'x' || *sy == 'X')
	*d = -1;
      else
	{
	  *d = atoi (sy);

	  if (*d > 31)
	    {
	      *d = 0;
	      SOMA_LOG_WARN ("Days are over 31.");
	    }
	  else if (*d < 0)
	    {
	      *d = 0;
	      SOMA_LOG_WARN ("Days are under 0.");
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
	      SOMA_LOG_WARN ("Months are over 11.");
	    }
	  else if (*m < 0)
	    {
	      *m = 0;
	      SOMA_LOG_WARN ("Months are under 0.");
	    }
	}

      if (*sm == 'x' || *sm == 'X')
	*d = -1;
      else
	{
	  *d = atoi (sm);

	  if (*d > 31)
	    {
	      *d = 0;
	      SOMA_LOG_WARN ("Days are over 31.");
	    }
	  else if (*d < 0)
	    {
	      *d = 0;
	      SOMA_LOG_WARN ("Days are under 0.");
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
	  SOMA_LOG_WARN ("Years are over 3000.");
	}
      else if (*y < 0)
	{
	  *y = 0;
	  SOMA_LOG_WARN ("Years are under 0.");
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
	  SOMA_LOG_WARN ("Months are over 11.");
	}
      else if (*m < 0)
	{
	  *m = 0;
	  SOMA_LOG_WARN ("Months are under 0.");
	}
    }

  if (*sd == 'x' || *sd == 'X')
    *d = -1;
  else
    {
      *d = atoi (sd);

      if (*d > 31)
	{
	  *d = 0;
	  SOMA_LOG_WARN ("Days are over 31.");
	}
      else if (*d < 0)
	{
	  *d = 0;
	  SOMA_LOG_WARN ("Days are under 0.");
	}
    }

  return 0;
}

/* EOF */
