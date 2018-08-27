/*
	Copyright 1995,1996,1997 Unitex+, ltd. Kiev, Ukraine
	Written by Oleg Smolyar
	E-mail: oleg@unitex.kiev.ua
*/
#ifndef lint
static const char rcsid[]="$Id: operation.c,v 2.2.1.1 1998/07/24 12:58:58 toor Exp toor $";
#endif

/*
 *  Общие операции над файлом
*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <tty_codes.h>
#include "ned.h"

extern NEF *AllFiles;
extern int LenAllFiles;

//#define X11

/* Склеивает текущую и следующую за ней строку в текущей позиции */
int StickStrInBuf(NEF *W,int nstr,int npoz)
{
	int err=0;
	STR *S1,*S2;

	InsertHistory(W->AS,0,0,U_BEGIN);
	if(nstr<NMAX(W)) {
		S1=DupSTR(GetSTR(W->AS,nstr));
		S2=GetMSTR(W->AS,nstr+1);
		if(!S1->rlen) {
			if(S2->rlen) {
				AddSTR(S1,S2->rlen+npoz+1);
				memset(S1->s,' ',npoz);
				err=(RP(W)>=0?RP(W):0);
				memcpy(S1->s+npoz,S2->s+err,S2->rlen+1-err);
				S1->rlen=npoz+S2->rlen-err;
				err=0;
			}
		}
		else {
			if(S2->rlen) {
				AddSTR(S1,max(S1->rlen,npoz)+S2->rlen+1);
				if(S1->rlen<npoz)
					memset(S1->s+S1->rlen,' ',npoz-S1->rlen);
				err=RP(W)>=0?RP(W):(W->af1 || W->af2 || W->af3)?0:NumBegSpace(S2->s);
				memcpy(S1->s+npoz,S2->s+err,S2->rlen+1-err);
				S1->rlen=npoz+S2->rlen-err;
				err=0;
			}
			else {
				if(S2->rlen>npoz) {
					S1->s[npoz]=0; S1->rlen=npoz;
				}
			}
		}
		PutSTR(W->AS,nstr,S1);
	        FreeSTR(S1);
		W->flscr=1;
		ChangeRed(W,1);
		MoveMemPoz(W->wd,nstr,-1);
		DeleteStrInBuf(W->AS,1,nstr+1);
	}
	InsertHistory(W->AS,0,0,U_END);
	return(err);
}

/* Разбивает строку nstr, poz - где "ломать" */
int BreakStrInBuf(NEF *W,int nstr,int poz)
{
	int err=0,l;
	STR *S1,*S2=NULL;

	InsertHistory(W->AS,0,0,U_BEGIN);
	if(nstr<NMAX(W)) {
		if((S1=GetMSTR(W->AS,nstr))==NULL) {
			err=-1; goto END;
		}
		if(nstr==NMAX(W)-1 && poz>=S1->rlen &&
		   !W->af1 && !W->af2 && !W->af3) {
			PutEmptyLineToEnd(W); goto END;
		}
		if((S2=InitSTR())==NULL) {
			err=-1; goto END;
		}
		MoveMemPoz(W->wd,nstr+1,1);
		if(InsertStrInBuf(W->AS,1,nstr+1)) {
			err=-1; goto END;
		}
		if(S1->s[0] && poz<S1->rlen) {
			AddSTR(S2,(err=(l=(RP(W)>=0?RP(W):
			    (W->af1 || W->af2 || W->af3?0:NumBegSpace(S1->s))))+
			    (S1->rlen-poz)+1));
			memset(S2->s,' ',l);
			for(err=poz;S1->s[err]==' ';err++);
			if(!S1->s[err]) {
				S2->s[0]=0; S2->rlen=0;
			} else {
				strcpy(S2->s+l,S1->s+err);
				S2->rlen=l+S1->rlen-err;
			}
			err=0;
			S1->s[S1->rlen=poz]=(char)0;
			ClearEndSTR(S1);
			PutSTR(W->AS,nstr,S1);
			PutSTR(W->AS,nstr+1,S2);
			FreeSTR(S2);
		}
		W->flscr=1;
		ChangeRed(W,1);
	}
END:
	if(err) FreeSTR(S2);
	InsertHistory(W->AS,0,0,U_END);
	return(err);
}

