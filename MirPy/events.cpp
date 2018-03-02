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

#include "events.h"

HANDLE hModulesLoaded;
HANDLE hPreShutDown;

HANDLE hAutoloadFolder;
HANDLE hModulesFolder;
HANDLE hScriptsFolder;

//HANDLE hMissingServices;
//HANDLE hMissingEvents;


int InitEvents()
{
	hModulesLoaded = HookEvent(ME_SYSTEM_MODULESLOADED, OnModulesLoaded);
	hPreShutDown = HookEvent(ME_SYSTEM_PRESHUTDOWN, OnPreShutDown);
	
	return 0;
}

int DestroyEvents()
{
	UnhookEvent(hModulesLoaded);
	UnhookEvent(hPreShutDown);
	
	//UnhookEvent(hMissingServices);
	//UnhookEvent(hMissingEvents);
	
	return 0;
}

#define HOST "http://eblis.tla.ro/projects"

#ifdef STATIC_RUNTIME_LIBRARY
	#define MIRPY_UPDATE_DATA "MirPy%20(static)"
#else
	#define MIRPY_UPDATE_DATA "MirPy"
#endif
#define MIRPY_VERSION_URL HOST "/miranda/MirPy/updater/MirPy.html"
#define MIRPY_UPDATE_URL HOST "/miranda/MirPy/updater/" MIRPY_UPDATE_DATA ".zip"
#define MIRPY_VERSION_PREFIX "MirPy version "

int OnModulesLoaded(WPARAM wParam, LPARAM lParam)
{
	Log("%s", "Entering function " __FUNCTION__);
	Log("%s", "Creating main menu item");
	CLISTMENUITEM mi = {0};
	mi.cbSize = sizeof(CLISTMENUITEM);
	mi.position = 400050000;
	mi.hIcon = hConsoleIcon;
	mi.pszPopupName = Translate("MirPy");
	mi.pszName = Translate("MirPy console");
	mi.pszService = MS_MIRPY_CONSOLE_SHOW;
	CallService(MS_CLIST_ADDMAINMENUITEM, 0, (LPARAM) &mi);
	Log("%s", "Registering with updater");
	
	char buffer[1024];
	Update update = {0};
	update.cbSize = sizeof(Update);
	update.szComponentName = __PLUGIN_DISPLAY_NAME;
	update.pbVersion = (BYTE *) CreateVersionString(VERSION, buffer);
	update.cpbVersion = strlen((char *) update.pbVersion);
	update.szUpdateURL = UPDATER_AUTOREGISTER;
	update.szBetaVersionURL = MIRPY_VERSION_URL;
	update.szBetaUpdateURL = MIRPY_UPDATE_URL;
	update.pbBetaVersionPrefix = (BYTE *) MIRPY_VERSION_PREFIX;
	update.cpbBetaVersionPrefix = strlen(MIRPY_VERSION_PREFIX);
	CallService(MS_UPDATE_REGISTER, 0, (LPARAM) &update);
	
	Log("%s", "Registering folders");
	//FOLDERSDATA fd = {0};
	//fd.cbSize = sizeof(FOLDERSDATA);
	//strcpy(fd.szSection, "MirPy");
	//strcpy(fd.szName, Translate("Extra modules"));
	hModulesFolder = (HANDLE) FoldersRegisterCustomPath("MirPy", Translate("Extra modules"), FOLDER_MIRPY_MODULES);
	//hModulesFolder = (HANDLE) CallService(MS_FOLDERS_REGISTER_PATH, (WPARAM) FOLDER_MIRPY_MODULES, (LPARAM) &fd);
	//strcpy(fd.szName, Translate("Autoloaded scripts"));
	hAutoloadFolder = (HANDLE) FoldersRegisterCustomPath("MirPy", Translate("Autoloaded scripts"), FOLDER_MIRPY_AUTOLOAD);
	hScriptsFolder = (HANDLE) FoldersRegisterCustomPath("MirPy", Translate("Scripts"), FOLDER_MIRPY_SCRIPTS);
	//hAutoloadFolder = (HANDLE) CallService(MS_FOLDERS_REGISTER_PATH, (WPARAM) FOLDER_MIRPY_AUTOLOAD, (LPARAM) &fd);
	
	Log("%s", "Starting python");
	StartPython();
	
	//hMissingServices = HookEvent(ME_SYSTEM_MISSINGSERVICE, OnMissingService);
	//hMissingEvents = NULL;
	
	//hook missing services and events (python needs to be running).
	HookReplacementFunctions();
	
	
	Log("%s", "Leaving function " __FUNCTION__);
	return 0;
}

int OnPreShutDown(WPARAM wParam, LPARAM lParam)
{
	Log("%s", "Unhooking replacement functions");
	UnhookReplacementFunctions(); //TODO
	
	return 0;
}

//int OnMissingService(WPARAM wParam, LPARAM lParam)
//{
	//TMissingServiceParams *params = (TMissingServiceParams *) lParam;
	//int res = CallPythonService(params->name, params->wParam, params->lParam);
	//if (res == 0)
	//	{
	//		res = 1;
	//	}
	//if (res == CALLSERVICE_NOTFOUND)
	//	{
	//		res = 0;
	//	}
//	return 0;
//}