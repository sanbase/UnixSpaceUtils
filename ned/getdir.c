/*
	Copyright 1995,1996,1997 Unitex+, ltd. Kiev, Ukraine
	Written by Oleg Smolyar
	E-mail: oleg@unitex.kiev.ua
*/
#ifndef lint
static const char rcsid[]="$Id: getdir.c,v 2.2.1.1 1998/07/24 12:58:58 toor Exp toor $";
#endif
/*
 *  Выбор файла из каталога
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <tty_codes.h>
#include "ned.h"

#define SHFSIZE    16
#define FSHFSIZE   18
#define FULLSIZE   74
/* Цвета для меню выбора файлов */
#define C_GROUND   COLMBGR   /* фон меню                                   */
#define C_CURRENT  COLMSTR   /* текущая строка                             */
#define C_DIR      COLDIR    /* цвет директории                            */
#define C_FILE     COLFILE   /* цвет файла                                 */
#define C_FSLNK    COLFLNK   /* символическая ссылка на файл               */
#define C_DSLNK    COLDLNK   /* символическая ссылка на директорий         */
#define C_WAIT     COLDWAIT  /* цвет текущей строки, пока идет чтение      */
			     /* следующего элемента файловой системы       */

#undef S_ISLNK
#undef S_ISDIR
#undef S_ISREG

#define S_ISLNK(m) ((m&S_IFLNK)==S_IFLNK)
#define S_ISDIR(m) ((m&S_IFDIR)==S_IFDIR && !S_ISBLK(m) && !S_ISFIFO(m))
#define S_ISREG(m) ((m&S_IFREG)==S_IFREG)

typedef struct dirc {
	char *elem;            /* имя элемента каталога */
	struct stat st;        /* его описатель         */
} DC;

extern char curdir[LINESIZE+1];

static int  MAXMENU_X,MAXMENU_Y;
static DC   *d;
static int  *mas,mmas=0;
static char FULL=1; /* 0 - полный показ,1 - неполный показ */
static char MODE=SHFSIZE;
static char *lastdir=NULL;

static int  show_menu(int,int,int,int,int,int,int,int,
		     int (*)(int,int,int,int),int,int);
static int  check_file(char *,DC *,char);
static int  screen_file(int,int,int,int);
static int  cmps(const void *,const void *);
static int dmenu(char *,char *,int (*check)(char *,DC *,char),int (*func)(int,int,int,int),char *);
static int get_gab(int,int,int *,int *,int *,int *);
static int getdir(char *ss,int (*)(char *,DC *,char *));
static int if_text(char *dir,char *name);
int menu(int,int,int,int,int (*)(int,int,int,int),int,int,int *,int *);

//#define X11

/* Бокс, обрабатывающий запрос на выбор файла */
void SelectNewFile() {
	char filename[LINESIZE+1];

	if(cmn.NoSelFile) {
		dpo(bl); return;
	}
	stor_poz();
	if(lastdir==NULL) lastdir=Strdup(curdir);
	line_status(33);
	if(cmn.RestProt) {
		switch(ReadComInProt(NULL)) {
		case C_RETURN:
			if(ReadComInProt(filename)>=0) {
			        Free(lastdir);
			        lastdir=Strdup(pathn(filename));
				if(access(filename,F_OK)) /* а вдруг удалили? */
				        CloseRestProt();
				else
			                InitEdit(filename,NORMWIND);
			} else
				CloseRestProt(); /* ошибка восстановления */
			break;
		case C_STOP:
			break;
		default:
			CloseRestProt(); /* ошибка восстановления */
			break;
		}
		rest_poz();
		return;
	}
	TmpUnsetProt();
	while(1)
	{
		switch(dmenu(lastdir,filename,check_file,screen_file,pmes(8)))
		{
		case C_STOP:
			chdir(curdir); rest_poz();
			TmpRestProt();
			WriteInProt(C_STOP,NULL);
			return;
		case C_RETURN:
			rest_poz();
			chdir(curdir);
			Free(lastdir);
			lastdir=Strdup(pathn(filename));
			strcpy(filename,norm_path(filename));
			TmpRestProt();
			WriteInProt(C_RETURN,filename);
			InitEdit(filename,NORMWIND);
			return;
		default:
			continue;
		}
	}
}

