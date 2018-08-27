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

	Last modification:Fri Mar 23 14:26:48 2001
			Module:strt.c
*/
/* áÄèìëä äéåÄçÑÎ ë èÄêÄåÖíêÄåà 'p', 'p2', 'p3' */

#include <signal.h>
#include "tty_codes.h"
#include <stdio.h>
#include "ssh.h"

extern int MDF;
static int _strt();
extern int rus_lat;

int strt(char *p,char *p2,char *p3,char *p4 )
{
	return( _strt( 0, p, p2, p3, p4 ) );
}

int strtv(char **av )
{
	return( _strt( av ) );
}

extern int in_fd;

static int _strt( av,   p, p2, p3, p4 )
char        *av[],*p,*p2,*p3,*p4;
{
	register int i=0;
	int lang;

	lang=rus_lat;
/*        dpp(0,ysize-1); dpo(es); */
	setcolor(03);
	scrbufout();

	Ttyreset();
	for( i=MDF; i<=16; ++i )  close(i);     /* ß†™‡Î¢†•¨ ¢·• §•·™‡®Ø‚Æ‡Î */

	if( (i=fork()) == 0 )
	{
		signal(SIGINT,SIG_DFL);
		signal(SIGQUIT,SIG_DFL);
		nice(addnice);
		if( av )
		{
			_execvp( av[0], av );
		}
		else
		{
			_execlp( p, p, p2, p3, p4, 0 );
		}
		Ttyset();
		setcolor(014);
		dpp( 0, ysize-1 );      dpo(dl);
		dps( av ? *av : p );
		dps(messages[54]);
		dpi();
		Ttyreset();
		exit(0);
	}
	i = wait_pid(i);
	Ttyset();
	if( i && i != -SIGINT )
	{
		if(i==123) goto END;
		setcolor(014);
		dps(messages[55]);
		dps(Conv((long)i,3,10,' '));
		dpo(')'); dpi();
	}
	rus_lat=lang;
END:
	return( i );
}
