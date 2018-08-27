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

	Last modification:Fri Mar 23 14:24:36 2001
			Module:gettune.c
*/
/* СЧИТыВАНИЕ ФАЙЛА С НАСТРОЙКОЙ */

#include "ssh.h"
#include "tty_codes.h"
#include <sys/stat.h>
#include <ar.h>
/*
#include <sys/a.out.h>
*/
#include <stdio.h>
#include <fcntl.h>

static char *DIRTUNE="/usr/local/lib";
char dest_name[512];
char *getenv();


#define ARCMAGIC        0xff65

#ifdef OARMAGIC1
#define A_MAGIC1        OARMAG1
#else
#define A_MAGIC1        0407
#endif

#ifdef OARMAG2
#define A_MAGIC2        OARMAG2
#else
#define A_MAGIC2        0410
#endif

#define A_MAGIC3        0411
#define A_MAGIC4        0405

boolean ch_name();
char   *cvmacro();

static char     *beg;
static int profile,size,bufsize;
static unsigned sdev, sino, udev, uino;

int   thead;            /* флаг: не показывать время */
int   uhead;            /* флаг: не показывать имя пользователя */
int   mhead;            /* флаг: не показывать режим */
int   phead;            /* флаг: не показывать процессы */
char *nhead;            /* новое имя заголовка (если не 0) */
int   invisible;        /* запуск команды без индикации */

extern int cat_size;

gettune() 
{
	struct stat    st;
	char buf[512];  /* буфер для имени домашнего каталога */

	if( beg==0 )
	{
		int i;
		char *end,*ch;

		strcpy( buf, getenv("HOME") );  
#ifdef XW
		strcat( buf, "/.Xs" );
#else
		strcat( buf, "/.ss" );
#endif
		if(!stat(buf,&st))
		{
		        profile=st.st_size;
		        end=beg=(char *)malloc(bufsize=profile);
		        if(i != -1 )
		        {
			        i=open(buf,O_RDONLY);
			        end+=read(i,beg,st.st_size);
			        close(i);
		        }
		}
		else
		{
			profile=0;
			end=beg=0;
		}
		if(!(ch=getenv("DESTDIR")))
			strcpy( buf, DIRTUNE );
		else
		{
		        strcpy(buf,ch);
			strcat(buf,"/lib");
		}
		strcpy(dest_name,buf);
#ifdef XW
		strcat( buf, "/.Xs" );
#else
		strcat( buf, "/.ss" );
#endif
		if( stat(buf,&st) != -1 )
		{
			int seek=end-beg;

		        beg=(char *)realloc(beg,bufsize=(profile+=st.st_size));
			i=open(buf,O_RDONLY);
			read(i,beg+seek,st.st_size);
			close(i);
		}
		size=0;
	}
	nhead = NULL;  
	mhead = uhead = thead = NO;
	sgettune();
}


/* ДОБАВЛЯЕМ В НАЧАЛО БУФЕРА ФАЙЛ 'name' */

sgettune()
{
	register int fd, add, lgt;
	struct  stat st;
	char dir[512];

	move( beg+size, beg, profile);  /* удалили старый файл */
	size=0;
	memset(beg+profile,0,bufsize-profile);
	getcwd(dir,255);
	if(!strcmp(dest_name,dir))
		return;
	if(!strcmp(getenv("HOME"),dir))
		return;
#ifdef XW
	if( (fd=open(".Xs",O_RDONLY)) <= 0)
#else
	if( (fd=open(".ss",O_RDONLY)) <= 0)
#endif
		return;
	if(fd>0 && fstat(fd,&st) == -1 )
	{
		close(fd);
		return;
	} 
	else
		lgt = st.st_size;
	add = lgt - (bufsize-profile);
	if(add>0)
		beg=(char *)realloc(beg,bufsize+=add+1);
	else    add=0;
	move( beg, beg+lgt, profile );  /* считали новое */
	memset(beg+lgt+profile,0,bufsize-profile-lgt);
	if(fd>0)
		read( fd, beg, lgt );
	size=lgt;
	if( *beg == '/' )
		gethead(beg);
	if(fd>0)
		close(fd);
}


/* ИНИЦИАЛИЗАЦИЯ ЗАГОЛОВКА */

gethead( p )  register char *p; 
{
	for( ; ; ++p ) {
		switch( *p ) {
		case 0:
		case '\n': 
			return;
		case 't':  
			thead++;    
			break;
		case 'u':  
			uhead++;    
			break;
		case 'm':  
			mhead++;    
			break;
		case 'p':  
			phead++;    
			break;
		case 'n':  
			nhead=p+1;  
			return;
		}
	}
}