/* Реализует изменение положения и размера текущего окна */
int MoveSizeWind(NEF *W)
{
	NEB eb;
	int c=C_MSWIND;

	W->EB.colorborder=W->rdonly?COLRDMSW:COLMSW;
	while(1) {
		line_status(0);
		line_status(40);
		if(c!=C_MSWIND) c=ReadCommand(W,-1);
		memcpy(&eb,&W->EB,sizeof(NEB));
		switch(c) {
		case C_STRLEFT:
			if(W->move)
				eb.x--;
			else
				eb.l--;
			break;
		case C_STRRIGHT:
			if(W->move)
				eb.x++;
			else
				eb.l++;
			break;
		case C_UP:
			if(W->move)
				eb.y--;
			else
				eb.h--;
			break;
		case C_DOWN:
			if(W->move)
				eb.y++;
			else
				eb.h++;
			break;
		case C_STOP:
		case C_RETURN:
QUIT:
	                W->EB.colorborder=W->rdonly?COLRDACTIV:COLACTIV;
			W->move=W->size=0;
			OutEditBox(W);
			return(0);
#ifdef X11
		case C_MOUSE:
			if(ev.b==3) goto QUIT;
			if(!IF_BO(W,ev.x,ev.y)) continue;
#endif /* X11 */
		case C_MSWIND:
		case C_SELNFILE:
			if(W->size) {
				W->size=0; W->move=1;
				ShowTitleEvent(W);;
			} else {
				if(W->move) {
				        W->move=0;
#ifdef X11
					goto QUIT;
#endif /* X11 */
				}
				W->size=1;
				OutEditBox(W);
				ShowTitleEvent(W);
			}
			c=0;
			continue;
		default:
			dpo(bl); continue;
		}
		if(!CheckBoxParam(W->FileName,eb.x,eb.y,eb.l,eb.h)) {
			memcpy(&W->EB,&eb,sizeof(NEB)); RepaintAllWind();
		}
	}
}

void InsPGraphInStr(char *str)
{
	int c;

	if((c=InputPsevdoGraph())>=0) {
		int i,l;

		l=strlen(str);
		if(cur_poz>=l) {
			for(i=l;i<cur_poz;i++)
				str[i]=' ';
			str[cur_poz+1]=0;
		}
		else
			if(insert)
				memmove(str+cur_poz+1,str+cur_poz,l-cur_poz);
		str[cur_poz]=c;
	}
}

/* Общий блок обрабатывающий запросы на следующие операции:
 *      - установка на строку;
 *      - пометка n строк;
 *      - удаление n строк;
 *      - вставка n пустых строк;
 *      - определение подстроки для поиска;
 *      - создание нового файла для редактирования;
 *      - задание редактируемого файла по имени;
 *      - создание метки
 *      - установка на метку
 *      - вызов man
 */
int InputArgument(NEF *W,int symb)
{
	char str[NAMESIZE+1];
	int c;

	*str=0;
	if(cmn.sarg<0) cmn.sarg=init_stk(LINESIZE);
	while(1) {
		int k;

		if((char)symb==' ') {
			char *ch;

			symb=0;
			if((ch=GetCurrentWord(W))!=NULL)
				strncpy(str,ch,NAMESIZE);
		}
		c=QuestBox(NULL,W!=NULL?W->EB.x+2:X+2,
			   W!=NULL?W->EB.y+W->EB.h-4:Y+H-4,NAMESIZE,
			   (W!=NULL?W->EB.l:L)-16,43,0,symb,str,W!=NULL?41:136,
			   cmn.sarg);
		symb=0;
		if(!W && c!=C_STOP && c!=C_BACKWIND && c!=C_SELNFILE && c!=C_HELP)
			continue;
		switch(c) {
		case C_STOP:
			if(!W) dpp(l_x-1,l_y);
			goto END;
		case C_BACKTAB:
			if(!nis_print((unsigned char)*str) || *(str+1)) {
				dpo(bl); continue;
			}
			PutMemPoz(W->wd,CS(W),*str);
			RepaintAllWind();
			goto END;
		case C_RETURN:
			{
				MP *s;

				if(!*str || *(str+1)) {
					dpo(bl); continue;
			        }
				if((s=FindMemPoz(*str))==NULL) {
					emess(pmes(106)); continue;
			        }
				if(s->wd!=W->wd) {
			                for(k=0;k<LenAllFiles && s->wd!=AllFiles[k].wd;k++);
			                if(k==LenAllFiles) /* ??? */
				                continue;
					RotateAllWind(k);
				}
				CS(W)=s->n;
				c=C_HOME;
				goto HOME;
			}
		case C_HOME:
			if(atof(str)>(double)0x7fffffff || (k=atoi(str))<=0) {
				dpo(bl); break;
			}
			CS(W)=k-1;
HOME:
			if(CS(W)<(MAXY(W)>>1))
				CY(W)=CS(W);
			else
				CY(W)=(MAXY(W)>>1);
			W->flscr=1;
			goto END;
		case C_DEL:
		case C_MARKSTR:
		case C_NEWSTR:
			if((k=atoi(str))<=0) {
				dpo(bl); break;
			}
			InitMark(W);
			CS(W)+=k-1;
			goto END;
		case C_BACKSEAR:
		case C_SEARCH:
			if(!*str) {
				emess(pmes(47)); break;
			}
			FindString(W,str,c==C_SEARCH?1:0);
	                goto END;
		case C_BACKWIND:
			if(cmn.NoNewFile || !*str) {
				dpo(bl); break;
			}
			if(!access(str,F_OK)) {
				emess(pmes(5)); break;
			}
			errno=0;
			strcpy(str,norm_path(str));
			{
				int fd;

				if((fd=creat(str,0660))<0) {
					char ss[NAMESIZE+1];

					sprintf(ss,pmes(9),str); emess(ss);
					break;
				}
				close(fd);
END1:
				InitEdit(str,NORMWIND);
				        if(LenAllFiles)
				                Dpp(&AllFiles[LenAllFiles-1].EB,
				                (int)CX(AllFiles+LenAllFiles-1),
				                (int)CY(AllFiles+LenAllFiles-1));
				c=RES1;
				goto END;
			}
		case C_SELNFILE:
			if(cmn.NoSelFile || !*str) {
				dpo(bl); break;
			}
			{
				char ss[LINESIZE+1];
				if(access(str,F_OK)) {
					sprintf(ss,pmes(27),str); emess(ss);
					break;
				}
			}
			strcpy(str,norm_path(str));
			goto END1;
		case C_HELP:
			if(!*str) {
				dpo(bl); break;
			}
			if(InitEdit(str,MANWIND)<0)
				if(LenAllFiles)
				        Dpp(&AllFiles[LenAllFiles-1].EB,
				        (int)CX(AllFiles+LenAllFiles-1),
				        (int)CY(AllFiles+LenAllFiles-1));
			c=RES1;
			goto END;
		default:
			dpo(bl); break;
		}
	}

END:
	return(c);
}

