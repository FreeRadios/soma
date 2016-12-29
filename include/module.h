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

#ifndef __SOMA_MODULE_H__
#define __SOMA_MODULE_H__

/* If you want write a module for somad, you must write these functions: */

/**
 * somad uses this function to get the name of your module
 * \return the name of your module.
 */
char *		module_name		(void);

/**
 * somad runs this function when your module must start. This is the first
 * function that somad run. 
 *
 * \param moduledata the moduledata value in the palinsesto.
 * \return if this function return a value != 0 somad does not schedule it
 */
int		module_init		(char *moduledata);

/**
 * If somad finds this function in your module, execs it. This function
 * must return a item that somad can play (local or from the distribuited
 * filesystem)
 *
 * \param moduledata the moduledata value in the palinsesto.
 * \return the item. If It is NULL, somad does not schedule your module.
 */
char *		module_run		(char *moduledata);

/**
 * If somad finds this function in your module, execs it. This function
 * must return a list of string. Somad exec the command explain in this list
 * with the execve function. The last item must be NULL.
 *
 * \param moduledata the moduledata value in the palinsesto.
 * \return the list. If It is NULL, somad does not schedule your module.
 */
char **		module_exec		(char *moduledata);

/**
 * If somad finds this function in your module, execs it. This function
 * must return a list of item. Somad schedule the list with the iteration
 * with the spot.
 *
 * \param moduledata the moduledata value in the palinsesto.
 * \return the list. If It is NULL, somad does not schedule your module.
 */
soma_item *	module_list		(char *moduledata);

/**
 * When your module must be stopped, somad runs this function.
 *
 * \param moduledata the moduledata value in the palinsesto.
 * \return a error code.
 */
int		module_quit		(char *moduledata);

/**
 * This function tells to somad the number of seconds of current item. 
 * If the number is != 0, after this seconds, somad stops the player.
 *
 * \param moduledata the moduledata value in the palinsesto.
 * \return a number of second.
 */
int		module_time_stop	(char *moduledata);

#endif
