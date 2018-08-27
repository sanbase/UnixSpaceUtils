/*
	Copyright 1995,1996,1997 Unitex+, ltd. Kiev, Ukraine
	Written by Oleg Smolyar
	E-mail: oleg@unitex.kiev.ua
*/
#ifndef lint
static const char rcsid[]="$Id: low.c,v 2.2.1.1 1998/07/24 12:58:58 toor Exp toor $";
#endif
/*
 *     ���� raw �஢��� ��ࠡ�⪨ 䠩���
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "ned.h"

static FILE *Fdb=NULL;       /* ���ਯ�� ���୮�� 䠩��                 */
static int seek_file=0;      /* ⥪��� ������ � ���୮� 䠩��           */
static int LenCash=0;        /* ��ப  � ���                              */
static off_t LenBufFile=0;   /* ⥪��� ����� ���. 䠩�� ����� � ��襬    */

#define CSIZE 24             /* ���� ��� ��ப � ���                     */

/* ������� ��� �� ������ ��� ���୮�� 䠩�� */
static struct scash {
	int curseek;        /* ���� ��ப� � ���. 䠩��                  */
	int oldseek;        /* ���� ���� ��ப�                        */
	STR *S;             /* ��ப�                                     */
} *Cash[CSIZE];

/* ���� ��� � 䠩� */
int FlushCash()
{
	int i;

	fseek(Fdb,0,SEEK_END);
	for(i=0;i<LenCash;i++) {
		if(fwrite(&Cash[i]->oldseek,sizeof(long),1,Fdb)!=1 ||
		   fwrite(&Cash[i]->S->rlen,sizeof(int),1,Fdb)!=1 ||
		   fwrite(Cash[i]->S->s,1,Cash[i]->S->rlen,Fdb)!=Cash[i]->S->rlen) {
			i=-1; emess(pmes(124)); goto END;
		}
		FreeSTR(Cash[i]->S);
		Free(Cash[i]);
	}
	LenCash=0; i=0;
	seek_file=LenBufFile;
END:
	fflush(Fdb);
	return(i);
}

/* ���������� STR � ��� */
static int PutCash(STR *S,int oldseek)
{
	int i;

	if(LenCash==CSIZE)  FlushCash();
	if((Cash[LenCash]=(struct scash *)Malloc(sizeof(struct scash)))==NULL)
		return(-1);
	Cash[LenCash]->curseek=(i=LenBufFile+1);
	Cash[LenCash]->oldseek=oldseek;
	if((Cash[LenCash]->S=DupSTR(S))==NULL) return(-1);
	LenBufFile+=S->rlen+sizeof(long)+sizeof(int);
	LenCash++;
	return(i);
}

/* �⥭�� STR �� ��� */
static STR *GetCash(long seek,long *oldseek)
{
	int i;

	if(seek>0) {
		for(i=0;i<LenCash;i++) {
			if(seek==Cash[i]->curseek) {
				if(oldseek!=NULL) *oldseek=Cash[i]->oldseek;
				return(Cash[i]->S);
			}
		}
	}
	return(NULL);
}

/* ��ନ஢���� ����� ���୮�� 䠩�� */
static char *CreatNameBufFile()
{
	static char str[LINESIZE+1],*ch;


	if((ch=getenv("TEMP"))!=NULL)
		sprintf(str,"%s/",ch);
	else
		*str=0;
	sprintf(str+strlen(str),BUFNAME,getlogin(),basen(get_tty()));
	return(str);
}

/* ����⨥ � �����஢���� ���୮�� 䠩�� */
static int OpenBufFile()
{
	if((Fdb=fopen(CreatNameBufFile(),"w+"))==NULL || block_file(Fdb))
	        return(-1);
	return(0);
}

