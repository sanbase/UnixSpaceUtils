/* �������� ���������� � ��������� ��������� */


#define maxrun 4                 /* �᫮ ��ࠫ������ ����ᮢ */
#define x_run  3                 /* ������ ����� �� �࠭� */
#define w_run  (D_NAME+5)        /* �ਭ� �⮫�� */

int   ch_flag;                   /* �஢����� �� ������ */

struct run {
    int  r_pid;                  /* ���ਯ�� ����� */
    int  r_code;                 /* ��� ������ �� �����襭�� */
    int  r_mark;                 /* ����⪠ (⮫쪮 -) */
    char r_name[D_NAME+1];       /* ��� 䠩�� */
} *at_run;
