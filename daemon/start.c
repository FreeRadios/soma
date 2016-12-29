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

typedef struct soma_pl_list
{
  soma_pl *pl;
  struct soma_pl_list *next;
} soma_pl_list;

soma_pl *palinsesto = NULL;

static int must_change = 0;

static void start_ratioitem (void);
static void start_ratiospot (void);
static soma_pl_list *start_play (soma_pl *);
static int start_check_item (int, int, int, int);
static char *start_time (int);

/* This is the principal function */
void
start (void)
{
  int w;
  soma_pl *tmp = NULL;
  soma_pl_list *list = NULL, *list_tmp = NULL;
  static int nextitem_id = 0;
  char *internal_moduledata = NULL;

  /* infinite loop. When set_quit is seted, exit. */
  while (!set_quit)
    {
      if (set_pause)
	{
	  SOMA_LOG_INFO ("Pause event.");
	  set_in_pause = 1;
	  sleep (1);
	  continue;
	}

      set_in_pause = 0;

      LOCK ();

      /* If there is the next item, play it */
      if (conf->next_item)
	{
	  char *ns;

	  if (conf->nextitem_spot && palinsesto
	      && palinsesto->RatioItem == nextitem_id)
	    {
	      nextitem_id = 0;
	      start_ratiospot ();
	    }

	  if (!(ns = strdup (conf->next_item->item)))
	    SOMA_FATAL ("Error memory.");

	  UNLOCK ();

	  nextitem_remove ();

	  nextitem_id++;

	  play (1, ns, 1);
	  free (ns);

	  continue;
	}

      else
	nextitem_id = 0;

      UNLOCK ();

      if (!conf->Palinsesto)
	{
	  sleep (1);
	  continue;
	}

      /* Set stop action */
      if (set_stop)
	{
	  if (!conf->Background && set_stop > 0)
	    SOMA_LOG_INFO ("Stop: %d", set_stop);

	  if (set_stop > 0)
	    {
	      if (set_stop == 1)
		SOMA_LOG_INFO ("somad started.");
	      set_stop--;
	    }

	  sleep (1);
	  continue;
	}

      if (LOCK ())
	{
	  SOMA_LOG_WARN ("Lock error.");
	  continue;
	}

      /* Get the palinsesto */
      list = start_play (conf->Palinsesto);

      /* If the old palinsesto is not in this new list or, if i must
       * change because the player return error (and the old
       * palinsesto is a Module)... */
      w = 0;

      if (!palinsesto)
	w = 1;

      else if (palinsesto && palinsesto->Type == SOMA_CODE_MODULE)
	{
	  list_tmp = list;
	  while (list_tmp)
	    {
	      if (palinsesto == list_tmp->pl)
		break;
	      list_tmp = list_tmp->next;
	    }

	  if (!list_tmp || must_change)
	    w = 1;
	}

      if (w)
	{
	  if (module && internal_module_quit)
	    {
	      int ret;

	      ret = internal_module_quit (internal_moduledata);
	      if (ret)
		SOMA_LOG_WARN ("internal_module_quit returned %d.", ret);
	    }

	  if (internal_moduledata)
	    {
	      free (internal_moduledata);
	      internal_moduledata = NULL;
	    }

	  clear_module ();
	}

      w = 0;

      /* What i must play? */
      if (!list)
	w = -1;

      else
	{
	  if (!must_change)
	    tmp = list->pl;
	  else
	    {
	      SOMA_LOG_WARN
		("Previous palinsesto return error. Switch to the next palinsesto.");

	      list_tmp = list;
	      while (list_tmp)
		{
		  if (palinsesto == list_tmp->pl)
		    break;
		  list_tmp = list_tmp->next;
		}

	      if (list_tmp && list_tmp->next)
		tmp = list_tmp->next->pl;
	      else
		tmp = list->pl;
	    }
	  w = start_check (tmp->timer);
	}

      /* Free changes */
      must_change = 0;

      /* Free memory list */
      list_tmp = list;
      while (list_tmp)
	{
	  list = list_tmp;
	  list_tmp = list_tmp->next;
	  free (list);
	}

      if (w)
	{
	  palinsesto = NULL;

	  if (w < 0)
	    SOMA_LOG_INFO ("Out from any palinsesto.");

	  else
	    SOMA_LOG_INFO ("Out from any palinsesto for %s (%s).",
			   start_time (w), tmp->timer->Start);

	  UNLOCK ();

	  sleep (1);
	  continue;
	}

      /* If the prev palinsesto is different then this, i must init it */
      if (tmp != palinsesto)
	{
	  palinsesto = tmp;

	  SOMA_LOG_INFO ("Palinsesto: %s", palinsesto->Description);

	  log_item_next_empty_nl (0);

	  id_item = id_spot = set_time_stop = 0;
	  if (tmp)
	    build_pl ();

	  if (palinsesto->Jingle)
	    {
	      char *a;
	      int softstop = palinsesto->SoftStop;

	      if (!(a = strdup (palinsesto->Jingle)))
		SOMA_FATAL ("Error memory.");

	      UNLOCK ();

	      must_change = play (softstop, a, 0);
	      free (a);

	      continue;
	    }
	}

      switch (palinsesto->Type)
	{

	  /* Type STREAMING */
	case SOMA_CODE_STREAM:
	  {
	    char *a;
	    int softstop = palinsesto->SoftStop;

	    if (!palinsesto->Stream)
	      {
		SOMA_LOG_WARN ("No audio file!");
		UNLOCK ();
		sleep (1);

		break;
	      }

	    if (!(a = strdup (palinsesto->Stream)))
	      SOMA_FATAL ("Error memory.");

	    UNLOCK ();

	    /* stream */
	    must_change = play (softstop, a, 0);
	    free (a);
	  }

	  break;

	  /* Type FILES */
	case SOMA_CODE_FILES:
	  {

	    if (!conf->n_item)
	      {
		SOMA_LOG_WARN ("No audio file!");
		UNLOCK ();
		sleep (1);

		break;
	      }

	    /* Check the radioitem and radiospot */

	    if (!palinsesto->RatioItem && !palinsesto->RatioSpot)
	      {
		SOMA_LOG_WARN ("Ratio error. No audio file!");
		UNLOCK ();
		sleep (1);

		break;
	      }

	    start_ratioitem ();

	    if (!palinsesto || start_check (palinsesto->timer)
		|| conf->next_item)
	      {
		UNLOCK ();
		break;
	      }

	    start_ratiospot ();

	    UNLOCK ();

	  }
	  break;

	  /* Type MODULE */
	case SOMA_CODE_MODULE:
	  {
	    char *a;
	    char **arg;
	    soma_item *list;

	    /* Upload the module... */
	    if (!module)
	      {
		char *name;

		if (!(module = check_module (palinsesto->Module)))
		  {
		    UNLOCK ();
		    break;
		  }

		name = internal_module_name ();
		SOMA_LOG_INFO ("Start module: %s", name ? name : "unknown");

		must_change = internal_module_init (palinsesto->ModuleData);

		if (must_change)
		  {
		    SOMA_LOG_WARN ("internal_module_init returned %d.",
				   must_change);
		    clear_module ();

		    UNLOCK ();
		    break;
		  }

		if (!(internal_moduledata = strdup (palinsesto->ModuleData)))
		  SOMA_FATAL ("Error memory.");
	      }

	    if (internal_module_run
		&& (a = internal_module_run (internal_moduledata)))
	      {
		set_time_stop =
		  internal_module_time_stop (internal_moduledata);

		UNLOCK ();

		must_change = play (1, a, 0);
		free (a);
	      }

	    else if (internal_module_exec
		     && (arg = internal_module_exec (internal_moduledata)))
	      {
		char **old;

		set_time_stop =
		  internal_module_time_stop (internal_moduledata);

		UNLOCK ();

		must_change = play_exec (1, arg);

		old = arg;
		while (*arg)
		  {
		    free (*arg);
		    arg++;
		  }

		free (old);
	      }

	    else if (internal_module_list
		     && (list = internal_module_list (internal_moduledata)))
	      {
		soma_item *list2;

		set_time_stop =
		  internal_module_time_stop (internal_moduledata);

		delete_pl ();

		build_tree (list, SOMA_PALINSESTO_ITEM);

		palinsesto->RatioItem = conf->n_item;

		start_ratioitem ();

		UNLOCK ();

		while (list)
		  {
		    list2 = list;
		    list = list->next;

		    free (list2->item);
		    free (list2);
		  }
	      }

	    else
	      {
		char *name = internal_module_name ();

		SOMA_LOG_ERR ("Module '%s' has not a compatible data!",
			      name ? name : "unknown");

		must_change = 1;

		UNLOCK ();
		break;
	      }
	  }

	  break;
	}
    }
}

