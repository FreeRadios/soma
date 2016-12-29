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

static int denyallow_check (void **ip, int *type, int *netmask, int addr);
static int denyallow_check_single (void *ip, int type, int netmask,
				   unsigned int addr);
static void *denyallow_parse_type (char *str, int *type);
static int denyallow_parse_netmask (int type, char *ip_str);
static int *denyallow_parse_netmask_data (char *ip_str);
static int *denyallow_parse_netmask_number (char *str);

int
denyallow_parser (int addr)
{
  struct denyallow_t *da;

  SOMA_NO_LOG_ALL ("DenyAllow Parser %d", addr);

  da = conf->host_allow;
  while (da)
    {
      if (!denyallow_check (da->ip, da->type, da->netmask, addr))
	{
	  SOMA_NO_LOG_ALL ("Accept a connect from this ip for allow low");
	  return 0;
	}

      da = da->next;
    }

  da = conf->host_deny;
  while (da)
    {
      if (!denyallow_check (da->ip, da->type, da->netmask, addr))
	{
	  SOMA_NO_LOG_ALL ("No accept a connect from this ip for deny low");
	  return 1;
	}

      da = da->next;
    }

  SOMA_NO_LOG_ALL ("Accept a connect from this ip for no lows");
  return 0;
}

static int
denyallow_check (void **ip, int *type, int *netmask, int addr)
{
  if (!denyallow_check_single
      (ip[0], type[0], netmask[0], ((unsigned int) addr >> 24))
      && !denyallow_check_single (ip[1], type[1], netmask[1],
				  ((unsigned int) (addr >> 16) % 256))
      && !denyallow_check_single (ip[2], type[2], netmask[2],
				  ((unsigned int) (addr >> 8) % 256))
      && !denyallow_check_single (ip[3], type[3], netmask[3],
				  ((unsigned int) (addr) % 256)))
    return 0;

  return 1;
}

static int
denyallow_check_single (void *ip, int type, int netmask, unsigned int addr)
{
  SOMA_NO_LOG_ALL ("Check %d as %s.", addr,
		   type == SOMA_DENYALLOW_NUMBER ? "number" : type ==
		   SOMA_DENYALLOW_CONDITION ? "condition" : "netmask");

  if (type == SOMA_DENYALLOW_NUMBER)
    {
      int value = (int) ip;

      if (value != -1 && value != addr)
	return 1;

      return 0;
    }

  if (type == SOMA_DENYALLOW_CONDITION)
    {
      char *value = (char *) ip;
      int numb;

      /* Major && = */
      if (*value == '>' && *(value + 1) == '=')
	{
	  numb = atoi (value + 2);
	  if (addr >= numb)
	    return 0;
	  return 1;
	}

      /* Major */
      if (*value == '>')
	{
	  numb = atoi (value + 1);
	  if (addr > numb)
	    return 0;
	  return 1;
	}

      /* Minor && = */
      if (*value == '<' && *(value + 1) == '=')
	{
	  numb = atoi (value + 2);
	  if (addr <= numb)
	    return 0;
	  return 1;
	}

      /* Minor */
      if (*value == '<')
	{
	  numb = atoi (value + 1);
	  if (addr < numb)
	    return 0;
	  return 1;
	}

      /* Different */
      if (*value == '!')
	{
	  numb = atoi (value + 1);
	  if (addr != numb)
	    return 0;
	  return 1;
	}

      return 0;
    }

  if (type == SOMA_DENYALLOW_NETMASK)
    {
      int value = (int) ip;

      if ((value & netmask) == (addr & netmask))
	return 0;

      return 1;
    }

  return 0;
}

void
denyallow_add (int type, char *ip_str)
{
  struct denyallow_t *da;
  void *ip[4];
  int t[4];
  int i;
  int j;
  int k;
  char *a;

  SOMA_NO_LOG_ALL ("DenyAllow Add %s", ip_str ? ip_str : "");

  if (!ip_str)
    return;

  if (!denyallow_parse_netmask (type, ip_str))
    return;

  if (!(a = (char *) malloc (sizeof (char) + strlen (ip_str) + 2)))
    SOMA_FATAL ("Error: memory.");

  sprintf (a, "%s.", ip_str);

  k = i = j = 0;
  while (*(a + i))
    {
      if (*(a + i) == '.')
	{
	  *(a + i) = 0;

	  ip[j] = denyallow_parse_type (a + k, &t[j]);
	  j++;
	  k = i + 1;
	}
      i++;
    }

  if (!(da = (struct denyallow_t *) malloc (sizeof (struct denyallow_t))))
    SOMA_FATAL ("Error: memory.");

  memset (da, 0, sizeof (struct denyallow_t));

  da->ip[0] = ip[0];
  da->type[0] = t[0];

  da->ip[1] = ip[1];
  da->type[1] = t[1];

  da->ip[2] = ip[2];
  da->type[2] = t[2];

  da->ip[3] = ip[3];
  da->type[3] = t[3];

  if (type == SOMA_HOST_DENY)
    {
      da->next = conf->host_deny;
      conf->host_deny = da;
    }
  else
    {
      da->next = conf->host_allow;
      conf->host_allow = da;
    }
}

