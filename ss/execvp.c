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

	Last modification:Fri Mar 23 14:23:56 2001
			Module:execvp.c
*/
/*
 *      execlp(name, arg,...,0) (like execl, but does path search)
 *      execvp(name, argv)      (like execv, but does path search)
 */
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#define NULL    ((char*)0)
extern  int     errno;
extern  char   *getenv(), *index();

static char  shell[]  = "/bin/sh";
static char  cshell[] = "/bin/csh";
static char *execat();
extern char **environ;
extern int x11;

_execlp(char *name, char *p1, char *p2, char *p3, char *p4 )
{
	char *cmd[5];

	cmd[0]=p1; cmd[1]=p2; cmd[2]=p3; cmd[3]=p4; cmd[4]=0;
	return(_execvp(name, cmd));
}
_execvp(   name,  argv )
char *name,**argv;
{
	register char *cp;
	char           fname[128];
	char          *newargs[256];
	register int   i;
	char           c;

	cp=getenv("PATH");
	if( index(name,'/') )  cp = "";
	do
	{
		cp = execat(cp, name, fname);
		if(access(fname,X_OK))
			continue;
		if(!x11)
			execve(fname, argv, environ);
		else
		{
			int i,sh=0;
			char *cmd=(char *)malloc(16);
			if(argv[0] && argv[1] && !strcmp(argv[0],"/bin/sh") && !strcmp(argv[1],"-c"))
				sh=1;
			*cmd=0;
			for(i=0;argv[i];)
			{
				cmd=(char *)realloc(cmd,strlen(cmd)+strlen(argv[i])+3);
				if(i==2 && sh)
					strcat(cmd,"\"");
				strcat(cmd,argv[i]);
				if(!argv[++i])
					break;
				strcat(cmd," ");
			}
			if(sh)
				strcat(cmd,"\"");
			execlp("/usr/X11R6/bin/xterm","xterm","-geometry","80x25+0-0","-fn","8x16","-fg", "black","-bg","white","-e","dpexe",cmd,0);
			exit(0);
		}
		if( errno == ENOEXEC )
		{
			if( (i=open(fname,O_RDONLY)) == -1 )
			{
				return(-1);
			}
			c = 0;
			read(i,&c,1);
			close(i);
			newargs[0] = (c=='#') ? "csh" : "sh";
			newargs[1] = fname;
			for (i=1; newargs[i+1]=argv[i]; i++)
			{
				if ( i>=254 )
				{
					return(-1);
				}
			}
			execve( c=='#' ? cshell : shell, newargs, environ);
			return(-1);
		}
	}
	while( cp );
	return(-1);
}
static char *execat(s1, s2, si)
register char  *s1,*s2;
char                   *si;
{
	register char *s;

	s = si;
	while( *s1 && *s1 != ':' && *s1 != '-' )  *s++ = *s1++;
	if( si != s )  *s++ = '/';
	strcpy( s, s2 );
	return( *s1 ? ++s1 : 0 );
}
