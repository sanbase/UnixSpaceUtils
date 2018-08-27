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

	Last modification:Fri Mar 23 14:24:03 2001
			Module:file_cat.c
*/
/* ëóàíõÇÄçàÖ íÖäìôÖÉé èëÖÇÑé-äÄíÄãéÉÄ
   ÇÆß¢‡†‚:  0 - éK,  1 - Ì‚Æ ≠• °®°´®Æ‚•™†,  -1 - ÆË®°™† Á‚•≠®Ô */

#define PORTAR
#include "tty_codes.h"
#include <ar.h>
#include "ssh.h"
#include <fcntl.h>
#include <stdio.h>
#define MAX_NAMLEN 32

typedef struct {
	off_t size;			/* size of the object in bytes */
	long date;			/* date */
	int l_name;                      /* size of the long name in bytes */
	int gid;			/* group */
	int uid;			/* owner */
	u_short mode;			/* permissions */
	char name[MAX_NAMLEN + 1];       /* name */
} CHDR;

static CHDR chdr;                       /* converted header */
typedef struct ar_hdr HDR;
static char hb[sizeof(HDR) + 1];	/* real header */

//char *strchr();

int file_cat()
{
	register int         lgt;
	register struct dir2 *pd;
	char                 *p,*s,str[9];
	long len=0;
	struct ar_hdr header;

	for(p=s=hcat; *p; ++p) if( *p=='/' )  s = p;  /* Æ‚·‚„Ø ¢ $c $a $p */
	if( arname==NULL or !strcmp(hcat,"/") or *s!='/' ) return(1);
	if((lgt=open_archive(arname))<0) return(1);
	maxcat=0;
	while(get_arobj(lgt)>0)
	{
		maxcat++;
		len+=strlen(chdr.name)+1;
		skip_arobj(lgt);
	}
	maxcat+=3;
	ind_malloc(maxcat,len);

	lseek(lgt,(off_t)SARMAG,0); get_arobj(lgt); skip_arobj(lgt);

	for(lcat=len=0;lcat<maxcat && get_arobj(lgt)>0;skip_arobj(lgt),lcat++)
	{
		char *ch;
		pd = (struct dir2*) (&cat[lcat]);
		if((ch=strchr(chdr.name,'/'))!=NULL)
			*ch=0;
		strcpy(d_names+len,chdr.name);
		pd->d_name=d_names+len;
		len+=strlen(chdr.name)+1;

		pd->size=chdr.size;
		pd->date=chdr.date;
		pd->mode=chdr.mode;
		pd->uid=chdr.uid;
		pd->gid=chdr.gid;
		pd->d_null=0;
	}
	close(lgt);
	first_and_last(maxcat);
	return(0);
}
open_archive(archive)
char *archive;
{
	int created, fd, nr;
	char buf[SARMAG];

	created = 0;
	if ((fd = open(archive, O_RDONLY)) < 0)
		return(-1);
	if ((nr = read(fd, buf, SARMAG) != SARMAG))
		return(-1);
	if (bcmp(buf, ARMAG, SARMAG))
		return(-1);
	return(fd);
}

/* Convert ar header field to an integer. */
#define	AR_ATOI(from, to, len, base) { \
	move(from, buf, len); \
	buf[len] = '\0'; \
	to = strtol(buf, (char **)NULL, base); \
}

/*
 * get_arobj --
 *	read the archive header for this member
 */
get_arobj(fd)
int fd;
{
	struct ar_hdr *hdr;
	register int len, nr;
	register char *p;
	char buf[128];

	nr = read(fd, hb, sizeof(HDR));
	if (nr != sizeof(HDR)) {
		if (!nr)
			return(0);
		return(-1);
	}

	hdr = (struct ar_hdr *)hb;
	if (strncmp(hdr->ar_fmag, ARFMAG, sizeof(ARFMAG) - 1))
		return(-1);

	/* Convert the header into the internal format. */
#define	DECIMAL	10
#define	OCTAL	 8

	AR_ATOI(hdr->ar_date, chdr.date, sizeof(hdr->ar_date), DECIMAL);
	AR_ATOI(hdr->ar_uid, chdr.uid, sizeof(hdr->ar_uid), DECIMAL);
	AR_ATOI(hdr->ar_gid, chdr.gid, sizeof(hdr->ar_gid), DECIMAL);
	AR_ATOI(hdr->ar_mode, chdr.mode, sizeof(hdr->ar_mode), OCTAL);
	AR_ATOI(hdr->ar_size, chdr.size, sizeof(hdr->ar_size), DECIMAL);

	/* Leading spaces should never happen. */
	if (hdr->ar_name[0] == ' ')
		return(-1);

	/*
	 * Long name support.  Set the "real" size of the file, and the
	 * long name flag/size.
	 */
#ifdef AR_EFMT1
	if (!bcmp(hdr->ar_name, AR_EFMT1, sizeof(AR_EFMT1) - 1))
	{
		chdr.l_name = len = atoi(hdr->ar_name + sizeof(AR_EFMT1) - 1);
		if (len <= 0 || len > MAX_NAMLEN)
			return(-1);
		nr = read(fd, chdr.name, len);
		if (nr != len)
			return(-1);
		chdr.name[len] = 0;
		chdr.size -= len;
	} else
#endif
	  {
		chdr.l_name = 0;
		move(hdr->ar_name, chdr.name, sizeof(hdr->ar_name));

		/* Strip trailing spaces, null terminate. */
		for (p = chdr.name + sizeof(hdr->ar_name) - 1; *p == ' '; --p);
		*++p = '\0';
	}
	return(1);
}

/*
 * skip_arobj -
 *	Skip over an object -- taking care to skip the pad bytes.
 */
skip_arobj(fd)
int fd;
{
	off_t len;

	len = chdr.size + (chdr.size + chdr.l_name & 1);
	if (lseek(fd, (off_t)len, SEEK_CUR) == (off_t)-1)
		return(-1);
}
#ifdef XENIX
int file_cat2()
{
	register int         lgt;
	register struct dir2 *pd;
	char                 *p,*s;

	struct {
		unsigned char  ar_name[14];
		unsigned char  ar_date[4];
		char           ar_uid;
		char           ar_gid;
		unsigned char  ar_mode[2];
		unsigned char  ar_size[4];
	} header;

	long len=0;

	for(p=s=hcat; *p; ++p) if( *p=='/' )  s = p;  /* Æ‚·‚„Ø ¢ $c $a $p */
	if( arname==NULL or !strcmp(hcat,"/") or *s!='/' )  return(1);
	if((lgt=open(arname,O_RDONLY))<0) return(-1);
	lseek(lgt,(off_t)28l,0);
	read(lgt,&maxcat,2);
	maxcat+=3;
	ind_malloc(maxcat,maxcat*16);
	lseek(lgt,(off_t)2l,0);
	for(lcat=len=0;lcat<maxcat&&read(lgt,&header,sizeof header)>0;lcat++)
	{
		pd = (struct dir2*) (&cat[lcat]);
		strncpy(d_names+len,header.ar_name,15);
		pd->d_name=d_names+len;
		len+=16;
		memcpy(&pd->date,&header.ar_date,4);
		pd->uid =header.ar_uid;
		pd->gid =header.ar_gid;
		memcpy(&pd->mode,&header.ar_mode,2);
		memcpy(&pd->size,&header.ar_size,4);
		pd->d_null=0;
		lseek(lgt,(off_t)((pd->size+1)/2)*2,1);
	}
	close(lgt);
	first_and_last(maxcat);
	return(0);
}
#endif
