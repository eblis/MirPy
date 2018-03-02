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

#ifndef M_MIRPY_DIALOG_HANDLERS_H
#define M_MIRPY_DIALOG_HANDLERS_H

#include "commonheaders.h"

#define MIN_CONSOLE_WIDTH 200
#define MIN_CONSOLE_HEIGHT 495

struct TLogMessage{
	char *message;
	COLORREF color;
};

typedef TLogMessage *PLogMessage;

#define MIRPY_LOGMESSAGE (WM_USER + 10)

LRESULT CALLBACK InputEditSubclassProc(HWND, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIDSubclass, DWORD_PTR data);

BOOL CALLBACK ConsoleDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif