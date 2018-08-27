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
			Module:tgoto.c
*/
/* Copyright (c) 1979 Regents of the University of California */ 
#define	CTRL(c)	('c' & 037) 
#include <string.h>
#include "tty_codes.h"

extern char *BC;
extern char *UP;

/* 
 * Routine to perform cursor addressing. 
 * CM is a string containing printf type escapes to allow 
 * cursor addressing.  We start out ready to print the destination 
 * line, and switch each time we print row or column. 
 * The following escapes are defined for substituting row/column: 
 * 
 *	%d	as in printf 
 *	%2	like %2d 
 *	%3	like %3d 
 *	%.	gives %c hacking special case characters 
 *	%+x	like %c but adding x first 
 * 
 *	The codes below affect the state but don't use up a value. 
 * 
 *	%>xy	if value > x add y 
 *	%r	reverses row/column 
 *	%i	increments row/column (for one origin indexing) 
 *	%%	gives % 
 *	%B	BCD (2 decimal digits encoded in one byte) 
 *	%D	Delta Data (backwards bcd) 
 * 
 * all other characters are ``self-inserting''. 
 */ 
char *tgoto(char *CM,int destcol,int destline)
{ 
	static char result[64]; 
	static char added[10]; 
	char *cp = CM; 
	register char *dp = result; 
	register int c; 
	int oncol = 0; 
	register int which = destline; 

	if (cp == 0) { 
toohard: 
		/* 
				 * ``We don't do that under BOZO's big top'' 
				 */ 
		return ("OOPS"); 
	} 
	added[0] = 0; 
	while (c = *cp++) { 
		if (c != '%') { 
			*dp++ = c; 
			continue; 
		} 
		switch (c = *cp++) { 

#ifdef CM_N 
		case 'n': 
			destcol ^= 0140; 
			destline ^= 0140; 
			goto setwhich; 
#endif 

		case 'd': 
			if (which < 10) 
				goto one; 
			if (which < 100) 
				goto two; 
			/* fall into... */ 

		case '3': 
			*dp++ = (which / 100) | '0'; 
			which %= 100; 
			/* fall into... */ 

		case '2': 
two:	 
			*dp++ = which / 10 | '0'; 
one: 
			*dp++ = which % 10 | '0'; 
swap: 
			oncol = 1 - oncol; 
setwhich: 
			which = oncol ? destcol : destline; 
			continue; 

#ifdef CM_GT 
		case '>': 
			if (which > *cp++) 
				which += *cp++; 
			else 
				cp++; 
			continue; 
#endif 

		case '+': 
			which += *cp++; 
			/* fall into... */ 

		case '.': 
			/* 
						 * This code is worth scratching your head at for a 
						 * while.  The idea is that various weird things can 
						 * happen to nulls, EOT's, tabs, and newlines by the 
						 * tty driver, arpanet, and so on, so we don't send 
						 * them if we can help it. 
						 * 
						 * Tab is taken out to get Ann Arbors to work, otherwise 
						 * when they go to column 9 we increment which is wrong 
						 * because bcd isn't continuous.  We should take out 
						 * the rest too, or run the thing through more than 
						 * once until it doesn't make any of these, but that 
						 * would make termlib (and hence pdp-11 ex) bigger, 
						 * and also somewhat slower.  This requires all 
						 * programs which use termlib to stty tabs so they 
						 * don't get expanded.  They should do this anyway 
						 * because some terminals use ^I for other things, 
						 * like nondestructive space. 
						 */ 
			if (which == 0 || which == CTRL(d) || /* which == '\t' || */ which == '\n') { 
				if (oncol || UP) /* Assumption: backspace works */ 
				/* 
									 * Loop needed because newline happens 
									 * to be the successor of tab. 
									 */ 
				do { 
					strcat(added, oncol ? (BC ? BC : "\b") : 
					UP); 
					which++; 
				} 
				while (which == '\n'); 
			} 
			*dp++ = which; 
			goto swap; 

		case 'r': 
			oncol = 1; 
			goto setwhich; 

		case 'i': 
			destcol++; 
			destline++; 
			which++; 
			continue; 

		case '%': 
			*dp++ = c; 
			continue; 

#ifdef CM_B 
		case 'B': 
			which = (which/10 << 4) + which%10; 
			continue; 
#endif 

#ifdef CM_D 
		case 'D': 
			which = which - 2 * (which%16); 
			continue; 
#endif 

		default: 
			goto toohard; 
		} 
	} 
	strcpy(dp, added); 
	return (result); 
} 