/* Перебор окон, arg задает направление перебора */
void RotateAllWind(int arg)
{
	NEF rr;

	if(LenAllFiles<2) goto END;
	switch(arg) {
	case -1:
		memcpy(&rr,AllFiles+LenAllFiles-1,sizeof(NEF));
		rr.EB.colorborder=rr.rdonly?COLRDPASS:COLPASS;
		OutEditBox(&rr);
		memmove(AllFiles+1,AllFiles,(LenAllFiles-1)*sizeof(NEF));
		memcpy(AllFiles,&rr,sizeof(NEF));
		break;
	default:
		memcpy(&rr,AllFiles+arg,sizeof(NEF));
		memcpy(AllFiles+arg,AllFiles+arg+1,(LenAllFiles-arg-1)*sizeof(NEF));
		memcpy(AllFiles+LenAllFiles-1,&rr,sizeof(NEF));
		AllFiles[LenAllFiles-2].EB.colorborder=AllFiles[LenAllFiles-2].rdonly?COLRDPASS:COLPASS;
		OutEditBox(AllFiles+LenAllFiles-2);
	}
	AllFiles[LenAllFiles-1].EB.colorborder=AllFiles[LenAllFiles-1].rdonly?COLRDACTIV:COLACTIV;
END:
	OutEditBox(AllFiles+LenAllFiles-1);
	return;
}

/* Закрывает окно */
void CloseWind(int i)
{
	MoveMemPoz(AllFiles[i].wd,-1,0);
	if(AllFiles[i].typ==TEMPWIND) unlink(AllFiles[i].FileName);
	if(AllFiles[i].AS->red)
	{
		int j=0;
	        char str[NAMESIZE+1];

		sprintf(str,pmes(99),basen(AllFiles[i].FileName));
		switch(y_n_m(8,str,pmes(100),&j,pmes(101),pmes(102),pmes(103),NULL))
		{
		case C_STOP:
			return;
		case C_RETURN:
			switch(j) {
			case 0:
				if(WriteEditFile(AllFiles+i,1)) return;
			case 1:
				break;
			case 2:
				return;
			}
		}
	}
	DelTextFile(i);
	if(LenAllFiles && i==LenAllFiles)
		AllFiles[LenAllFiles-1].EB.colorborder=AllFiles[LenAllFiles-1].rdonly?COLRDACTIV:COLACTIV;
	RepaintAllWind();
	if(LenAllFiles==0)
		PutTitleEvent(NULL,NULL);
}

/* Расположить окна каскадом */
void Cascade()
{
	int i;

	for(i=0;i<LenAllFiles;i++) InitEditBoxForFileB(i);
	RepaintAllWind();
}

/* Расположить окна мозаикой */
void Mozaik()
{
	int i,gy,lenall,k;

	if(LenAllFiles==1) {
		InitEditBoxForFile(AllFiles,NORMWIND); return;
	}
	if((MXWIND<<1)>L)
		lenall=1;
	else
		for(lenall=LenAllFiles;(gy=2*(H-1)/(lenall+1))<MYWIND+2;lenall--);
	for(k=LenAllFiles-lenall,i=0;i<lenall/2;i++)
	{
		AllFiles[2*i+k].EB.x=X;
		AllFiles[2*i+k].EB.y=Y+i*gy;
		AllFiles[2*i+k].EB.l=(L>>1)-1;
		AllFiles[2*i+k].EB.h=gy-1;
		if(i*2+k<=LenAllFiles) {
			AllFiles[2*i+1+k].EB.x=X+(L>>1);
			AllFiles[2*i+1+k].EB.y=Y+i*gy;
			AllFiles[2*i+1+k].EB.l=(L>>1)-1;
			AllFiles[2*i+1+k].EB.h=gy-1;
		}
	}
	if(lenall%2==1) {
		AllFiles[LenAllFiles-1].EB.x=X;
		AllFiles[LenAllFiles-1].EB.y=Y+i*gy;
		AllFiles[LenAllFiles-1].EB.l=L-1;
		AllFiles[LenAllFiles-1].EB.h=H-(i*gy)-1;
	}
	else {
		if(LenAllFiles!=2) {
			i--;
			AllFiles[LenAllFiles-2].EB.x=X;
			AllFiles[LenAllFiles-2].EB.y=Y+i*gy;
			AllFiles[LenAllFiles-2].EB.l=(L>>1)-1;
			AllFiles[LenAllFiles-2].EB.h=H-(i*gy)-1;
			AllFiles[LenAllFiles-1].EB.x=X+(L>>1);
			AllFiles[LenAllFiles-1].EB.y=Y+i*gy;
			AllFiles[LenAllFiles-1].EB.l=(L>>1)-1;
			AllFiles[LenAllFiles-1].EB.h=H-(i*gy)-1;
		}
		else {
			AllFiles[0].EB.x=X;
			AllFiles[0].EB.y=Y;
			AllFiles[0].EB.l=L-1;
			AllFiles[0].EB.h=(H>>1)-1;
			AllFiles[1].EB.x=X;
			AllFiles[1].EB.y=Y+(H>>1);
			AllFiles[LenAllFiles-1].EB.l=L-1;
			AllFiles[LenAllFiles-1].EB.h=H-(H>>1)-1;
		}
	}
	for(i=0;i<k;i++) InitEditBoxForFileB(i);
	RepaintAllWind();
	line_status(0);
	line_status(31);
}