/* АНАЛИЗ ИМЕНИ ФАЙЛА */

static char   bname[D_NAME+1];
char *fname = bname;             /* имя без суффикса */
char *fsuff;                     /* суффикс файла */
char *ffull;                     /* полное имя файла */
char  ftype;                     /* тип файла (n c s o a d x) */
char  ffirs;                     /* первый символ файла */

static filenm(char *name)
{
	register char *p;
	int l;
	ffull  = name;
	*fname = 0;
	fsuff  = "";
	ftype  = 0;

	if( name == NULL )  return;

	if( p=index(name,'/') )  ++p;         /* ВЫДЕЛЯЕМ В ИМЕНИ СУФФИКС */
	else                      p = name;
	strncpy(fname,p,D_NAME);

	if((l=strlen(fname)-1)>1)
		for(p=fname+l;p!=fname;p--)
		{
			if(*p=='.' && p[1]) {
				*p=0;
				fsuff=p+1;
				break;
			}
		}
	/*    if( (p=index(fname,'.')) && p[1] && p!=fname) {*p = 0; fsuff = p+1;} */
}


/* АНАЛИЗ ТИПА ФАЙЛА */

/*static*/ filetp()
{
	register int   fd, lread;
	struct   stat  st;
	union
	{
		unsigned  int  word;
		char       byte[16];
	}              
	first;

	ffirs  = 0;

	if(ffull==NULL || (stat(ffull,&st)==-1 && lstat(ffull,&st)==-1))
		ftype = 'n';        /* неизвестно что */
	else if((st.st_mode & S_IFMT)==S_IFDIR )
	{
		int i;
		ftype = 'c';        /* каталог */
		if((i=if_base("",ffull))==3)
			ftype='x';  /* CX база */
		else if(i==4)
			ftype='X';  /* CX4 база */
		else if(i==5)
			ftype='T';  /* CX5 база */
#ifdef S_IFLNK
	} 
	else if( (st.st_mode & S_IFMT)==S_IFLNK )
	{
		ftype = 'l';        /* символическая ссылка */
#endif

#ifdef S_IFQUOT
	} 
	else if( (st.st_mode & S_IFMT)==S_IFQUOT )
	{
		ftype = 'q';        /* quota */
#endif

	} 
	else if((st.st_mode & S_IFMT)==S_IFIFO)
		ftype='f';
#ifdef S_IFNAM
	else if((st.st_mode & S_IFMT)==S_IFNAM)
		ftype='p';
#endif
#ifdef S_IFSOCK
	else if((st.st_mode & S_IFMT)==S_IFSOCK)
		ftype='t';
#endif
	else {
		fd = open(ffull,0);
		first.word = 0;
		lread = read( fd, &first, sizeof first );
		close(fd);

		if( first.word == ARCMAGIC)
			ftype='A';      /* library in XENIX format */
		else
		if(!memcmp(first.byte,"!<arch>\n",8))
			ftype = 'a';         /* библиотека */
		else if( first.word==A_MAGIC1  ||  first.word==A_MAGIC2
		    || first.word==A_MAGIC3  ||  first.word==A_MAGIC4)
			ftype = 'o';      /* объектный */
		else
		{
			ftype = 's';      /* текстовый? */
			for( fd=lread; --fd>=0; )
			{
				register int c;
				c = first.byte[fd]&0377; /* данные */
				if( !is_print(c) && c!='\t' && c!='\n' && c!='\r')
				{
					ftype = 'd';
					break;
				}
			}
		}

		ffirs = first.byte[0];  /* ПЕРВыЙ СИМВОЛ ФАЙЛА */
	}
}


/* ПОИСК РЕАКЦИИ НА СИМВОЛ 'symb', НАЖАТЫЙ ОКОЛО ФАЙЛА 'name' */

