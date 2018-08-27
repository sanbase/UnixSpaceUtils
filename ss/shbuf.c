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

	Last modification:Fri Mar 23 14:26:25 2001
			Module:shbuf.c
*/
/* ”Œˆ‚€ˆ… ‘’Šˆ „‹Ÿ ‚›‡‚€ SHELL'  */

#include "ssh.h"
#include <setjmp.h>
#include "tty_codes.h"

extern jmp_buf on_break;

#define           lshbuf 512
static char shbuf[lshbuf+1], *pshbuf;
static char metasymb[] = "\\ ;!\"#$%&'()|*?^<>{}:@";


/* ˆˆ–ˆ€‹ˆ‡€–ˆŸ ‡€‹…ˆŸ “”…€ */

ini_shbuf() { 
	pshbuf = shbuf; 
}


/* Œ…™€…’ „ˆ ‘ˆŒ‚‹ ‚ “”… */

put_sh(  symb )
char symb;
{
	if( pshbuf >= &shbuf[lshbuf] )
	{
		dpp(0,ysize-1);  
		dpo(el);
		dps(messages[56]);
		dpi();
		longjmp( on_break, 1 );
	}
	*pshbuf++ = symb;
}


/* Šˆ‚€ˆ… ‘’Šˆ 'p' ‚ “”… */

cpy_shbuf(         p )
register char *p;
{
	while( *p )  put_sh( *p++ );
}


/* Šˆ‚€ˆ… ‘’Šˆ 'p' ‚ “”… ‘ Š€ˆ‚€ˆ…Œ Œ…’€‘ˆŒ‚‹‚ */

mcpy_shbuf(        p )
register char *p;
{
	while( *p )
	{
		if( index( metasymb, *p ) != 0 )  put_sh( '\\' );
		put_sh( *p++ );
	}
}


/* „€‚‹…ˆ… ˆŒ…ˆ ˆ‡ ‘’€’œˆ pd */

add_name(                 pd, aflag )
register struct dir2 *pd;
int                       aflag;
{
	register char c;
	put_sh( ' ' );
	if( (c=pd->d_mark)=='<' or c=='>' )
		put_sh( c );
	if( aflag==1 )
	{
		mcpy_shbuf(altcat);
		if( altcat[1] and altcat[0] )
			put_sh( '/' );
	} 
	else if( aflag==2 )
	{
		if( altpath )
		{
			mcpy_shbuf(altpath); 
			put_sh('/');
		}
	}
	mcpy_shbuf(pd->d_name);
}


/* ‚‡‚€’ ‘”Œˆ‚€‰ ‘’Šˆ */

char *get_shbuf()
{
	put_sh( 0 );
	return( shbuf );
}
