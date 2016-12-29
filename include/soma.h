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

#ifndef __SOMA_SOMA_H__
#define __SOMA_SOMA_H__

#include "code.h"
#include "commons.h"
#include "module.h"

/* Next item struct */
struct nextitem {
  char *item;
 struct nextitem *next;
};

/* List for PathItem and PathSpot of all audio file in playlist.
 * This simple list is pointed into soma_data. */
typedef struct item item;
struct item
{
  char *name;

  /* If this item is played, this int is 1 else 0. When all items are to 1,
   * all items will be set to 0 */
  int play;

  /* If this item is from the current palinsesto, it is to 1 else 0. */
  int pl;

  struct item *next;
  struct item *prev;
};

#define SOMA_MAX_MARKER 100

typedef struct marker marker;
struct marker
{
  struct item *item;

  struct marker *next;
  struct marker *prev;

  int elements;
};

typedef struct soma_time soma_time;
struct soma_time
{
  /* Syntax of these values: YYYY-MM-DD DAY HH:MM */
  char *Start;
  char *Stop;

  int start_min;
  int start_hour;
  int start_wday;
  int start_mday;
  int start_month;
  int start_year;

  int stop_min;
  int stop_hour;
  int stop_wday;
  int stop_mday;
  int stop_month;
  int stop_year;
 
  int TimeContinued;
};

/* Palinsesto struct: man palinsesto.cfg */
typedef struct soma_pl soma_pl;
struct soma_pl
{
  char *Description;

  int Priority;

  soma_time *timer;
  
  /* Read code.h */
  int Type;

  int RandomItem;
  int RandomSpot;

  int SoftStop;

  int RatioItem;

  int RatioSpot;

  int SpotController;

  soma_item *PathItem;
  soma_item *PathItemLast;
  soma_item *PathSpot;
  soma_item *PathSpotLast;

  char *Stream;

  char *Jingle;
  char *PreSpot;
  char *PostSpot;

  char *Module;
  char *ModuleData;

  struct soma_pl *next;
  struct soma_pl *prev;
};

/* Spot Item struct data */
typedef struct spot_item spot_item;
struct spot_item
{
  char *item;
  int repeat;
  struct spot_item *next;
};

/* Spot struct data */
typedef struct spot_data spot_data;
struct spot_data
{
  char *Description;

  soma_time *timer;

  spot_item *Path;

  int Repeat;

  struct spot_data *next;
  struct spot_data *prev;
};

/* Spot for controller work */
typedef struct spot_list spot_list;
struct spot_list
{
  spot_item *data;
  struct spot_list *next;
};

/* Options element for ProgramItem and ProgramSpot.
 * With this list will be recreated the execve array */
typedef struct soma_opt soma_opt;
struct soma_opt
{
  char *opt;
  struct soma_opt *next;
};

/* DenyAllow struct: */
struct denyallow_t
{
  void *ip[4];
  int netmask[4];
  int type[4];

  struct denyallow_t *next;
};

/* Data config for soma. */
typedef struct soma_data soma_data;
struct soma_data
{
  /* Config File */
  char *FileCfg;

  int Debug;
  char *LogFile;
  char *PidFile;

  char Status[SOMA_MAX_BUFF];
  char Item[SOMA_MAX_BUFF];

  int Background;
  char *User;
  char *Group;
  char *Password;

  soma_item *PathItem;
  soma_item *PathSpot;

  /* Number of item and spot in DB. This file is realy util for random sequence */
  int n_item;
  int n_spot;

  marker *marker_item;
  marker *marker_item_last;
  item *item;
  item *item_last;

  marker *marker_spot;
  marker *marker_spot_last;
  item *spot;
  item *spot_last;

  char *ProgramItem;
  soma_opt *OptionsItem;

  int n_OptionsItem;

  char *ProgramStream;
  soma_opt *OptionsStream;

  int n_OptionsStream;

  char *Palinsesto_fl;
  char *Old_Palinsesto;

  char *Spot_fl;
  char *Old_Spot;

  soma_pl *Palinsesto;
  spot_data *Spot;

  /* SSL: */
#ifdef SOMA_USE_OPENSSL
  int Ssl;
  char *Certificate;
  char *PrivateKey;
#endif

  /* SOCKET: */
  int UnixSocket;

  char *UnixPath;

  char *ServerName;
  int Port;
  int Listen;

  /* MODULES */
  char *PathModules;

  soma_item *DistribuitedPath;

  time_t time_play;

  /* NEXT_ITEM */
  struct nextitem *next_item;
  int nextitem_spot;

  int SymLinks;

  struct denyallow_t *host_deny;
  struct denyallow_t *host_allow;

  /* NEXT ITEM (cache) */
  char *Item_next;
  int Item_next_softstop;
  int Item_next_from_ns;
};

#endif

/* EOF */