static int maxlname;

/* Следующие две функции реализуют выбор окна через меню */
int OutWindName(int x,int y,int j,int col)
{
	setcolor(col); dpp(x,y); dpn(maxlname,' ');
	dpp(x,y); dps(GetFileName(AllFiles+j));
	if(AllFiles[j].AS->red) dps(" #");
}

void SelectWindName()
{
	int i,l;

	if(!LenAllFiles) return;
	for(maxlname=0,i=0;i<LenAllFiles;i++)
		if((l=Strlen(GetFileName(AllFiles+i)))>maxlname)
			maxlname=l;
	maxlname+=3; i=l=0;
	line_status(38);
	while(1) {
		setcolor(COLMBGR);
	        title(1,pmes(2));
		switch(menu(l_x-7,l_y-9,LenAllFiles,maxlname,OutWindName,
		       COLMBGR,COLMSTR,&i,&l))
		{
		case C_STOP:
			break;
		case C_RETURN:
			if(l!=LenAllFiles-1) RotateAllWind(l);
			break;
		case C_STRDEL:
			RepaintAllWind();
			CloseWind(l);
			if(!LenAllFiles) break;
			i=l=0;
			continue;
		default:
			dpo(bl); continue;
		}
		break;
	}
	RepaintAllWind();
	return;
}

/* Следующие две функции реализуют вставку символа через меню */
int ScreenPsevdoGraph(int x,int y,int j,int col)
{
	char str[5];

	setcolor(col); dpp(x,y); sprintf(str," %3d  ",j); dps(str);
	if(nis_print(j))
		dpo(j);
	else {
		setcolor(invers(col)); dpo('A'+j); setcolor(col);
	}
	dpo(' ');
	return(0);
}

int InputPsevdoGraph()
{
	int nb;
	static int i_psm=0,j_psm=0;

	nb=get_box(0,1,l_x-1,4);
	setcolor(COLMBGR);
	title(1,pmes(4));
	line_status(39);
	while(1) {
		switch(menu(l_x-7,l_y-9,256,8,ScreenPsevdoGraph,COLMBGR,COLMSTR,&i_psm,&j_psm))
		{
		case C_STOP:
			put_box(0,1,nb); free_box(nb); return(-1);
		case C_RETURN:
			goto END;
		default:
			dpo(bl); continue;
		}
	}
END:
	put_box(0,1,nb); free_box(nb);
	return(j_psm);
}

/* Восстановление экрана */
void RestoreScreen()
{
	int nb;

	stor_poz();
	nb=get_box(X,Y,L+1,H+1);
	zero_scrold(); /* вместо ClrScr */
	put_box(X,Y,nb); free_box(nb);
	rest_poz();
}

#define COLBSIZE 16
static void DrawColorBox(int,int,int);

/* Следующие четыре функции реализуют замену цветов */
static int RunColorBox(int x,int y,int *col)
{
	int i=(*col>>4),j=(*col&0xf),c;

	line_status(150);
	while(1) {
		DrawColorBox(x,y,*col=((i<<4)|j));
		switch((c=ReadCommand(NULL,-1))) {
		case C_STRLEFT:
			if(j>0) j--;
			break;
		case C_STRRIGHT:
			if(j<COLBSIZE-1) j++;
			break;
		case C_UP:
			if(i>0) i--;
			break;
		case C_DOWN:
			if(i<COLBSIZE-1) i++;
			break;
		default:
			if(c==C_TAB || c==C_RETURN || c==C_STOP) return(c);
			break;
		}
	}
}

static void DrawColorBox(int x,int y,int col)
{
	int i,j;
	int x1=x+2*(col&0xf)+1,y1=y+((col&0xff)>>4)+1;

	setcolor(0x70);
	wind(x,y,2*COLBSIZE+2,COLBSIZE+2);
	for(i=0;i<=COLBSIZE;i++)
		for(j=0;j<=COLBSIZE;j++) {
			int x2=x+2*j+1,y2=y+i+1;
			char str[3];

			if(i!=COLBSIZE)
				if(j!=COLBSIZE) {
					dpp(x2,y2); setcolor((i<<4)|j); dps("x ");
				} else {
					dpp(x2,y2); sprintf(str,"%x",i);
					setcolor(0x70); dpo(*str);
				}
			else
				if(j!=COLBSIZE) {
					dpp(x2,y2); sprintf(str,"%x ",j);
					setcolor(0x70); dps(str);
				}
		}
	dpp(x1-1,y1);setcolor(0xf0); dpo('>');
	dpp(x1+1,y1);setcolor(0xf0); dpo('<');
	setcolor(COLMBGR);
	wind(45,20,34,3);
	dpp(46,21);
	setcolor(col);
	dps("TEXT TEXT TEXT TEXT TEXT TEXT TE");
	dpp(x1,y1);

}