static int dmenu(char *line,char *ss,int (*check)(char *,DC *,char),int (*func)(int,int,int,int),char *titl)
{
	int i,n,j,c=0,n_std=0,nb,poz,ele;
	char pozicon,*lastdir=NULL;

	if(*line && access(line,F_OK)) return(-1);
	strcpy(ss,line);
	nb=get_box(0,0,l_x+1,l_y+1);
	Set_New_Color(C_GROUND);
	title(1,titl);
BEGIN:
	if(n_std>0) {
		for(i=0;i<n_std;i++)
			if(d[i].elem!=NULL)
		                Free(d[i].elem);
		Free(d);  Free(mas); mmas=0;
	}
	n=getdir(ss,(int (*)(char *,DC *,char *))check);
	if(n<=0) {
	        if(lastdir!=NULL) Free(lastdir);
		return(n);
	}
	n_std=n;
	if(FULL) {
FU:
		for(i=j=0;i<n_std;i++)
			if(!(*check)(ss,d+i,FULL)) {
				mas[j]=i; j++;
			}
		n=j;
	}
	poz=0; pozicon=0;
	if(c==C_CTRLENT && lastdir!=NULL) {
		for(j=i=0;i<n;j++,i++)
			if(!strcmp(d[mas[i]].elem,lastdir)) {
			        ele=j; c=1;
				if(lastdir!=NULL) {
				        Free(lastdir); lastdir=NULL;
				}
				break;
			}
		if(i==n) ele=0;
	}
	else
		ele=0;
	while(1) {
		int nb1;

		nb1=get_box(0,l_y-4,l_x,5);
		Set_New_Color(C_GROUND);  title(l_y-4,ss);
		c=(int)menu(l_x-3,l_y-12,n,MODE+1,(int (*)(int,int,int,int))func,C_GROUND,C_CURRENT,&poz,&ele);
		errno=0;
		put_box(0,l_y-4,nb1); free_box(nb1);
		switch(c) {
		case C_STOP:
			goto END;
		case C_RETURN:
			if(S_ISDIR(d[mas[ele]].st.st_mode)) {
				if(!strcmp(d[mas[ele]].elem,".")) continue;
				if(!strcmp(d[mas[ele]].elem,"..")) goto UP;
				if(!strcmp(ss,"/"))
					strcat(ss,d[mas[ele]].elem);
				else
					sprintf(ss,"%s/%s",ss,d[mas[ele]].elem);
				if(lastdir!=NULL) Free(lastdir);
				lastdir=Strdup(basen(ss));
				goto BEGIN;
			}
			if(!(*check)(ss,d+mas[ele],FULL))
			{
				if(!strcmp(ss,"/"))
					sprintf(ss,"/%s",d[mas[ele]].elem);
				else
					sprintf(ss+strlen(ss),"/%s",d[mas[ele]].elem);
				goto END;
			}
			continue;
		case '~':
			{
				char *ch;

				if((ch=getenv("HOME"))==NULL) continue;
				if(access(ch,F_OK)) continue;
				strcpy(ss,ch);
				goto BEGIN;
			}
		case C_CTRLENT:
UP:
			if(!strcmp(d[mas[ele]].elem,"/")) continue;
			strcpy(ss,pathn(ss));
			goto BEGIN;
		case '+':
			if(cmn.NoPlusFile || pozicon) {
				dpo(bl); continue;
			}
			FULL=!FULL;
			goto FU;
		case '/':
			if(!strcmp(ss,"/")) continue;
			strcpy(ss,"/");
			goto BEGIN;
		case C_STRDEL:
			if(!pozicon) continue;
			pozicon=0;
			goto FU;
		case C_INPSEVDO:
			MODE=(MODE==FULLSIZE)?SHFSIZE:FULLSIZE;
			continue;
		default:
			if(nis_print(c)) {
				for(i=j=0;i<n;i++)
					if((unsigned char)d[mas[i]].elem[pozicon]==c)
						mas[j++]=mas[i];
				if(j) {
					n=j; poz=0; ele=0; pozicon++;
					continue;
				}
			}
			dpo(bl);
			continue;
		}
	}
END:
	put_box(0,0,nb); free_box(nb);
	if(n_std>0) {
		for(i=0;i<n_std;i++)
		        if(d[i].elem!=NULL)
			        Free(d[i].elem);
		Free(d);
	}
	Free(mas); mmas=0;
	return(c);
}