static void *
denyallow_parse_type (char *str, int *type)
{
  int i;
  int numb;
  char *s;
  char *trim;

  if (!(s = strdup (str)))
    SOMA_FATAL ("Error memory.");

  trim = soma_trim (s);

  /* is empty ? */
  if (!strncasecmp (trim, "x", 1))
    {
      free (s);

      *type = SOMA_DENYALLOW_NUMBER;
      return (void *) -1;
    }

  i = 0;
  numb = 1;

  /* is number ? */
  while (*(str + i))
    {
      if (*(str + i) < '0' || *(str + i) > '9')
	numb = 0;

      i++;
    }

  if (numb)
    {
      numb = atoi (trim);
      free (s);

      *type = SOMA_DENYALLOW_NUMBER;
      return (void *) numb;
    }

  /* is a condition ? */
  *type = SOMA_DENYALLOW_CONDITION;

  if (!(trim = strdup (trim)))
    SOMA_FATAL ("Error memory.");

  free (s);

  return trim;
}

static int
denyallow_parse_netmask (int type, char *ip_str)
{
  struct denyallow_t *da;

  int len = strlen (ip_str);
  char *str = strdup (ip_str);
  char *netmask;
  int i, k;
  int *netmask_data;
  int *ip_data;

  for (i = k = 0; i < len; i++)
    {
      if (str[i] == '/')
	{
	  if (!k)
	    k = i;
	  else
	    {
	      free (str);
	      return 1;
	    }
	}
    }

  if (!k)
    {
      free (str);
      return 1;
    }

  netmask = strdup (str + k + 1);
  str[k] = 0;

  if (!(netmask_data = denyallow_parse_netmask_number (netmask)) &&
      !(netmask_data = denyallow_parse_netmask_data (netmask)))
    {
      free (str);
      free (netmask);
      return 1;
    }

  if (!(ip_data = denyallow_parse_netmask_data (str)))
    {
      free (str);
      free (netmask);
      free (netmask_data);
      return 1;
    }

  if (!(da = (struct denyallow_t *) malloc (sizeof (struct denyallow_t))))
    SOMA_FATAL ("Error: memory.");

  memset (da, 0, sizeof (struct denyallow_t));

  da->ip[0] = (void *) ip_data[0];
  da->type[0] = SOMA_DENYALLOW_NETMASK;
  da->netmask[0] = netmask_data[0];

  da->ip[1] = (void *) ip_data[1];
  da->type[1] = SOMA_DENYALLOW_NETMASK;
  da->netmask[1] = netmask_data[1];

  da->ip[2] = (void *) ip_data[2];
  da->type[2] = SOMA_DENYALLOW_NETMASK;
  da->netmask[2] = netmask_data[2];

  da->ip[3] = (void *) ip_data[3];
  da->type[3] = SOMA_DENYALLOW_NETMASK;
  da->netmask[3] = netmask_data[3];

  if (type == SOMA_HOST_DENY)
    {
      da->next = conf->host_deny;
      conf->host_deny = da;
    }
  else
    {
      da->next = conf->host_allow;
      conf->host_allow = da;
    }

  free (netmask_data);
  free (ip_data);
  free (netmask);
  free (str);

  return 0;
}