/* �⥭�� ��ப� �� 䠩�� ��ࠧ� */
static int ReadSTRFile(NSB *W,STR *S,long seek)
{
	register int ch;
	int err=0;

	if(W->fd==NULL) {
		err=-1; S->rlen=0; goto END;
	}
	seek=-seek-1;
	/* fseek ��뢠�� ��� ���� � 㢥��稢���
	   �६� �⥭�� �� ���冷�, ���⮬� ����
	   ��᫥����⥫쭮 ���� ᨬ���� �� ����
	   �� ���⨦���� �㦭�� ����樨 (����� ��
	   ��������). � ⮦� �६�, �᫨ ᫥���饥
	   ���饭�� 㦥 � �� �������� � ⥪�騩
	   ����, ���� ����樮��஢�����.
	   ��᫥���� �ࠢ����� � �����筮� ���
	   ���筮, �� ��㣮�� ᯮᮡ� ��।�����,
	   �������� �� �� � ����, ��� */
	if(W->seekf<0 || W->seekf>seek || seek-W->seekf>BUFSIZ) {
		fseek(W->fd,W->seekf=seek,SEEK_SET);
	}
	else {
		/* �⠥� getc ᨬ���� �� 䠩��, ����
		   �� ���⨣��� �ॡ㥬�� ����樨 */
		for(;W->seekf<seek;W->seekf++)
			if(getc((FILE *)W->fd)==EOF) {
			/* ������ ��㭤� - ��������� �� ���殬 䠩��*/
				err=-1; S->rlen=0;goto END;
			}
	}
	/* ��⠥� �ॡ㥬�� ��ப� */
	for(S->rlen=0;;S->rlen++) {
		W->seekf++;
		switch(ch=getc((FILE*)W->fd)) {
		case EOF:
		case '\n':
			goto END;
		case 0:
			/* �㫨 ��� �� ������, ���� ��
			   ��-� ��������. ��蠥� �,
			   �� � ��ଠ�쭮� ⥪�⮢��
			   䠩�� �㫥��� ᨬ����� �� �㤥� */
			ch=255;
		}
		AddSTR(S,S->rlen+1);
		S->s[S->rlen]=(char)ch;
	}
END:
	AddSTR(S,S->rlen+1);
	S->s[S->rlen]=0;
	return(err);
}

/* �⥭�� ��ப� �� ���୮�� 䠩�� */
static int ReadSTRBufFile(FILE *fd,STR *S,long seek,long *oldseek)
{
	register int ch;
	int err=0;
	STR *chh;

	if(fd==NULL) {
		err=-1;
		if(S!=NULL) S->rlen=0;
		goto END;
	}
	if((chh=GetCash(seek,oldseek))!=NULL) {
		if(oldseek==NULL) {
			AddSTR(S,(S->rlen=chh->rlen)+1);
			strcpy(S->s,chh->s);
		}
		goto END;
	}
	seek--;
	if(seek_file>seek || seek-seek_file>BUFSIZ) {
		fseek(fd,seek_file=seek,SEEK_SET);
	}
	else {
		for(;seek_file<seek;seek_file++)
			if(getc(fd)==EOF) {
				err=-1;
				if(S!=NULL) S->rlen=0;
				goto END;
			}
	}
	if(fread(&err,sizeof(long),1,fd)!=1) {
		err=-1;
		if(S!=NULL) S->rlen=0;
		goto END;
	}
	seek_file+=sizeof(long);
	if(oldseek!=NULL) {
		*oldseek=err; err=0; goto END;
	}
	if(fread(&S->rlen,sizeof(int),1,fd)!=1) {
		err=-1; S->rlen=0; goto END;
	}
	AddSTR(S,S->rlen+1);
	if(S->rlen && fread(S->s,1,S->rlen,fd)!=S->rlen) {
		err=-1; S->rlen=0; goto END;
	}
	seek_file+=S->rlen+sizeof(int);
	err=0;
END:
	if(S!=NULL) S->s[S->rlen]=0;
	return(err);
}

/* ������ ��ப� � ����� 䠩� */
static int WriteSTRFile(STR *S,long *seek)
{
	return(*seek=PutCash(S,*seek));
}

/* ���뢠�� ���� �� 䠩��-��ࠧ� ���� �� ����� ��ப� �
   �����頥� �� � ���� STR */
static STR *ReadSTR(register NSB *W,int i)
{
	static STR  *S=NULL;

	if(S==NULL) S=InitSTR();
	if(i>=BMAX(W) || !W->Buf[i]) {
		AddSTR(S,1); S->s[S->rlen=0]=0;
	}
	else
		if(W->Buf[i]>0) {
			/* �⠥� ��ப� �� ���୮�� 䠩�� */
			if(ReadSTRBufFile(Fdb,S,W->Buf[i],NULL)) emess(pmes(30));
		}
		else
			/* �⠥� ��ப� �� 䠩�� ��ࠧ� */
			if(ReadSTRFile(W,S,W->Buf[i])) emess(pmes(115));
	return(S);
}