/* Check of the currect palinsesto */
static soma_pl_list *
start_play (soma_pl * pl)
{
  soma_pl_list *list, *head, *tail;
  int i;

  SOMA_NO_LOG_ALL ("Start Play: create the list of possible palinsesti");

  list = head = tail = NULL;

  while (pl)
    {
      i = start_check (pl->timer);

      if (!i)
	{
	  if (!list)
	    {
	      if (!(list = (soma_pl_list *) malloc (sizeof (soma_pl_list))))
		SOMA_FATAL ("Error: memory.");

	      list->pl = pl;
	      list->next = NULL;
	      head = tail = list;
	    }

	  else if (pl->Priority)
	    {
	      if (!(list = (soma_pl_list *) malloc (sizeof (soma_pl_list))))
		SOMA_FATAL ("Error: memory.");

	      list->pl = pl;
	      list->next = head;
	      head = list;
	    }

	  else
	    {
	      if (!(list = (soma_pl_list *) malloc (sizeof (soma_pl_list))))
		SOMA_FATAL ("Error: memory.");

	      list->pl = pl;
	      list->next = NULL;
	      tail->next = list;
	      tail = list;
	    }
	}

      pl = pl->next;
    }

  return head;
}

/* Check the pl */
int
start_check (soma_time * timer)
{
  time_t j;
  struct tm *k;
  int date_ret, day_ret, min_ret;
  int a, b, now, max;

  if (!timer)
    return -1;

  j = time (NULL);
  k = localtime (&j);

  date_ret = day_ret = min_ret = -4;
  a = b = now = max = 0;

  /* YYYYDDMM... */
  if (timer->start_year != -1 && timer->stop_year != -1
      && timer->start_year <= timer->stop_year)
    {
      a = timer->start_year;
      b = timer->stop_year;
      now = k->tm_year + 1900;
      max = 3000;
    }

  /* or DDMM... */
  if (timer->start_month != -1 && timer->stop_month != -1)
    {
      a = (a * 12) + timer->start_month;
      b = (b * 12) + timer->stop_month;
      now = (now * 12) + k->tm_mon + 1;
      max = 36000;		/* 3000 years * 12 months */
    }
  else if (max)
    {
      a = a * 12;
      b = b * 12;
      now = (now * 12) + k->tm_mon;
      max = 36000;		/* 3000 years * 12 months */
    }

  /* or MM... */
  if (timer->start_mday != -1 && timer->stop_mday != -1)
    {
      a = (a * 31) + timer->start_mday;
      b = (b * 31) + timer->stop_mday;
      now = (now * 31) + k->tm_mday;
      max = 1116000;		/* 3000 years * 12 months * 31 days */
    }
  else if (max)
    {
      a = a * 31;
      b = b * 31;
      now = (now * 31) + k->tm_mday;
      max = 1116000;		/* 3000 years * 12 months * 31 days */
    }

  if (max)
    {
      date_ret = start_check_item (a, b, now, max);
      if (date_ret > 0)
	return date_ret * 1440;	/* 24 hours * 60 minutes */

      if (timer->TimeContinued)
	{
	  switch (date_ret)
	    {
	    case 0:
	      return 0;

	    case -1:
	      a = now = 0;
	      b = -1;

	      if (!timer->start_hour != -1)
		{
		  a = timer->start_hour;
		  now = k->tm_hour;
		  b = 0;
		}
	      if (!timer->start_min != -1)
		{
		  a = (a * 60) + timer->start_min;
		  now = (now * 60) + k->tm_min;
		  b = 0;
		}

	      if (b < 0)
		break;

	      if (now >= a)
		return 0;

	      break;

	    case -2:
	      a = now = 0;
	      b = -1;

	      if (!timer->stop_hour != -1)
		{
		  a = timer->stop_hour;
		  now = k->tm_hour;
		  b = 0;
		}

	      if (!timer->stop_min != -1)
		{
		  a = (a * 60) + timer->stop_min;
		  now = (now * 60) + k->tm_min;
		  b = 0;
		}

	      if (b < 0)
		break;

	      if (now < a)
		return 0;

	      break;

	    case -3:
	      break;
	    }
	}
    }

  /* Checking the day of week */
  day_ret =
    start_check_item (timer->start_wday, timer->stop_wday, k->tm_wday, 7);
  if (day_ret > 0)
    return day_ret * 1440;	/* 24 hours * 60 minutes */

  if (timer->TimeContinued && !day_ret && timer->start_wday != -1
      && timer->stop_wday != -1)
    return 0;

  a = b = now = max = 0;

  /* Checking the hours:minutes */
  if (timer->start_hour != -1 && timer->stop_hour != -1)
    {
      a = timer->start_hour;
      b = timer->stop_hour;
      now = k->tm_hour;
      max = 24;
    }

  if (timer->start_min != -1 && timer->stop_min != -1)
    {
      a = (a * 60) + timer->start_min;
      b = (b * 60) + timer->stop_min;
      now = (now * 60) + k->tm_min;
      max = 1440;
    }
  else if (max)
    {
      a = a * 60;
      b = b * 60;
      now = (now * 60) + k->tm_min;
      max = 1440;
    }

  if (max)
    {
      if (a == b)
	return 0;

      min_ret = start_check_item (a, b, now, max);

      if (timer->TimeContinued)
	{
	  if (a < b)
	    {
	      if ((now < a && (day_ret == -2 || date_ret == -2)) ||
		  (now > b && (day_ret == -1 || date_ret == -1)))
		return 0;
	    }
	  else if (a > b)
	    {
	      if (now < a && (day_ret == -1 || date_ret == -1))
		return min_ret + (a - b);
	      else if (now > b && (day_ret == -2 || date_ret == -2))
		{
		  return 1440 - b;	/* Correct: Tomorrow + (1440 - b) */
		}
	    }
	}

      if (min_ret > 0)
	return min_ret;

      if (min_ret == -1 || min_ret == 0)
	return 0;

      /* If NOW is the end of PALINSESTO i can't know the time of next
       * palinsesto, so... */
      if (min_ret == -2)
	{
	  min_ret = start_check_item (a, b, now + 1, max);
	  if (min_ret > 0)
	    return min_ret + 1;
	}

      if (min_ret < 0)
	return 0;

      return min_ret;
    }

  return 0;
}