int ScreenOneColor(int x,int y,int j,int col)
{
	char str[NAMESIZE+1];

	setcolor(COLMBGR); dpp(x,y); dpn(38,' ');
	setcolor(col); dpp(x,y);
#ifdef X11
	sprintf(str," %-38s",pmes(51+j));
#else /* X11 */
	sprintf(str," %-38s",pmes(51+j));
#endif /* X11 */
	dps(str);
	dpp(x+39,y); setcolor(C[j]); dps("xxx");
	if(col!=COLMBGR)
		DrawColorBox(45,1,C[j]);
	return(0);
}

void ChangeColor()
{
	int c,x=0,y=1;
	static poz=0,ele=0;
#ifdef X11
	short Cold[MAXCOLOR];
#else /* X11 */
	char Cold[MAXCOLOR];
#endif /* X11 */

	stor_poz();
	memcpy(Cold,C,sizeof C);
	while(1) {
		char str[NAMESIZE+1];
MENU:
		setcolor(COLMBGR);
		line_status(135);
		switch(c=amenu(x,y,42,20,USECOLOR,ScreenOneColor,COLMBGR,COLMSTR,
			     &poz,&ele,0))
		{
		case C_STOP:
			if(memcmp(C,Cold,sizeof C)) {
				c=1;
		                switch(y_n_m(8,pmes(134),NULL,&c,pmes(101),pmes(102),pmes(103),NULL))
		                {
		                case C_STOP:
			                continue;
		                case C_RETURN:
			                switch(c) {
			                case 0:
			                        {
				                        char *ch;

				                        if((ch=getenv("TEMP"))==NULL) ch=getenv("HOME");
				                        if(ch!=NULL) {
					                        int fd;

					                        sprintf(str,"%s/%s.%s",ch,CNFNAME,getlogin());
					                        if(access(str,F_OK)) {
						                        if((fd=creat(str,0640))>0) {
SAVEC:
							                        write(fd,C,sizeof C);
							                        close(fd);
						                        } else
							                        errno=0;
					                        } else
						                        if((fd=open(str,O_WRONLY))>0)
							                        goto SAVEC;
						                        else
							                        errno=0;
				                        }
			                        }
			                        for(c=0;c<LenAllFiles;c++) {
				                        AllFiles[c].EB.colorborder=
				                        AllFiles[c].rdonly?(c==LenAllFiles-1?COLRDACTIV:COLRDPASS):
				                        (c==LenAllFiles-1?COLACTIV:COLPASS);
				                        AllFiles[c].EB.colortext=AllFiles[c].rdonly?COLRDTEXT:COLTEXT;
			                        }
						break;
			                case 1:
						memcpy(C,Cold,sizeof C);
					        break;
			                case 2:
				                break;
			                }
		                }
			}
			RepaintAllWind();
			rest_poz();
			return;
		case C_HOME:
			c=1;
			switch(y_n_m(8,pmes(7),NULL,&c,pmes(101),pmes(102),pmes(103),NULL)) {
			case C_STOP:
				continue;
			case C_RETURN:
				switch(c) {
				case 0:
					color_defa();
					break;
				case 1:
				case 2:
					break;
				}
			}
			break;
		case C_RETURN:
		case C_TAB:
			{
				int col=C[ele];

			        if(RunColorBox(45,y,&col)!=C_STOP)
					C[ele]=col;
				continue;
			}
		default:
			dpo(bl); continue;
		}
	}
}

/* Удаляет часть текущей строки справа от курсора */
int DelPartStr(NEF *W)
{
	int err=0;
	STR *S;

	unscp(W);
	S=DupSTR(GetMSTR(W->AS,CS(W)));
	if(SP(W)>S->rlen) goto END;
	InitMark(W);
	CX(W)+=(S->rlen-GP(W)-(err=CX(W)));
	EndMark(W);
	CX(W)=err; err=0;
	if(PutMemBlock(W,0)) {
		err=-1; goto END;
	}
	S->s[S->rlen=SP(W)]=0;
	ClearEndSTR(S);
	PutSTR(W->AS,CS(W),S);
	FreeSTR(S);
	W->flscr=1;
	ChangeRed(W,1);
END:
	return(err);
}

int InputPoz(NEF *W)
{
	char str[4];
	int c;

	stor_poz();
	sprintf(str,"%d",SP(W)+1);
	while(1) {
		c=QuestBox(NULL,W->EB.x+2,W->EB.y+W->EB.h-4,3,3,145,'c',0,str,146,-1);
		switch(c) {
		case C_STOP:
			c=-1; goto END;
		case C_RETURN:
			if((c=atoi(str))<=0) {
				dpo(bl); continue;
			}
			c--;
			goto END;
		default:
			dpo(bl); continue;
		}
	}
END:
	rest_poz();
	return(c);
}

