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

static int
palinsesto_free_memory (void)
{
  soma_pl *pl_tmp;

  /* Remove the element of the prev palinsesto in the trees */
  if (delete_pl ())
    return 1;

  /* Remove the prev struct */
  pl_tmp = conf->Palinsesto;

  while (pl_tmp)
    {
      soma_item *item, *item2;

      item = pl_tmp->PathItem;
      while (item)
	{

	  item2 = item;
	  item = item->next;

	  free (item2->item);
	  free (item2);
	}

      item = pl_tmp->PathSpot;
      while (item)
	{
	  item2 = item;
	  item = item->next;

	  free (item2->item);
	  free (item2);
	}

      if (pl_tmp->timer)
	{
	  if (pl_tmp->timer->Start)
	    free (pl_tmp->timer->Start);
	  if (pl_tmp->timer->Stop)
	    free (pl_tmp->timer->Stop);

	  free (pl_tmp->timer);
	}

      pl_tmp = pl_tmp->next;
      if (pl_tmp)
	free (pl_tmp->prev);
    }

  conf->Palinsesto = NULL;

  return 0;
}

static void
build_palinsesto_null (void *fl, const char *str, ...)
{
  if (conf->Debug >= SOMA_DEBUG_INFO)
    {
      va_list va;

      va_start (va, str);
      vfprintf (stdout, str, va);
      va_end (va);
    }
}