/* �����⪠ ��ப� - 㡮ઠ ⠡��権 � �஡���� � ����*/
static STR *Dechar(register STR *S,int arg)
{
	register int i;
	int j;

	for(j=i=0;S->s[i];i++) {
		if(S->s[i]=='\t') {
			/* �����塞 ⠡��樨 �� �஡��� */
			AddSTR(S,S->rlen+dost(i,SCRTAB));
			memmove(S->s+dnext(i,SCRTAB),S->s+i+1,S->rlen-i);
			memset(S->s+i,' ',dost(i,SCRTAB));
			S->rlen+=dost(i,SCRTAB)-1;
			i+=dost(i,SCRTAB)-1;
		}
		else
			/* �᫨ arg!=0 㭨�⮦��� ᯥ���᪨� ���
			   man ᨬ���� */
			if(arg && i && S->s[i+1] && S->s[i]==SPECMANCODE1) {
				memcpy(S->s+i-1,S->s+i+1,S->rlen-i);
				S->rlen-=2;
				i--;
			}
		if(S->s[i]!=' ') j=i;
	}
	AddSTR(S,i+1);
	S->s[S->rlen=min(i,j+1)]=0;
	return(S);
}

/* �����頥� STR ��� 㭨�⮦���� SPECMACODE1 � SPECMACODE2 */
STR *GetSTR(NSB *W,int i)
{
	return(Dechar(ReadSTR(W,i),0));
}

/* STR � 㭨�⮦����� SPECMANCODE1 � SPECMANCODE2 */
STR *GetMSTR(NSB *W,int i)
{
	return(Dechar(ReadSTR(W,i),1));
}

/* ����稢��� ��ப� �� �ନ���쭮� ��� � 䠩����� (� ��⮬
   �।��饣� ���ﭨ� ��ப�) � �����뢠�� �� */
int PutSTR(NSB *W,int i,STR *S)
{
	int t,err=0;
	char *ch;

	InsertHistory(W,0,0,U_BEGIN);
	if(S==NULL && i<BMAX(W)) {
		W->Buf[i]=0; goto END;
	}
	if(i>=BMAX(W)) InsertStrInBuf(W,i-BMAX(W)+1,BMAX(W));
	ch=Malloc(S->rlen+1);
	strcpy(ch,S->s);
	if((S=ReadSTR(W,i))==NULL) {
		err=-1; goto END;
	}
	for(t=S->rlen=0;ch && ch[t];t++,S->rlen++) {
		if(S->s[S->rlen]==ch[t]) continue;
		if(!err && S->s[S->rlen]=='\t') {
			int k;

			for(k=t;ch[k]==' ' && k<dnext(t,SCRTAB);k++);
			if(k>=dnext(t,SCRTAB)) {
				t=k-1; continue;
			}
		}
		err=1;
		AddSTR(S,S->rlen+1);
		S->s[S->rlen]=ch[t];
	}
	AddSTR(S,S->rlen+1);
	S->s[S->rlen]=0;
	Free(ch);
	/* ��砫�� �஡��� �����塞 �� ⠡��樨 */
	for(t=0;t<S->rlen && (S->s[t]==' ' || S->s[t]=='\t');t++)
		if(!memcmp(S->s+t,"        ",SCRTAB)) {
			memcpy(S->s+t+1,S->s+t+SCRTAB,S->rlen-t); S->s[t]='\t'; S->rlen-=SCRTAB-1;
		}
	err=0;
	if(Fdb==NULL && OpenBufFile()) {
		emess(pmes(117)); err=-1; goto END;
	}
	WriteSTRFile(S,&W->Buf[i]);
END:
	InsertHistory(W,i,i,U_CHANGE);
	InsertHistory(W,0,0,U_END);
	return(err);
}

/* ��堭��� ��� ��ࠡ�⪨ �������᪨� ��ப */
STR *InitSTR()
{
	STR *s;

	s=(STR *)Malloc(sizeof(STR)); bzero(s,sizeof(STR));
	return(s);
}

void FreeSTR(STR *sd)
{
	if(sd!=NULL) {
	        Free(sd->s); Free(sd);
	}
}