char *lookcom( symb, name )
int        symb;
char            *name;
{
	register char    *p;
	register int      level;
	register boolean  ifhead;

	filenm(name);
	invisible = 0;
	level = 0;
	ifhead = NO;
lk: 
	if(!beg)
	{
		BOX(xsize/2-17,ysize/2,34,3,' ',016,016);
		dpp(xsize/2-17+1,ysize/2+1);
		setcolor(0x4e|0x500);
		dps("Configuration file doesn't exist");
		return(0);
	}
	for( p=beg;p-beg<bufsize && *p; )
        {
		while( *p == ' ' )  ++p;    /* фильтр пробелов */
		switch( *p )
		{
		case '*':
		case '\n':
			goto nx;                    /* комментарий */
		case '(':                               /* IF */
			if( level )
			{
				++level;  
				goto nx;
			}
			ifhead = YES;
			filenm(cvmacro(p[1]));
			break;
		case '<':
			if( level )
			{
				++level;  
				goto nx;
			}
			if( symb != cvsymb(p+1) )  level = 1;
			goto nx;
		case '+':                               /* ELSE */
			if(      level==1 )  level = 0;
			else if( level==0 )  level = 1;
			goto nx;
		case ')':                               /* ENDIF */
			if( level )  --level;
			goto nx;
		default:                                /* нажатый символ */
			if( level )  goto nx;
			if( symb != cvsymb(p) )  goto nx;
			break;
		}
		if( level )  goto nx;               /* пропуск частей IF'а */
		while( *p!=' ' ) ++p;
		while( *p==' ' ) ++p;

		if( *p == '=' )
		{                   /* переопределение символа */
			symb = cvsymb( p+2 );
			goto lk;
		}

		if(      *p=='_' )  ++p;            /* помечены ли файлы */
		else if( *p!=' ' ) {
			register int n;
			n = nmarked + nalt;
			if(      *p == 'n' )
			{
				if( n != 0 )        goto nx;
			}
			else if( *p == 'y' )
			{
				if( n == 0 )        goto nx;
			}
			else
			{
				if( n != *p -'0' )  goto nx;
			}
			++p;
		}

		if(*p=='_' )  ++p;            /* тип файла */
		else if( *p!=' ' )
		{
			if( ftype==0 )  filetp();
			if(*p=='c' && (ftype=='x' || ftype=='X' || ftype =='T'))
				ftype='c';      /* база тоже каталог */
			if(*p++ != ftype )
				goto nx;
		}

		if(*p=='_' )  ++p;            /* доступ файла */
/*                else if(*p!=' ' and access(ffull,*p++ -'0')==-1 and !name)*/
		else if(*p!=' ' and access(ffull,*p++ -'0'))
					       goto nx;
		if(*p=='_' )  ++p;            /* первый символ файла */
		else if( *p!=' ' )
		{
			if(ftype==0)  filetp();
			if(*p=='c' && (ftype=='x' || ftype=='X' || ftype =='T'))
				ftype='c';
			if( *p++ != ffirs )  goto nx;
		}

		if(      *p=='_' )  ++p;            /* имя файла */
		else if( *p!=' ' and not ch_name(p,ffull) )  goto nx;

		while( *p!=' ' ) ++p;               /* переход к полю команды */
		while( *p==' ' ) ++p;

		if( not ifhead )
		{                        /* ОК - найдено */
			static   char  im[] = ":,\001'";  /* вид показа */
			register char *s;
			invisible = 2;
			if( s=index(im,*p) )
			{
				++p;  
				invisible = s - im;
			}
			return( p );
		}
		goto fnxl;                            /* выполнено условие */

nx:     
		if( ifhead )
		{                        /* не выполнено условие */
			level = 1;
fnxl:       
			filenm(name);
			ifhead = NO;
		}
		while( *p++ != '\n' )
		{
		};             /* переход к след.строке */
	}
	return(0);
}


/* СРАВНЕНИЕ ШАБЛОНА 'sample' С ИМЕНЕМ ФАЙЛА 'name' С УЧЕТОМ '*' И '?' */

/*static*/ boolean ch_name( sample, name )
register char      *sample,*name;
{
#   define sample_end    ' '
#   define name_end      '\00'
#   define is_sample_end (*sample == sample_end)
#   define is_name_end   (*name   == name_end)
#   define OK            return(YES)
#   define FAIL          return(NO)

	for( ; not is_sample_end; ++sample, ++name )
	{
		switch( *sample ) {
		case '*':
			if( sample[1] == sample_end )  OK; /* только '*' в образце */
			++sample;
			while( not is_name_end )
			{
				if( ch_name( sample, name ) )  OK;
				/* if( *name == '/' )  FAIL;       /* '*' не сравн. с '/' */
				++name;
			}
			FAIL;
		case '?':
			if( is_name_end )  FAIL;
			break;
		default:
			if( *name != *sample )  FAIL;
			break;
		}
	}
	return(is_name_end);
}


/* ПРЕОБРАЗОВАНИЕ СИМВОЛА ИЗ СТРОКИ 'pp' ИЗ ТЕКСТОВОГО ВИДА В КОД */

