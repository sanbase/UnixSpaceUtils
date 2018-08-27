#ifndef _TTY_CODES_H
#define _TTY_CODES_H
#include <strings.h>

struct clr  { unsigned char bg:6,atr2:2,fg:6,atr1:2;};
struct pics { unsigned short ch; struct clr clr;};
union fpic  { struct pics p; unsigned int f;};

struct s_dpd {short xdim, ydim, tflag;}  dpd();

struct area {unsigned char x,y,h,l; union fpic ar[1];};

struct s_tdescr
{
    struct s_dpd param;
    char         ct;
};

#define ctrl(x) ('x'&037)
#define s_spc   0x100
#define spec(x) ((x)|s_spc)

#define Esc        0
#define bb         1   /* включить рамку */
#define bf         2   /* убрать рамку */
#define F12        3
#define s_break    3  /* аварийный конец работы */
#define s_debug    4
#define F3         4
#define fl         5  /* рус/лат. */
#define s_finish   6  /* конец работы */
#define bl         7  /* гудок */
#define dc         8  /* удаление символа (опция) */
#define ta         9  /* табуляция */
#define lf        10  /* перевод курсора в 0-ую поз. след. строки */
#define s_arg     11  /* ввод аргумента - вместо '!' */
#define F1        11
#define F2        12
#define rn        13  /* перевод курсора в 0-ую поз. текущ.строки */
#define F4        14
#define F5        15
#define F6        16  /* строка позиционирования */
#define el        17  /* стирание конца строки */
#define F7        17
#define rd        18  /* прокрутить текст назад  End  */
#define ru        19  /* прокрутить текст вперед Home */
#define cd        20  /* сдвиг курсора вниз   */
#define cu        21  /* сдвиг курсора вверх  */
#define cr        22  /* сдвиг курсора вправо */
#define Cr        22  /* сдвиг курсора вправо */
#define cl        23  /* сдвиг курсора влево  */
#define ic        24  /* вставка  символа (опция) */
#define s_help    25  /* вызов подсказки */
#define il        26  /* вставка  строки (опция) PageDown*/
#define dl        27  /* удаление строки (опция) PageUp  */
#define es        28  /* стирание конца экрана */
#define F8        28
#define F9        29
#define ps        29  /* установка цвета Cf */
#define F10       30
#define F11       31
#define so        30  /* включение */
#define se        31  /*          и выключение инверсии изображения */
#define de        0177     /* забой */

/* S.O.------------*/
/* Символы обозначенные ??? зависят от системной реализации  */
#define CtrlA    1  /* эти два кода оставлены для совместимости          */
#define CtrlB    2  /* со старыми версиями библиотеки                    */
     /* CtrlC - прерывание с клавиатуры */
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
     /* CtrlN - первая альтернативная раскладка */
#define CtrlO    270   /* ??? */
#define CtrlP    271
     /* CtrlQ - продолжение вывода */
#define CtrlR    273
     /* CtrlS - прерывание вывода  */
#define CtrlT    275
     /* CtrlU - вторая альтернативная раскладка */
#define CtrlV    277  /* ??? */
#define CtrlW    278
#define CtrlX    279
#define CtrlY    280
#define CtrlZ    281
/* S.O.---------------*/


#define s_erase  es       /* уничтожение объекта */
#define s_refuse de       /* отказ от действия */
#define s_spec   ps       /* префикс к составным символам */

#define is_control(c) ((unsigned)(c) < ' ')
#define is_lower(c) (((c)&0140)==0140)
#define is_upper(c) (((c)&0140)==0100)
#define is_rus(c)   (((unsigned)(c)>=0200&&(unsigned)(c)<0260)||((unsigned)(c)>=0340&&(unsigned)(c)<0360 ))
#define is_print(c) ((unsigned)(c)>=' ' && (c)!=0234)
#define to_lower(c) ((c)|040)
#define to_upper(c) ((c)&(~040))


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
void dpo(unsigned short c );
int dpin();
int difstr(char *a,char *b);
int dpi();
void dpp(int x,int y);
void flush_dpi();
void s_dpo(unsigned short i);
void s_dpp(int x,int y );
void s_dps(const char *str);
struct clr color_to_clr(int i);
int clr_to_col(struct clr clr);
unsigned int get_s_color();
unsigned int setcolor(int i);
unsigned int Set_Color(int fon,int let);
unsigned int s_setcolor(int i);
void sdpo(char c );
void scrbufout();
void ind_lang();
/* screen.c */
void ini_screen();
void zero_box(int x,int y,int l,int h);
void clear_scr_old();
void put_scrold(char *buf);
void free_screen();
unsigned short get_ch(int x,int y);
int put_ch(int x,int y,unsigned short ch);
unsigned int get_fg(int x,int y);
unsigned int get_bg(int x,int y);
unsigned int get_a1(int x,int y);
unsigned int get_a2(int x,int y);
unsigned int get_clr(int x,int y);
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
void a_dpo(unsigned short c );
void dpo_scr(unsigned short c);
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
void box1(int x,int y,int l,int h,char ch,int bord_fg,int bord_bg,int text_fg,int text_bg);
void box2(int x,int y,int l,int h,char ch,int bord_fg,int bord_bg,int text_fg,int text_bg);
void BOX(int x,int y,int l,int h,char ch,int bord,int text);
void shadow(int x,int y,int l,int h);
void revers(int x,int y);
void goriz(int x,int y,int l);
void goriz_s(int x,int y,int l);
void vert(int x,int y,int h);
void vert_s(int x,int y,int h);
void krest(int x,int y);
/* edit.c */
int dpedit(unsigned char i,char *line,int size,int show,int x,int y,int arg);
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
/* void TtyFlush (); */
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
void Frame(char *);
void cursor_visible();
void cursor_invisible();
void New_Color(int i,int r,int g,int b);
void Set_Box_BG(int x,int y,int l,int h,int color);
void Init_Color();
void JMenu(char *menu);
void Del_Menu();
char *get_menu_item();
void Set_Title(char *);
void sdps(const char *str);
void read_byte(unsigned char *buf);

/* int Execvp(char *name, char *a1, char *a2, char *a3); */


struct ret_xy {unsigned char x,y,ch;};
struct ret_xy dpm(int l,int h,int c1,int c2,int c3,char *( (*dpms)(int,int)));
struct mouse {int b,x,y;};
#endif