/* �㡫���� STR */
STR *DupSTR(STR *S)
{
	STR *S1=NULL;

	if(S!=NULL && (S1=InitSTR())!=NULL) {
	        AddSTR(S1,S->len);
	        memcpy(S1->s,S->s,S->rlen+1);
	        S1->rlen=S->rlen;
	}
	return(S1);
}

int AddMemSTR(STR *S,int l)
{
	if(l>S->len)
	        if((S->s=Realloc(S->s,S->len=dnext(l,80)))==NULL)
			return(-1);
	return(0);
}

/* ������� STR �� ���筮� ��ப� */
STR *CreatSTR(char *s)
{
	STR *S;

	if((S=InitSTR())!=NULL) {
		AddSTR(S,(S->rlen=Strlen(s))+1);
		strcpy(S->s,s);
	}
	return(S);
}

/* ��������� ࠧ��஢ ����, � ���஬� �����뢠���� ��ப� */
int ControlMemoryBuffer(NSB *W,int num)
{
	int err=0;

	if(BMAX(W)+num>W->MaxLen) {
		long *Atmp;

		err=dnext((BMAX(W)+num),256);
		if((Atmp=(long *)Realloc(W->Buf,err*sizeof(long)))==NULL) {
			err=-1; goto END;
		}
		W->Buf=Atmp; W->MaxLen=err; err=0;
	}
	BMAX(W)+=num;
END:
	return(err);
}

/* �������� ���� ��� ����� ��ப� � ���� ��ப */
int InsertStrInBuf(register NSB *W,int num,int i)
{

	register int j;

	j=BMAX(W);
	if(ControlMemoryBuffer(W,num)<0) return(-1);
	if(i<j) memmove(W->Buf+i+num,W->Buf+i,(j-i)*sizeof(long));
	for(j=0;j<num;j++) {
		if(W->undo && !j) InsertHistory(W,i,i+num-1,U_INSERT);
		W->Buf[i+j]=0;
	}
	return(0);
}

/* �������� ��ப� �� ���� ��ப */
void DeleteStrInBuf(register NSB *W,int num,int i)
{
	int j;
	long seek;

	if((j=min(BMAX(W)-i,num))>0) {
		if(W->undo) {
			STR *S=InitSTR();
			int t;

			InsertHistory(W,0,0,U_BEGIN);
			AddSTR(S,1); S->s[0]=0;
			for(t=i;t<i+j;t++)
				if(t==i)
					seek=WriteSTRFile(S,W->Buf+t);
				else
					WriteSTRFile(S,W->Buf+t);
			FreeSTR(S);
		}
		memcpy(W->Buf+i,W->Buf+i+j,(BMAX(W)-i-j)*sizeof(long));
		ControlMemoryBuffer(W,-j);
		if(W->undo) {
			InsertHistory(W,seek,0,U_SPEC);
			InsertHistory(W,i,i+j-1,U_DELETE);
			InsertHistory(W,0,0,U_END);
		}
	}
}

/* ������ 㭨�⮦���� ���� ��ப � ����易���� � ���� ��ப � ����� */
void ResetStrBuf(NSB *W)
{
	if(BMAX(W)) {
		Free(W->Buf); W->Buf=NULL; W->MaxLen=0; BMAX(W)=0;
	}
}

/* �⥭�� 䠩�� - �筥�, ����筮, ࠧ��⪠ � ����������� ����権,
   � ������ ��稭����� ��ப�. �᪫�祭��� ���� �⥭�� �� ������ -
   ��室���� ���� �ࠧ� � ����� 䠩�. */
int ReadFile(NEF *W,int begstr,FILE *fd,int arg)
{
	int err=0,ch,i=0,j=0,k=0;
	STR *S=NULL;
	errno=0;

	if(arg) S=InitSTR();
	do {
		if((ch=getc(fd))==EOF) {
			if(i==k) break;
			ch='\n';
		}
		if(ch=='\n') {
			draw_b();
			if(InsertStrInBuf(W->AS,1,begstr+j)) {
				err=-1; goto END;
			}
			if(k==i) W->AS->Buf[begstr+j]=0;
			else {
				if(!arg)
					W->AS->Buf[begstr+j]=-k-1;
				else {
					AddSTR(S,i-k+1);
					S->s[S->rlen=i-k]=0;
					PutSTR(W->AS,begstr+j,S);
				}
			}
			j++; k=i+1;
		}
		else
			if(arg) {
				AddSTR(S,i-k+1); S->s[i-k]=(char)ch;
			}
		i++;
	}
	while(!feof(fd));
END:
	if(arg) FreeSTR(S);
	if(ch==EOF && errno) emess(pmes(122));
	MoveMemPoz(W->wd,begstr,j);
	return(err);
}


