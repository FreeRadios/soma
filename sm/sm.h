/*
 *    Copyright (C) 2002  Andrea Marchesini - Bakunin - bakunin@autistici.org
 *                        http://autistici.org/bakunin/
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

#ifndef SM_H
#define SM_H

#include <sched.h>
#include <time.h>
#include <unistd.h>

/* Max second */
#define TMAX 10

#define SM_ERROR  -1
#define SM_MEMORY -2
#define SM_OK      0
#define SM_BUSY    1

/********
 * Base *
 ********/

typedef struct
{
  int spinlock;
  int conflicts;
  unsigned long timer;
}
sm;

/* Init of mutex structure. Return SM_ERROR or SM_OK */
int sm_init (sm *);

/* Trylock. Return SM_ERROR or 0 or 1*/
int sm_trylock (sm *);

/* Lock. Return S_ERROR, SM_BUSY, SM_OK */
int sm_lock (sm *);

/* Unlock. Return S_ERROR, S_OK */
int sm_unlock (sm *);

/* Return the number of conflicts in reading. Return SM_ERROR  */
int sm_conflicts (sm *);

/* Clean the number of conflicts. Return SM_ERROR or SM_OK */
void sm_conflicts_clean (sm *);


/*******************
 * Readers-Writers *
 *******************/

typedef struct
{

  sm mutex;
  int rc;

  sm writer;
  sm reader;

  unsigned int max_number;

}
sm_rw;

/* Inizialize and set the max number of reader. Return SM_ERROR or SM_OK */
int sm_rw_init (sm_rw *, unsigned int);

/* Reader */
int sm_r_lock (sm_rw *);	/* Return SM_ERROR, SM_BUSY or SM_OK */
int sm_r_unlock (sm_rw *);	/* Return SM_ERROR or SM_OK */

/* Writer */
int sm_w_lock (sm_rw *);	/* Return SM_ERROR, SM_OK or SM_BUSY */
int sm_w_unlock (sm_rw *);	/* Return SM_ERROR or SM_OK */


/************************
 * Conditional Variable *
 ************************/

typedef struct __sm_cond_wait_str
{
  sm sm;
  struct __sm_cond_wait_str *prev;
  struct __sm_cond_wait_str *next;

}
sm_cond_wait_str;

typedef struct __sm_cond_str
{
  void *cond;

  sm sm;

  sm_cond_wait_str *w0;
  sm_cond_wait_str *w1;
  sm_cond_wait_str *w2;

  struct __sm_cond_str *next;

}
sm_cond_str;

sm_cond_str *sm_cond_list;

#define sm_cond_wait(a) sm_cond_wait_priority(a, 0)

/* Init a variable. Return SM_ERROR or SM_OK */
int sm_cond_init (void *);

/* Wait. The second variable is the priority. This is:
 * 0 -> indifferent
 * 1 -> hight
 * other -> low
 *
 * After 10 second of inattivity, start a sm_cond_signal.
 *
 * Return SM_ERROR or SM_OK
 */
int sm_cond_wait_priority (void *, char);

/* Signal. Return SM_ERROR or SM_OK */
int sm_cond_signal (void *);


#endif

/* EOF */
