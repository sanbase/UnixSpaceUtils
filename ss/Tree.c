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

	Last modification:Fri Mar 23 14:22:58 2001
			Module:Tree.c
*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <dirent.h>
/* #include <sys/file.h>  */
#include <signal.h>
#include <tty_codes.h>

#define BSIZE 1024
#define BUFPC 512

char    Home[128];
char    *Fname;
unsigned long seek;
struct stat Statb;
int fd,fdt;
main()
{
	char name[128];
	getcwd(Home,128);
	chdir(Home);
	if((fd=creat("/tmp/.tree",0644))<=0) exit(-1);
	if((fdt=creat("/tmp/.tree_s",0644))<=0) exit(-2);
	strcpy(name,"/");
	seek=0;
	f_name(name,Fname=name);
END:
	close(fd);
	close(fdt);
	exit(0);
}
f_name(name, fname)
char *name, *fname;
{
	int     dsize, entries, dirsize;
	off_t   offset;
	DIR *dir=NULL;
	struct dirent *dentry;
	struct dirent  *dp;
	char *c1, *c2;
	int i;
	int rv;
	char *endofname;

	if(lstat(fname, &Statb)<0)
		return(0);

	if((Statb.st_mode&0170000)!=S_IFDIR)
		return(1);
	write(fd,name,strlen(name));
	write(fd,"\n",1);
	write(fdt,&seek,sizeof seek);
	seek+=strlen(name)+1;


	rv=0;

	for(c1 = name; *c1; ++c1);
	if(*(c1-1) == '/')
		--c1;
	endofname = c1;
	dirsize = Statb.st_size;

	if(chdir(fname) == -1)
		return(0);
	if((dir=opendir("."))==NULL)
	{
		rv=0;
		goto ret;
	}
	for(dp=readdir(dir); dp!=NULL; dp=readdir(dir))
	{ /* each directory entry */
		    if((dp->d_name[0]=='.' && dp->d_name[1]=='\0')
		    || (dp->d_name[0]=='.' && dp->d_name[1]=='.'
		    && dp->d_name[2]=='\0'))
			continue;
		c1 = endofname;
		*c1++ = '/';
		c2 = dp->d_name;
		for(i=0; i<14; ++i)
			if(*c2)
				*c1++ = *c2++;
			else
				break;
		*c1 = '\0';
		if(c1 == endofname)
		{ /* ?? */
			rv = 0;
			goto ret;
		}
		Fname = endofname+1;
		if((i=f_name(name, Fname))==(-1)) return(-1);
		else if(!i)
		{
			*endofname = '\0';
			chdir(Home);
			if(chdir(name) == -1)
				exit(1);
		}
	}
	rv = 1;
ret:
	if(dir!=NULL)
		closedir(dir);
	if(chdir("..") == -1)
	{
		*endofname = '\0';
		rv = 1;
	}
	return(rv);
}