int QuestBox(NEF *W,int x,int y,int size,int show,
	     int mess,char arg,int symb,char *str,int status,int nsteck)
{
	int len,c;
	int nb;

	len=strlen(pmes(mess));
	nb=get_box(x,y,len+show+7,4);
	setcolor(COLMBGR);
	wind(x,y,len+show+5,3);
	dpp(x+2,y+1); dps(pmes(mess)); dpo(' ');
	while(1)
	{
		line_status(status);
		setcolor(COLEDIT);
		c=Edit(W,symb,str,size,show,x+len+3,y+1,arg,nsteck);
		symb=0;
		if(!arg) {
			switch(c) {
			case C_NEWSTR:
				*str=0; continue;
			case C_INPSEVDO:
				InsPGraphInStr(str);
				continue;
#ifdef X11
			case C_MOUSE:
				if(ev.b==3) {
					c=C_STOP; break;
				}
#endif
			}
		}
		break;
	}
	put_box(x,y,nb); free_box(nb);
	if(W) W->nocom=0;
	return(c);
}

int StringsInPage(NEF *W)
{
	char str[4];
	int c;

	stor_poz();
	*str=0;
	while(1)
	{
		c=QuestBox(NULL,W?W->EB.x+2:X+2,W?W->EB.y+W->EB.h-4:Y+H-4,3,3,147,'c',0,str,146,-1);
		switch(c) {
		case C_STOP:
			c=-1; goto END;
		case C_RETURN:
			if((c=atoi(str))<=0) {
				dpo(bl); continue;
			}
			cmn.StrInPage=c;
			goto END;
		default:
			dpo(bl); continue;
		}
	}
END:
	rest_poz();
	return(c);
}

/* Вызов специальных окон */
void CallSpecWindow(int arg)
{
	int i;

	for(i=0;i<LenAllFiles;i++)
		if(arg==HELPWIND?AllFiles[i].typ==HELPWIND:arg==CBWIND?AllFiles[i].typ==CBWIND:AllFiles[i].typ==DBWIND)
		{
			RotateAllWind(i); break;
		}
	if(i==LenAllFiles) InitEdit(arg==HELPWIND?HELPNAME:NULL,arg);
}

static char wordbord[] = "\\;!\"#$%&'()|*?^<>{}:@,.= ";

char *GetCurrentWord(NEF *W)
{
	int i,j;
	STR *S;
	static STR *S1=NULL;

	S=GetMSTR(W->AS,CS(W));
	if(S==NULL || S->rlen<=SP(W) || (S1==NULL && (S1=InitSTR())==NULL))
	        return(NULL);
	AddSTR(S1,1);
	for(i=SP(W);i>=0 && strchr(wordbord,(int)S->s[i])==NULL;i--);
	i++;
	for(j=0;S->s[i+j]  &&  strchr(wordbord,(int)S->s[i+j])==NULL;j++) {
		AddSTR(S1,j+1); S1->s[j]=S->s[i+j];
	}
	S1->s[j]=0; S1->rlen=j;
	return(S1->s);
}

static MP *mp=NULL;
static LenMP=0;

int PutMemPoz(int wd,int n,char c)
{
	int err=0;
	MP *s;

	if(mp==NULL && (mp=(MP *)Malloc(MEMPOZ*sizeof(MP)))==NULL) {
		err=-1; goto END;
	}
	if((s=FindMemPoz(c))!=NULL) {
		s->wd=wd; s->n=n; s->c=c;
	}
	else {
		if(LenMP==MEMPOZ) {
			err=-1; emess(pmes(105)); goto END;
		}
		mp[LenMP].wd=wd; mp[LenMP].n=n; mp[LenMP++].c=c;
	}
END:
	return(err);
}

MP *FindMemPoz(char c)
{
	int i;

	for(i=0;i<LenMP;i++)
		if(mp[i].c==c)
			return(mp+i);
	return(NULL);
}

MP *FindMemPozStr(int wd,int n,int num)
{
	static int i=0,d=-1;

	if(wd>=0) {
		d=wd; i=0;
	}
	for(;i<LenMP;i++)
		if(mp[i].wd==d && (n<0 || (mp[i].n>=n && mp[i].n<=n+num)))
			return(mp+i++);
	return(NULL);
}

void MoveMemPoz(int wd,int n,int num)
{
	int i;

	for(i=0;i<LenMP;i++)
		if(mp[i].wd==wd) {
			if(n<0)  {
				memcpy(mp+i,mp+i+1,((LenMP--)-i)*sizeof(MP));
				continue;
			}
			if(num>0) {
				if(mp[i].n>=n) mp[i].n+=num;
			} else {
				if(mp[i].n>=n-num)
					mp[i].n+=num;
				else
					if(mp[i].n<n-num && mp[i].n>=n) mp[i].n=n;
			}
		}
}

#ifdef X11