static int getdir(char *ss,int (*check)(char *,DC *,char *))
{
	struct dirent *dp;
	int n,len,l;
	DIR *df=NULL;

	strcpy(ss,norm_path(ss));
	chdir((const char *)ss);
	if((df=opendir("."))==NULL) {
		errno=0; n=-1; goto END;
	}
	for(len=0,dp=readdir(df);dp!=NULL;dp=readdir(df),len++);
	if(!len) {
		n=-1; goto END;
	}
	if((d=(DC *)Malloc(len*sizeof(DC)))==NULL) {
		n=-1; goto END;
	}
	rewinddir(df);
	for(n=l=0;l<len && (dp=readdir(df))!=NULL;l++) {
		lstat(dp->d_name,&d[n].st);
		if(S_ISLNK(d[n].st.st_mode)) {
			char str[LINESIZE+1];
			int k;

			if((k=readlink(dp->d_name,str,LINESIZE))<=0) continue;
			str[k]=0;
			if(access(str,F_OK)) continue;
			stat(dp->d_name,&d[n].st); d[n].st.st_mode|=S_IFLNK;
		}
		d[n].elem=Strdup(dp->d_name);
		if((*check)(ss,d+n,0)) continue;
		n++;
	}
	if((mas=(int *)Malloc((mmas=n)*sizeof(int)))==NULL) {
		mmas=0; n=-1; free(d); goto END;
	}
	for(l=0;l<mmas;l++) mas[l]=l;
	qsort(d,n,sizeof(DC),cmps);
END:
	if(df!=NULL) closedir(df);
	return(n);
}

int menu(int l,int h,int len,int lenel,int (* func)(int,int,int,int),
	 int col1,int col2,int *i,int *j)
{
	int xold=0,yold=0,nbm=-1;
	int stlen,locklen,x,y,lines,columns,l_i;
	int c;

	MAXMENU_X=l; MAXMENU_Y=h;
	stlen=get_gab(len,lenel,&x,&y,&lines,&columns);
B:
	if(*j<0)
		*j=0;
	else
		if(*j>=len) *j=len-1;
	if(len-*j<stlen && len%stlen && *j>=len-len%stlen)
		locklen=get_gab(len%stlen,lenel,&x,&y,&lines,&columns);
	else
		locklen=get_gab(len,lenel,&x,&y,&lines,&columns);
	if(stlen)
	        *i=*j%stlen;
	else
		*i=*j=0;
	if(nbm!=-1) {
		put_box(xold,yold,nbm); free_box(nbm);
	}
	xold=x; yold=y;
	nbm=get_box(x,y,columns*lenel+5,lines+6);
#ifdef X11
SHOW:
#endif

	show_menu(x,y,columns,lines,locklen,lenel,*i,*j-*i,(int (*)(int,int,int,int))func,col1,col2);
	while(1) {
#ifndef NOBORDIND
		show_bord_vert(x+columns*lenel+2,y+1,lines-2,len,*j,col1);
#endif
		l_i=*i;
		dpp(x+2+lenel*(*i/lines),y+(*i%lines)+1);
		c=ReadCommand(NULL,-1);
#ifdef X11

		if(c==C_MOUSE) {
			if(ev.b!=3) {

#ifndef NOBORDIND
				int p;

				if((p=get_bord_vert(ev.x,ev.y))>=0) {
					if(p<*j-*i || p>=*j-*i+locklen) {
						*j=p; goto B;
					}
					else {
						*i+=p-*j; *j=p; goto SHOW;
					}
				}
#endif
			        if(ev.y>y && ev.y<=y+lines) {
				        if(ev.x>x && ev.x<x+columns*lenel) {
					        *i=((ev.x-x-1)/lenel)*lines+(ev.y-y-1)%lines;
					        if(*i==l_i) {
							if(ev.b==2)
								c=C_CTRLENT;
							else
						                c=C_RETURN;
							goto SWI;
					        }
					        if(*i>=locklen) {
						        *i=l_i; continue;
					        }
					        goto M;
				        }
			        }
			        continue;
			}
			else
				c=C_STOP;
		}
#endif
		if(!len) goto DEF;
#ifdef X11
SWI:
#endif
		switch(c) {
		case C_STRRIGHT:
			if((*i+=lines)>=locklen) *i=(*i+1)%lines;
			break;
		case C_STRLEFT:
			if((*i-=lines)<0) {
				*i+=((locklen+lines-1)/lines)*lines-1;
				if(*i>=locklen) *i-=lines;
				if(*i<0) *i=0;
				break;
			}
			break;
		case C_DOWN:
			if(*i+1<locklen)
				*i+=1;
			else
				*i=0;
			break;
		case C_UP:
	                if(*i>0)
				*i-=1;
			else
				*i=locklen-1;
			break;
		case C_STRHOME:
			*i=0; break;
		case C_STREND:
			*i=locklen-1; break;
		case C_PGDN:
			*j+=stlen; *i=locklen-1;
			goto B;
		case C_PGUP:
			*j-=stlen; goto B;
		case C_CTRLENT:
		case C_RETURN:
			(*func)(x+2+(*i/lines)*lenel,y+(*i%lines)+1,*j,C_WAIT);
			scrbufout();
		default:
DEF:
			put_box(x,y,nbm); free_box(nbm); nbm=-1;
			return(c);
		}
#ifdef X11
M:
#endif
		{
			int l_j;

			l_j=*j; *j=*j-l_i+*i;
			(*func)(x+2+(l_i/lines)*lenel,y+(l_i%lines)+1,l_j,col1);
		}
		(*func)(x+2+(*i/lines)*lenel,y+(*i%lines)+1,*j,col2);
	}
}