/* Check 2 element */
static int
start_check_item (int start, int stop, int now, int max)
{
  if (start == now && stop == now)
    return -3;

  if (start == now)
    return -1;

  if (stop == now)
    return -2;

  if (start < 0 || stop < 0)
    return 0;

  if (start < stop)
    {
      if (start < now && stop > now)
	return 0;

      if (start < now && stop < now)
	return max - now + start;

      return start - now;
    }

  else if (start > stop)
    {
      if (start > now && stop > now)
	return 0;

      if (start > now && stop < now)
	return start - now;

      return 0;
    }

  else
    {
      if (start < now)
	return max - now + start;

      return start - now;
    }
}

static char *
start_time (int t)
{
  static char buf[SOMA_MAX_BUFF];

  char min[20];
  char hour[20];
  char day[20];
  char month[20];

  int ret[5], i = 0;

  memset (ret, 0, sizeof (ret));

  /* Minutes */
  ret[0] = t % 60;
  t = t / 60;

  if (!t)
    i = 1;

  if (!i)
    {
      /* Hours */
      ret[1] = t % 24;
      t = t / 24;

      if (!t)
	i = 2;
    }

  if (!i)
    {
      /* Days */
      ret[2] = t % 31;
      t = t / 31;

      if (!t)
	i = 3;
    }

  if (!i)
    {
      /* Months */
      ret[3] = t % 12;
      t = t / 12;

      if (!t)
	i = 4;
    }

  if (!i)
    {
      /* Years */
      ret[4] = t;
      i = 5;
    }

  min[0] = 0;
  hour[0] = 0;
  day[0] = 0;
  month[0] = 0;

  switch (i)
    {
    case 1:
      if (ret[0] == 1)
	snprintf (buf, SOMA_MAX_BUFF, "some seconds");
      else
	snprintf (buf, SOMA_MAX_BUFF, "%d minutes", ret[0]);
      break;

    case 2:
      if (ret[0] == 1)
	snprintf (min, SOMA_MAX_BUFF, " and some seconds");
      else if (ret[0] > 0)
	snprintf (min, SOMA_MAX_BUFF, " and %d minutes", ret[0]);

      snprintf (buf, SOMA_MAX_BUFF, "%d hour%s%s",
		ret[1], ret[1] != 1 ? "s" : "", min);
      break;

    case 3:
      if (ret[0] == 1)
	snprintf (min, SOMA_MAX_BUFF, " and some seconds");
      else if (ret[0] > 0)
	snprintf (min, SOMA_MAX_BUFF, " and %d minutes", ret[0]);

      if (ret[1])
	snprintf (hour, SOMA_MAX_BUFF, "%s %d hour%s",
		  min[0] ? "," : " and", ret[1], ret[1] != 1 ? "s" : "");

      snprintf (buf, SOMA_MAX_BUFF, "%d day%s%s%s",
		ret[2], ret[2] != 1 ? "s" : "", hour, min);
      break;

    case 4:
      if (ret[0] == 1)
	snprintf (min, SOMA_MAX_BUFF, " and some seconds");
      else if (ret[0] > 0)
	snprintf (min, SOMA_MAX_BUFF, " and %d minutes", ret[0]);

      if (ret[1])
	snprintf (hour, SOMA_MAX_BUFF, "%s %d hour%s",
		  min[0] ? "," : " and", ret[1], ret[1] != 1 ? "s" : "");

      if (ret[2])
	snprintf (day, SOMA_MAX_BUFF, "%s %d day%s",
		  min[0] || hour[0] ? "," : " and",
		  ret[2], ret[2] != 1 ? "s" : "");

      snprintf (buf, SOMA_MAX_BUFF, "%d month%s%s%s%s",
		ret[3], ret[3] != 1 ? "s" : "", day, hour, min);
      break;

    case 5:
      snprintf (buf, SOMA_MAX_BUFF, "over 1 year");
      break;
    }

  return buf;
}

