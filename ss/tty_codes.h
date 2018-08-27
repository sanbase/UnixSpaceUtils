/* ����� �������� ��� ������ � ������� ���������
 *
 * $Log:	tty_codes.h,v $
 * Revision 1.1  88/09/26  16:29:38  guba
 * Initial revision
 *
 *
 * T�窨 �室�:
 *    dpbeg()     ���樠������ �࠭���� ०���
 *    dpend()     �����襭�� �࠭���� ०���
 *    {⨯}=dpd() ᮮ�頥� ��ࠬ���� �ନ����:
 *                   xdim    ࠧ��� �࠭� �� ��ਧ��⠫�
 *                   ydim    ࠧ��� �࠭� �� ���⨪���
 *                   tflag   䫠�� ���������⥩ �ନ����
 *    dpo(c)      �뢮� ������ ᨬ����
 *    c=dpi()     ���� ������ ᨬ����
 *    c=dpa()     �ᨭ�஭�� ���� ������ ᨬ����,
 *                   <0 �᫨ ᨬ��� �� �� ������
 *    dpp(x,y)    ��⠭�� ����� � ������ x,y
 *    dps(s)      �뢮� ��ப� ᨬ����� �� '\0'
 *    dpn(n,c)    �뢮� 'n' ࠧ ᨬ���� 'c'
 */
#ifndef _TTY_CODES_H
#define _TTY_CODES_H
#include <strings.h>
#ifndef    tty_codes_h
#   define tty_codes_h

struct clr  { unsigned char bg:6,atr2:2,fg:6,atr1:2;};
struct pics { unsigned char ch; struct clr clr;};


union fpic  { struct pics p; unsigned int f;};

struct s_dpd {short xdim, ydim, tflag;}  dpd();

struct area {unsigned char x,y,h,l; union fpic ar[1];};

struct s_tdescr
{                     /* ��������� �������� ��������� */
    struct s_dpd param;               /* ��ࠬ���� �ନ���� */
    char         ct;                  /* �᫨ �� 0 - 梥⭮� */
};

/* ��������� ������������ ��������� */

#define ctrl(x) ('x'&037)
#define no_symb (-1)         /* �訡��, ᨬ���� ��� */
#define MAXREPT 255          /* ����. �����樥�� ����७�� */
#define s_spc   0x100        /* �ਧ���, �� ������ s_spec */
#define spec(x) ((x)|s_spc)  /* ������ s_spec + ᨬ��� */

#define Esc        0
#define bb         1   /* ������� ࠬ�� */
#define bf         2   /* ���� ࠬ�� */
#define F12        3
#define s_break    3  /* ���਩�� ����� ࠡ��� */
#define s_debug    4
#define F3         4
#define fl         5  /* ���/���. */
#define s_finish   6  /* ����� ࠡ��� */
#define bl         7  /* �㤮� */
#define dc         8  /* 㤠����� ᨬ���� (����) */
#define ta         9  /* ⠡���� */
#define lf        10  /* ��ॢ�� ����� � 0-�� ���. ᫥�. ��ப� */
#define s_arg     11  /* ���� ��㬥�� - ����� '!' */
#define F1        11
#define F2        12
#define rn        13  /* ��ॢ�� ����� � 0-�� ���. ⥪��.��ப� */
#define F4        14
#define F5        15
#define F6        16  /* ��ப� ����樮��஢���� */
#define el        17  /* ��࠭�� ���� ��ப� */
#define F7        17
#define rd        18  /* �ப����� ⥪�� �����  End  */
#define ru        19  /* �ப����� ⥪�� ���। Home */
#define cd        20  /* ᤢ�� ����� ����   */
#define cu        21  /* ᤢ�� ����� �����  */
#define cr        22  /* ᤢ�� ����� ��ࠢ� */
#define cl        23  /* ᤢ�� ����� �����  */
#define ic        24  /* ��⠢��  ᨬ���� (����) */
#define s_help    25  /* �맮� ���᪠��� */
#define il        26  /* ��⠢��  ��ப� (����) PageDown*/
#define dl        27  /* 㤠����� ��ப� (����) PageUp  */
#define es        28  /* ��࠭�� ���� �࠭� */
#define F8        28
#define F9        29
#define ps        29  /* ��⠭���� 梥� Cf */
#define F10       30
#define F11       31
#define so        30  /* ����祭�� */
#define se        31  /*          � �몫�祭�� �����ᨨ ����ࠦ���� */
#define de        0177     /* ����� */

