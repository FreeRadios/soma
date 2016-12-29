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

static void
check_data_palinsesto (char *start, char *stop, int timecontinued, char *desc,
		       int r, int s, int ratioitem, int ratiospot,
		       int spotcontroller, char *type, char *stream,
		       char *jingle, char *module, char *moduledata,
		       int priority);

static void
check_palinsesto_null (void *fl, const char *str, ...)
{
}

void
check_palinsesto (char *fl)
{
  xmlDocPtr doc;
  xmlNodePtr cur, cur_pl;

  xmlSetGenericErrorFunc (xmlGenericErrorContext, check_palinsesto_null);

  /* Read and parse the conf file */
  if (!(doc = xmlParseFile (fl)))
    fatal ("Error syntax in palinsesto file %s\n", fl);

  if (!(cur = xmlDocGetRootElement (doc))
      || xmlStrcmp (cur->name, (xmlChar *) "PalinsestoXML"))
    fatal ("Error syntax in palinsesto file %s\n", fl);

  for (cur = cur->children; cur != NULL; cur = cur->next)
    {

      if (!xmlStrcmp (cur->name, (xmlChar *) "Palinsesto"))
	{
	  char *v_start = NULL;
	  char *v_stop = NULL;
	  int v_timecontinued = 0;
	  char *v_description = NULL;
	  int v_random = 0, v_softstop = 0, v_ratioitem = 0, v_priority = 0;
	  int v_ratiospot = 0, v_spotcontroller = 0;
	  char *v_type = NULL;
	  char *v_stream = NULL;
	  char *v_jingle = NULL;
	  char *v_module = NULL;
	  char *v_moduledata = NULL;
	  xmlNodePtr pathitem = NULL;
	  xmlNodePtr pathspot = NULL;
	  char *c;
	  struct stat st;

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

	      else if (!xmlStrcmp (cur_pl->name, (xmlChar *) "Random"))
		{
		  if (!
		      (c =
		       (char *) xmlNodeListGetString (doc,
						      cur_pl->xmlChildrenNode,
						      1)) || !atoi (c))
		    v_random = 0;
		  else
		    v_random = 1;

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

	  check_data_palinsesto (v_start, v_stop, v_timecontinued,
				 v_description, v_random, v_softstop,
				 v_ratioitem, v_ratiospot, v_spotcontroller,
				 v_type, v_stream, v_jingle, v_module,
				 v_moduledata, v_priority);

	  if (v_start)
	    free (v_start);

	  if (v_stop)
	    free (v_stop);

	  if (v_type)
	    free (v_type);

	  if (v_stream)
	    free (v_stream);

	  if (v_jingle)
	    free (v_jingle);

	  if (v_module)
	    free (v_module);

	  if (v_moduledata)
	    free (v_moduledata);

	  /* Add the PathItem */
	  if (pathitem)
	    {
	      for (; pathitem; pathitem = pathitem->next)
		{
		  if (!xmlStrcmp (pathitem->name, (xmlChar *) "item")
		      && (c =
			  (char *) xmlNodeListGetString (doc,
							 pathitem->
							 xmlChildrenNode, 1)))
		    {
		      if (lstat (c, &st))
			msg
			  ("In palinsesto '%s', PathSpot list, the element '%s' does not exist.",
			   v_description ? v_description : "", c);
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
							 xmlChildrenNode, 1)))
		    {
		      if (lstat (c, &st))
			msg
			  ("In palinsesto '%s', PathSpot list, the element '%s' does not exist.",
			   v_description ? v_description : "", c);
		      free (c);
		    }
		}
	    }

	  if (v_description)
	    free (v_description);

	}
    }

  xmlFreeDoc (doc);
}

/* New palinsesto */
static void
check_data_palinsesto (char *start, char *stop, int timecontinued, char *desc,
		       int r, int s, int ratioitem, int ratiospot,
		       int spotcontroller, char *type, char *stream,
		       char *jingle, char *module, char *moduledata,
		       int priority)
{
  if (check_time (start, stop))
    fatal
      ("In the palinsesto '%s' the start time/stop are no correct formatted!\n",
       desc ? desc : "");

  if (!desc)
    msg ("Insert the description in your palinsesti!");

  if (!strcmp (type, SOMA_STR_FILES) && !ratioitem && !ratioitem)
    fatal ("Insert a correct ratioitem and ratiospot in '%s' palinsesto!\n",
	   desc ? desc : "");

  if (!strcmp (type, SOMA_STR_STREAM) && (!stream || !*stream))
    fatal ("Insert the stream in '%s' palinsesto!\n", desc ? desc : "");

  if (!strcmp (type, SOMA_STR_MODULE) && (!module || !*module))
    fatal ("Insert the module in '%s' palinsesto!\n", desc ? desc : "");
}

/* EOF */
