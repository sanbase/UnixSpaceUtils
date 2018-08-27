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

	Last modification:Fri Mar 23 14:16:21 2001
			Module:help.c
*/
/* ÇÎÑÄóÄ äêÄíäéâ íÖêåàçÄãé-çÖáÄÇàëàåéâ ëèêÄÇéóçéâ àçîéêåÄñàà */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "tty_codes.h"

extern struct s_tdescr t;
extern int rus_lat;
int Execvp(char *name, char *a1, char *a2, char *a3);

void help(char *name,char symbol )
{
	int j;
	char *ch;
	static char ssym[2];
	unsigned    stt;
	char buf[80];
	int rl;

	rl=rus_lat;
/*        dpend(); */
	ssym[0] = symbol;
	signal(SIGINT,SIG_IGN);
	if((stt = fork()) == 0 )
	{
		if(!(ch=getenv("DESTDIR")))
			strcpy( buf, HELPDIR );
		else
		{
	                strcpy(buf,ch);
			strcat(buf,"/lib/help");
		}
		strcat( buf, "/HELP" );
		Execvp( buf, "HELP", name, ssym);
		exit(0);
	}
	wait((int *)0);
/*        dpbeg(); */
	dpinit();
	rus_lat=rl;
}
/*
 *      Execlp(name, arg,...,0) (like execl, but does path search)
 *      Execvp(name, argv)      (like execv, but does path search)
 */
#include <errno.h>
extern  int     errno;

static char  shell[]  = "/bin/sh";
static char  cshell[] = "/bin/csh";
static char *execat(char *s1,char *s2,char *si);
extern char **environ;


int Execvp(char *name, char *a1, char *a2, char *a3)
{
	register char *cp;
	char           fname[128];
	char          *newargs[256];
	register int   i;
	char           c;

	cp=getenv("PATH");
	if( strchr(name,'/')!=NULL )  cp = "";
	do
	{
		cp = execat(cp, name, fname);
		execle(fname, a1, a2, a3, NULL, environ);
	} 
	while( cp );
	return(-1);
}
static char *execat(char *s1,char *s2,char *si)
{
	register char *s;

	s = si;
	while( *s1 && *s1 != ':' && *s1 != '-' )  *s++ = *s1++;
	if( si != s )  *s++ = '/';
	strcpy( s, s2 );
	return( *s1 ? ++s1 : 0 );
}
