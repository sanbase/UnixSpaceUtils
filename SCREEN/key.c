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
			Module:key.c
*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
long time();

key(line)
unsigned char *line;
{
	struct stat buf;
	char str[80];
	strcpy(str,"/usr/local/bin/");
	strcat(str,line);
	if(stat(str,&buf))
	{
		dps("Файл должен иметь имя:");
		dps(str);
		dpi();
		return(-1);
	}
#ifdef LTD
	if(time(0)>720000000l) goto MES;
	if(time(0)>725000000l) return(-1);
#endif
	if(buf.st_ctime<buf.st_mtime || buf.st_atime<buf.st_ctime)
	{
		dps("Что-то не так...");
		dpi();
		return(-1);
	}
	if(buf.st_ctime-buf.st_mtime>2600000l)
	{
MES:
		dps("Это незарегистрированная копия\n\rЗвоните по телефону (044) 547-25-64 Киев - Александр Ляшенко");
		dpi();
		if(buf.st_atime-buf.st_ctime>2600000l) return(-1);
		return(0);
	}
	return (0);
}
