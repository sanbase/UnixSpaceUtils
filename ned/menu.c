/*
	Copyright 1995,1996,1997 Unitex+, ltd. Kiev, Ukraine
	Written by Oleg Smolyar
	E-mail: oleg@unitex.kiev.ua
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <tty_codes.h>
#include "ned.h"

#ifndef lint
static const char rcsid[]="$Id: menu.c,v 2.2.1.1 1998/07/24 12:58:58 toor Exp toor $";
#endif

extern int LenAllFiles;
extern NEF *AllFiles;

static IT*   ItemCreat(char *,Menu *,int,char *);
static int   ItemNext(Menu *,int,int);
static void  ItemDrawSub(IT *,int,int,int,int);
static void  ItemDrawBar(IT *,int,int,int);
static int   _SubMenuRun(int,int,Menu *);
static void  SubMenuDraw(Menu *);
static void  MenuBarDraw(Menu *);
static void  HelpLineDraw(IT *t,int col);
static void  SetMenuColor(int,int,int,int);
static void  GetMenuColor(Menu *);
static void  MenuBarFree(Menu *);
static void  SubMenuFree(Menu *);
static void  ItemFree(IT *);
static int  check_active(int);
static Menu *InitMenu(int);
static Menu *MenuBarCreat(char *,...);
static Menu *SubMenuCreat(char *,...);
static int    SubMenuRun(int,int,Menu *);
static char *GetLastUndoMode(void);


#define LENMENU   12
#define MAIN_MENU 8
#define M_FORMAT  450
#define M_CARR    451
#define M_MACRO   452
#define M_BLOCK   453
#define M_GOTO    454
#define M_GAB     455

//#define X11

int RunMenu()
{
	int c;

	stor_poz();
	c=MenuBarRun(0,InitMenu(1));
	InitMenu(0);
	rest_poz();
	return(c);
}

static Menu *InitMenu(int arg)
{
	static Menu **s=NULL;

	if(!arg) {
		if(s!=NULL) {
			MenuBarFree(s[MAIN_MENU]); Free(s);
		}
		s=NULL;
		return(NULL);
	}
	if(s!=NULL) return(s[MAIN_MENU]);
	if((s=(Menu **)Malloc(LENMENU*sizeof(Menu *)))==NULL) return(NULL);
#ifdef RUSDIAG
	s[0]=SubMenuCreat(
	"~Н~овый",                 0,C_ARG,     "Создать новый файл",
	"~О~ткрыть",               0,C_SELNFILE,"Открыть файл для редактирования",
	"~З~аписать",              0,C_QWRITE,  "Записать текущий файл",
#ifndef NO_SHELL
	"",                        0,0,         0,
	"~S~hell",                 0,C_CALLSH,  "Выход в командную строку",
#endif
			      NULL);
	s[1]=SubMenuCreat(
	"~Н~ачало пометки",        0,C_BMARKBOX,"Начало пометки блока",
	"~К~онец пометки",         0,C_EMARKBOX,"Конец пометки блока",
	"~С~копировать",           0,C_COPYBOX, "Скопировать блок в текущую позицию"
	"~У~далить",               0,C_DELBOX,  "Удалить помеченный блок",
	"~В~ставить",              0,C_NEWBOX,  "Вставить пустой блок",
	"~Ф~орматировать",         0,C_FORMAT,  "Форматировать блок",
			      NULL);
	s[2]=SubMenuCreat(
	"В ~н~ачало файла",        0,C_HOME,"Стать в начало текущего файла",
	"В ~к~онец файла",         0,C_END, "Стать в  конец текущего файла",
			      NULL);
	s[3]=SubMenuCreat(
	"~Б~ез выравнивания",      0,C_AUTOF1,"Разрывать строку по границе слова",
	"~Р~азрыв по слову",       0,C_AUTOF2,"Переносить слово и выравнивать",
	"~В~ыравнивание и перенос",0,C_AUTOF3,"Разрывать строку по границе слова и выравнивать",
	"~О~тмена",                0,C_CLAF,  "Выход из режима автоформатирования",
			      NULL);
	s[4]=SubMenuCreat(
	"~У~становить",            0,C_RETPOZ  ,"Установить границу возврата каретки",
	"У~б~рать",                0,C_CLRETPOZ,"Убрать границу возврата каретки",
			      NULL);
	s[5]=SubMenuCreat(
	"~В~вод аргумента",       0,C_ARG,    "Ввод для аргумента для операций над файлом ",
	"~З~аменить",             0,C_REPLACE,"Замена подстроки в текущем файле",
	"",                       0,0,        0,
	"~А~втоформатирование",s[3],M_FORMAT, "Форматирование текста",
	"",                       0,0,        0,
	"~В~озврат каретки",   s[4],M_CARR,   "Установка левой границы текста",
	"",                       0,0,0,
	"~С~трок в странице",     0,C_SETSINP,"Количество строк в одной странице",
	"~П~робелов в табуляции", 0,C_DEFTAB, "Установить количество пробелов в табуляции",
			     NULL);
	s[6]=SubMenuCreat(
	"~С~оздать макро",        0,C_BMACRO, "Начать создание макро. Конец ввода макро - EscEsc",
	"~В~ыполнить макро",      0,C_WMACRO, "Выполнить макро",
			     NULL);
	s[7]=SubMenuCreat(
	GetLastUndoMode(),        0, C_UNDO,     "Восстановление измененного",
	"~О~перации с блоками",s[1],M_BLOCK,    "Копирование, удаление блоков",
	"",                       0,0,          0,
	"~G~o to",             s[2],M_GOTO,     "Позиционирование по файлу",
	"",                       0,0,          0,
	"~Т~аблица символов",     0,C_INPSEVDO, "Выбор символа из таблицы символов",
	"~Ф~ильтр",               0,C_EXEFILTR, "Запустить фильтр для текущего файла",
	"",                       0,0,          0,
	"~М~акро",             s[6],M_MACRO,    "Использование макро для работы с файлом",
	"~Р~исовать",             0,C_PGRAPH,   "Рисование псевдографики",
			      NULL);
	s[11]=SubMenuCreat(
	"~Н~а весь экран",     0,C_FULLSCR,"Сделать текущее окно максимально большого размера",
	"~Р~азмер"       ,     0,C_MSWIND, "Изменить размер окна",
	"~П~оложение"    ,     0,C_MSWIND, "Изменить положение окна",
			      NULL);
	s[9]=SubMenuCreat(
	"~К~аскад",             0,C_CASCAD,  "Расположить окна упорядоченно с наложением",
	"~М~озаика",            0,C_MOZAIK,  "Расположить максимально возможное число окон на экране без наложения",
	"",                     0,0,         0,
	"~Ц~вета",              0,C_COL,      "Цвета редактора",
	"",                     0,0,         0,
	cmn.UseColour?"~В~ыключить подсветку":"~В~ключить подсветку",
	                        0,C_COLOUR,  "Режим подсветки",
	"",                     0,0,         0,
	"~Г~абариты",       s[11],M_GAB,         "Изменение габаритов или расположения окна",
	"",                     0,0,         0,
	"~С~ледующее",          0,C_NEXTWIND,"Перейти к следующему окну",
	"~П~редыдущее",         0,C_BACKWIND,"Перейти к предыдущему окну",
	"~З~акрыть",            0,C_CLOSE,   "Закрыть окно текущее окно",
	"",                     0,0,         0,
	"С~п~исок окон",        0,C_SELWIND, "Список окон",
				 NULL);
	s[10]=SubMenuCreat(
	"Выход ~б~ез записи",  0,C_QNOWRITE,"Выход без записи",
	"Выход ~с~ записью",     0,C_STOP,    "Выход с записью всех изменений во всех файлах",
				 NULL);
	s[MAIN_MENU]=MenuBarCreat(
	 "~Ф~айл",   s[0],0,      "Операции с файлами",
	"~Т~екст",   s[7],0,      "Операции по редактированию",
	"О~п~ции",   s[5],0,      "Общие операции над файлом",
	"~О~кна",    s[9],0,      "Операции над окнами",
	"~П~омощь",     0,C_HELP, "Вызвать окно с помощью",
	"~В~ыход ", s[10],0,      "Выход из редактора",
				 NULL);
#else
	s[0]=SubMenuCreat(
	"~N~ew",                   0,C_ARG,     "Creat new file",
	"~O~pen file",             0,C_SELNFILE,"Open file for edit" ,
	"~S~ave",                  0,C_QWRITE,  "Save current file",
#ifndef NO_SHELL
	"",                        0,0,         0,
	"~S~hell",                 0,C_CALLSH,  "Call shell",
#endif
			      NULL);

	s[1]=SubMenuCreat(
	"~B~egin mark",           0,C_BMARKBOX,"Begin mark block",
	"~E~nd mark",             0,C_EMARKBOX,"End mark block",
	"~C~opy",                 0,C_COPYBOX, "Copy block in current pozitioin",
	"~D~elete",               0,C_DELBOX,  "Delete mark block",
	"~I~nsert",               0,C_NEWBOX,  "Insert empty block",
	"~F~ormat",               0,C_FORMAT,  "Форматировать блок",
			      NULL);

	s[2]=SubMenuCreat(
	"~B~egin file",        0,C_HOME,"Go to begin file",
	"~E~nd file",          0,C_END, "Go to end file",
			      NULL);

	s[3]=SubMenuCreat(
	"~W~ithout align",      0,C_AUTOF1,"Break string on border word",
	"~B~reak",              0,C_AUTOF2,"Break string on border word and align",
	"~A~lign & break",      0,C_AUTOF3,"Hyphen word and align",
	"~C~lose autoformating",0,C_CLAF,  "Quit from mode autoformat",
			      NULL);

	s[4]=SubMenuCreat(
	"~S~et left margin",      0,C_RETPOZ  ,"Set border left margin",
	"~C~lose left margin",    0,C_CLRETPOZ,"Close border left margin",
			      NULL);

	s[5]=SubMenuCreat(
	"~C~ommand box",          0,C_ARG,    "Command box for operations on file",
	"~R~eplace",              0,C_REPLACE,"Replace substring in current file",
	"",                       0,0,        0,
	"~A~utoformat",        s[3],M_FORMAT,"Format string",
	"",                       0,0,        0,
	"~L~eft margin",       s[4],M_CARR,   "Set left margin for text",
	"",                       0,0,        0,
	"~S~trings in page",      0,C_SETSINP,"Number strings in one page",
	"~S~paces in tab",        0,C_DEFTAB, "Number space in one tab",
			     NULL);

	s[6]=SubMenuCreat(
	"~C~reat macro",        0,C_BMACRO,"Begin creat macro. End - EscEsc",
	"~R~un macro",          0,C_WMACRO,"Run macro",
			     NULL);

	s[7]=SubMenuCreat(
	"~U~ndo",        0, C_UNDO,               "Undo",
	"~B~lock operation",s[1],M_BLOCK,   "Copy,insert and delete blocks",
	"",                          0,0,         0,
	"~G~o to",                s[2],M_GOTO,    "Seeking in file",
	"",                          0,0,         0,
	"~S~ymbol",                  0,C_INPSEVDO,"Select from table of symbols",
	"~F~ilter",                   0,C_EXEFILTR,"Run filtr for current file",
	"",                          0,0,         0,
	"~M~acro",                s[6],M_MACRO,   "Creat and run macro",
	"~D~raw",                    0,C_PGRAPH,  "Draw psevdograph",

			      NULL);

	s[11]=SubMenuCreat(
	"~F~ull window",     0,C_FULLSCR,"Maximize window",
	"~S~ize window",     0,C_MSWIND, "Change window size",
	"~M~ove window",     0,C_MSWIND, "Moving window",
			      NULL);
	s[9]=SubMenuCreat(
	"~C~ascade",            0,C_CASCAD,  "Cascade windows",
	"~T~ile",               0,C_MOZAIK,  "Mozaic windows",
	"",                     0,0,         0,
	"C~o~lors",             0,C_COL,     "Editor colors",
	"",                     0,0,         0,
	cmn.UseColour?"~H~ighlight off":"~H~ighlight on",
	                        0,C_COLOUR,  "Highlighted mode",
	"",                     0,0,         0,
	"~S~izes",          s[11],M_GAB,         "Change size or move windows",
	"",                     0,0,         0,
	"~N~ext",               0,C_NEXTWIND,"Next window",
	"~P~revious",           0,C_BACKWIND,"Previous window",
	"C~l~ose",              0,C_BACKWIND,"Close current window",
	"",                     0,0,         0,
	"~L~ist of the windows",0,C_SELWIND, "List of the windows",
				 NULL);
	s[10]=SubMenuCreat(
	"~Q~uit without saving",   0,C_QNOWRITE,"Quit without saving",
	"Quit ~w~ith saving",      0,C_STOP,    "Quit with saving",
				 NULL);

	s[MAIN_MENU]=MenuBarCreat(
	"~F~ile",      s[0],0,     "Operations with file",
	"~E~dit",      s[7],0,     "Edit file",
	"~M~ode",      s[5],0,     "Mode work with files",
	"~W~indow",    s[9],0,     "Operation with windows",
	"~H~elp",         0,C_HELP,"Call window with help",
	"~Q~uit",      s[10],0,     "Quit from editor",
				 NULL);
#endif
	return(s[MAIN_MENU]);
}

static IT *ItemCreat(char *s,Menu *m,int c,char *h)
{
	IT  *i;

	i=(IT *)malloc(sizeof(IT));
	bzero(i,sizeof(IT));
	if(s!=NULL && *s) i->s=s;
	memcpy(&i->m,&m,sizeof(Menu *));
	i->m=m; i->c=c;
	if(h!=NULL && *h) i->h=h;
	i->active=check_active(c);
	return(i);
}

static int ItemNext(Menu *m,int i,int arg)
{
	for(;m->it[i]->s==NULL || !m->it[i]->active;
	     i=(arg?((i==m->num-1)?0:i+1):(!i?m->num-1:i-1)));
	return(i);
}

static void ItemDrawSub(IT *i,int x,int y,int len,int cola)
{
	dpp(x,y);
	if(i->s==NULL) {
		dpp(x-1,y); dpo('├'); dpn(len,'─'); dpo('┤');
	} else {
		int p,t,k,v;

		dpp(x,y);
		for(k=t=p=v=0;v<len;p++) {
			if(t || (!t && !i->s[p])) {
				dpo(' ');
				if(!i->s[p]) t=1;
			} else {
				if(i->s[p]=='~') {
					if(i->active)
						if(!k)  {
							k=setcolor(-1); setcolor((cola&0x000f)|(k&0xfff0));
						} else {
							setcolor(k); k=0;
						}
					continue;
				}
				dpo(i->s[p]);
			}
			v++;
		}
		if(k) setcolor(k);
	}
	if(i->m!=NULL) {
		dpp(x+len-1,y); dpo('>');
	}
}

static void ItemDrawBar(IT *i,int x,int y,int cola)
{
	int j,k;

	dpp(x,y);
	for(k=j=0;i->s[j];j++) {
		if(i->s[j]=='~')  {
			if(i->active)
				if(!k) {
					k=setcolor(-1); setcolor((cola&0x000f)|(k&0xfff0));
				} else {
					setcolor(k); k=0;
				}
			continue;
		}
		dpo(i->s[j]);
	}
}

Menu *MenuBarCreat(char *t,...)
{
	Menu *m;
	va_list ap;

	m=(Menu *)malloc(sizeof(Menu));
	bzero(m,sizeof(Menu));
	va_start(ap,t);
	for(m->num=0;;m->num++) {
		char *ch;

		if(m->num && va_arg(ap,char *)==NULL) break;
		va_arg(ap,Menu *);
		va_arg(ap,int);
		va_arg(ap,char *);
	}
	va_end(ap);
	if(!m->num) { Free(m); return(NULL); }
	m->it=(IT **)malloc(m->num*sizeof(IT *));
	va_start(ap,t);
	for(m->cur=0;m->cur<m->num;m->cur++) {
		IT *i;
		char *ch1,*ch4;
		Menu *ch2;
		int ch3;

		ch1=!m->cur?t:va_arg(ap,char *);
		ch2=va_arg(ap,Menu *);
		ch3=va_arg(ap,int);
		ch4=va_arg(ap,char *);
		i=ItemCreat(ch1,ch2,ch3,ch4);
		if(i->s) m->len=max(strlenm(i->s)+2,m->len);
		m->it[m->cur]=i;
	}
	va_end(ap);
	m->cur=0; m->area=-1; m->colcur=COLMSTR;  m->colno=COLMNOACT;
	return(m);
}

static void MenuBarDraw(Menu *m)
{
	int len,k,i,j;

	len=l_x/m->num;
	if(m->area<0) {
		m->area=get_box(0,m->y,l_x+1,1);
		m->areah=get_box(0,l_y,l_x+1,1);
	}
	Set_New_Color(COLMBGR);
	dpp(m->x,m->y);
	dpn(l_x+1-m->x,' ');
	for(j=0,k=m->x+1;j<m->num;k+=len,j++) {
		int l;

		if(j==m->cur)
			setcolor(m->colcur);
		else
			if(!m->it[j]->active)
				setcolor(m->colno);
		ItemDrawBar(m->it[j],l=k+((len-strlenm(m->it[j]->s))>>1),m->y,COLMACT);
		if(j==m->cur) {
			i=l; Set_New_Color(COLMBGR);
			down_mes(m->it[j]->h,COLCSTL,COLCSTL);
		} else {
			if(!m->it[j]->active)
				Set_New_Color(COLMBGR);
//                                setcolor(COLMBGR);
		}

	}
	dpp(i,m->y);
}

int MenuBarRun(int y,Menu  *m)
{
	int c,arg=0;

	m->x=0; m->y=y;
	if(m->next!=NULL) {
		for(c=0;c<m->num;c++)
			if(m->next==m->it[c]->m) {
				m->cur=c; c=1000; goto SWITCH;
			}
		c=0;
	}
	MenuBarDraw(m);
	while(1) {
		if(c!=1000) c=ReadCommand(NULL,-1);
SWITCH:
		switch(c) {
		case C_STRRIGHT:
			if(m->cur<m->num-1)
				m->cur++;
			else
				m->cur=0;
			m->cur=ItemNext(m,m->cur,1);
			if(arg) { c=1000; continue; }
			break;
#ifdef X11
		case C_MOUSE:
			switch(ev.b) {
			case 1:
				if(IF_MB(m,ev)) {
					if(m->cur==ev.x*m->num/l_x) {
						c=C_RETURN; goto SWITCH;
					}
					m->cur=ev.x*m->num/l_x;
				}
				break;
			case 2:
			case 3:
				goto END;
			}
			break;
#endif
		case C_STRLEFT:
			if(m->cur>0)
				m->cur--;
			else
				m->cur=m->num-1;
			m->cur=ItemNext(m,m->cur,0);
			if(arg) { c=1000; continue; }
			break;
		case C_STOP:
		        c=RES1; goto END;
		case C_RETURN:
		case 1000:
			if(m->it[m->cur]->m!=NULL) {
				int xx;
				Menu *mm=m->it[m->cur]->m;

				arg=1;
				m->colcur=invers(m->colcur);
				MenuBarDraw(m);
				xx=x_c-2;
				if(xx<0)
					xx=1;
				else
					if(xx+mm->len+2>l_x) xx=l_x-mm->len-2;
				m->next=mm; mm->last=m;
				c=_SubMenuRun(xx,m->y+1,mm);
				m->colcur=invers(m->colcur);
				MenuBarDraw(m);
				switch(c) {
				case C_STRLEFT:
				case C_STRRIGHT:
					arg=1; goto SWITCH;
				case RES1:
					arg=0; continue;
#ifdef X11
				case C_MOUSE:
					if(IF_MB(m,ev))
						goto SWITCH;
					else
						goto END;
#endif
				default:
					goto END;
				}
			} else

				if(!arg || c==C_RETURN) {
					c=m->it[m->cur]->c; m->next=NULL;
					goto END;
				}
				else {
					MenuBarDraw(m); c=0;
				}
			continue;
		default:
			if(FindActive(m,c)<0)  {
				dpo(bl); continue;
			}
		}
		MenuBarDraw(m);
	}
END:
	put_box(m->x,m->y,m->area); free_box(m->area);
	put_box(0,l_y,m->areah); free_box(m->areah);
	m->area=-1;
	return(c);
}

Menu *SubMenuCreat(char *t,...)
{
	Menu *m;
	va_list ap;

	m=(Menu *)malloc(sizeof(Menu));
	bzero(m,sizeof(Menu));
	va_start(ap,t);
	for(m->num=0;;m->num++) {
		if(m->num && va_arg(ap,char *)==NULL) break;
		va_arg(ap,Menu *);
		va_arg(ap,int);
		va_arg(ap,char *);
	}
	va_end(ap);
	if(!m->num) { Free(m); return(NULL); }
	m->it=(IT **)malloc(m->num*sizeof(IT *));
	va_start(ap,t);
	for(m->cur=0;m->cur<m->num;m->cur++) {
		IT *i;
		char *ch1,*ch4;
		Menu *ch2;
		int ch3;

		ch1=!m->cur?t:va_arg(ap,char *);
		ch2=va_arg(ap,Menu *);
		ch3=va_arg(ap,int);
		ch4=va_arg(ap,char *);
		i=ItemCreat(ch1,ch2,ch3,ch4);
		if(i->s)
			m->len=max(strlenm(i->s)+(i->m!=NULL?2:0),m->len);
		m->it[m->cur]=i;
	}
	va_end(ap);
	m->cur=0; m->area=-1; m->colcur=COLMSTR; m->colno=COLMNOACT;
	return(m);
}

static void SubMenuDraw(Menu *m)
{
	int k,i;

	if(m->area<0) {
		m->area=get_box(m->x,m->y,m->len+3,m->num+3);
		m->areah=get_box(0,l_y,l_x+1,1);
	}
	Set_New_Color(COLMBGR);
	wind(m->x,m->y,m->len+2,m->num+2);
	for(k=0;k<m->num;k++)
	{
		if(k==m->cur) {
			i=k; setcolor(m->colcur);
		} else
			if(!m->it[k]->active)
				setcolor(m->colno);
		ItemDrawSub(m->it[k],m->x+1,m->y+1+k,m->len,COLMACT);
		if(k==m->cur) {
			Set_New_Color(COLMBGR);
			/*down_mes(m->it[k]->h,COLCSTL,COLCSTL);*/
		} else
			if(!m->it[k]->active)
				Set_New_Color(COLMBGR);
	}
	dpp(m->x+1,m->y+i+1);
}

