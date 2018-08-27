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

	Last modification:Tue Sep 19 14:49:56 2000
			Module:mouse.c
*/
#include "tty_codes.h"
#include <stdio.h>
static int ii=0,jj=0;
mouse(i,xx,yy)
int i,xx,yy;
{
#ifdef UNIX
BEGIN:
	switch(i)
	{
	case 0xb0:
		if((++ii)>xx)
		{
			i=cr;
			goto END;
		}
		goto NEXT;
	case 0xb1:
		if((--ii)<(-xx))
		{
			i=cl;
			goto END;
		}
		goto NEXT;
	case 0xb2:
		if((++jj)>yy)
		{
			i=cu;
			goto END;
		}
		goto NEXT;
	case 0xb3:
		if((--jj)<(-yy))
		{
			i=cd;
			goto END;
		}
		goto NEXT;
	default:
		ii=0;
		jj=0;
		return(i);
	}
NEXT:
	i=dpi();
	goto BEGIN;
END:
	ii=0;
	jj=0;
	return(i);
#else
	return(i);
#endif
}
