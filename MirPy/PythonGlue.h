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

#ifndef M_MIRPY_PYTHON_GLUE_H
#define M_MIRPY_PYTHON_GLUE_H

#include "commonheaders.h"
#include "MirPymodule.h"

#include <richedit.h>

#define LOG_MESSAGE						1
#define LOG_ERROR							2
#define LOG_OUTPUT						3
#define LOG_INFO							4
#define LOG_UNKNOWN						100

#define COLOR_MESSAGE RGB(0, 0, 0)
#define COLOR_ERROR   RGB(255, 0, 0)
#define COLOR_OUTPUT  RGB(0, 0, 255)
#define COLOR_INFO    RGB(65, 135, 0)
#define COLOR_UNKNOWN RGB(128, 128, 128)

typedef int (*MIRANDAHOOK)(WPARAM,LPARAM);


extern DWORD dwMainId;

int InitPython();
int StartPython();
int StopPython();
int DestroyPython();

int IsPythonAvailable();

/***************Python modules*****************/

int IsConsoleAvailable();
int ConsoleRestart();
int ConsoleShow(int show = SW_SHOW);
int ConsoleClear();
int ConsoleLog(char *message, int type = LOG_MESSAGE);
int ConsoleLogColor(char *message, COLORREF color);
int CallMirandaService(const char *service, WPARAM wParam, LPARAM lParam);
int HookMirandaEvent(char *service, MIRANDAHOOK callback);
int UnhookMirandaEvent(HANDLE hookHandle);
char *MirandaTranslate(const char *szEnglish);
int CallPythonService(const char *service, WPARAM wParam, LPARAM lParam);
int CallPythonServiceParam(PyObject *callback, WPARAM wParam, LPARAM lParam);
int PythonServiceExists(const char *service);

#endif //M_MIRPY_PYTHON_GLUE_H