/*
	Copyright 1995,1996,1997 Unitex+, ltd. Kiev, Ukraine
	Written by Oleg Smolyar
	E-mail: oleg@unitex.kiev.ua
*/
#ifndef lint
static const char rcsid[]="$Id: init.c,v 2.2.1.1 1998/07/24 12:58:58 toor Exp toor $";
#endif
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <tty_codes.h>
#include <errno.h>
#include "ned.h"

extern NEF    *AllFiles;
extern int    LenAllFiles;

int TestRight(mode_t mode,uid_t uid,gid_t gid);
static int InitTextFile(char *,int);

int InitEdit(char *name,int arg)
{
	int err=0;

	if(!(err=InitTextFile(name,arg))) {
		if(LenAllFiles>1) {
			AllFiles[LenAllFiles-2].EB.colorborder=AllFiles[LenAllFiles-2].rdonly?COLRDPASS:COLPASS;
			OutEditBox(AllFiles+LenAllFiles-2);
		}
		if(arg==TEMPWIND) {
			AllFiles[LenAllFiles-1].typ=TEMPWIND;
			if(LenAllFiles>1) AllFiles[LenAllFiles-2].nocom=0;
		} else
			if(arg==HELPWIND) {
				AllFiles[LenAllFiles-1].typ=HELPWIND;
				AllFiles[LenAllFiles-1].rdonly=1;
			} else
				if(arg==CBWIND) {
					AllFiles[LenAllFiles-1].typ=CBWIND;
					AllFiles[LenAllFiles-1].rdonly=1;
					arg=NORMWIND;
				} else
					if(arg==MANWIND) {
						AllFiles[LenAllFiles-1].typ=MANWIND;
						AllFiles[LenAllFiles-1].rdonly=1;
						arg=FULLWIND;
					} else
						if(arg==DBWIND) {
							AllFiles[LenAllFiles-1].typ=DBWIND;
							AllFiles[LenAllFiles-1].rdonly=1;
							arg=NORMWIND;
						}
						else
							if(cmn.AltName!=NULL && cmn.NumAltName<0)
								cmn.NumAltName=(int)AllFiles[LenAllFiles-1].wd;
		AllFiles[LenAllFiles-1].WindName=strdup(GetFileName(AllFiles+LenAllFiles-1));
		InitEditBoxForFile(AllFiles+LenAllFiles-1,arg);
	}
	if(err=1) err=0;
	return(err);
}