static int sq0[]={
	0,4,9,16,25,36,49,64,81,100,121,144,169,196,225,256,289,324,361 };

static int get_gab(int len,int lenel,int *x,int *y,int *l,int *c)
{
	for(*c=MAXMENU_X/lenel;(*c)--;)
		if(len>=sq0[*c]) break;
	(*c)++; *l=(len+((*c)-1))/(*c);
	if(!(*l)) *l=1;
	if(*l>MAXMENU_Y) *l=MAXMENU_Y;
	*x=((l_x-(*c)*lenel)>>1)-1; *y=5;
	return((*l)*(*c)>len?len:(*l)*(*c));
}

static int show_menu(int x,int y,int columns,int lines,
	      int len,int lenel,int p,int t,int (*func)(int,int,int,int),
	      int col1,int col2)
{
	int l,c;

	Set_New_Color(col1);
	wind(x,y,columns*lenel+3,lines+2);
	for(c=0;c<columns;c++)
		for(l=0;l<lines;l++) {
			if(c*lines+l>=len) return(0);
			dpp(x+2+c*lenel,y+l+1);
			(*func)(x+2+c*lenel,y+l+1,t+c*lines+l,(c*lines+l==p)?col2:col1);
		}
	return(0);
}

static int cmps(const void *A,const void *B)
{
	DC *a=(DC *)A;
	DC *b=(DC *)B;

	if((a->elem[0]!='.' && b->elem[0]!='.') || (a->elem[0]=='.' && b->elem[0]=='.'))
		return(strcmp(a->elem,b->elem));
	return((a->elem[0]=='.')?-1:1);
}

void root_path(char *name,char *direct)
{
	char *cur,*ch;

	cur=getcwd(NULL,LINESIZE);
	if(cur==NULL || chdir(direct)) return;
	ch=getcwd(NULL,LINESIZE);
#ifdef BUG_GETCWD
	errno=0;
#endif
	if(ch!=NULL) {
		strcpy(name,ch); free(ch);
	}
	chdir(cur);
	free(cur);
}

char *pathn(char *s)
{
	static char path[256];
	char *p;

	strncpy(path,s,255);
	p=strrchr(path,'/');
	if(p==NULL) {
		p=path; path[0] ='.'; path[1] =0;
	}
	else
		if(p!=path)
			*p=0;
		else
			*(p+1)=0;
	return(path);
}

char *basen(char *s)
{
	static char path[LINESIZE+1];
	char *p;

	strcpy(path,s);
	p=strrchr(path,'/');
	if(p==NULL) return(path);
	return(p+1);
}