int SelectWindMouse(int x,int y)
{
	int i;

	for(i=LenAllFiles-2;i>=0;i--) {
		NEF *W=AllFiles+i;

		if(IF_FU(W,x,y)) {
			CS(W)=CS(W)-CY(W)+y-W->EB.y-1; CY(W)=y-W->EB.y-1; CX(W)=x-W->EB.x-1;
			if(CY(W)<0) {
				CS(W)-=CY(W); CY(W)=0;
			}
			else
				if(CY(W)>MAXY(W)) {
					CS(W)-=(-MAXY(W)+CY(W)); CY(W)=MAXY(W);
				}
			if(CX(W)<0)
				CX(W)=0;
			else
				if(CX(W)>MAXX(W)) CX(W)=MAXX(W);
			return(i);
		}
	}
	return(-1);
}

#endif

/* Char-овский массив для описания символов псевдографики             */
/* Имеется 4 элемента для описания символа (есть ли черточка в нужном */
/* направлении) 0-нет черты, 1-одинарная, 2-двойная.                  */
/* 0-й эл-т - вверх                                                   */
/* 1-й эл-т - вправо                                                  */
/* 2-й эл-т - вниз                                                    */
/* 3-й эл-т - влево                                                   */

static char mas2char(unsigned char *,unsigned char);
static int char2mas(int, char *);
static void FillNext(NEF *W,STR *S,int i,int j,char *);

int WhatDraw(NEF *W,STR *S,int *i,int c)
{
	char CurC[4],NextC[4];
	int Poz;

	if(W->pgraph==3)
	{
		c=(cmn.last?cmn.last:' '); goto END;
	}
	Poz=*i;
	switch(c)
	{
	case C_UP:
		CS(W)++; break;
	case C_DOWN:
		CS(W)--; break;
	case C_STRRIGHT:
	        Poz--; break;
	case C_STRLEFT:
		Poz++; break;
	default:
		return(c);
	}
	bzero(CurC,4); bzero(NextC,4);
	char2mas(S!=NULL?GetChSTR(S,Poz):GetCh(W,Poz,CS(W)),CurC);
	switch(c)
	{
	case C_STRRIGHT:
		if(CurC[1]==W->pgraph)
		{
			Poz++; NextC[3]=W->pgraph;
		}
		else
		{
			(*i)--; NextC[1]=W->pgraph;
		}
		FillNext(W,S,Poz,CS(W),NextC);
		if(!NextC[0] && !NextC[2])
		        NextC[(NextC[3]==W->pgraph)?1:3]=W->pgraph;
		if(NextC[1] && NextC[3] && NextC[1]!=NextC[3])
		        NextC[1]=NextC[3]=W->pgraph;
		break;
	case C_STRLEFT:
		if(CurC[3]==W->pgraph)
		{
			Poz--; NextC[1]=W->pgraph;
		}
		else
		{
			(*i)++; NextC[3]=W->pgraph;
		}
		FillNext(W,S,Poz,CS(W),NextC);
		if(!NextC[0] && !NextC[2])
		        NextC[(NextC[1]==W->pgraph)?3:1]=W->pgraph;
		if(NextC[1] && NextC[3] && NextC[1]!=NextC[3])
		        NextC[1]=NextC[3]=W->pgraph;
		break;
	case C_UP:
		if(CurC[0]==W->pgraph)
		{
			CS(W)--; NextC[2]=W->pgraph;
		}
		else
		{
			CY(W)++; NextC[0]=W->pgraph;
		}
		FillNext(W,S,Poz,CS(W),NextC);
		if(!NextC[1] && !NextC[3])
		        NextC[(NextC[2]==W->pgraph)?0:2]=W->pgraph;
		if(NextC[0] && NextC[2] && NextC[0]!=NextC[2])
		        NextC[0]=NextC[2]=W->pgraph;
		break;
	case C_DOWN:
		if(CurC[2]==W->pgraph)
		{
			CS(W)++; NextC[0]=W->pgraph;
		}
		else
		{
			CY(W)--; NextC[2]=W->pgraph;
		}
		FillNext(W,S,Poz,CS(W),NextC);
		if(!NextC[1] && !NextC[3])
		        NextC[(NextC[0]==W->pgraph)?2:0]=W->pgraph;
		if(NextC[0] && NextC[2] && NextC[0]!=NextC[2])
		        NextC[0]=NextC[2]=W->pgraph;
	}
	c=(unsigned char)mas2char(NextC,W->pgraph);
END:
	return(c);
}

void FillNext(NEF *W,STR *S,int i,int j,char *NextC)
{
	char CurC[4];
	int c;

	if(i!=MAXX(W)-1)
		c=get_ch(W->EB.x+i-GP(W)+2,W->EB.y+CY(W)+1);
	else
	        c=(S!=NULL?GetChSTR(S,i+1):GetCh(W,i+1,j));
	char2mas(c,CurC);
	if(CurC[3] && !NextC[1] ) NextC[1]=CurC[3];
	if(i!=GP(W))
		c=get_ch(W->EB.x+i-GP(W),W->EB.y+CY(W)+1);
	else
		c=(S!=NULL?GetChSTR(S,i-1):GetCh(W,i-1,j));
	char2mas(c,CurC);
	if(CurC[1] && !NextC[3]) NextC[3]=CurC[1];
	if(j)
		c=get_ch(W->EB.x+i-GP(W)+1,W->EB.y+CY(W));
	else
		c=GetCh(W,i,j-1);
	char2mas(c,CurC);
	if(CurC[2] && !NextC[0]) NextC[0]=CurC[2];
	if(j!=MAXY(W)-1)
		c=get_ch(W->EB.x+i-GP(W)+1,W->EB.y+CY(W)+2);
	else
		c=GetCh(W,i,j+1);
	char2mas(c,CurC);
	if(CurC[0] && !NextC[2]) NextC[2]=CurC[0];
}