static int InitTextFile(char *name,int arg)
{
	int err=0,atr=0;
	NEF *W;
	FILE *fd=NULL;
	struct stat st;
	char str[LINESIZE+1],Name[LINESIZE+1];

	if(LenAllFiles==MAXFILES) {
		emess(pmes(119)); err=-2; goto END;
	}
	if(name)
	        strcpy(Name,name);
	else
		*Name=0;
	if(arg==NORMWIND || arg==HELPWIND || arg==TEMPWIND || arg==MANWIND) {
		sprintf(str,pmes(45),arg!=MANWIND?basen(Name):"man");
		init_long_pause(10,str,1);
		if(arg!=MANWIND) {
			if(lstat(Name,&st)) {
ERRNO:
				if(errno==ENOMEM || errno==ENFILE ||
				   errno==EMFILE)
					err=-2;
				else
					err=-1;
				sprintf(str,pmes(27),basen(Name));
				emess(str); goto END;
			}
			else {
				if((st.st_mode&S_IFLNK)==S_IFLNK) {
					if((err=readlink(Name,str,LINESIZE))<0)
						goto ERRNO;
					str[err]=0; err=0;
					strcpy(Name,norm_path(str));
			                if(stat(Name,&st))
						goto ERRNO;
				}

			}
			if((st.st_mode&S_IFMT)!=S_IFREG)
				goto ERRNO;
			if((atr=TestRight(st.st_mode,st.st_uid,st.st_gid))==2)
				goto ERRNO;
			if(cmn.Rdonly) atr=1;
			if((fd=fopen(Name,atr?"r":"r+"))==NULL)
				if(atr || (!atr && (fd=fopen(Name,"r"))==NULL))
					goto ERRNO;
				else
					atr=1;
			if(!atr && block_file(fd)) atr=2;
		} else {
			int fdd,j;
			pid_t pid;

			sprintf(str,MANNAME,getlogin(),get_tty(),Name);
			if((fdd=open(str,O_CREAT|O_RDWR,0660))<0) goto ERRNO;
			pid=getpid();
			if(fork()==0) {
				close(1); close(2);
				if(creat(str,O_RDONLY)>=0) {
					char str1[LINESIZE+1];

					sprintf(str1,F_NAMEERR,TMPDIR,pid);
					creat(str1,0600);
					sprintf(str1,CALLMAN,Name,str);
					ExeCommand(str1);
				}
				exit(RETNOFIND);
			}
			wait(&j);
			j>>=8;
			if(j==RETNOFIND) {
ERRMAN:
			        close(fdd); unlink(str);
				sprintf(str,pmes(108),Name);
				err=-1;
				close_long_pause();
				emess(str);
				goto END;
			} else {
				if(j) {
					unlink(str);
					err=RunMess(NULL,j,getpid());
					if(err==1) {
						err=1; goto END;
					} else if(err==-1) {
						err=-1; goto END;
					}
				} else {
					sprintf(str,F_NAMEERR,TMPDIR,pid);
					unlink(str);
				}
			}
			if((fd=fdopen(fdd,"r"))==NULL) goto ERRMAN;
		}
	}
	if(AddMemoryAllFiles(1)) {
		err=-1; goto END;
	}
	W=AllFiles+LenAllFiles-1;
	bzero(W,sizeof(NEF));
	if(arg==NORMWIND || arg==HELPWIND || arg==TEMPWIND || arg==MANWIND) {
		NEF *V;
		char *ch;

		if(arg==MANWIND) sprintf(str,"man %s",Name);
		if((W->FileName=(char *)Strdup(arg==MANWIND?str:Name))==NULL)
		{
			err=-1; goto END;
		}
		if(atr) {
			W->rdonly=1;
			if(atr==2) W->block=1;
		}
		if(IfNeedColour(W->FileName)) W->colour=1;
		if((V=IfAlreadyUse(Name,LenAllFiles-1))==NULL) {
			if((W->AS=(NSB *)Malloc(sizeof(NSB)))==NULL) {
				err=-1; goto END;
			}
			bzero(W->AS,sizeof(NSB));
			W->AS->fd=(void *)fd; W->AS->seekf=-1;
			if(ReadFile(W,0,fd,0)) {
				fd=NULL; DelTextFile(LenAllFiles-1); err=-1;
			}
		} else {
			/* В случае нескольких окон на один и тот же файл */
			W->AS=V->AS; W->rdonly=V->rdonly; fclose(fd);
		}
	} else
		W->AS=(arg==DBWIND?&DB:&CB);
	RP(W)=-1;
	for(atr=0;atr<LenAllFiles-1;atr++) err=max(err,AllFiles[atr].wd);
	W->wd=err+1; err=0;
	if(arg==NORMWIND) W->AS->undo=1;
END:
	close_long_pause();
	if(fd!=NULL && err) {
		unblock_file(fd); fclose(fd);
	}
	return(err);
}

int AddMemoryAllFiles(int num)
{
	int err=0;

	if(LenAllFiles==0 && num>0) {
		if((AllFiles=(NEF *)Malloc(num*sizeof(NEF)))==NULL) {
			err=-1; goto END;
		}
	}
	else {
		NEF *AF;

		if(LenAllFiles+num<=0)
			Free(AllFiles);
		else {
			if((AF=(NEF *)Malloc((LenAllFiles+num)*
			    sizeof(NEF)))==NULL)
			{
				err=-1; goto END;
			}
			memcpy(AF,AllFiles,sizeof(NEF)*(LenAllFiles+(num<0?num:0)));
			Free(AllFiles);
			AllFiles=AF;
		}
	}
	LenAllFiles+=num;
END:
	return(err);
}

void FreeUndo(NSB *W)
{
	if(W!=NULL && W->U!=NULL) Free(W->U);
	W->U=NULL; W->CurU=W->MemU=W->NumU=0;
}

void DelTextFile(int i)
{
	if(AllFiles[i].typ!=CBWIND && AllFiles[i].typ!=DBWIND) {
		if(IfAlreadyUse(AllFiles[i].FileName,i)==NULL) {
		     /* в случае если окон на этот файл больше нет */
			if(AllFiles[i].AS->fd!=NULL) {
				fclose((FILE *)AllFiles[i].AS->fd);
				AllFiles[i].AS->fd=NULL;
			}
			ResetStrBuf(AllFiles[i].AS);
			FreeUndo(AllFiles[i].AS);
			Free(AllFiles[i].AS);
		}
	}
	if(AllFiles[i].FileName) {
		if(AllFiles[i].typ==MANWIND) {
			char str[NAMESIZE+1];

			sprintf(str,MANNAME,getlogin(),get_tty(),AllFiles[i].FileName+4);
			unlink(str);
		}
		Free(AllFiles[i].FileName);
		Free(AllFiles[i].WindName);
	}
	if(LenAllFiles-1!=i)
		memcpy(AllFiles+i,AllFiles+i+1,(LenAllFiles-1-i)*sizeof(NEF));
	AddMemoryAllFiles(-1);
}

