/*
MirPy plugin for Miranda IM

Copyright © 2006-2008 Cristian Libotean

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef M_MIRPY_UTILS_H
#define M_MIRPY_UTILS_H

#include "commonheaders.h"

#define LOG_FILE "MirPy.log"

#define ANCHOR_LEFT     0x000001
#define ANCHOR_RIGHT		0x000002
#define ANCHOR_TOP      0x000004
#define ANCHOR_BOTTOM   0x000008
#define ANCHOR_ALL      ANCHOR_LEFT | ANCHOR_RIGHT | ANCHOR_TOP | ANCHOR_BOTTOM

int LogInit();
int Log(char *format, ...);

int Info(char *title, char *format, ...);

char *StrReplace(char *source, const char *what, const char *withWhat);
char *StrCopy(char *source, int index, const char *what, int count);
char *StrDelete(char *source, int index, int count);
char *StrInsert(char *source, int index, const char *what);
char *StrTrim(char *szText, const char *szTrimChars);

void ScreenToClient(HWND hWnd, LPRECT rect);
void AnchorMoveWindow(HWND window, const WINDOWPOS *parentPos, int anchors);
RECT AnchorCalcPos(HWND window, const RECT *rParent, const WINDOWPOS *parentPos, int anchors);

#endif //M_MIRPY_UTILS_H