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
			Module:termcap.c
*/
/* Copyright (c) 1979 Regents of the University of California */
#include <stdio.h>
#ifdef SPARC
#include "termios.h"
#else
#include <termios.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include "tty_codes.h"

#define	BUFSIZ	1024 
#define MAXHOP	32	/* max number of tc= indirections */ 
#define E_TERMCAP "/etc/termcap"
char *termc="/termcap";
char *etc="/etc";
static char *tskip(char *);
static char *err_mes="The termcap is too long!\n";
static char *tdecode(char *str,char **area);
static char *tskip(char *bp);

#include <ctype.h> 
/* #include "local/uparm.h" */
/* 
 * termcap - routines for dealing with the terminal capability data base 
 * 
 * BUG:		Should use a "last" pointer in tbuf, so that searching 
 *		for capabilities alphabetically would not be a n**2/2 
 *		process when large numbers of capabilities are given. 
 * Note:	If we add a last pointer now we will screw up the 
 *		tc capability. We really should compile termcap. 
 * 
 * Essentially all the work here is scanning and decoding escapes 
 * in string capabilities.  We don't use stdio because the editor 
 * doesn't, and because living w/o it is not hard. 
 */

static	char *tbuf; 
static	int hopcount;	/* detect infinite loops in termcap, init 0 */

/* 
 * Get an entry for terminal name in buffer bp, 
 * from the termcap file.  Parse is very rudimentary; 
 * we just notice escaped newlines. 
 */ 
int tgetent(char *bp,char *name)
{ 
	register char *cp; 
	register int c; 
	register int i = 0, cnt = 0; 
	int skip = 0; 
	char *ibuf;
	char *cp2; 
	int tf; 

	ibuf=(char *)malloc(BUFSIZ);
	tbuf = bp; 
	tf = 0; 
#ifndef V6 
	cp = getenv("TERMCAP");
	strcpy(ibuf,getenv("HOME"));
	if(strcmp(ibuf,"/"))
		strcat(ibuf,termc);
	else
		strcpy(ibuf,termc);
	if(access(ibuf,04))
	{
		strcpy(ibuf,"/usr/local/lib/termcap");
		if(access(ibuf,04))
		{
			strcpy(ibuf,etc);
			strcat(ibuf,termc);
		}
	}
	/* 
			 * TERMCAP can have one of two things in it. It can be the 
			 * name of a file to use instead of /etc/termcap. In this 
			 * case it better start with a "/". Or it can be an entry to 
			 * use so we don't have to read the file. In this case it 
			 * has to already have the newlines crunched out. 
			 */ 
	if (cp && *cp) { 
		if (*cp!='/') { 
			cp2 = getenv("TERM"); 
			if (cp2==(char *) 0 || strcmp(name,cp2)==0) { 
				strcpy(bp,cp); 
				free(ibuf);
				return(tnchktc()); 
			} 
			else { 
				tf = open(ibuf, 0);
			} 
		} 
		else 
			tf = open(cp, 0); 
	} 
	if (tf==0) 
		tf = open(ibuf, 0);
#else 
	tf = open(ibuf, 0);
#endif 
	if (tf < 0) 
	{
		free(ibuf);
		return (-1); 
	}
	for (;;) { 
		cp = bp; 
		for (;;) { 
			if (i == cnt) { 
				cnt = read(tf, ibuf, BUFSIZ); 
				if (cnt <= 0) { 
					close(tf); 
					free(ibuf);
					return (0); 
				} 
				i = 0; 
			} 
			c = ibuf[i++]; 
			if (c == '\n') { 
				if (cp > bp && cp[-1] == '\\'){ 
					cp--; 
					skip++; 
					continue; 
				} 
				break; 
			} 
			if (skip && (c == ' ' || c == '\t')) 
				continue; 
			skip = 0; 
			if (cp >= bp+BUFSIZ) { 
				write(2,err_mes, 23);
				break; 
			} 
			else 
				*cp++ = c; 
		} 
		*cp = 0; 

		/* 
		* The real work for the match.
		*/
		if (tnamatch(name)) { 
			close(tf); 
			free(ibuf);
			return(tnchktc());
		} 
	} 
} 

/* 
 * tnchktc: check the last entry, see if it's tc=xxx. If so, 
 * recursively find xxx and append that entry (minus the names) 
 * to take the place of the tc=xxx entry. This allows termcap 
 * entries to say "like an HP2621 but doesn't turn on the labels". 
 * Note that this works because of the left to right scan. 
 */ 
int tnchktc()
{ 
	register char *p, *q; 
	char tcname[16];	/* name of similar terminal */

	char *tcbuf;
	char *holdtbuf=tbuf;
	int l; 

	tcbuf=(char *)malloc(BUFSIZ);

	p = tbuf + strlen(tbuf) - 2;	/* before the last colon */

	while (*--p != ':') 
		if (p<tbuf) { 
			write(2, "Bad termcap\n", 12);
			free(tcbuf);
			return (0); 
		} 
	p++; 
	/* p now points to beginning of last field */ 
	if (p[0] != 't' || p[1] != 'c') 
	{
		free(tcbuf);
		return(1); 
	}
	strcpy(tcname,p+3); 
	q = tcname; 
	while (*q && *q != ':') 
		q++; 
	*q = 0; 
	if (++hopcount > MAXHOP) { 
		write(2, "tc=error!\n", 10);
		free(tcbuf);
		return (0); 
	} 
	if (tgetent(tcbuf, tcname) != 1) 
	{
		free(tcbuf);
		return(0); 
	}
	for (q=tcbuf; *q != ':'; q++) 
		; 
	l = p - holdtbuf + strlen(q); 
	if (l > BUFSIZ) { 
		write(2, err_mes, 23);
		q[BUFSIZ - (p-tbuf)] = 0; 
	} 
	strcpy(p, q+1); 
	tbuf = holdtbuf; 
	free(tcbuf);
	return(1); 
} 

