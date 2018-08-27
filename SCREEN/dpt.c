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
			Module:dpt.c
*/
/* èêõÜäà èé èéãüå (¨•≠Ó)
 *
 * $Log:	dpt.c,v $
 * Revision 1.1  88/09/26  16:17:21  guba
 * Initial revision
 *
 */

#include "tty_codes.h"
extern int x_c,y_c;
int dpt( elements, current, lines, width, x0, y0 )
    int  elements,*current, lines, width, x0, y0;
{
    register int c;
/*    register int current; */

/*    current = *cur; */
    if( *current<0   ||  *current>=elements  ||  lines>elements ) {
	dpend();  exit( 100 );
    }

    for(;;) {
	dpp( x0 + ((*current/lines)*width), y0 + (*current%lines) );
	switch( c=dpi() ) {
	    case cu:
		if( lines<=1 )  goto retdpt;
		if(--*current < 0)  *current += elements;
		break;
	    case cd:
		if( lines<=1 )  goto retdpt;
		if(++*current >= elements) *current -= elements;
		break;
	    case cr:
		if( lines==elements )  goto retdpt;
		if((*current += lines)>= elements) {
		    *current = (*current+1) % lines;
		}
		break;
	    case cl:
		if( lines==elements )  goto retdpt;
		if((*current -= lines) < 0) {
		    *current += ((elements+lines-1)/lines)*lines-1;
		    if(*current>=elements)  *current -= lines;
		}
		break;
	    default:
		goto retdpt;
	}
    }
retdpt:
/*    *cur = current; */
    return( c );
}
