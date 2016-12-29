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

#include <sm.h>

int
sm_init (sm * s)
{
  if (!s)
    return SM_ERROR;

  s->spinlock = 0;
  s->conflicts = 0;
  s->timer = 1;

  return SM_OK;
}

int
sm_trylock (sm * s)
{

#ifdef i386
  int ret;

  if (!s)
    return SM_ERROR;

  __asm__ __volatile__ ("xchgl %0, %1":"=r" (ret), "=m" (s->spinlock):"0" (1),
			"m" (s->spinlock));

#elif alpha
  long ret, temp;

  if (!s)
    return SM_ERROR;

  __asm__ __volatile__ ("/* Inline spinlock test & set */\n"
			"1:\t"
			"ldl_l %0,%3\n\t"
			"bne %0,2f\n\t"
			"or $31,1,%1\n\t"
			"stl_c %1,%2\n\t"
			"beq %1,1b\n"
			"2:\tmb\n"
			"/* End spinlock test & set */":"=&r" (ret),
			"=&r" (temp),
			"=m" (s->spinlock):"m" (s->spinlock):"memory");

#elif m68k
  char ret;

  if (!s)
    return SM_ERROR;

  __asm__ __volatile__ ("tas %1; sne %0":"=g" (ret),
			"=m" (s->spinlock):"m" (s->spinlock):"cc");

#elif mips
  long ret, temp;

  if (!s)
    return SM_ERROR;

  __asm__ __volatile__ ("# Inline spinlock test & set\n\t"
			".set\tmips2\n"
			"1:\tll\t%0,%3\n\t"
			"bnez\t%0,2f\n\t"
			".set\tnoreorder\n\t"
			"li\t%1,1\n\t"
			".set\treorder\n\t"
			"sc\t%1,%2\n\t"
			"beqz\t%1,1b\n"
			"2:\t.set\tmips0\n\t"
			"/* End spinlock test & set */":"=&r" (ret),
			"=&r" (temp),
			"=m" (s->spinlock):"m" (s->spinlock):"memory");


#elif ppc
  int ret;

  if (!s)
    return SM_ERROR;

  __asm__ __volatile__ ("1:\t"
			"lwarx %0, 0, %2\n\t"
			"stwcx. %3, 0, %2\n\t"
			"bne 1b\n"
			"2:":"=&r" (ret),
			"=m" (s->spinlock):"r" (s->spinlock), "r" (1):"cr0");

#elif sparc
  int ret;

  if (!s)
    return SM_ERROR;

  __asm__ __volatile__ ("ldstub %1,%0":"=r" (ret),
			"=m" (s->spinlock):"m" (s->spinlock));

#else
#error Unknow tsl.
#endif

  if (ret)
    {
      s->conflicts++;
      s->timer += s->timer;
    }
  else
    s->timer = 1;

  return (int) ret;
}

int
sm_lock (sm * s)
{

  time_t t;

  if (!s)
    return SM_ERROR;

  t = time (NULL);

  while (sm_trylock (s))
    {
      if (t < (time (NULL) - TMAX))
	{
	  s->timer = 1;
	  return SM_BUSY;
	}

      usleep (s->timer);

      sched_yield ();
    }

  return SM_OK;
}

int
sm_unlock (sm * s)
{

  if (!s)
    return SM_ERROR;

#ifdef i386
  s->spinlock = 0;

#elif alpha
  __asm__ __volatile__ ("mb":::"memory");
  s->spinlock = 0;

#elif m68k
  s->spinlock = 0;

#elif mips
  s->spinlock = 0;

#elif ppc
  s->spinlock = 0;

#elif sparc
  __asm__ __volatile__ ("stbar; stb %1,%0":"=m" (s->spinlock):"r" (0));

#else
#error Unknow tsl.
#endif

  return SM_OK;
}

int
sm_conflicts (sm * s)
{
  if (!s)
    return SM_ERROR;

  return s->conflicts;
}

int
sm_clean_conflicts (sm * s)
{
  if (!s)
    return SM_ERROR;
  s->conflicts = 0;

  return SM_OK;
}

/* EOF */
