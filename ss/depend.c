/*
	Copyright (c) 1990-1995 Unitex+, ltd. Kiev, Ukraine
	Copyright (c) 1996 SOFTEX PLUS inc. Toronto ON, Canada.
	Copyright (c) 1996-2000 SanBase inc. Toronto ON, Canada.
	Written by Alexander L. Lashenko (1990-2000)
			   and M. Flerov (1986-1988).

	E-mail: lashenko@unixspace.com
	http://www.unixspace.com/tools

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

	Last modification:Fri Mar 23 14:23:41 2001
			Module:depend.c
*/
#include "ssh.h"
static char *suffixes[]={"",".y",".cpp",".c",".cc",".s",".o",".b",".bak",",v"};
static int  depmap[] = {03600,0150,0140,0140,0140,0100,0,0,0,0,01400};
#define nsuf ( (sizeof suffixes)/(sizeof suffixes[0]) )
int depend( old, new )
char   *old,*new;
{
	register int lroot;

	if( *old != *new )              return( 0 );      /* эвристика */
	if( lroot = is_boss(old,new) )  return(  lroot );
	if( lroot = is_boss(new,old) )  return( -lroot );
	return( 0 );
}

int is_boss( master, slave )
char    *master,*slave;
{
	register char *n, *e;
	register int   i;
	char          *s;
	int            l, lmaster, lroot, j;
	int           deb;

	lmaster = strlen(master);
	e       = &master[lmaster];

	for( i=0; i<nsuf; ++i )
	{                   /* ищем суффикс у master */
		n = suffixes[i];  
		l = strlen( n );
		if( l<lmaster && !strcmp(e-l,n)
		    && !strncmp(master,slave,lmaster-l) )
		{
			deb   = depmap[i];      /* множество суф.у slave */
			lroot = lmaster - l;    /* длина корня */
			s     = &slave[lroot];  /* адрес суффикса slave */
			for( j=0; j<nsuf; ++j )
			{           /* проверяем суфф. slave */
				if((deb&1) && !strcmp(s,suffixes[j]))  return(lroot);
				deb >>= 1;
			}
		}
	}
	return( 0 );                 /* нет известного суффикса */
}