static int screen_file(int x,int y,int j,int col)
{
	int i;
	DC *e=d+mas[j];

	dpp(x,y);
	if(col!=C_CURRENT && col!=C_WAIT) {
		if(S_ISDIR(e->st.st_mode))
			if(S_ISLNK(e->st.st_mode))
			        setcolor(C_DSLNK);
			else
			        setcolor(C_DIR);
		else
			if(S_ISLNK(e->st.st_mode))
			        setcolor(C_FSLNK);
			else
				Set_New_Color(C_FILE);
	}
	else
		setcolor(col);
	switch(MODE) {
	case SHFSIZE:
	        for(i=0;i<SHFSIZE && e->elem[i];i++) dpo(e->elem[i]);
		break;
	case FULLSIZE: {
		        char str[NAMESIZE+1];
		        struct passwd *p;
		        struct group  *g;
		        struct tm *t;
		        int i,k,j;

			for(k=i=0;i<FSHFSIZE;i++) {
				if(!e->elem[i]) k=1;
				if(k)
					str[i]=' ';
				else
				        str[i]=e->elem[i];
			}
			i+=sprintf(str+i," %10d",e->st.st_size);
		        if((p=getpwuid(e->st.st_uid))==NULL || !*p->pw_name)
				i+=sprintf(str+i," %-8d ",e->st.st_uid);
		        else
			        i+=sprintf(str+i," %-8s ",p->pw_name);
		        if((g=getgrgid(e->st.st_gid))==NULL || !*g->gr_name)
			        i+=sprintf(str+i," %-8d ",e->st.st_gid);
		        else
				i+=sprintf(str+i," %-8s ",g->gr_name);
#ifdef NO_STRMODE
		        switch(e->st.st_mode&S_IFMT) {
		        case S_IFIFO:
				str[i]='f'; break;
		        case S_IFCHR:
			        str[i]='c'; break;
		        case S_IFDIR:
			        str[i]='d'; break;
		        case S_IFBLK:
			        str[i]='b'; break;
		        case S_IFREG:
			        str[i]='-'; break;
#ifndef NO_IFSOCK
		        case S_IFSOCK:
			        str[i]='s'; break;
#endif
			default:
				if(S_ISLNK(e->st.st_mode))
					str[i]='l';
				else
					str[i]='?';
		        }
		        for(i++,k=0,j=S_IRUSR;j>0;j>>=1,k++,i++) {
			        if(!(j&e->st.st_mode))
				        str[i]='-';
			        else
				        if(k==0)
					        str[i]='r';
				        else
					        if(k==1)
						        str[i]='w';
					        else
						        str[i]='x';
			        if(k==2) k=-1;
		        }
#else
			strmode(e->st.st_mode,str+i); i+=10;
#endif
			str[i++]=' ';
			t=gmtime(&e->st.st_mtime);
			strftime(str+i,18,"%d.%m.%y %H:%M",t);
		        dps(str);
		}
	}
	return(0);
}

static int check_file(char *dir,DC *e,char reg)
{
	int len;

	if(reg) {
		if(*e->elem=='.') return(-1);
		len=strlen(e->elem);
		if((len>1 && S_ISREG(e->st.st_mode) && e->elem[len-2]=='.' &&
		   (e->elem[len-1]=='b' || e->elem[len-1]=='o')))
			return(-1);
		if((S_ISREG(e->st.st_mode) && !S_ISDIR(e->st.st_mode) &&
		    if_text(dir,e->elem)<0))
			return(-1);
	}
	if(S_ISREG(e->st.st_mode) || S_ISDIR(e->st.st_mode))
		return(0);
	return(-1);
}

static int if_text(char *dir,char *name)
{
	int fd,size,ret=0;
	unsigned char str[LINESIZE+1];

	if(dir!=NULL)
	        if(strcmp(dir,"/"))
		        sprintf((char *)str,"%s/%s",dir,name);
	        else
		        sprintf((char *)str,"/%s",name);
	else
		strcpy((char *)str,name);
	if(IfAlreadyUse((char *)str,-1)!=NULL) return(0);
	if((fd=open((char *)str,O_RDONLY|O_NONBLOCK))<0) ret=-1;
	if(!ret) {
	        size=read(fd,str,LINESIZE); close(fd);
	}
	for(fd=0;ret>=0 && fd<size;fd++)
		if(str[fd]<' ') {
			 if(str[fd]==SPECMANCODE1)
			         ret=1;
			 else
		                if(str[fd]!='\t' && str[fd]!='\n' &&
				   str[fd]!='\r')
				{
			                ret=-1; break;
			        }
		}
	return(ret);
}

/* Возвращает реальный путь, исключая символические ссылки */
char *norm_path(char *old)
{
	static char ss[LINESIZE+1];

	root_path(ss,pathn(old));
	if(!strcmp(ss,"/"))
		strcat(ss,basen(old));
	else
		sprintf(ss+strlen(ss),"/%s",basen(old));
	return(ss);

}
