/*
	Copyright 1995,1996,1997 Unitex+, ltd. Kiev, Ukraine
	Written by Oleg Smolyar
	E-mail: oleg@unitex.kiev.ua
*/
#ifndef lint
static const char rcsid[]="$Id: rwconf.c,v 2.2.1.1 1998/07/24 12:58:58 toor Exp toor $";
#endif
/* Чтение конфигураций; работа с протокола */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include "ned.h"

extern NEF    *AllFiles;
extern int    LenAllFiles;

static int  ReadCnfFile(char *);
static int  SaveConfig(char *,int);
static int  ReadConfig(char *,int);
static void SaveTabConfig(int);
static void ReadTabConfig(int);
static void ReadColourConf(int);
static void SaveColourConf(int);

int SaveS()
{
	char str[LINESIZE+1],*ch;

	if(cmn.UseConf) return(0); /* не создаем конфигурацию */
	if((ch=getenv("TEMP"))!=NULL)
		sprintf(str,"%s/",ch);
	else
		*str=0;
	errno=0;
	sprintf(str+strlen(str),CONFNAME,getlogin(),basen(get_tty()));
	return(SaveConfig(str,0));
}

int SaveP()
{
	char str[LINESIZE+1],*ch;

	if((ch=getenv("TEMP"))!=NULL)
		sprintf(str,"%s/",ch);
	else
		*str=0;
	errno=0;
	sprintf(str+strlen(str),PROTNAME,getlogin(),basen(get_tty()));
	return(SaveConfig(str,1));
}

static int SaveConfig(char *str,int arg)
{
	int fd,i;
	struct stat st;
	char *ch;

	if((fd=creat(str,0640))<0) {
		if(arg) cmn.UseProt=0;
		return(-1);
	}
	lseek(fd,(off_t)4,SEEK_SET);
	for(i=0;i<LenAllFiles;i++) {
		if(AllFiles[i].typ==TEMPWIND || AllFiles[i].typ==HELPWIND ||
		   AllFiles[i].typ==CBWIND || AllFiles[i].typ==DBWIND ||
		   AllFiles[i].typ==MANWIND)
			continue;
		write(fd,AllFiles[i].FileName,Strlen(AllFiles[i].FileName)+1);
		write(fd,&AllFiles[i].EB,sizeof(NEB));
		write(fd,&CX(AllFiles+i),sizeof(int));
		write(fd,&CY(AllFiles+i),sizeof(int));
		write(fd,&GP(AllFiles+i),sizeof(int));
		write(fd,&CS(AllFiles+i),sizeof(int));
		SavePozConf(AllFiles[i].wd,fd);
	}
	fstat(fd,&st);
	if(!st.st_size && !arg) {
		close(fd); unlink(str);
	}
	else
	{
		lseek(fd,(off_t)0,SEEK_SET);
		st.st_size-=4;
		write(fd,(long *)&st.st_size,4);
		if(!arg)
			close(fd);
		else {
			Fdp=fd; lseek(Fdp,(off_t)0,SEEK_END);
		}
	}
	if(!arg) {
		char sss[LINESIZE+1];

		if((ch=getenv("TEMP"))==NULL)
	                if((ch=getenv("HOME"))==NULL) errno=0;
		if(ch!=NULL) {
		        sprintf(sss,"%s/%s.%s",ch,CNFNAME,getlogin());
		        if((fd=creat(sss,0660))>0) {
			        write(fd,C,sizeof C);
			        SaveStecksConf(fd);
			        SaveTabConfig(fd);
	                        SaveColourConf(fd);
			        close(fd);
			}
		}
	}
	return(0);
}

int RestoreS()
{
	char str[LINESIZE+1],*ch;

	if((ch=getenv("TEMP"))!=NULL)
		sprintf(str,"%s/",ch);
	else
		*str=0;
	errno=0;
	sprintf(str+strlen(str),CONFNAME,getlogin(),basen(get_tty()));
	return(ReadConfig(str,0));
}

int RestoreP()
{
	char str[LINESIZE+1],*ch;

	if((ch=getenv("TEMP"))!=NULL)
		sprintf(str,"%s/",ch);
	else
	       *str=0;
	errno=0;
	sprintf(str+strlen(str),PROTNAME,getlogin(),basen(get_tty()));
	return(ReadConfig(str,1));
}