/* S.O.------------*/
/* ������� ������祭�� ??? ������� �� ��⥬��� ॠ����樨  */
#define CtrlA    1  /* �� ��� ���� ��⠢���� ��� ᮢ���⨬���          */
#define CtrlB    2  /* � ���묨 ����ﬨ ������⥪�                    */
     /* CtrlC - ���뢠��� � ���������� */
#define CtrlD    259
#define CtrlE    260
#define CtrlF    261
#define CtrlG    262
#define CtrlH    dc
#define CtrlI    ta
#define CtrlJ    lf
#define CtrlK    266
#define CtrlL    267
#define CtrlM    rn
     /* CtrlN - ��ࢠ� ����ୠ⨢��� �᪫���� */
#define CtrlO    270   /* ??? */
#define CtrlP    271
     /* CtrlQ - �த������� �뢮�� */
#define CtrlR    273
     /* CtrlS - ���뢠��� �뢮��  */
#define CtrlT    275
     /* CtrlU - ���� ����ୠ⨢��� �᪫���� */
#define CtrlV    277  /* ??? */
#define CtrlW    278
#define CtrlX    279
#define CtrlY    280
#define CtrlZ    281
/* S.O.---------------*/


#define s_erase  es       /* 㭨�⮦���� ��ꥪ� */
#define s_refuse de       /* �⪠� �� ����⢨� */
#define s_spec   ps       /* ��䨪� � ��⠢�� ᨬ����� */

/* ������� ��� ������� �������� */

#define is_control(c) ((unsigned)(c) < ' ')
#define is_lower(c) (((c)&0140)==0140)
#define is_upper(c) (((c)&0140)==0100)
#define is_rus(c)   (((unsigned)(c)>=0200&&(unsigned)(c)<0260)||((unsigned)(c)>=0340&&(unsigned)(c)<0360 ))
#define is_print(c) ((unsigned)(c)>=' ' && (c)!=0234)
#define to_lower(c) ((c)|040)
#define to_upper(c) ((c)&(~040))

/* ����� ������������ ��������� */

#define a_icdc   1   /* ����� �뢮���� ic-dc */
#define a_ildl   2   /* ����� �뢮���� il-dl */
#define a_rd     4   /* ����� �뢮���� rd */
#define a_lower  8   /* ���� �����쪨� �㪢� */
#define a_font  16   /* ���� ࠧ�� ����� */

/* ����� ���������� ���������� ������*/

#define on_control   1   /* ����� �� ��� �����. ᨬ����*/
#define on_border    2   /* ����� �� ��室� �� �࠭��� ���*/
#define on_space     4   /* ����� �� �஡���, ⠡��樨,ic*/
#define on_icdc      8   /* ����� �� ��⠢��, 㤠����� ᨬ*/
#define on_no_digit 16   /* ����� �� �� ��� */

#endif