/* ������ 䠩��, ��୥� ᫨ﭨ� �� ��ப, ����� ࠧ��饭� � 䠩��-����
   � ��ப �� 䠩��-��ࠧ� � �����뢠��� 䠩� */
int WriteFile(NEF *W,int begstr,int nstr,FILE *fd)
{
	int i,err=0;

	for(i=begstr;i<begstr+nstr && i<NMAX(W);i++) {
		STR *S;

		S=ReadSTR(W->AS,i);
		if(fwrite(S->s,1,S->rlen,fd)!=S->rlen || putc('\n',fd)==EOF) {
			err=-1; emess(pmes(124)); break;
		}
		draw_b();
	}
	return(err);
}

/* �������� ���୮�� 䠩�� � ���� ࠡ��� */
void DelBufFile()
{
	if(Fdb!=NULL) {
		fclose(Fdb); unlink(CreatNameBufFile());
	}
}

/*void BufToBuf(NSB *T,int t,NSB *S,int s)
{
	if(S->Buf==NULL || BMAX(S)<=s) {
		T->Buf[t]=0; return;
	}
	if(S->Buf[s]>=0) {
		T->Buf[t]=S->Buf[s];
	} else {
		PutSTR(T,t,GetSTR(S,s));
	}
} */

/* ��⠥� i-� ᨬ��� �� ��ப� j ���ਯ�� W */
int GetCh(NEF *W,int i,int j)
{
	if(j<0 || j>=NMAX(W)) return(' ');
	return(GetChSTR(GetMSTR(W->AS,j),i));
}

/* ���� j-� ᨬ��� � ��ப� i ���ਯ�� W � ��⮬ ०���
   ��⠢��-������ */
int PutCh(NEF *W,int i,int j,int c,int arg)
{
	if(j<0 || j>=NMAX(W)) return(-1);
	return(PutChSTR(GetMSTR(W->AS,i),j,c,arg));
}

/* ����뢠�� j-� ᨬ��� �� STR */
int GetChSTR(STR *S,int j)
{
	if(S==NULL || j<0 || S->rlen<=j)
		return(' ');
	return((unsigned char)S->s[j]);
}

/* ���� j-� ᨬ��� � STR � ��⮬ ०��� ��⠢��-������ */
int PutChSTR(STR *S,int j,int c,int arg)
{
	if(S==NULL || j<0) return(-1);
	if(S->rlen<=j) {
		if(c!=' ') {
			AddSTR(S,j+2);
			memset(S->s+S->rlen,' ',j-S->rlen);
			S->s[S->rlen=j+1]=0;
		}
		else
			return(0);
	} else {
		 if(arg) {
			 register int i;

			 AddSTR(S,++(S->rlen)+1);
			 for(i=S->rlen;i>j;i--)
				 S->s[i]=S->s[i-1];
		 }
	}
	S->s[j]=c;
	return(0);
}

int UndoOne(NEF *W,int i)
{
	int j,err=0;

	W->AS->undo=0;
	switch(W->AS->U[i].mode&U_MASK) {
	case U_CHANGE:
		for(j=W->AS->U[i].addr1;j<=W->AS->U[i].addr2;j++)
			if(W->AS->Buf[j]>0 && ReadSTRBufFile(Fdb,NULL,W->AS->Buf[j],W->AS->Buf+j)) {
				emess(pmes(30)); err=-1; break;
			}
		break;
	case U_INSERT:
		DeleteStrInBuf(W->AS,W->AS->U[i].addr2-W->AS->U[i].addr1+1,
			       W->AS->U[i].addr1);
		break;
	case U_DELETE:
		InsertStrInBuf(W->AS,W->AS->U[i].addr2-W->AS->U[i].addr1+1,
			       W->AS->U[i].addr1);
		break;
	case U_SPEC: /* �����, ⮫쪮 �� �뫠 U_DELETE */
		for(err=W->AS->U[i].addr1,j=W->AS->U[i+1].addr1;
			j<=W->AS->U[i+1].addr2;j++,err+=sizeof(long)+sizeof(int)) {
			if(ReadSTRBufFile(Fdb,NULL,err,W->AS->Buf+j)) {
				emess(pmes(30)); err=-1; break;
			}
		}
		err=0;
		break;
	default:
		break;
	}
	W->AS->undo=1;
	return(err);
}


