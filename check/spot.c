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
check_data_spot (char *description, char *start, char *stop,
		 int timecontinued, int repeat);

static void
check_spot_null (void *fl, const char *str, ...)
{
}

void
check_spot (char *fl)
{
  xmlDocPtr doc;
  xmlNodePtr cur, cur_pl;
  struct stat st;

  xmlSetGenericErrorFunc (xmlGenericErrorContext, check_spot_null);

  /* Read and parse the conf file */
  if (!(doc = xmlParseFile (fl)))
    fatal ("Error open spots file %s\n", fl);

  if (!(cur = xmlDocGetRootElement (doc))
      || xmlStrcmp (cur->name, (xmlChar *) "SpotXML"))
    fatal ("Syntax error in spot file %s\n", fl);

  for (cur = cur->children; cur != NULL; cur = cur->next)
    {

      if (!xmlStrcmp (cur->name, (xmlChar *) "Spot"))
	{
	  char *v_description = NULL;
	  char *v_start = NULL;
	  char *v_stop = NULL;
	  int v_timecontinued = 0;
	  int v_repeat = 0;
	  xmlNodePtr path = NULL;
	  char *c;

	  cur_pl = cur->children;

	  for (; cur_pl != NULL; cur_pl = cur_pl->next)
	    {
	      if (!xmlStrcmp (cur_pl->name, (xmlChar *) "Description"))
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

	      else if (!xmlStrcmp (cur_pl->name, (xmlChar *) "Repeat"))
		{
		  if (!
		      (c =
		       (char *) xmlNodeListGetString (doc,
						      cur_pl->xmlChildrenNode,
						      1)) || !atoi (c))
		    v_repeat = 0;
		  else
		    v_repeat = 1;

		  if (c)
		    free (c);
		}

	      else if (!xmlStrcmp (cur_pl->name, (xmlChar *) "Path"))
		path = cur_pl->children;
	    }

	  check_data_spot (v_description, v_start, v_stop, v_timecontinued,
			   v_repeat);

	  if (v_start)
	    free (v_start);

	  if (v_stop)
	    free (v_stop);

	  /* Add the Path */
	  if (path)
	    {
	      for (; path; path = path->next)
		{
		  if (!xmlStrcmp (path->name, (xmlChar *) "item")
		      && (c =
			  (char *) xmlNodeListGetString (doc,
							 path->
							 xmlChildrenNode, 1)))
		    {

		      if (lstat (c, &st))
			msg
			  ("In spot '%s', Path list, the element '%s' does not exist.",
			   v_description ? v_description : "", c);

		      free (c);
		    }
		}
	    }

	  if (v_description)
	    free (v_description);

	}
    }
}

static void
check_data_spot (char *description, char *start, char *stop,
		 int timecontinued, int repeat)
{
  if (!description)
    msg ("Insert a description in your spot list!");

  if (check_time (start, stop))
    fatal ("Start/Stop syntax error in the spot element '%s'\n",
	   description ? description : "");
}

/* EOF */