/*static*/ int cvsymb( pp )
register char *pp;
{
	register int   c;
	register char *p;
	static   int   last_c;
	static   char  ico[] = {
		'i','c', ic,   'd','c', dc,   'i','l', il,   'd','l', dl,
		'l','f', lf,   'r','n', rn,   'r','e', 32,   'b','l', bl,
		'e','s', es,   'e','l', el,   't','a', ta,   'd','e', 32,
		'F','1',F1,'F','2',F2,'F','3',F3,'F','4',F4,'F','5',F5,
		'F','6',F6,'F','7',F7,'F','8',F8,'F','9',F9,
		0
	};

	if( *pp=='.' )        return( last_c );         /* . */
	c = 0;
	if( *pp=='_' )
	{
		c=0x0200; 
		++pp;
	}          /* _a ... */
	if( *pp=='!' )
	{
		c=0x0100; 
		++pp;
	}          /* spec */
	if( *pp=='\'' )
	{
		c += pp[1] & 0x1f;
	}       /* control */
	else if( *pp=='\\' )
	{
		c += pp[1];
	}              /* escape */
	else {
		for( p=ico; *p; p+=3 )
		{                    /* мнемон. */
			if(*p==*pp && p[1]==pp[1])
			{
				c+=p[2]; 
				goto fn;
			}
		}
		c += *pp;
fn: 
		;
	}
	return( last_c = c );
}
check(name)
char *name;
{
	return(access(name,02));
}

move(a,b,len)
char *a,*b;
int len;
{
	register int i;
	if(a==b || len==0)
		return;
	if(a<b)
		for(i=len-1;i>=0;i--)
			b[i]=a[i];
	else
		for(i=0;i<len;i++) b[i]=a[i];
}

char        *name_f, *name_g;
char *cvmacro(sym)  char sym; 
{
	register char *s;

	switch(sym) {
	case '@': 
		s = ffull;           
		break;   /* полное имя */
	case 'n': 
		s = fname;           
		break;   /* имя без суффикса */
	case 's': 
		s = fsuff;           
		break;   /* суффикс */
	case 'c': 
		s = hcat;            
		break;   /* имя каталога */
	case 'a': 
		s = arname;          
		break;   /* имя библиотеки */
	case 'p': 
		s = arpath;          
		break;   /* имя пути в библ. */
	case 'C': 
		s = altcat;          
		break;   /* имя др.каталога */
	case 'A': 
		s = altarname;       
		break;   /* имя др.библиотеки */
	case 'P': 
		s = altpath;         
		break;   /* имя пути в др.библ*/
	case '~': 
		s = getenv("HOME");  
		break;   /* дом.кат */
	case 'f': 
		s = name_f;          
		break;   /* второе имя */
	case 'g': 
		s = name_g;          
		break;   /* третье имя */
	default:  
		s = NULL;            
		break;
	}
	return(s);
}
#ifndef CX

#include <fcntl.h>
#include <unistd.h>
#define CXKEY  514142603    /* признак Context-4 базы */

static void d_full(char *dir,char *name,char *full_name)
{
	full( *name == '/' ? "/" : *dir ? dir : "./", name, full_name );
}

static  long get_key(char *dir,char *name)
{
	register int   df;
	long           key = -1L;
	char  full_name[512];
	d_full(dir,name,full_name);
	if( (df = open(full_name,O_RDONLY)) >= 0 )
	{
		read(df,(char*)&key,sizeof(key));
		close(df);
	}
	return(key);
}

if_base(char *dir,char *name)
{
	long i;
	char  full_name[512];

	full(name,name,full_name);
	i=get_key(dir,full_name);
	if(i==-5472564l || i==5472564l)
		return(3);
	if(i==CXKEY || i==-1959680738l)
		return(4);
	if(i==-CXKEY || i==5472566)
		return(5);
	sprintf(full_name,"%s/%s.dbf",name,name);
	if(!access(full_name,R_OK))
			return(1);
	for(i=strlen(name);i;i--)
		if(name[i]=='.')
			break;
	if(!strcmp(name+i,".dbf"))
		return(1);
	return(0);
}

int PathName(char *pathname, char *path, char *name)
{
	if( !pathname )
	{
		return -1;
	}
	if( path && (!name || *name != '/') )
	{
		if( pathname != path )
			strcpy( pathname, path );
		if( *pathname && pathname[strlen(pathname)-1] != '/' )
			strcat( pathname, "/" );
	}
	else
		*pathname = 0;
	if( name )
		strcat( pathname, name );
	return 0;
}

int HeadName(char *headname, char *basename)
{
	if( basename )
	{
		char *s = strrchr( basename, '/' );
		return PathName( headname, basename, s ? s + 1 : basename );
	}
	return -1;
}

int full(char *a,char *b,char *l)
{
	return (a && b && !strcmp(a, b)) ? HeadName( l, a ) : PathName( l, a, b );
}

#endif