#ifdef DEBUG

static int flag_one=0;

#endif

int InsertHistory(NSB *W,int addr1,int addr2,int mode)
{
	int err=0;
#ifndef DEBUG
	static int flag_one=0;
#endif
	if(!W->undo) goto END;
	if(mode&U_BEGIN) {
		if(flag_one)
			mode&=(~U_BEGIN);
		flag_one++;
		if(!mode) goto END;
	}
	if(W->CurU && W->U[W->CurU-1].mode==U_BEGIN) {
		if(mode==U_END) {
			W->CurU--; goto END;
		}
		W->U[W->CurU-1].mode|=mode;
		W->U[W->CurU-1].addr1=addr1;
		W->U[W->CurU-1].addr2=addr2;
		goto END;
	} else {

		if(W->CurU && flag_one &&
		  (W->U[W->CurU-1].mode&U_MASK)==(mode&U_MASK) &&
		  addr1==W->U[W->CurU-1].addr2+1) {
			W->U[W->CurU-1].addr2=addr2; goto END;
		} else
			if(mode==U_END && W->CurU) {
				flag_one--;
				if(!flag_one)
					W->U[W->CurU-1].mode|=U_END;
				goto END;
			}
	}
	if(W->NumU==W->MemU) {
		if((W->U=(UN *)realloc(W->U,(W->MemU=dnext(W->CurU+1,16))*sizeof(UN)))==NULL) {
			err=-1; goto END;
		}
	}
	if(W->NumU==W->CurU) W->NumU++;
	W->U[W->CurU].addr1=addr1; W->U[W->CurU].addr2=addr2;
	W->U[W->CurU].mode=mode;
	W->CurU++;
END:
#ifdef DEBUG
	debug_U(W,"-1-");
#endif
	return(err);
}

#ifdef DEBUG

char *get_undo_mode(int mode)
{
	static char str[100];
	int i;

	*str=0;
	for(i=0;i<6;i++)
		switch(mode&(1<<i)) {
		case U_CHANGE:
			strcat(str,"U_CHANGE|"); break;
		case U_DELETE:
			strcat(str,"U_DELETE|"); break;
		case U_INSERT:
			strcat(str,"U_INSERT|"); break;
		case U_SPEC:
			strcat(str,"U_SPEC|"); break;
		case U_BEGIN:
			strcat(str,"U_BEGIN|"); break;
		case U_END:
			strcat(str,"U_END|"); break;
		default:
			if(mode&(1<<i))
				strcat(str,"UNKNOWN|"); break;
		}
	i=strlen(str);
	if(i>0 && str[i-1]=='|')
		str[i-1]=0;
	return(str);
}

debug_U(NSB *W,char *str)
{
	static FILE *fdd=NULL;
	int i;

	if(fdd==NULL)
		fdd=fopen("_debug","w+");
	fprintf(fdd,"flag_one=%d str=%s\nmem=%d num=%d cur=%d\n",flag_one,str,W->MemU,W->NumU,W->CurU);
	for(i=0;i<W->NumU;i++)
		fprintf(fdd,"addr1=%d addr2=%d mode=%s\n",W->U[i].addr1,W->U[i].addr2,
			 get_undo_mode(W->U[i].mode));
	fprintf(fdd,"\n");
	fflush(fdd);
}
#endif

int Undo(NEF *W)
{
	int i,err=0;

	if(!W->AS->CurU) return(0);
	for(i=W->AS->CurU-1;i>=0;i--) {
		if(!err && W->AS->U[i].mode&U_END) err=1;
		UndoOne(W,i);
		if(!err || W->AS->U[i].mode&U_BEGIN) {
			if(W->AS->U[i].mode&U_SPEC)
				CS(W)=W->AS->U[i+1].addr1;
			else
				CS(W)=W->AS->U[i].addr1;
			if(CS(W)<MAXY(W))
				CY(W)=CS(W);
			else
				CY(W)=(MAXY(W)>>1);
			break;
		}
	}
	if(!(W->AS->CurU=i)) ChangeRed(W,0);
	W->flscr=1; err=0;
	return(err);
}
