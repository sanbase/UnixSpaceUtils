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

	Last modification:Fri Mar 23 14:24:23 2001
			Module:getname.c
*/
#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>
/* #include <sys/dir.h> */
#include <stdio.h>
#include <ctype.h>
#include <pwd.h>
#include <grp.h>
#include <utmp.h>
struct  utmp    utmp;
char *Conv();
#define NUID    64      /* макс. количество пользователей */
#define NGID    32      /* макс. количество групп */
#define NMAX    (sizeof (utmp.ut_name))
struct nametable {
	char    nt_name[NMAX+1];
	unsigned short  nt_id;
};
struct nametable        unames[NUID];
struct nametable        gnames[NGID];

struct nametable *findslot (id, tbl, len)
unsigned int  id;
struct nametable        *tbl;
int             len;
{
	register struct nametable       *nt, *nt_start;

	/*
	 * find the id or an empty slot.
	 * return NULL if neither found.
	 */

	nt = nt_start = tbl + (id % (len - 20));
	while (nt->nt_name[0] && nt->nt_id != id) {
		if ((nt += 5) >= &tbl[len])
		    nt -= len;
		if (nt == nt_start)
		    return((struct nametable *)NULL);
	}
	return(nt);
}
char *getname (uid)
uid_t uid;
{
	struct passwd  *pw;

	pw=getpwuid(uid);
	if (pw == NULL)
		return(Conv((long)uid,8,10,0));
	return(pw->pw_name);

}

char *getgroup (gid)
unsigned int  gid;
{
	register struct group   *gr;
	static int      init = 0;
	register struct nametable       *n;

	/*
	 * find gid in hashed table; add it if not found.
	 * return pointer to name.
	 */

	if ((n = findslot(gid, gnames, NGID)) == NULL)
	    return((char *)NULL);

	if (n->nt_name[0])      /* occupied? */
	    return(n->nt_name);

	switch (init) {
	case 0:
		setgrent();
		init = 1;
		/* intentional fall-thru */
	case 1:
		while (gr = getgrent())
		{
			n = findslot(gr->gr_gid, gnames, NGID);
			if (n == NULL) {
				endgrent();
				init = 2;
				return((char *)NULL);
			}
			if (n->nt_name[0])
			    continue;       /* duplicate, not gid */
			strncpy(n->nt_name, gr->gr_name, NMAX);
			n->nt_id = gr->gr_gid;
			if (gr->gr_gid == gid)
			    return (n->nt_name);
		}
		endgrent();
		init = 2;
		/* intentional fall-thru */
	case 2:
		return(Conv((long)gid,8,10,0));
	}
}