/* 
 * Tnamatch deals with name matching.  The first field of the termcap 
 * entry is a sequence of names separated by |'s, so we compare 
 * against each such name.  The normal : terminator after the last 
 * name (before the first field) stops us. 
 */ 
int tnamatch(char *np)
{ 
	register char *Np, *Bp; 

	Bp = tbuf; 
	if (*Bp == '#') 
		return(0); 
	for (;;) { 
		for (Np = np; *Np && *Bp == *Np; Bp++, Np++) 
			continue; 
		if (*Np == 0 && (*Bp == '|' || *Bp == ':' || *Bp == 0)) 
			return (1); 
		while (*Bp && *Bp != ':' && *Bp != '|') 
			Bp++; 
		if (*Bp == 0 || *Bp == ':') 
			return (0); 
		Bp++; 
	} 
} 

/* 
 * Skip to the next field.  Notice that this is very dumb, not 
 * knowing about \: escapes or any such.  If necessary, :'s can be put 
 * into the termcap file in octal. 
 */ 
static char *tskip(char *bp)
{ 

	while (*bp && *bp != ':') 
		bp++; 
	while (*bp == ':') 
		bp++; 
	return (bp); 
} 

/* 
 * Return the (numeric) option id. 
 * Numeric options look like 
 *	li#80 
 * i.e. the option string is separated from the numeric value by 
 * a # character.  If the option is not found we return -1. 
 * Note that we handle octal numbers beginning with 0. 
 */ 
int tgetnum(char *id)
{ 
	register int i, base; 
	register char *bp = tbuf; 

	for (;;) { 
		bp = tskip(bp); 
		if (*bp == 0) 
			return (-1); 
		if (*bp++ != id[0] || *bp == 0 || *bp++ != id[1]) 
			continue; 
		if (*bp == '@') 
			return(-1); 
		if (*bp != '#') 
			continue; 
		bp++; 
		base = 10; 
		if (*bp == '0') 
			base = 8; 
		i = 0; 
		while (isdigit(*bp)) 
			i *= base, i += *bp++ - '0'; 
		return (i); 
	} 
} 

/* 
 * Handle a flag option. 
 * Flag options are given "naked", i.e. followed by a : or the end 
 * of the buffer.  Return 1 if we find the option, or 0 if it is 
 * not given. 
 */ 
int tgetflag(char *id)
{ 
	register char *bp = tbuf; 

	for (;;) { 
		bp = tskip(bp); 
		if (!*bp) 
			return (0); 
		if (*bp++ == id[0] && *bp != 0 && *bp++ == id[1]) { 
			if (!*bp || *bp == ':') 
				return (1); 
			else if (*bp == '@') 
				return(0); 
		} 
	} 
} 

/* 
 * Get a string valued option. 
 * These are given as 
 *	cl=^Z 
 * Much decoding is done on the strings, and the strings are 
 * placed in area, which is a ref parameter which is updated. 
 * No checking on area overflow. 
 */ 
char *tgetstr(char *id,char **area)
{ 
	register char *bp = tbuf; 
	for (;;) { 
		bp = tskip(bp); 
		if (!*bp) 
			return (NULL);
		if (*bp++ != id[0] || *bp == 0 || *bp == ':' || *bp++ != id[1]) 
			continue; 
		if (*bp == '@') 
			return(NULL);
		if (*bp != '=')
			continue; 
		bp++; 
		return (tdecode(bp, area)); 
	} 
} 

/* 
 * Tdecode does the grung work to decode the 
 * string capability escapes. 
 */ 
static char *tdecode(char *str,char **area)
{ 
	register char *cp; 
	register int c; 
	register char *dp; 
	int i,jdelay=0;
/*
	while(*str>='0' && *str<='9') {
		jdelay=jdelay*10+(*str++ - '0');
	}
*/
	cp = *area; 
	while ((c = *str++) && c != ':') { 
		switch (c) { 

		case '^': 
			c = *str++ & 037; 
			break; 

		case '\\': 
			dp = "E\033^^\\\\::n\nr\rt\tb\bf\f"; 
			c = *str++; 
nextc: 
			if (*dp++ == c) { 
				c = *dp++; 
				break; 
			} 
			dp++; 
			if (*dp) 
				goto nextc; 
			if (isdigit(c)) { 
				c -= '0', i = 2; 
				do 
				    c <<= 3, c |= *str++ - '0'; 
				while (--i && isdigit(*str)); 
			} 
			break; 
		} 
		*cp++ = c; 
	} 
/*
	if ( jdelay )
	{ 
		extern ospeed;
		register int i = ospeed;
		while ( i < B9600 )
			jdelay /= 2, i++;
		jdelay++;
	}
	if(jdelay>100) jdelay=100;
	while(jdelay--) *cp++='\0';
*/
	*cp++ = 0; 
	str = *area; 
	*area = cp; 
	return (str); 
} 