static int ReadConfig(char *str,int arg)
{
	int fd,i,size;
	char *buf;

	if((fd=open(str,O_RDONLY))<0) {
		if(arg) cmn.RestProt=0;
		errno=0;
		return(0);
	}
	read(fd,&size,4);
	if((buf=Malloc(size))==NULL) return(-1);
	read(fd,buf,size);
	if(!arg)
		close(fd);
	else
		Fdp=fd;
	for(i=0;i<size;) {
		int j;

		for(j=0;str[j]=buf[i];j++,i++);
		i++;
		if(*str && !access(str,F_OK)) {
			if(!InitEdit(norm_path(str),NORMWIND)) {
				NEF *W;

				W=AllFiles+LenAllFiles-1;
				memcpy(&W->EB,buf+i,sizeof(NEB));
				i+=sizeof(NEB);
				memcpy(&CX(W),buf+i,sizeof(int));
				i+=sizeof(int);
				memcpy(&CY(W),buf+i,sizeof(int));
				i+=sizeof(int);
				memcpy(&GP(W),buf+i,sizeof(int));
				i+=sizeof(int);
				memcpy(&CS(W),buf+i,sizeof(int));
				i+=sizeof(int);
				W->EB.colorborder=W->rdonly?COLRDPASS:COLPASS;
				W->EB.colortext=W->rdonly?COLRDTEXT:COLTEXT;
				if(CheckBoxParam(W->FileName,W->EB.x,W->EB.y,W->EB.l,W->EB.h))
				{
					W->EB.x=X; W->EB.y=Y;
					W->EB.l=L-1; W->EB.h=H-2;
				}
				CorrXY(W);
				ReadPozConf(W->wd,buf,size,&i);
				OutEditBox(W);
				continue;
			}
		}
		i+=(sizeof(NEB)+4*sizeof(int));
		ReadPozConf(-1,buf,size,&i);

	}
	if(LenAllFiles>0) {
		AllFiles[LenAllFiles-1].EB.colorborder=AllFiles[LenAllFiles-1].rdonly?COLRDACTIV:COLACTIV;
		OutEditBox(AllFiles+LenAllFiles-1);
	}
	Free(buf);
	return(0);
}


void SavePozConf(int wd,int fd)
{
	int i,k;
	MP *s;

	for(i=0,k=wd;FindMemPozStr(k,-1,0)!=NULL;k=-1,i++);
	write(fd,&i,4);
	for(i=0,k=wd;(s=FindMemPozStr(k,-1,0))!=NULL;k=-1,i++) {
		write(fd,&(s->n),4); write(fd,&(s->c),1);
	}
}

void ReadPozConf(int wd,char *buf,int size,int *k)
{
	int i,j;

	memcpy(&i,buf+(*k),sizeof(int));
	for(j=0;wd>=0 && j<i && *k+j*(sizeof(int)+1)<size;j++)
		PutMemPoz(wd,*(int *)(buf+(*k)+j*(sizeof(int)+1)+sizeof(int)),*(buf+*k+j*(sizeof(int)+1)+2*sizeof(int)));
	(*k)+=(i*(sizeof(int)+1)+sizeof(int));
}

#ifdef  BUG_GETLOGIN

char *getlogin()
{
	return(cuserid(NULL));
}

#endif

char *get_tty()
{
	char *ch;

#ifdef X11W
	return("xterm");
#endif
	if((ch=ttyname(0))==NULL) {
		if((ch=ttyname(1))==NULL) {
			return("tty??");
		}
	}
	errno=0;
	return(basen(ch));
}

#ifdef X11W
unsigned int C[MAXCOLOR];
#else
unsigned char C[MAXCOLOR];
#endif

void GetColors()
{
	char str[LINESIZE+1],*ch;

	if((ch=getenv("TEMP"))==NULL) ch=getenv("HOME");
	errno=0;
#ifdef X11W
	bzero(C,sizeof(short)*MAXCOLOR);
#else
	bzero(C,MAXCOLOR);
#endif
	color_defa();
	if(ch!=NULL) {
		sprintf(str,"%s/%s.%s",ch,CNFNAME,getlogin());
		ReadCnfFile(str);
	}
}

static int ReadCnfFile(char *name)
{
	int fd;

	if((fd=open(name,O_RDONLY))<0) {
		errno=0; return(-1);
	}
	read(fd,C,sizeof C);
	ReadStecksConf(fd);
	ReadTabConfig(fd);
	ReadColourConf(fd);
	close(fd);
	return(0);
}

