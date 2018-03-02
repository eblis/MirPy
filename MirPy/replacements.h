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

#ifndef M_MIRPY_REPLACEMENTS_H
#define M_MIRPY_REPLACEMENTS_H

#include "commonheaders.h"

typedef int (*MIRANDACALLSERVICE)(const char *, WPARAM, LPARAM);
typedef int (*MIRANDASERVICEEXISTS)(const char *);

extern MIRANDACALLSERVICE realCallService;
extern MIRANDACALLSERVICE realCallServiceSync;
extern MIRANDASERVICEEXISTS realServiceExists;

int HookReplacementFunctions();
int UnhookReplacementFunctions();

int MirPyCallServiceReplacement(const char *service, WPARAM wParam, LPARAM lParam);
int MirPyCallServiceSyncReplacement(const char *service, WPARAM wParam, LPARAM lParam);
int MirPyServiceExists(const char *service);
int MirPyCallServiceParam(WPARAM wParam, LPARAM lParam, LPARAM pyCallback);

#endif //M_MIRPY_REPLACEMENTS_H