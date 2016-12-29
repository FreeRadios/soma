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

#ifndef __SOMA_LOCAL_H__
#define __SOMA_LOCAL_H__

#include <sys/types.h>
#include <time.h>

#ifdef __cplusplus
extern "C"
{
#endif

  /**
   * This function initializes the ffmpeg format. It must be exec as
   * first
   */
  void				soma_local_init		(void);

  /**
   * This function return a soma_stat struct from a local file.
   *
   * \param file the local file
   * \return the soma_stat struct. You must free it after usage with
   * soma_local_stat_free function
   */
  soma_stat *			soma_local_stat		(char *file);

  /**
   * This function return a soma_stat_dir struct from a local directory.
   *
   * \param directory the local directory
   * \param symlink if it is 0 the function does not read the symlink file
   * else yes.
   * \return the soma_stat_dir struct. You must free it after usage with
   * soma_local_stat_dir_free function
   */
  soma_stat_dir *		soma_local_stat_dir	(char *directory,
		  					 int symlink);

  /**
   * This function frees a soma_stat struct
   *
   * \param output the soma_stat struct
   */
  void				soma_local_stat_free	(soma_stat *output);

  /**
   * This function frees a soma_stat_dir struct
   *
   * \param output the soma_stat_dir struct
   */
  void				soma_local_stat_dir_free
	  						(soma_stat_dir *output);

#ifdef __cplusplus
}
#endif

#endif