void  color_defa()
{
	C[0]=0x8a; /* рамка окна при изменении габаритов                  */
	C[1]=0x0a; /* рамка только читаемого окна при изменении габаритов */
	C[2]=0x8e; /* рамка текущего окна                                 */
	C[3]=0x0e; /* рамка текущего только читаемого окна                */
	C[4]=0x87; /* рамка неактивного окна                              */
	C[5]=0x07; /* рамка неактивного только читаемого окна             */
	C[7]=0x03; /* текст в только читаемом окне                        */
	C[8]=0x82; /* фон под окнами                                      */
#ifdef X11W
	C[6]=0x482; /* текст в окне                                        */
	C[9]=0x442e;/* рабочее меню                                        */
	C[10]=0x1e0;/* текущая строка в рабочих меню                       */
	C[11]=0x170;/* непечатный символ                                   */
	C[14]=0x53a;/* ... и что при этом будет                            */
	C[12]=0x53a;/* обозначение действия внизу окна                     */
#else
	C[6]=0x6a; /* текст в окне                                        */
	C[9]=0x70; /* рабочее меню                                        */
	C[10]=0xf0;/* текущая строка в рабочих меню                       */
	C[11]=0x70;/* непечатный символ                                   */
	C[14]=0x3a;/* ... и что при этом будет                            */
	C[12]=0x70;/* обозначение действия внизу окна                     */
#endif
	C[13]=0x07;/* что нажимать в строке статуса ...                   */
	C[15]=0x0f;/* строчный строковый редактор                         */
#ifdef X11W
	C[16]=0x51a;/* каталог в меню выбора файлов                        */
	C[17]=0x442a;/* файл в меню выбора файлов                           */
	C[23]=0x513;/* символическая ссылка на каталог                     */
	C[24]=0x461e;/* символическая ссылка на файл                       */
	C[19]=0x53a;/* строка пояснения при выборе из меню                 */
	C[25]=0x24e;/* ожидание при выборе другой директории               */
	C[26]=0x01ef;/* подсвеченные буквы в главном меню                    */
#else
	C[16]=0x1a; /* каталог в меню выбора файлов                         */
	C[17]=0x70;/* файл в меню выбора файлов                            */
	C[23]=0x13;/* символическая ссылка на каталог                      */
	C[24]=0x0e;/* символическая ссылка на файл                         */
	C[19]=0x3a;/* строка пояснения при выборе из меню                  */
	C[25]=0x4e;/* ожидание при выборе другой директории               */
	C[26]=0x74;/* подсвеченные буквы в главном меню                    */
#endif
	C[18]=0x0b;/* сообщение об ошибках                                 */
	C[20]=0x70;/* блок при пометке                                     */
	C[21]=0x0b;/* стек                                                 */
	C[22]=0xe0;/* текущая строка в стеке                               */
	C[27]=0x6e;/* операторы                                            */
	C[28]=0x6f;/* препроцессор                                         */
	C[29]=0x6f;/* типы                                                 */
	C[30]=0x6f;/* классы памяти                                        */
	C[31]=0x6e;/* названия функций                                     */
	C[32]=0x6b;/* фигурные скобки                                      */
	C[33]=0x6b;/* круглые скобки                                       */
	C[34]=0x6b;/* комментарии                                          */
	C[35]=0x6b;/* русские буквы                                        */
	C[36]=0x6f;/* кавычки                                              */
	C[37]=0x78;/* неактивный пункт меню                                */
}

static void SaveTabConfig(int fd)
{
	write(fd,&SCRTAB,1);
	write(fd,&POZTAB,1);
}

static void ReadTabConfig(int fd)
{
	read(fd,&SCRTAB,1);
	if(SCRTAB<=0 || SCRTAB>8) SCRTAB=8;
	read(fd,&POZTAB,1);
	if(POZTAB<=0 || POZTAB>8) POZTAB=8;
}

static void ReadColourConf(int fd)
{
	char c=1;

	read(fd,&c,1);
	cmn.UseColour=(c!=0);
}

static void SaveColourConf(int fd)
{
	char c=cmn.UseColour;

	write(fd,&c,1);
}

static int fdp_std=0;

/* Временно отказаться от записи в протокола */
void TmpUnsetProt()
{
	if(cmn.UseProt && !fdp_std) { fdp_std=Fdp;Fdp=0; }
}

/* Восстановить запись в протокол */
void TmpRestProt()
{
	if(cmn.UseProt && fdp_std) { Fdp=fdp_std; fdp_std=0; }
}


void WriteInProt(int c,char *s)
{
	if(cmn.UseProt && Fdp) {
	        if(c>=0) write(Fdp,&c,4);
		if(s!=NULL) write(Fdp,s,strlen(s)+1);
	}
	return;
}

int ReadComInProt(char *buf)
{
	int ret=-1;

	if(cmn.RestProt)
		if(buf==NULL) { /* читаем команду */
			if(read(Fdp,&ret,4)<0) {
				errno=0; ret=-1;
			}
		} else { /* читаем строку */
			ret=-1;
			do {
				ret++;
				if(read(Fdp,buf+ret,1)<0) {
					errno=0; ret=-1; break;
				}
			}
			while(buf[ret]);
			if(ret>=0) ret=0;
		}
	if(ret<0) CloseRestProt();
	return(ret);
}

void CloseRestProt()
{
	close(Fdp); cmn.RestProt=0; SaveP();
}

void CloseUseProt()
{
	close(Fdp); cmn.UseProt=0; SaveP();
}
