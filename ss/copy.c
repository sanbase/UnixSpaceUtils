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

	Last modification:Fri Mar 23 14:23:28 2001
			Module:copy.c
*/
/* Šˆ‚€ˆ… Œ…—…›• ”€‰‹‚ ‚ ’…Š“™ˆ‰ Š€’€‹ƒ ‘‹… ”€‰‹€ 'name' */

#include "tty_codes.h"
#include "ssh.h"
#include <sys/stat.h>

extern char  ftype;
static char *scopy(), *afnm;

char *copy( name, mv_fl )
char   *name;
int           mv_fl;
{
	register int   i;
	register char *ret;
	afnm = name;
	for( i=0; i<nalt;    ++i )  ret=scopy(&altnam[i],1,mv_fl);
	for( i=0; i<nmarked; ++i )  ret=scopy(&cat[marked[i]],0,mv_fl);
	return( ret );
}


/* Šˆ‚€ˆ… ”€‰‹€ 'pd' ‚ ’…Š“™ˆ‰ Š€’€‹ƒ */
char *inp_name();

extern int no_del;
static char *scopy( pd, altflag, mv_fl )
struct dir2    *pd;
int                 altflag, mv_fl;
{
	char *name, *new;
	register char *s;
	char fullname[lname+1];
	struct stat st;
	if( pd->d_mark=='>' or pd->d_mark=='<' )  return(0);
	new = name = pd->d_name;
	s = 0;
	stat(altcat,&st);
	if((st.st_mode&0170000)!=S_IFDIR)
	{
		strcpy(fullname,name);
		goto A;
	}
	if( !altflag ) {                 /* ä®à¬¨àã¥¬ ¯®«­®¥ ¨¬ï ä ©«  */
		strcpy(fullname,name);               /* ¡¥§ ¨¬¥­¨ ª â «®£  */
	}
	else {
		strcpy(fullname,altcat);   /* á ¨¬¥­¥¬ ­ ç «ì.ª â «®£  */
		strcat(fullname,"/");
		strcat(fullname,name);
	}
A:
	stat(hcat,&st);
	if((st.st_mode&0170000)==S_IFDIR)
		if( exist(new) )  new = inp_name( new, "", 'c' );
	if( new == NULL )  return( afnm );
	if( rspec( 'e'+mv_fl, afnm, fullname, new ) == 0 )
	{
		register int   lgt, finp, fout;
		char           buf[1024];

		finp = fout = -1;  
		errno = 0;

		if( !mv_fl ) {
			if( stat(fullname,&st) == -1)  goto ei;
			if(no_del && !access(new,0))
			{
		                if( s )  *s = 0;
		                return( new );
			}
			if( (finp=open(fullname,0))<= 0 ) goto ei;
			if( (fout=creat(new,0600)) <= 0 ) {
				name=new;
				goto ei;
			}
			dpp(0,24); 
			dpo(es); 
			dps(name); 
			scrbufout();
			while( (lgt=read(finp,buf,sizeof buf)) > 0 )
			{
				if( write(fout,buf,lgt) != lgt )
				{
					name=new;
					goto ei;
				}
			}
			chmod(new,st.st_mode);
		} 
		else {
			link( fullname, new );
			if( ind_err(new,1)  )  goto er;
			unlink( fullname );
			if( ind_err(fullname,1) )  goto er;
#ifndef BSD
			{
#               include <sys/types.h>
//                                long   time();
				time_t timep[2];
				timep[0] = timep[1] = time(0);
				utime( new, timep );
			}
#endif
		}

rt:     
		close(finp);  
		close(fout);
		if( s )  *s = 0;
		return( new );

ei:     
		ind_err(fullname,1);
		unlink(new);
er:     
		new[0] = 0;
		goto rt;
	}
	return(new);
}
