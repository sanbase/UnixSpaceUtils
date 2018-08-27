/*
	Copyright 1995,1996,1997 Unitex+, ltd. Kiev, Ukraine
	Written by Oleg Smolyar
	E-mail: oleg@unitex.kiev.ua
*/
#ifndef lint
static const char rcsid[]="$Id: write.c,v 2.2.1.1 1998/07/24 12:58:58 toor Exp toor $";
#endif

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "ned.h"

extern NEF *AllFiles;
extern int LenAllFiles;

/* arg=1 - не перечитывать файл */
int WriteEditFile(NEF *W,int arg)
{
	int err=0;
	FILE *fd=NULL;
	struct stat st;

	if(W->rdonly || !W->AS->red) goto END;
	if(IfAlreadyUse(W->FileName,W-AllFiles))  arg=2;
	init_long_pause(10,pmes(114),1);
	if(!stat(W->FileName,&st)) {
		if(TestRight(st.st_mode,st.st_uid,st.st_gid)) {
			err=-1; goto END;
		}
		if(W==AllFiles+LenAllFiles-1) {
			PutTitleEvent(W,pmes(10)); scrbufout();
		}
		if((fd=fopen(make_bak_name(W->FileName),"w+"))==NULL) {
			err=-1; goto END;
		}
		if(st.st_nlink>1 || chown(make_bak_name(W->FileName),st.st_uid,st.st_gid) ||
		   chmod(make_bak_name(W->FileName),st.st_mode)) {
		   /* если не хватает прав установить изначальные моды
		      доступа или файл имеет >1 hard линка придется
		      копировать с целью сохранения мод доступа файла-образца.
		      Это замедлит процесс записи, но по другому действовать
		      нельзя. В этом случае моды у bak файла будут нарушены! */
			rewind(W->AS->fd);
			while((err=getc((FILE *)W->AS->fd))!=EOF)
				putc(err,fd);
			fclose(W->AS->fd);
			W->AS->fd=fd; W->AS->seekf=-1;
			err=0;
		} else { /* это стандартный случай */
			fclose(fd);
			if(rename(W->FileName,make_bak_name(W->FileName))) {
				err=-1; goto END;
			}
		}
	}
	else {
		st.st_mode=0664; st.st_uid=getuid(); st.st_gid=getgid();
	}
	ClearTail(W);
	if((fd=fopen(W->FileName,"w+"))==NULL) {
		err=-2; goto END;
	}
	block_file(fd);
	if(WriteFile(W,0,NMAX(W),fd)) {
		err=-3; goto END;
	}
	chown(W->FileName,st.st_uid,st.st_gid);
	chmod(W->FileName,st.st_mode);
	ChangeRed(W,0);
	W->AS->seekf=-1;
	if(W->AS->fd!=NULL) fclose((FILE *)W->AS->fd);
	if(!arg || arg==2) {
		FreeUndo(W->AS);
		ResetStrBuf(W->AS);
		rewind(fd);
		W->AS->undo=0;
		ReadFile(W,0,(FILE *)fd,0);
		W->AS->undo=1;
		W->AS->fd=(void *)fd;
	}
	else
		fclose(fd);
	if(arg==2) {
	        /* существуют еще окна на этот файл */
	        for(err=0;err<LenAllFiles;err++)
		        if(W!=AllFiles+err && AllFiles[err].FileName!=NULL &&
			   !strcmp(AllFiles[err].FileName,W->FileName)) {
				AllFiles[err].AS=W->AS;
				ChangeRed(AllFiles+err,0);
			}

	        err=0;
	}
END:
	if(W==AllFiles+LenAllFiles-1)
		ShowTitleEvent(W);
	close_long_pause();
	if(err) {
		char ss[NAMESIZE+1],*ch;

		if(err==-3) {
			fclose(fd); unlink(W->FileName);
		}
		if(err<-1)
			if(access(ch=make_bak_name(W->FileName),F_OK))
				rename(ch,W->FileName);
		sprintf(ss,pmes(27),basen(W->FileName));
		emess(ss);
	}
	return(err);
}

char *make_bak_name(char *name)
{
	static char str[NAMESIZE+1];

	sprintf(str,"%s.b",name);
	return(str);
}

char *make_link_name(char *name)
{
	static char str[NAMESIZE+1];

	sprintf(str,"%s.b",name);
	return(str);
}


int WriteAll(int arg)
{
	int i;

	for(i=0;i<LenAllFiles;i++)
		if(WriteEditFile(AllFiles+i,arg) && arg)
			return(-1);
	return(0);
}
