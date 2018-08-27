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

	Last modification:Fri Mar 23 14:25:16 2001
			Module:inp_name.c
*/
/* ‚‚Ž„ ˆŒ…ˆ ”€‰‹€ ‘ Ž‚…ŠŽ‰, —’Ž ’Ž Ž‚Ž… ˆŒŸ */

#include "tty_codes.h"
#include "ssh.h"
#define DD_NAME 64
int no_del;
char *inp_name( old, ask, symhelp )
char  *old,*ask, symhelp;
{
	register int c, i;
	int          pos;
	static char  new[DD_NAME+1];
#ifdef RUSDIAG
	static char  rename[] = "á¬¥­¨â¥ ¨¬ï ";
#else
	static char  rename[] = "rename  ";
#endif
	no_del=0;
	if(arname) return(NULL);
	strcpy( new, old );
	for(;;)
	{
		if( not exist(new) )
		{
			if( new[0] ) break;
			goto cin;
		} else {
			if( new[0] )
			{
				dpo(bl);
				dpp(0,ysize-1);
				dps(rename);
			} else
			{
cin:
				dpp(0,ysize-1);
			}
		}

		dps(ask);
		dps(" >");
		dpo(es);
		dpn(DD_NAME,' ');
		dpo('<');
		dpn(DD_NAME+1,cl);
		/*
		dps(new);
*/
		if( (pos=strlen(new)) >= DD_NAME )
		{
			--pos;
			dpo(cl);
		}

in:
		for( i=strlen(new); i<DD_NAME; ++i )  new[i] = ' ';
		c = dpr( new, DD_NAME, &pos, on_control );
		for( i=DD_NAME; --i>=0 and new[i]==' ';  new[i]=0 )  ;
		switch( c )
		{
		case F10:
		case Esc: 
			onbreak();
		case s_refuse:
			goto ref;
		case dc:
		case F1:
		case s_help:
			help("ssh",symhelp);
			goto cin;
		case rn:
			break;
		case s_erase:
			srm(new);
			if(errno) goto ref;
			return( new );
		case F5:
			return( new );
		case F11:
			no_del=1;
	                return( new );
		case fl:
			goto in;
		default:
			dpo(bl);
			goto in;
		}
	}
	return( new );

ref:
	dpp(0,ysize-1);
	dpo(el);
	return(NULL);
}
