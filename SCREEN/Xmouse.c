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

	Last modification:Fri Mar 23 14:15:24 2001
			Module:Xmouse.c
*/
#include <stdio.h>

char *Keys[]=
{
       "Esc", "Ctrl/A", "Ctrl/B",    "F12",    "F3",  "",     "",     "",
       "BackSpace",  "Tab", "Ctrl/Enter", "F1",      "F2",  "Enter", "F4",  "F5",
       "F6",   "F7",  "End",        "Home",    "Down","Up",    "->",  "<-",
       "Ins",  "",    "PageUp",    "PageDown", "F8",  "F9",    "F10", "F11"
};

#include "tty_codes.h"
extern struct mouse ev;
extern struct s_tdescr t;
int Xmouse(int i)
{
	char str[81];
	int l=0,x,y;

	y=t.param.ydim-1;
	if(i || (ev.y!=y) || ev.b!=1)
		return(i);
	x=ev.x;
	while(x && (get_clr(x,y)&0xff)!=07) x--;
	while(x &&  get_clr(x,y)==07) x--;
	if(x) x++;
	while(x<t.param.xdim-1 && get_ch(x,y)==' ') x++;
	while(x<t.param.xdim-1 && (str[l]=get_ch(x++,y))!=' ') l++;
	str[l]=0;
	for(x=0;x<32;x++)
	{
		if(!strcmp(str,Keys[x]))
		{
			ev.x=ev.y=-1;
			return(x);
		}
	}
END:
	return(i);
}
