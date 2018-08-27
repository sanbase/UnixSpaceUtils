/*
	Copyright (c) 1990-1995 Unitex+, ltd. Kiev, Ukraine
	Copyright (c) 1996 SOFTEX PLUS inc. Toronto ON, Canada.
	Copyright (c) 1996-2000 SanBase inc. Toronto ON, Canada.
	Written by Alexander L. Lashenko (1990-2000)
	and M. Flerov (1986-1988).
	E-mail: lashenko@unixspace.com

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

	Last modification:Wed Feb 14 14:41:32 2001
			Module:dpr.c
*/
/** ЭКРАННыЙ РЕДАКТОР СТРОКИ (Имитатор)
 *
 * $Log:	dpr.c,v $
 * Revision 1.1  88/09/26  16:10:20  guba
 * Initial revision
 *
 */
#include <stdio.h>
extern int x_c,y_c;                 /* координаты курсора виртуальные */

#include "tty_codes.h"
#define and &&
#define or  ||
#define not !
extern char insert;
extern int ssh_flg;

int dpr(     buf, lgt, pos, mode )
char    *buf;                           /* редактируемый буфер */
int           lgt;                    /* его длина */
int               *pos;               /* смещение */
unsigned                mode;           /* флаги */
{
	register int c;
	int x,y;
	char line[129];
	x=x_c;
	y=y_c;
	for(c=lgt;--c>=0;)
		if(buf[c]!=' ') break; /* ищем смысл. конец */
	memcpy(line,buf,c+1);
	line[c+1]=0;
	if(c==lgt-1)
		insert=0;
	else
		insert=1;
	ssh_flg=1;
	c=edit(0,line,128,lgt,x,y,0);
	strcpy(buf,line);
	return( c );
}