int SubMenuRun(int x,int y,Menu *m)
{
	int c;

	while(1) {
		switch(c=_SubMenuRun(x,y,m)) {
		case C_STRLEFT:
		case C_STRRIGHT:
			continue;
		default:
			return(c);
		}
	}
}

static int _SubMenuRun(int x,int y,Menu  *m)
{
	int c;

	m->x=x; m->y=y;
	if(m->next!=NULL) {
		for(c=0;c<m->num;c++)
			if(m->next==m->it[c]->m) {
				m->cur=c; c=C_RETURN; goto SWITCH;
			}
		c=0;
	}
	m->cur=ItemNext(m,m->cur,1);
	SubMenuDraw(m);
	while(1)
	{
		c=ReadCommand(NULL,-1);
SWITCH:
		switch(c) {
#ifdef X11
		case C_MOUSE:
			switch(ev.b) {
			case 1:
				if(IF_SW(m,ev))
				{
					if(m->cur==ev.y-m->y-1) {
						c=C_RETURN; goto SWITCH;
					}
					if(m->it[ev.y-m->y-1]->s==NULL)
						continue;
					m->cur=ev.y-m->y-1;
				}
				else {
					Menu *mm;
					int ii;

					for(ii=0,mm=m->last;mm!=NULL;ii++,mm=mm->last) {
						if(mm->last==NULL) {
							if(IF_MB(mm,ev)) goto END;
						}
						else
							if(IF_SW(mm,ev)) goto END;
					}
				}
				break;
			case 2:
			case 3:
				c=RES1; goto SWITCH;
			}
			break;
#endif
		case C_DOWN:
			if(m->cur<m->num-1)
				m->cur++;
			else
HOME:
				m->cur=0;
			m->cur=ItemNext(m,m->cur,1);
			break;
		case C_STRHOME:
			goto HOME;
		case C_STREND:
			goto LAST;
		case C_UP:
			if(m->cur>0)
				m->cur--;
			else
LAST:
				m->cur=m->num-1;
			m->cur=ItemNext(m,m->cur,0);
			break;
		case C_STOP:
			c=RES1;
			goto END;
		case C_RETURN:
			if(m->it[m->cur]->m!=NULL) {
				int xx,yy;
				Menu *mm=m->it[m->cur]->m;
				int col;

				m->colcur=invers(m->colcur);
				SubMenuDraw(m);
				if(m->x+m->len+mm->len+6>l_x)
					xx=m->x-mm->len-4;
				else
					xx=m->x+m->len+3;
				if(m->y+m->cur+mm->num+5>l_y)
					yy=m->y+m->cur-mm->num-1;
				else
					yy=m->y+m->cur+2;
				m->next=mm; mm->last=m;
				c=SubMenuRun(xx,yy,mm);
				m->colcur=invers(m->colcur);
				SubMenuDraw(m);
				switch(c) {
				case RES1:
					m->next=0; mm->last=0;
					break;
#ifdef X11
				case C_MOUSE:
					m->next=0; mm->last=0;
					if(IF_SW(m,ev)) {
						m->cur=-1; goto SWITCH;
					}
					else
						goto END;
#endif
				default:
					goto END;
				}
			}
			else {
				c=m->it[m->cur]->c; goto END;
			}
			continue;
		case C_STRLEFT:
		case C_STRRIGHT:
			goto END;
		default:
			if(FindActive(m,c)<0)  {
				dpo(bl); continue;
			}
		}
		SubMenuDraw(m);
	}
END:
	put_box(m->x,m->y,m->area); free_box(m->area);
	put_box(0,l_y,m->areah); free_box(m->areah);
	m->area=-1;
	return(c);
}

