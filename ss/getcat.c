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

	Last modification:Fri Mar 23 14:24:17 2001
			Module:getcat.c
*/
/* СЧИТыВАНИЕ КАТАЛОГА */

#include "ssh.h"
#include <sys/stat.h>
#include <stdio.h>
#include <sys/types.h>
/* #include <sys/dir.h> */
#include <dirent.h>
#include <stdlib.h>
#include <signal.h>
/* char *malloc(); */
extern char full_flg;
extern int summ;
extern char new_inf;
int sort_arg,int_flg;
extern int summ;
/* Читаем каталог библиотеки и составляем таблицу индексов файлов */
getcat()
{
	register int i;
	long len;
	void  *old_sig;
/* #ifdef BSD
	struct direct *dp;
#else
*/
	struct dirent *dp;
/* #endif */
	DIR *fd;
	struct stat  st;
	struct {unsigned long d_ino; unsigned int d_reclen,d_namelen;} tcat;

	errno = 0;
	if( stat(".", &st ) == -1 )  
		return(-1);
	if( new_inf!=2 && st.st_mtime==last_mod )
	{   /* не менялся     */
		new_inf=0;
		return(0);
	}
	new_inf=1;
	last_mod = st.st_mtime;
	nmarked = summ = 0;
	lind=0;
	/* Читаем каталог, составляем таблицу индексов файлов, упорядоченных по
	   алфавиту. Обнуляем маркеры и ставим нули на конце имен
	   (в следующей статье!). */
	if( file_cat() > 0 )    /* это не библиотека */
#ifdef XENIX
	if( file_cat2() > 0 )    /* это не библиотека */
#endif
	{       /* сначала определяем число файлов в каталоге */
		if((fd=opendir("."))==NULL)
			return(-1);
		old_sig=signal(SIGALRM,SIG_IGN);
AGAIN:
		rewinddir(fd);
		len=0;
		maxcat=0;
		for(dp=readdir(fd);dp!=NULL;dp=readdir(fd))
		{
			struct stat buf;
			if(lstat(dp->d_name,&buf))
				continue;
			len+=strlen(dp->d_name)+1;
			maxcat++;
		}
		if( !maxcat ) {
err:        
			(void)closedir(fd);
			ind_err(".",0);
			maxcat = 0;
			lind = 0;
		        signal(SIGALRM,old_sig);
			return(0);
		}
		/* потом читаем их имена */
		ind_malloc(maxcat,len);
		rewinddir(fd);
		for(lcat=len=0,dp=readdir(fd);dp!=NULL;dp=readdir(fd))
		{
			int size,st;
			struct stat buf;
			register struct dir2  *pd;

			pd = (struct dir2*) (&cat[lcat]);
			if(lcat>maxcat)
				goto AGAIN;
			strcpy(d_names+len,dp->d_name);
			pd->d_name=d_names+len;
			len+=strlen(dp->d_name)+1;
			pd->d_null=0;
			if(!int_flg && maxcat<2000)
			{
			        if(st=lstat(dp->d_name,&buf))
				        continue;
				pd->size=buf.st_size;
				pd->mode=buf.st_mode;
				if((pd->mode&0170000)==S_IFDIR && if_base("",pd->d_name))
					pd->mode=0170000;
				pd->date=buf.st_mtime;
				pd->uid =buf.st_uid;
				pd->gid =buf.st_gid;
				if((buf.st_mode&0170000)==S_IFDIR)
					pd->size=lendir(pd->d_name);
			}
			else
				pd->mode=pd->uid=pd->date=pd->size=0;
			lcat++;
		}
		(void)closedir( fd );
		first_and_last(maxcat);
	}
	/* упорядочим индексы */
	for( i=lcat; --i>=0; )
	{
		register struct dir2  *pd;
		register int         put_to;

		pd = (struct dir2*) (&cat[i]);
		pd->d_mark  = 0;
		put_to = search_ind( pd ,sort_arg);
		if( put_to < 0 )  insert_ind( i, ~put_to );
	}

	/* Восстанавливаем запомненную пометку */
	summ=0;
	if( !strcmp( hcat, altcat ) ) {
		register int i, m;
		while( --nalt >= 0 ) {
			register struct dir2 *pd;
			for( i=maxcat; --i>=0; ) {
				pd = &cat[i];
				if( !strcmp( pd->d_name, altnam[nalt].d_name ) ) {
					m = altnam[nalt].d_mark;
					if( m<'1' or m>'9' ) {
						pd->d_mark = m;
					} 
					else {
						if( pd->d_mark == 0 )  pd->d_mark = '1';
						else pd->d_mark++;
					}
					marked[nmarked++] = i;
					summa(pd->d_name,1,pd->d_mark);
				}
			}
		}
		nalt = altcat[0] = 0;
	}
	signal(SIGALRM,old_sig);
	return(0);
}
int cat_size;
ind_malloc(num,len)
int num;
long len;
{
	cat_size=(num+3)*(sizeof (struct dir2))+4*num+len+num;
	cat = (struct dir2 *)realloc(cat,(num+3)*(sizeof (struct dir2)));
	indcat=(int *)realloc(indcat,num*sizeof (int));
	indico=(int *)realloc(indico,num*sizeof (int));
	d_names=(char *)realloc(d_names,len);
	sqmem();
}
lendir(name)
char *name;
{
	DIR *fd;
	struct dirent *dp;
	int num=0,flg=0;

	if((fd=opendir(name))==NULL) return(0);
	for(dp=readdir(fd);dp!=NULL;dp=readdir(fd))
	{
		if(flg<2 && (!strcmp(dp->d_name,".") || !strcmp(dp->d_name,"..")))
		{
			flg++;
			continue;
		}
		num++;
	}
	closedir(fd);
	return(num);
}

first_and_last(maxcat)
int maxcat;
{
	cat[maxcat].d_name=")----->";
	cat[maxcat+1].d_name="<-----<";
	cat[maxcat].d_mark=cat[maxcat+1].d_mark=0;
	cat[maxcat].mode=cat[maxcat+1].mode=cat[maxcat].size=cat[maxcat+1].size=0;
}

#ifndef CX
sqmem()
{
}
#endif
