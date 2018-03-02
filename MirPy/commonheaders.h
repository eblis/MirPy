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

#ifndef M_MIRPY_COMMON_HEADERS_H
#define M_MIRPY_COMMON_HEADERS_H

#define _WIN32_WINNT 0x0501

#define FOLDER_MIRPY FOLDER_SCRIPTS "\\" "MirPy"
#define FOLDER_MIRPY_MODULES FOLDER_MIRPY "\\" "modules"
#define FOLDER_MIRPY_AUTOLOAD FOLDER_MIRPY "\\" "autoload"
#define FOLDER_MIRPY_SCRIPTS FOLDER_MIRPY

#ifdef _DEBUG 
	#undef _DEBUG 
	#include <Python.h>
	#define _DEBUG 
#else 
	#include <Python.h>
#endif 

#include <windows.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

#include "resource.h"
#include "version.h"

#include "mirandaMem.h"
#include "utils.h"
#include "services.h"
#include "events.h"
#include "replacements.h"
#include "dlgHandlers.h"
#include "PythonGlue.h"

#include "../../../include/newpluginapi.h"
#include "../../../include/m_database.h"
#include "../../../include/m_utils.h"
#include "../../../include/m_system.h"
#include "../../../include/m_skin.h"
#include "../../../include/m_options.h"
#include "../../../include/m_clist.h"
#include "../../../include/m_langpack.h"
#include "../../../include/m_history.h"
#include "../../../include/m_contacts.h"
#include "../../../include/m_utils.h"
#include "../../../include/m_popup.h"

#include "sdk/m_ieview.h"
#include "sdk/m_updater.h"
#include "sdk/m_folders.h"

#include "m_missing.h"

extern HICON hConsoleIcon; //console icon
extern char ModuleName[];
extern HINSTANCE hInstance; //dll instance
extern HWND hConsoleWnd;

extern PLUGININFOEX pluginInfo;
extern PLUGINLINK *pluginLink;
extern CRITICAL_SECTION mirPyCriticalSection;

extern HANDLE hAutoloadFolder;
extern HANDLE hModulesFolder;
extern HANDLE hScriptsFolder;

#define OLD_MIRANDAPLUGININFO_SUPPORT PLUGININFO oldPluginInfo = { \
	sizeof(PLUGININFO), \
	pluginInfo.shortName, \
	pluginInfo.version, \
	pluginInfo.description, \
	pluginInfo.author, \
	pluginInfo.authorEmail, \
	pluginInfo.copyright, \
	pluginInfo.homepage, \
	pluginInfo.flags, \
	pluginInfo.replacesDefaultModule \
}; \
\
extern "C" __declspec(dllexport) PLUGININFO *MirandaPluginInfo(DWORD mirandaVersion) \
{ \
	if (mirandaVersion < PLUGIN_MAKE_VERSION(0, 6, 0, 0)) \
	{ \
		return NULL; \
	} \
	return &oldPluginInfo; \
}

#endif //M_MIRPY_COMMON_HEADERS_H