int ClearTail(NEF *W)
{
	int err=0;

#ifndef NO_CLEAR_TAIL

	int i;
	STR *S;

	for(i=NMAX(W)-1;i>0;i--) {
		S=GetSTR(W->AS,i);
		if(S->s[0]) break;
	}
	if(i!=NMAX(W)-1) {
		if(ControlMemoryBuffer(W->AS,-(NMAX(W)-1-i))) {
			err=-1; goto END;
		}
	}
END:
#endif
	return(err);
}

int Strlen(char *str)
{
	return(str==NULL?0:strlen(str));
}

void ClearEndSTR(STR *S)
{
	if(S->s[S->rlen]) /* ??? */
		S->rlen=Strlen(S->s);
	for(;S->rlen && S->s[S->rlen-1]==' ';S->rlen--);
	S->s[S->rlen]=0;
}

int ClearEndStr(char *s)
{
	int i;

	for(i=Strlen(s);i && s[i-1]==' ';i--);
	s[i]=0;
	return(i);
}

int NumBegSpace(char *s)
{
	int i,j;

	for(j=i=0;s && (s[i]=='\t' || s[i]==' ');i++,j++)
		if(s[i]=='\t') j+=7;
	return(j);
}

char *Strdup(char *str)
{
	char *ch;

	if(str==NULL || !*str)
		ch=NULL;
	else {
		ch=(char *)Malloc(Strlen(str)+1);
		if(ch!=NULL) strcpy(ch,str);
	}
	return(ch);
}

/* Изменение признака редактирования файла */
void ChangeRed(NEF *W,char arg)
{
	if(W->AS->red==arg || W->rdonly || W->typ==HELPWIND || W->typ==TEMPWIND ||
	   W->typ==CBWIND  || W->typ==DBWIND || W->typ==MANWIND)
		return;
	W->AS->red=arg;
}

int TestRight(mode_t mode,uid_t uid,gid_t gid)
{
	char atr=0;

	if(getuid()==uid || getuid()==0) {
		if(!(mode&S_IWUSR))
			if(!(mode&S_IRUSR))
				atr=2;
			else
				atr=1;
	} else {
		if(getgid()==gid) {
			if(!(mode&S_IWGRP))
				if(!(mode&S_IRGRP))
					atr=2;
				else
					atr=1;
		}
		else
			if(!(mode&S_IWOTH))
				if(!(mode&S_IROTH))
					atr=2;
				else
					atr=1;
	}
	return((int)atr);
}

/* Блокировка файла */
int block_file(FILE *fd)
{
	struct flock lock;

	lock.l_type=F_WRLCK; lock.l_whence=lock.l_start=0; lock.l_len=1;
	if(fcntl(fileno(fd),F_SETLK,&lock)) {
#ifdef NO_RBLOCK
		if(errno==EOPNOTSUPP)   /* удаленную блокировку BSD */
			return(errno=0);/*   не поддерживает! */
		else
#endif
	                return(-1);
	}
	return(0);
}

/* Разблокирует файл */
int unblock_file(FILE *fd)
{
	struct flock lock;

	lock.l_type=F_UNLCK; lock.l_whence=lock.l_start=0; lock.l_len=1;
	if(fcntl(fileno(fd),F_SETLK,&lock)) return(-1);
	return(0);
}

/* Определяет, открыт ли файл редактором, и если открыт, возвращает
   указатель на его дескриптор. str - абсолютное имя */
NEF *IfAlreadyUse(char *str,int j)
{
	int i;

	for(i=0;i<LenAllFiles;i++)
		if(i!=j && AllFiles[i].FileName!=NULL && !strcmp(AllFiles[i].FileName,str))
			return(AllFiles+i);
	return(NULL);
}

int RunMess(NEF *W,int ret_code,pid_t pid)
{
	struct stat st;
	char str[LINESIZE];
	int err=0;

	sprintf(str,F_NAMEERR,TMPDIR,pid);
	if(!stat(str,&st)) {
		if(st.st_size!=0 || (ret_code && ret_code!=255)) {
			NEF *V;

			if(W!=NULL && ret_code && ret_code!=255) {
				sprintf(str,pmes(17),ret_code);
				emess(str);
			}
			if(W!=NULL) unscp(W);
			sprintf(str,F_NAMEERR,TMPDIR,pid);
			chmod(str,0400);
			if((V=IfAlreadyUse(str,-1))!=NULL) {
				DelTextFile(V-AllFiles);
			}
			if(InitEdit(str,TEMPWIND)) {
				err=-1; unlink(str);
			} else
				err=1;
		} else
			unlink(str);
	}
	return(err);
}
