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

#ifndef RTLD_NOW
#  define RTLD_NOW 2
#endif

#include "somad.h"

/* Cheks and upload the module char *str */
void *
check_module (char *str)
{
  void *module;
  struct stat st;
  char tmp[SOMA_MAX_BUFF];

  SOMA_LOG_ALL ("Check Module: %s", str);

  if (!str)
    {
      SOMA_LOG_WARN ("Error in palinsesto file.");
      return NULL;
    }

  if (!lstat (str, &st))
    snprintf (tmp, SOMA_MAX_BUFF, "%s", str);

  else
    {
      snprintf (tmp, SOMA_MAX_BUFF, "%s/%s", conf->PathModules, str);

      if (lstat (tmp, &st))
	{
	  SOMA_LOG_WARN ("Module open error %s.", str);
	  return NULL;
	}
    }

  if (!(module = dlopen (tmp, RTLD_NOW | RTLD_GLOBAL)))
    {
      SOMA_LOG_WARN ("Module open error %s: %s.", str, dlerror ());
      return NULL;
    }

  if (!(internal_module_init = dlsym (module, "module_init")))
    {
      SOMA_LOG_WARN ("No found module_init function %s: %s", str, dlerror ());
      dlclose (module);
      return NULL;
    }

  internal_module_run = dlsym (module, "module_run");

  internal_module_exec = dlsym (module, "module_exec");

  internal_module_list = dlsym (module, "module_list");

  if (!internal_module_run && !internal_module_exec && !internal_module_list)
    {
      SOMA_LOG_WARN
	("No found module_exec, module_run or module_list function: %s", str);
      dlclose (module);
      return NULL;
    }

  if (!(internal_module_quit = dlsym (module, "module_quit")))
    {
      SOMA_LOG_WARN ("No found module_quit function %s: %s", str, dlerror ());
      dlclose (module);
      return NULL;
    }

  if (!(internal_module_name = dlsym (module, "module_name")))
    {
      SOMA_LOG_WARN ("No found module_name function %s: %s", str, dlerror ());
      dlclose (module);
      return NULL;
    }

  if (!(internal_module_time_stop = dlsym (module, "module_time_stop")))
    {
      SOMA_LOG_WARN ("No found module_time_stop function %s: %s", str,
		     dlerror ());
      dlclose (module);
      return NULL;
    }

  return module;
}

void
clear_module (void)
{
  SOMA_LOG_ALL ("Clean Module");

  if (module)
    dlclose (module);

  module = NULL;

  internal_module_init = NULL;
  internal_module_quit = NULL;
  internal_module_run = NULL;
}

/* EOF */
