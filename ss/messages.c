/*
	Copyright (c) 1990-1995 Unitex+, ltd. Kiev, Ukraine
	Copyright (c) 1996 SOFTEX PLUS inc. Toronto ON, Canada.
	Copyright (c) 1996-2000 SanBase inc. Toronto ON, Canada.
	Written by Alexander L. Lashenko (1990-2000)
			   and M. Flerov (1986-1988).

	E-mail: lashenko@unixspace.com
	http://www.unixspace.com/tools

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

	Last modification:Fri Mar 23 14:25:39 2001
			Module:messages.c
*/

char *head="Autor: Alexander Lashenko (C)1996 SOFTEX+ Toronto ON.E-mail:lashenko@unixspace.com";
char *messages[] =
{
#ifdef RUSDIAG
/* 0*/  "�� �����稫�? ",
/* 1*/  "�� ᢨ�����!  ",
/* 2*/  "��࠭�:",
/* 3*/  "�ᥣ�: ",
/* 4*/  "F1 ������ F2 ��⪠ F4 ।���� F5 ᥫ���� F6 ०�� F10 ��室 F12 �������.",
/* 5*/  "��⪠",
/* 6*/  "��ᬮ��",
/* 7*/  "०��",
/* 8*/  "��室",
/* 9*/  "��ॢ�",
/*10*/  "������� ",
/*11*/  "�� ����� �ࠢ�",
/*12*/  "��� ⠪��� 䠩�� ��� ��⠫���",
/*13*/  "��� ⠪��� �����",
/*14*/  "�訡�� �����/�뢮��",
/*15*/  "��� ⠪��� �-�� ��� ���� ",
/*16*/  "�ॢ�襭�� ᯨ᪠ ��㬥�⮢ ",
/*17*/  "���ࠢ���� ����� 䠩�� ",
/*18*/  "����� �������� ����� ",
/*19*/  "��� ᢮������ �����",
/*20*/  "����襭�� �����",
/*21*/  "������ ���� ���筮� �-��",
/*22*/  "�-�� �����",
/*23*/  "䠩� 㦥 �������",
/*24*/  "��� ����� ���ன�⢠��",
/*25*/  "��� ⠪��� ���ன�⢠",
/*26*/  "�� ��⠫��",
/*27*/  "��⠫��",
/*28*/  "���ࠢ���� ��㬥��",
/*29*/  "⠡��� 䠩��� ��९������",
/*30*/  "᫨誮� ����� ������� 䠩���",
/*31*/  "�� �ନ���",
/*32*/  "ࠧ���塞� 䠩� �����",
/*33*/  "䠩� ᫨誮� �����",
/*34*/  "��� ᢮������� ����࠭�⢠",
/*35*/  "䠩����� ��⥬� ����� ⮫쪮 ��� �⥭��",
/*36*/  "᫨誮� ����� �痢�",
/*37*/  "���ࠢ���� ����",
/*38*/  "�������⭮ ������ ��� ����祭��� 䠩���",
/*39*/  "��⠫��: ",
/*40*/  "����.䠩� c: ",
/*41*/  "����.䠩� b: ",
/*42*/  "�����: ",
/*43*/  "CX ����: ",
/*44*/  "����. ���.: ",
/*45*/  "����⮢�: ",
/*46*/  "������⥪�: ",
/*47*/  "SH �ண�.: ",
/*48*/  "�����: ",
/*49*/  " ࠧ�.=",
/*50*/  " ����.=",
/*51*/  " ���.=",
/*52*/  " ���.=",
/*53*/  "---��⮢�---",
/*54*/  " �� ������",
/*55*/  " ��� �����襭�� (",
/*56*/  "���誮� ������� ��ப� ��㬥�⮢",
/*57*/  "����� ���஢�� 䠩��� ��⠫���:",
/*58*/  "a    - ���஢�� �� ��䠢���",
/*59*/  "u    - ���஢�� �� ���������",
/*60*/  "s[S] - ���஢�� �� ࠧ����",
/*61*/  "d[D] - ���஢�� �� ���",
/*62*/  "       ������ ������ ��⠫���:",
/*63*/  "s   - �������� [���] ࠧ���� 䠩���",
/*64*/  "a   - �������� [���] �� 䠩�� ",
/*65*/  "c   - �������� [���] 梥⮬ ⨯ 䠩���",
/*66*/  "f   - ����� ����� (⨯,���,ࠧ����)",
/*67*/  "�訡��: ",
/*68*/  "�������: ",
/*69*/  "��� ��諠 ",
/*70*/  "ScreenShell: Good Bye.",
/*71*/  "����",
/*72*/  "Ctrl(@)\nCtrl(A)\nCtrl(B)\nF12\nF3\nCtrl(N)\nCtrl(F)\nCtrl(G)\nBackSp\nTab\nCtrl/Ent\nF1\nF2\nEnter\nF4\nF5\nF6\nF7\nEnd\nHome\n����\n�����\n-->\n<--\nInsert\nDelete\nPagDn\nPagUp\nF8\nF9\nF10\nF11\n",
/*73*/  "���������: ",
/*74*/  "��뫪�: ",
/*75*/  "���� ������: ",
/*76*/  "Up �।���� Down ᫥�����_�������� F7 ���� Enter �믮����� F10 ��室",
/*77*/  "F1 man Page/Up ��७��� Page/Do ᪮��஢��� Delete �⬥���� Tab �믮����� F12 �������",
/*78*/  "��������. �� 㢥७�?"
#else
/* 0*/  "Quit! Are you sure?",
/* 1*/  "Good bye.          ",
/* 2*/  "Select:",
/* 3*/  "Total: ",
/* 4*/  "F1 help F2 mark F4 edx F5 select F6 mode F8 delete Tab exe F10 exit F12 command",
/* 5*/  "Mark ",
/* 6*/  "View/Edit",
/* 7*/  "Show_mode",
/* 8*/  "Exit",
/* 9*/  "Tree",
/*10*/  "SHell",
/*11*/  "You are not superuser!",
/*12*/  "can't found the file",
/*13*/  "process is absent",
/*14*/  "write/read error",
/*15*/  "can't open device",
/*16*/  "argument's line too long",
/*17*/  "number of file is wrong",
/*18*/  "can't execut the process",
/*19*/  "not free memory",
/*20*/  "securiry error",
/*21*/  "must be a block-device",
/*22*/  "device is buzy",
/*23*/  "file is present alredy",
/*24*/  "connect ",
/*25*/  "not device",
/*26*/  "not directory",
/*27*/  "directory",
/*28*/  "argument is wrong",
/*29*/  "file tables is buzy",
/*30*/  "too many open files",
/*31*/  "it isn't terminal",
/*32*/  "shared file is buzy",
/*33*/  "file is too large",
/*34*/  "no free space",
/*35*/  "file system read only",
/*36*/  "links too many",
/*37*/  "address is wrong",
/*38*/  "don't know full names",
/*39*/  "Directory: ",
/*40*/  "Special file c: ",
/*41*/  "Special file b: ",
/*42*/  "Chanel: ",
/*43*/  "???: ",
/*44*/  "Execute.: ",
/*45*/  "Text: ",
/*46*/  "Library: ",
/*47*/  "SH programm.: ",
/*48*/  "Data: ",
/*49*/  " size.=",
/*50*/  " user.=",
/*51*/  " mode.=",
/*52*/  " group.=",
/*53*/  "---OK---",
/*54*/  " cannot find",
/*55*/  " exit (",
/*56*/  "Line of arguments is too long",
/*57*/  "Select sortinig mode:",
/*58*/  "a    - sorting by ABC",
/*59*/  "u    - sorting by users",
/*60*/  "s[S] - sorting by size",
/*61*/  "d[D] - sorting by date",
/*62*/  "   Select show mode:",
/*63*/  "s   - show [no] size of files",
/*64*/  "a   - show [no] all files ",
/*65*/  "c   - show [no] types of files",
/*66*/  "f   - full show (mode,date,size)",
/*67*/  "Error: ",
/*68*/  "Closed: ",
/*69*/  "You have ",
/*70*/  "ScreenShell: Good Bye.",
/*71*/  "local mail",
/*72*/  "Ctrl(@)\nCtrl(A)\nCtrl(B)\nF12\nF3\nCtrl(N)\nCtrl(F)\nCtrl(G)\nBackSp\nTab\nCtrl/Ent\nF1\nF2\nEnter\nF4\nF5\nF6\nF7\nEnd\nHome\nDown\nUp\n-->\n<--\nInsert\nDelete\nPagDn\nPagUp\nF8\nF9\nF10\nF11\n",
/*73*/  "unknown: ",
/*74*/  "symlink: ",
/*75*/  "Data Base: ",
/*76*/  "Up last_command Down next_command F7 menu_command Enter execut F10 exit",
/*77*/  "F1 man F2 mark Page/Up move Page/Do copy Del erase Tab execut F12 command",
/*78*/  "Remove! Are you sure?"
#endif
};