static int *
denyallow_parse_netmask_data (char *ip_str)
{
  void *ip[4];
  int t;
  int i;
  int j;
  int k;
  char *a;
  int *ret;

  if (!(a = (char *) malloc (sizeof (char) + strlen (ip_str) + 2)))
    SOMA_FATAL ("Error: memory.");

  sprintf (a, "%s.", ip_str);

  k = i = j = 0;
  while (*(a + i))
    {
      if (*(a + i) == '.')
	{
	  *(a + i) = 0;

	  ip[j] = denyallow_parse_type (a + k, &t);

	  if (t != SOMA_DENYALLOW_NUMBER)
	    {
	      free (ip[j]);
	      free (a);
	      return NULL;
	    }

	  j++;
	  k = i + 1;
	}
      i++;
    }

  if (!(ret = (int *) malloc (sizeof (int) * 4)))
    SOMA_FATAL ("Error: memory.");

  ret[0] = (int) ip[0];
  ret[1] = (int) ip[1];
  ret[2] = (int) ip[2];
  ret[3] = (int) ip[3];

  return ret;
}

static int *
denyallow_parse_netmask_number (char *str)
{
  int *ret;
  int i, numb;
  int a[4];

  i = 0;
  numb = 1;

  /* is number ? */
  while (*(str + i))
    {
      if (*(str + i) < '0' || *(str + i) > '9')
	numb = 0;

      i++;
    }

  if (!numb)
    return NULL;

  numb = atoi (str);

  if (!(ret = (int *) malloc (sizeof (int) * 4)))
    SOMA_FATAL ("Error: memory.");

  memset (ret, 0, sizeof (int) * 4);

  a[3] = numb - 24 < 0 ? 0 : numb - 24;
  numb -= a[3];

  a[2] = numb - 16 < 0 ? 0 : numb - 16;
  numb -= a[2];

  a[1] = numb - 8 < 0 ? 0 : numb - 8;
  numb -= a[1];

  a[0] = numb;

  for (i = 0; i < 4; i++)
    switch (a[i])
      {
      case 1:
	ret[i] = 128;
	break;
      case 2:
	ret[i] = 192;
	break;
      case 3:
	ret[i] = 224;
	break;
      case 4:
	ret[i] = 240;
	break;
      case 5:
	ret[i] = 248;
	break;
      case 6:
	ret[i] = 252;
	break;
      case 7:
	ret[i] = 254;
	break;
      case 8:
	ret[i] = 255;
	break;
      }

  return ret;
}

/*
0           00000000.00000000.00000000.00000000  0.0.0.0         256 A
1           10000000.00000000.00000000.00000000  128.0.0.0       128 A
2           11000000.00000000.00000000.00000000  192.0.0.0        64 A
3           11100000.00000000.00000000.00000000  224.0.0.0        32 A
4           11110000.00000000.00000000.00000000  240.0.0.0        16 A
5           11111000.00000000.00000000.00000000  248.0.0.0         8 A
6           11111100.00000000.00000000.00000000  252.0.0.0         4 A
7           11111110.00000000.00000000.00000000  254.0.0.0         2 A
8           11111111.00000000.00000000.00000000  255.0.0.0         1 A
9           11111111.10000000.00000000.00000000  255.128.0.0     128 B
10          11111111.11000000.00000000.00000000  255.192.0.0      64 B
11          11111111.11100000.00000000.00000000  255.224.0.0      32 B
12          11111111.11110000.00000000.00000000  255.240.0.0      16 B
13          11111111.11111000.00000000.00000000  255.248.0.0       8 B
14          11111111.11111100.00000000.00000000  255.252.0.0       4 B
15          11111111.11111110.00000000.00000000  255.254.0.0       2 B
16          11111111.11111111.00000000.00000000  255.255.0.0       1 B
17          11111111.11111111.10000000.00000000  255.255.128.0   128 C
18          11111111.11111111.11000000.00000000  255.255.192.0    64 C
19          11111111.11111111.11100000.00000000  255.255.224.0    32 C
20          11111111.11111111.11110000.00000000  255.255.240.0    16 C
21          11111111.11111111.11111000.00000000  255.255.248.0     8 C
22          11111111.11111111.11111100.00000000  255.255.252.0     4 C
23          11111111.11111111.11111110.00000000  255.255.254.0     2 C
24          11111111.11111111.11111111.00000000  255.255.255.0     1 C
25          11111111.11111111.11111111.10000000  255.255.255.128
26          11111111.11111111.11111111.11000000  255.255.255.192
27          11111111.11111111.11111111.11100000  255.255.255.224
28          11111111.11111111.11111111.11110000  255.255.255.240
29          11111111.11111111.11111111.11111000  255.255.255.248
30          11111111.11111111.11111111.11111100  255.255.255.252
31          11111111.11111111.11111111.11111110  255.255.255.254
32          11111111.11111111.11111111.11111111  255.255.255.255
*/

/* EOF */