static void
start_ratioitem (void)
{
  int a;
  int ss;

  if (!palinsesto)
    return;

  ss = palinsesto->SoftStop;

  /* Play RatioItem time... */
  for (a = 0;
       palinsesto && a < palinsesto->RatioItem && !set_stop && !set_quit; a++)
    {
	    while(!set_quit && set_pause)
	{
	  UNLOCK ();
	  SOMA_LOG_INFO ("Pause event.");
	  set_in_pause = 1;
	  sleep (1);
	  LOCK ();
	}

      set_in_pause = 0;

      if (conf->next_item)
	break;

      if (start_check (palinsesto->timer))
	break;

      if (palinsesto->RandomItem)
	{
	  UNLOCK ();
	  must_change =
	    search_item (getrandom (0, conf->n_item), SOMA_ITEM, ss);
	}

      else
	{
	  UNLOCK ();
	  must_change = search_item (id_item++, SOMA_ITEM, ss);
	}

      if (LOCK ())
	{
	  SOMA_LOG_WARN ("Lock error.");
	  break;
	}
    }
}

static void
start_ratiospot (void)
{
  int a, ss;
  char *ps;

  if (!palinsesto)
    return;

  ss = palinsesto->SoftStop;

  if (palinsesto->PreSpot)
    {
      if (!(ps = strdup (palinsesto->PreSpot)))
	SOMA_FATAL ("Error memory.");

      UNLOCK ();

      play (ss, ps, 0);
      free (ps);

      LOCK ();
    }

  /* Play RatioSpot time... */
  if (palinsesto->RatioSpot && palinsesto->SpotController && conf->Spot)
    {
      int number = palinsesto->RatioSpot;
      int softstop = palinsesto->SoftStop;

      /* Spot Controller Activated */
      spot_controller (number, softstop);
    }

  else if (conf && conf->spot)
    {
      /* If Spot Controller is no activate... */
      for (a = 0;
	   palinsesto && a < palinsesto->RatioSpot && !set_stop
	   && !set_quit; a++)
	{
	    while(!set_quit && set_pause)
	    {
	      UNLOCK ();
	      SOMA_LOG_INFO ("Pause event.");
	      set_in_pause = 1;
	      sleep (1);
	      LOCK ();
	    }

	  set_in_pause = 0;

	  if (conf->next_item)
	    break;

	  if (start_check (palinsesto->timer))
	    break;

	  if (palinsesto->RandomSpot)
	    {
	      UNLOCK ();
	      search_item (getrandom (0, conf->n_spot), SOMA_SPOT, ss);
	    }
	  else
	    {
	      UNLOCK ();
	      search_item (id_spot++, SOMA_SPOT, ss);
	    }

	  if (LOCK ())
	    {
	      SOMA_LOG_WARN ("Lock error.");
	      break;
	    }
	}
    }
  else
    {
      SOMA_LOG_WARN ("No Spot item founded!");
    }

  if (palinsesto->PostSpot)
    {
      if (!(ps = strdup (palinsesto->PostSpot)))
	SOMA_FATAL ("Error memory.");

      UNLOCK ();

      play (ss, ps, 0);
      free (ps);

      LOCK ();
    }
}

/* EOF */