/* dpi.c */
#ifdef GCC
int dpbeg(char *name="");
#else
int dpbeg(char *name);
#endif
void dpinit();
void dpend();
struct s_dpd dpd();
struct s_tdescr dptd();
void dpo(char c );
int dpin();
int difstr(char *a,char *b);
int dpi();
void dpp(int x,int y);
void flush_dpi();
void s_dpo(int i);
void s_dpp(int x,int y );
void s_dps(char *str);
struct clr color_to_clr(int i);
int clr_to_col(struct clr clr);
unsigned get_s_color();
unsigned setcolor(int i);
unsigned  Set_Color(int fon,int let);
unsigned s_setcolor(int i);
void sdpo(char c );
void scrbufout();
void ind_lang();
/* screen.c */
void ini_screen();
void zero_box(int x,int y,int l,int h);
void clear_scr_old();
void put_scrold(char *buf);
void free_screen();
unsigned get_ch(int x,int y);
int put_ch(int x,int y,char ch);
unsigned get_fg(int x,int y);
unsigned get_bg(int x,int y);
unsigned get_a1(int x,int y);
unsigned get_a2(int x,int y);
unsigned get_clr(int x,int y);
struct clr get_clrn(int x,int y);
int put_fg(int x,int y,int color);
int put_fg_old(int x,int y,int color);
int put_bg(int x,int y,int color);
int put_bg_old(int x,int y,int color);
int put_a1(int x,int y,int atr1);
int put_a1_old(int x,int y,int atr1);
int put_a2(int x,int y,int atr2);
int put_clr(int x,int y,int color);
union fpic get_f(int x,int y);
int put_f(int x,int y,union fpic f);
int put_f_old(int x,int y,union fpic f);
void cp_screen();
void cp_screenold();
void screen_out();
void a_dpo(int c );
void dpo_scr(int c);
void screen_clear();
void clear_scr(int col);
void scroll_up(int y);
void screen_cl_end(int x,int y);
void zero_scrold();
/* area.c */
int get_box(int x,int y,int l,int h);
void free_box(int n);
int restore_box(int n);
int put_box(int x,int y,int n);
int clear_box(int x,int y,int n);
int get_h(int n);
int get_l(int n);
struct area *get_reg(int n);
/* boxes.c */
void box(int x,int y,int l,int h,char ch);
#ifdef X11
void box1(int x,int y,int l,int h,char ch,int bord_fg,int bord_bg,int text_fg,int text_bg);
void box2(int x,int y,int l,int h,char ch,int bord_fg,int bord_bg,int text_fg,int text_bg);
#else
void box1(int x,int y,int l,int h,char ch,int bord,int text);
void box2(int x,int y,int l,int h,char ch,int bord,int text);
#endif
void BOX(int x,int y,int l,int h,char ch,int bord,int text);
void shadow(int x,int y,int l,int h);
void revers(int x,int y);
void goriz(int x,int y,int l);
void goriz_s(int x,int y,int l);
void vert(int x,int y,int h);
void vert_s(int x,int y,int h);
void krest(int x,int y);
/* edit.c */
int edit(unsigned char i,char *line,int size,int show,int x,int y,int arg);
void ind_ins(int x,int y);
void clear_line(int x,int y,int l);
int seek_poz(int znak,int poz,int size);
void indstr(int ind,char *line,int poz,int x,int y,int size);
void inds(char *str,char l,int ch,int color);
int Xmouse(int i);

void dps(char *str );
void dpsn(char *str, int len);
void dpn(int n,char c );

void Ttyset ();
void Ttyreset ();
//void TtyFlush ();
void help(char *name,char symbol );

void Chart(int *value,int num,int x,int y,int l,int h,int color,int num_chart);
void Reper_Down();
void Reper_Up(int,int);
void Flush_Screen();
int get_Reper_x();
void DrawRectangle(int color,int x,int y,int l, double h);
void Del_Rectangles();
void DrawLine(int color,int x0,int y0,int x1, int y1);
void DrawVect(int x,int y);
void DrawCircle(int x,int y,int l, int h);
void DrawArc(int arc,int color);
void Del_Last_Line();
void Del_Chart();
void BlackWhite(int x,int y,int l,int h);
void MultiColor(int x,int y,int l,int h);

int tgetent(char *bp,char *name);
int tnchktc();
int tnamatch(char *np);
int tgetnum(char *id);
int tgetflag(char *id);
char *tgetstr(char *id,char **area);
char *tgoto(char *CM,int destcol,int destline);

void Show_Image(int x, int y, char *name, char *description);
#ifdef GCC
void Show_Image(int x, int y, char *name);
#endif
void Del_Image(int x,int y);
void Del_All_Images();
void Window(char *);
void cursor_visible();
void cursor_invisible();
void New_Color(int i,int r,int g,int b);
void Set_Box_BG(int x,int y,int l,int h,int color);
void Init_Color();
void Menu(char *menu);
void Del_Menu();
char *get_menu_item();

/* int Execvp(char *name, char *a1, char *a2, char *a3); */


struct ret_xy {unsigned char x,y,ch;};
struct ret_xy dpm(int l,int h,int c1,int c2,int c3,char *( (*dpms)(int,int)));
struct mouse {int b,x,y;};
#endif
