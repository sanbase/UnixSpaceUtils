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

	Last modification:Fri Mar 23 14:16:01 2001
			Module:dps.c
*/
/* ÇÎÇéÑ ëíêéäà ëàåÇéãéÇ Ñé \0 */

#include "tty_codes.h"
#include <stdio.h>
extern int x_c,y_c;
extern int x11;

void dps(char *str_in)
{
	register unsigned char x;
	register int i=0;
	int flg=0;
	unsigned char *str=(unsigned char *)str_in;
	int x_std=x_c,y_std=y_c;

	if(str==NULL)
		return;
	for(i=0;*str;i++ )
	{
		if(*str==010)
		{
			str++;
			if(!*str)
				break;
			x_c--;
			flg=1;
			continue;
		}
		if( *str == ps )
		{
			++str;  
			x = *str++;  
			dpp( x, *str++ );
		}
		else
			if(*str==0x9b)
			{
				dpo(0xeb);
				str++; 
			} 
			else
				if(*str<' ' && *str!='\t')
				{
					dpo('?');
					str++; 
				} 
				else
				        dpo( *str++ );
			if(flg)
			        put_clr(x_c-1,y_c,0160);
			flg=0;
	}
	if(x11 && get_a2(x_std,y_std))
	{
		put_ch(x_std,y_std,get_ch(x_std,y_std)|0x200);
		put_ch(x_std+i-1,y_std,get_ch(x_std+i-1,y_std)|0x100);
	}
}

/* ÇÎÇéÑ ëàåÇéãÄ 'c' 'n' êÄá */
void dpn(int n,char c )
{
	int x_std=x_c,y_std=y_c;

	for(; --n>=0;)  dpo( c );
	if(x11 && x_c!=x_std && get_a2(x_std,y_std))
	{
		put_ch(x_std,y_std,get_ch(x_std,y_std)|0x200);
		put_ch(x_c-1,y_c,get_ch(x_c-1,y_c)|0x100);
	}
}

void dpsn(char *c,int n)
{
	if(c==NULL)
		return;
	while((*c) && (--n))
	{
		if(*c<' ')
		{
			if(*c=='\n' || *c=='\t' || *c=='\r')
			{
				dpo(' ');
				c++;
			}
			else
			{
				dpo('?');
				break;
			}
		}
		else
			dpo(*c++);
	}
}

/*
#include <stdarg.h>

void dprintf(const char *arg, ...)
{
	char str[256];
	va_list ap;

	va_start(ap,arg);
	vsprintf(str,arg,ap);
	va_end(ap);
	dps(str);
}
*/
