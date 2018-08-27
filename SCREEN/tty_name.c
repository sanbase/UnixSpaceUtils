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

	Last modification:Fri Mar 23 09:33:46 2001
			Module:tty_name.c
*/
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <dirent.h>
#include <sys/stat.h>
#ifdef SPARC
 #include "termios.h"
#else
 #include <termios.h>
#endif
#include <string.h>
#include <strings.h>
#include <unistd.h>
char *UNKNOWN="unknown";

#ifndef TCGETA
#define TCGETA TIOCGETA
#endif

char *tty_name()
{
	struct stat sb;
	struct termios oldtio;
	register struct dirent *dirp;
	register DIR *dp;
	struct stat dsb;
	struct
	{
		mode_t type;
		dev_t dev;
	}
	bkey;
	static char buf[128];

	/* Must be a terminal. */
	if (ioctl (0, TCGETA, (char *) &oldtio) < 0)
		return (UNKNOWN);
	/* Must be a character device. */
	if (fstat(0, &sb) || !S_ISCHR(sb.st_mode))
		return (UNKNOWN);

	if ((dp = opendir("/dev")) == NULL)
		return (UNKNOWN);

	strcpy(buf,"/dev/");
	while (dirp = readdir(dp))
	{
#ifdef BSD
		if (dirp->d_fileno != sb.st_ino)
			continue;
		bcopy(dirp->d_name, buf+5, dirp->d_namlen + 1);
#else
#ifdef SPARC
		bcopy(dirp->d_name, buf+5, dirp->d_reclen - sizeof (struct dirent)+ 1);
#else
		strcpy(buf+5,dirp->d_name);
		(void)closedir(dp);
		return (UNKNOWN);
#endif
#endif
		if (stat(buf, &dsb) || sb.st_dev != dsb.st_dev || sb.st_ino != dsb.st_ino)
			continue;
		(void)closedir(dp);
		return (buf+5);
	}
	(void)closedir(dp);
	return (UNKNOWN);
}