#define NUMSYMB 40

struct symb_cod
{
	unsigned char c;
	unsigned char s[4];
} os[NUMSYMB]= {
 {'│','\1','\0','\1','\0',},{'┤','\1','\0','\1','\1',},{'╡','\1','\0','\1','\2',},
 {'╢','\2','\0','\2','\1',},{'╖','\0','\0','\2','\1',},{'╕','\0','\0','\1','\2' },
 {'╣','\2','\0','\2','\2',},{'║','\2','\0','\2','\0',},{'╗','\0','\0','\2','\2',},
 {'╝','\2','\0','\0','\2',},{'╜','\2','\0','\0','\1',},{'╛','\1','\0','\0','\2',},
 {'┐','\0','\0','\1','\1',},{'└','\1','\1','\0','\0',},{'┴','\1','\1','\0','\1',},
 {'┬','\0','\1','\1','\1',},{'├','\1','\1','\1','\0',},{'─','\0','\1','\0','\1',},
 {'┼','\1','\1','\1','\1',},{'╞','\1','\2','\1','\0',},{'╟','\2','\1','\2','\0',},
 {'╚','\2','\2','\0','\0',},{'╔','\0','\2','\2','\0',},{'╩','\2','\2','\0','\2',},
 {'╦','\0','\2','\2','\2',},{'╠','\2','\2','\2','\0',},{'═','\0','\2','\0','\2',},
 {'╬','\2','\2','\2','\2',},{'╧','\1','\2','\0','\2',},{'╨','\2','\1','\0','\1',},
 {'╤','\0','\2','\1','\2',},{'╥','\0','\1','\2','\1',},{'╙','\2','\1','\0','\0',},
 {'╘','\1','\2','\0','\0',},{'╒','\0','\2','\1','\0',},{'╓','\0','\1','\2','\0',},
 {'╫','\2','\1','\2','\1',},{'╪','\1','\2','\1','\2',},{'┘','\1','\0','\0','\1',},
 {'┌','\0','\1','\1','\0',},
};

static char mas2char(unsigned char *s,unsigned char st)
{
	int i;

	for(i=0;i<NUMSYMB;i++)
	{
		if(!memcmp(os[i].s,s,4))
			return(os[i].c);
	}
	for(i=0;i<4;i++)
		if(s[i])
			s[i]=st;
	return(mas2char(s,st));
}

static int char2mas(int c, char *s)
{
	int j;

	if(!nis_graph(c))
	{
		bzero(s,4); return(-1);
	}
	if(c-179 > NUMSYMB)
		return(-1);
	memcpy(s,os[c-179].s,4);
	return(0);
}

void ChangeTab()
{
	char str[2];

	stor_poz();
	sprintf(str,"%1d",SCRTAB); insert=0;
	while(1)
	{
		switch(QuestBox(NULL,X+2,Y+H-4,1,1,132,'c',0,str,133,-1))
		{
		case C_STOP:
			goto END;
		case C_RETURN:
			if(atoi(str)>=1 && atoi(str)<=8) {
				SCRTAB=POZTAB=atoi(str); goto END;
			}
		default:
			dpo(bl); continue;
		}
	}
END:
	RepaintAllWind();
	rest_poz();
}

int ReallyQuit()
{
	int c;

	TmpUnsetProt();
	for(c=0;c<LenAllFiles;c++)
		if(AllFiles[c].AS->red) break;
	if(c!=LenAllFiles) {
		c=1;
	        switch(y_n_m(8,pmes(151),NULL,&c,pmes(101),pmes(102),NULL)) {
	        case C_STOP:
		        c=0; break;
	        case C_RETURN:
			c=!c; break;
	        }
	} else
		c=1;
	TmpRestProt();
	return(c);
}

int NextWord(NEF *W)
{
	int i,j=SP(W);
	STR *S;

	S=GetMSTR(W->AS,CS(W));
	if(S!=NULL && S->rlen>SP(W)) {
		if(S->s[i=SP(W)]==' ')
			for(;S->s[i]==' ';i++);
		else {
			for(;S->s[i] && S->s[i]!=' ';i++);
			for(;S->s[i] && S->s[i]==' ';i++);
		}
		if(S->s[i]) j=i;
	}
	return(j);
}

int BackWord(NEF *W)
{
	int i,j=SP(W);
	STR *S;

	S=GetMSTR(W->AS,CS(W));
	if(S!=NULL && S->rlen) {
		if(S->s[i=(SP(W)-1)]==' ') {
			for(;i>=0 && S->s[i]==' ';i--);
			for(;i>=0 && S->s[i]!=' ';i--);
		}
		else
			for(;i>=0 && S->s[i]!=' ';i--);
		if(i>=0) j=i+1;
	}
	return(j);
}
