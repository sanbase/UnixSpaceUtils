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

	Last modification:Fri Mar 23 14:26:42 2001
			Module:start.c
*/
/* ‡€“‘ €„› 'name', ‡€“™…ƒ ‘‚‹ 'symb' */

#include <signal.h>
#include <stdio.h>
#include "tty_codes.h"
#include "ssh.h"
#include "run.h"

#define      max_ac 500

extern char *catalog;
extern int   invisible;
extern char *fname;
extern char        *name_f, *name_g;
static char *amp_last;
extern int MDF;
char new_inf;

start(    name, symb, shline )
char *name, symb;
boolean           shline;
{
	int         ac,i;
	char       *av[max_ac];
	boolean     use_sh, parallel;
	char       *ind_name;
	char        nbuf[D_NAME];
	static char sh[]   = "/bin/sh";
	static char exec[] = "exec ";

	int  f0=0, f1=0, f2=0;
	register char *p;

	if( amp_last )
		*amp_last = '&';
	amp_last = 0;
	for( p=name; *p and *p!='\n'; ++p );
	while( *--p == ' '  and p>=name );
	parallel = NO;
	if( *p   == '&' )
	{
		parallel=YES;
		amp_last=p;
		*p=' ';
	}
	if( symb == '&' )
		parallel=YES;

	use_sh = NO;
	ini_shbuf();
	cpy_shbuf(exec);
	if( shline )
	{
		sh_line( name );
		invisible = 1;
	}
	else cm_line( name );
	name = get_shbuf();
	ind_name = name + (sizeof exec-1);
	if( amp_last )  *amp_last = '&';
	if( looksym( ind_name, "()<>{}[]`*?'\"$#" ) )  use_sh=YES;
	if( looksym( ind_name, ";|&"      ) )
	{
		use_sh=YES;
		name=ind_name;
	}
	if( not use_sh )  name = ind_name;
	strncpy( nbuf, shline ? ind_name : fname, D_NAME );

	if( not shline and symb>0 )
		dpo( (unsigned)symb>=' ' ? symb : '+' );
/*        dpp(xsize,ysize-1); */

		dpp(0,0); dpo(es);

/*
#ifdef XW
	for(i=xsize-2;i && get_clr(i,ysize-1)==0x53a;i--)
		put_clr(i,ysize-1,0x64e);
	dpp(0,ysize-1);

#else
	setcolor(03);
	dpp(0,ysize-2); dpo(es);
#endif
*/
	scrbufout();
	if( invisible==0 or invisible==3 )
	{          /* ―®ª §λΆ ¥¬ § ―γαª */
		/*                dpp( 0, ysize - ((strlen(ind_name)+xsize+1)/xsize) -1); */
		if(!shline)
		{
			dpo('>');
			dps(ind_name);
			/* dpo(el); */
			if( parallel )  dpo('&');
			setcolor(02);
		}
	}
	if( not use_sh )
	{                  /* δ®ΰ¬¨ΰ®Ά ­¨¥ argv */
		register char *p;
		ac = 0;
		for( p=name; ; ) {
			while( *p == ' ' )  ++p;
			if( *p == 0 )  break;
			if( ac < max_ac-1 )  av[ac++] = p;
			if( (p=no_esc(p)) == 0 )  break;
			++p;
		}
		av[ac] = 0;
	}
	if( symb < 0 )
	{    /* § ―γαª α 'βΰγ΅®©' */
		if( fork() == 0 )
		{
			f0 = f2 = open("/dev/null",2);
			f1 = -symb;
			goto ex;
		}
	}
	else if( not parallel )
	{  /* § ―γαª α ®¦¨¤ ­¨¥¬ */
		if(ac==2 && !strcmp(av[0],"cd"))
		{
			catalog=av[1];
			return(-1);
		}
		if( (use_sh ? strt( sh, "-c", name, 0 ): strtv( av )) == 0 && invisible<2 );
//                use_sh ? strt( sh, "-c", name+sizeof(exec)-1, 0 ): strtv( av );
	}
	else
	{  /* § ―γαª Ά ― ΰ ««¥«μ */
		register int i;

		for( i=MDF; i<=16; ++i )  close(i); /* § ªΰλΆ ¥¬ Άα¥ ¤¥αªΰ¨―β®ΰλ */
		i=0;
femp:
		dpo(0);
		f1 = open( "/dev/null", 2 );
		if(fork() == 0 ) /* § ―γαª Ά ― ΰ ««¥«μ */
		{
			f0 = open("/dev/null",0);
			f2 = f1;
ex:
			signal(SIGINT, SIG_IGN);
			signal(SIGQUIT,SIG_IGN);
			dup2( f0, 0 );
			dup2( f1, 1 );
			dup2( f2, 2 );
			close(f0);
			close(f1);
			close(f2);
			nice(addnice);
			if( use_sh )
				_execlp( sh, "sh", "-c", name, 0 );
			else
			        _execvp( av[0], av );
			if( symb < 0 )  write(f1,"",1);
			exit(0);
		}
		if(f1>0) close(f1);
	}
	nmarked=0;
		printf("\n\n\n\n"); fflush(stdout);
	flush_dpi();
	/*        dpi_nit(); */
	new_inf=1;
	return(1);
}