/* (Re)build the palinsesto struct */
int
build_palinsesto ()
{
  soma_pl *pl_tmp;
  xmlDocPtr doc;
  xmlNodePtr cur, cur_pl;

  SOMA_LOG_INFO ("Read palinsesto file...");
  xmlSetGenericErrorFunc (xmlGenericErrorContext, build_palinsesto_null);

  /* Read and parse the conf file */
  if (!(doc = xmlParseFile (conf->Palinsesto_fl)))
    {
      SOMA_LOG_ERR ("Error open palinsesto file %s", conf->Palinsesto_fl);

      if (conf->Palinsesto)
	{
	  SOMA_LOG_ERR ("Don't change the actual palinsesto.");
	  return 1;
	}
      else
	{
	  SOMA_LOG_ERR ("No palinsesto founded!");
	  return 1;
	}
    }

  if (!(cur = xmlDocGetRootElement (doc))
      || xmlStrcmp (cur->name, (xmlChar *) "PalinsestoXML"))
    {
      SOMA_LOG_ERR ("Error open palinsesto file %s", conf->Palinsesto_fl);
      xmlFreeDoc (doc);

      if (conf->Palinsesto)
	{
	  SOMA_LOG_ERR ("Don't change the actual palinsesto.");
	  return 1;
	}
      else
	{
	  SOMA_LOG_ERR ("No palinsesto founded!");
	  return 1;
	}
    }

  if (palinsesto_free_memory ())
    return 1;

  for (cur = cur->children; cur != NULL; cur = cur->next)
    {

      if (!xmlStrcmp (cur->name, (xmlChar *) "Palinsesto"))
	{
	  char *v_start = NULL;
	  char *v_stop = NULL;
	  int v_timecontinued = 0;
	  char *v_description = NULL;
	  int v_random_item = 0, v_random_spot = 0;
	  int v_softstop = 0, v_ratioitem = 0, v_priority = 0;
	  int v_ratiospot = 0, v_spotcontroller = 0;
	  char *v_type = NULL;
	  char *v_stream = NULL;
	  char *v_jingle = NULL;
	  char *v_prespot = NULL;
	  char *v_postspot = NULL;
	  char *v_module = NULL;
	  char *v_moduledata = NULL;
	  xmlNodePtr pathitem = NULL;
	  xmlNodePtr pathspot = NULL;
	  char *c;

	  cur_pl = cur->children;

	  for (; cur_pl != NULL; cur_pl = cur_pl->next)
	    {
	      if (!xmlStrcmp (cur_pl->name, (xmlChar *) "SpotController"))
		{
		  if (!
		      (c =
		       (char *) xmlNodeListGetString (doc,
						      cur_pl->xmlChildrenNode,
						      1)) || !atoi (c))
		    v_spotcontroller = 0;
		  else
		    v_spotcontroller = 1;

		  if (c)
		    free (c);
		}

	      else if (!xmlStrcmp (cur_pl->name, (xmlChar *) "Description"))
		v_description =
		  (char *) xmlNodeListGetString (doc, cur_pl->xmlChildrenNode,
						 1);

	      else if (!xmlStrcmp (cur_pl->name, (xmlChar *) "Start"))
		v_start =
		  (char *) xmlNodeListGetString (doc, cur_pl->xmlChildrenNode,
						 1);

	      else if (!xmlStrcmp (cur_pl->name, (xmlChar *) "Stop"))
		v_stop =
		  (char *) xmlNodeListGetString (doc, cur_pl->xmlChildrenNode,
						 1);

	      else if (!xmlStrcmp (cur_pl->name, (xmlChar *) "Type"))
		v_type =
		  (char *) xmlNodeListGetString (doc, cur_pl->xmlChildrenNode,
						 1);

	      else if (!xmlStrcmp (cur_pl->name, (xmlChar *) "Module"))
		v_module =
		  (char *) xmlNodeListGetString (doc, cur_pl->xmlChildrenNode,
						 1);

	      else if (!xmlStrcmp (cur_pl->name, (xmlChar *) "ModuleData"))
		v_moduledata =
		  (char *) xmlNodeListGetString (doc, cur_pl->xmlChildrenNode,
						 1);

	      else if (!xmlStrcmp (cur_pl->name, (xmlChar *) "Stream"))
		v_stream =
		  (char *) xmlNodeListGetString (doc, cur_pl->xmlChildrenNode,
						 1);

	      else if (!xmlStrcmp (cur_pl->name, (xmlChar *) "Jingle"))
		v_jingle =
		  (char *) xmlNodeListGetString (doc, cur_pl->xmlChildrenNode,
						 1);

	      else if (!xmlStrcmp (cur_pl->name, (xmlChar *) "PreSpot"))
		v_prespot =
		  (char *) xmlNodeListGetString (doc, cur_pl->xmlChildrenNode,
						 1);

	      else if (!xmlStrcmp (cur_pl->name, (xmlChar *) "PostSpot"))
		v_postspot =
		  (char *) xmlNodeListGetString (doc, cur_pl->xmlChildrenNode,
						 1);

	      else if (!xmlStrcmp (cur_pl->name, (xmlChar *) "TimeContinued"))
		{
		  if (!
		      (c =
		       (char *) xmlNodeListGetString (doc,
						      cur_pl->xmlChildrenNode,
						      1)) || !atoi (c))
		    v_timecontinued = 0;
		  else
		    v_timecontinued = 1;

		  if (c)
		    free (c);
		}

	      else if (!xmlStrcmp (cur_pl->name, (xmlChar *) "Priority"))
		{
		  if (!
		      (c =
		       (char *) xmlNodeListGetString (doc,
						      cur_pl->xmlChildrenNode,
						      1)) || !atoi (c))
		    v_priority = 0;
		  else
		    v_priority = 1;

		  if (c)
		    free (c);
		}

	      else if (!xmlStrcmp (cur_pl->name, (xmlChar *) "RandomItem"))
		{
		  if (!
		      (c =
		       (char *) xmlNodeListGetString (doc,
						      cur_pl->xmlChildrenNode,
						      1)) || !atoi (c))
		    v_random_item = 0;
		  else
		    v_random_item = 1;

		  if (c)
		    free (c);
		}

	      else if (!xmlStrcmp (cur_pl->name, (xmlChar *) "RandomSpot"))
		{
		  if (!
		      (c =
		       (char *) xmlNodeListGetString (doc,
						      cur_pl->xmlChildrenNode,
						      1)) || !atoi (c))
		    v_random_spot = 0;
		  else
		    v_random_spot = 1;

		  if (c)
		    free (c);
		}

	      else if (!xmlStrcmp (cur_pl->name, (xmlChar *) "SoftStop"))
		{
		  if (!
		      (c =
		       (char *) xmlNodeListGetString (doc,
						      cur_pl->xmlChildrenNode,
						      1)) || !atoi (c))
		    v_softstop = 0;
		  else
		    v_softstop = 1;

		  if (c)
		    free (c);
		}

	      else if (!xmlStrcmp (cur_pl->name, (xmlChar *) "RatioItem"))
		{
		  if (!
		      (c =
		       (char *) xmlNodeListGetString (doc,
						      cur_pl->xmlChildrenNode,
						      1)) || !atoi (c))
		    v_ratioitem = 0;
		  else
		    v_ratioitem = atoi (c);

		  if (c)
		    free (c);
		}

	      else if (!xmlStrcmp (cur_pl->name, (xmlChar *) "RatioSpot"))
		{
		  if (!
		      (c =
		       (char *) xmlNodeListGetString (doc,
						      cur_pl->xmlChildrenNode,
						      1)) || !atoi (c))
		    v_ratiospot = 0;
		  else
		    v_ratiospot = atoi (c);

		  if (c)
		    free (c);
		}

	      else if (!xmlStrcmp (cur_pl->name, (xmlChar *) "PathItem"))
		pathitem = cur_pl->children;

	      else if (!xmlStrcmp (cur_pl->name, (xmlChar *) "PathSpot"))
		pathspot = cur_pl->children;
	    }

	  pl_tmp =
	    soma_pl_add (v_start, v_stop, v_timecontinued, v_description,
			 v_random_item, v_random_spot, v_softstop,
			 v_ratioitem, v_ratiospot, v_spotcontroller, v_type,
			 v_stream, v_jingle, v_module, v_moduledata,
			 v_priority, v_prespot, v_postspot);

	  if (v_start)
	    free (v_start);

	  if (v_stop)
	    free (v_stop);

	  if (v_description)
	    free (v_description);

	  if (v_type)
	    free (v_type);

	  if (v_stream)
	    free (v_stream);

	  if (v_jingle)
	    free (v_jingle);

	  if (v_prespot)
	    free (v_prespot);

	  if (v_postspot)
	    free (v_postspot);

	  if (v_module)
	    free (v_module);

	  if (v_moduledata)
	    free (v_moduledata);

	  if (!pl_tmp)
	    continue;

	  /* If is Type=files, so... */
	  if (pl_tmp->Type == SOMA_CODE_FILES)
	    {
	      /* Add the PathItem */
	      if (pathitem)
		{
		  for (; pathitem; pathitem = pathitem->next)
		    {
		      if (!xmlStrcmp (pathitem->name, (xmlChar *) "item")
			  && (c =
			      (char *) xmlNodeListGetString (doc,
							     pathitem->
							     xmlChildrenNode,
							     1)))
			{
			  soma_item_pl_add (pl_tmp, 0, c);
			  free (c);
			}
		    }
		}

	      /* Add the PathSpot */
	      if (pathspot)
		{
		  for (; pathspot; pathspot = pathspot->next)
		    {
		      if (!xmlStrcmp (pathspot->name, (xmlChar *) "item")
			  && (c =
			      (char *) xmlNodeListGetString (doc,
							     pathspot->
							     xmlChildrenNode,
							     1)))
			{
			  soma_item_pl_add (pl_tmp, 1, c);
			  free (c);
			}
		    }
		}
	    }
	}
    }

  /* The winner is... */
  if (!conf->Background && conf->Palinsesto)
    {
      fprintf (stderr, "Palinsesto info:\n\n");
      pl_tmp = conf->Palinsesto;

      while (pl_tmp)
	{
	  fprintf (stderr, "%s\nStart: %s - Stop: %s\n\n",
		   pl_tmp->Description, pl_tmp->timer->Start,
		   pl_tmp->timer->Stop);

	  pl_tmp = pl_tmp->next;
	}
    }

  palinsesto = NULL;

  xmlFreeDoc (doc);

  return 0;
}

/* EOF */