int strlenm(char *str)
{
	int i,j;

	for(j=i=0;str[i];i++) if(str[i]!='~') j++;
	return(j);
}

int FindActive(Menu *m,int c)
{
	int j;
	unsigned char *ch;

	for(j=0;j<m->num;j++) {
		if(m->it[j]->s!=NULL && (ch=(unsigned char *)strchr(m->it[j]->s,'~'))!=NULL)
			if(ch[1]==tolower(c) || ch[1]==toupper(c))
				return(m->cur=j);
	}
	return(-1);
}

void MenuBarFree(Menu *m)
{
	SubMenuFree(m);
}

void SubMenuFree(Menu *m)
{
	int i;

	for(i=0;i<m->num;i++) {
		if(m->it[i]->m!=NULL) SubMenuFree(m->it[i]->m);
		ItemFree(m->it[i]);
	}
	Free(m->it); Free(m);
}

void ItemFree(IT *i)
{
	Free(i);
}

static int check_active(int i)
{
	int ret=1;

	switch(i) {
	case C_QWRITE:
		if(!LenAllFiles || !AllFiles[LenAllFiles-1].AS->red)
			ret=0;
		break;
	case C_UNDO:
		if(!LenAllFiles || !AllFiles[LenAllFiles-1].AS->CurU)
			ret=0;
		break;
	case M_BLOCK:
	case M_GOTO:
	case C_MOZAIK:
	case C_CASCAD:
	case C_COLOUR:
	case M_GAB:
	case C_NEXTWIND:
	case C_BACKWIND:
	case C_SELWIND:
	case C_CLOSE:
		if(!LenAllFiles) ret=0;
		break;
	case C_PGRAPH:
	case C_EXEFILTR:
	case C_REPLACE:
	case M_FORMAT:
	case M_CARR:
		if(!LenAllFiles || AllFiles[LenAllFiles-1].rdonly) ret=0;
		break;
	default:
		ret=1;
	}
	return(ret);
}

static char *GetLastUndoMode()
{
	char *ch;

	if(!LenAllFiles || !AllFiles[LenAllFiles-1].AS->CurU)
#ifdef RUSDIAG
		ch="Нечего восстанавливать";

#else
		ch="Can't Undo";
#endif
	else
		switch(AllFiles[LenAllFiles-1].AS->U[AllFiles[LenAllFiles-1].AS->CurU-1].mode&U_MASK)
		{
#ifdef RUSDIAG
		case U_DELETE:
			ch="~В~осстановить удаленное"; break;
		case U_INSERT:
			ch="~О~тменить вставку"; break;
		case U_CHANGE:
			ch="~В~осстановить редактированное"; break;

#else
		case U_DELETE:
			ch="~U~ndo Delete"; break;
		case U_INSERT:
			ch="~U~ndo Insert"; break;
		case U_CHANGE:
			ch="~U~ndo Edit"; break;
#endif
		}
	return(ch);
}

Set_New_Color(int color)
{
	struct s_tdescr t,dptd();

	t=dptd();
	if(t.ct==15)
	{
	 Set_Color(24|((color&0x400)>>2),016);
	}
	else
	   setcolor(color);
}
