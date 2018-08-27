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

	Last modification:Fri Mar 23 14:16:55 2001
			Module:move.c
*/
#include "tty_codes.h"
/* переместить область n к координатам x,y (только визуально) */
void _scrbufout();

move_box(int x,int y,int n)
{
	register int i;
	struct area *region;
	region=get_reg(n);
	i=get_box(x,y,region->l,region->h); /* запомним, что там было */
	put_box(x,y,n);     /* поместим туда область     */
	screen_out();        /* отобразим, что получилось */
	_scrbufout();
	put_box(x,y,i);     /* восстановим видеопамять   */
	free_box(i);
	return(0);
}
