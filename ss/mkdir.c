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

	Last modification:Fri Mar 23 14:25:46 2001
			Module:mkdir.c
*/
#include "ssh.h"
char *inp_name();
char *Mkdir( p )  /* p -> "á¨¬¢®« ­ §¢ ­¨¥" */
char    *p;
{
    char *new;
    new = inp_name( Select, p+2, 'c' );
    if( new != NULL ) {
        if( rspec( *p, new, NULL, NULL ) == 0 ) {
           errno = 0;
           close( creat(new,0666) );
           ind_err(new,1);
        }
    }
    return( new );
}


/* Ž‚…Š€, …‘’œ ‹ˆ “†… ‚ Š€’€‹Žƒ… ’€ŠŽ… ˆŒŸ */

int exist( name )
char  *name;
{
    register int i;

    for( i=lind; --i>=0; ) {
	if( !strcmp( cat[indcat[i]].d_name, name ) )  return(1);
    }
    return(0);
}