/* “„€‹…’ ‡ ‘’ 'p' ESCAPE-‘‚‹›.
   ‚¥ΰ­¥β  ¤ΰ¥α 1-®£® ―ΰ®΅¥«  ¨«¨ 0 ¥α«¨ ª®­¥ζ αβΰ®ª¨ */

char *no_esc(      p )
register char *p;
{
	register char *s, *q;

	s = p;
	while( *p and *p!=' ' ) {
		if( *p != '\\' ) {
			*s++ = *p++;
		}
		else {
			static char esc[]             = "tnrf0";
			if( q=index(esc,*++p) )  *s++ = "\t\n\r\f\0"[q-esc];
			else *s++ = *p;
			++p;
		}
	}
	if( *p == 0 )  p = 0;
	*s = 0;
	return( p );
}


/* ‘ „ƒ ‡ ‘‚‹‚ 'symset' ‚ ‘’… 'str'. ‚……’ 1 …‘‹ …‘’*/

int looksym(       str, symset )
register char *str,*symset;
{
	while( *symset )  if( index(str,*symset++) )  return(1);
	return(0);
}


/* „€‚‹…  ‡€’€‚… 'p' €„‰ ‘’ …—…›… ”€‰‹› */

sh_line(           p )
register char *p;
{
	register int   i;
	struct   dir2 *pd;

	while( *p && *p!='|' && *p!='>' )  put_sh( *p++ );  /* ¤® |, > */

	for( i=0; i<nmarked; ++i )  {     /* ―¥ΰ¥αλ«ª  ¨¬¥­ Άε®¤­λε δ ©«®Ά */
		pd = &cat[marked[i]];
		if( pd->d_mark != '>' )  add_name(pd,0);
	}
	for( i=0; i<nalt; ++i ) {
		pd = &altnam[i];
		if( pd->d_mark != '>' )  add_name(pd,1);
	}

	cpy_shbuf( p );                         /* ®αβ β®ª ª®¬ ­¤λ */

	for( i=0; i<nmarked; ++i )  {           /* Άλε®¤­®© δ ©« */
		pd = &cat[marked[i]];
		if( pd->d_mark == '>' )  {
			add_name(pd,0);
			break;
		}
	}
	for( i=0; i<nalt; ++i ) {
		pd = &altnam[i];
		if( pd->d_mark == '>' )  {
			add_name(pd,1);
			break;
		}
	}
}


/* ‡€“‘ ‘…–€‹‰ €„› ‘ €€…’€ 'name1', 'name2' */
int rspec( symb, name, namef, nameg )
char   symb,*name,*namef,*nameg;
{
	register char *p;

	name_f = namef;
	name_g = nameg;
	p = lookcom( symb | 0x0200, name );
	if( p )
	{
		start( p, 0, NO );
		if(invisible<2) ok();
		dpi_nit();
		new_inf=1;
	}
	return( (int)p );
}

dpi_nit()
{
	dpp(0,0); 
	setcolor(03); 
	dpo(es);
	scrbufout();
}

ok()
{
	dpp(0,ysize-1);
	setcolor(02);
	dpo(el);
	dps( messages[53] );
	dpi();
/*        dpi_nit(); */
}
