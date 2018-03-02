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

#include "commonheaders.h"

HINSTANCE hInstance;
PLUGINLINK *pluginLink;
HICON hConsoleIcon;

CRITICAL_SECTION mirPyCriticalSection;

char ModuleName[] = "MirPy";

PLUGININFOEX pluginInfo = {
					sizeof(PLUGININFOEX),
					__PLUGIN_DISPLAY_NAME,
					VERSION,
					__DESC,
					__AUTHOR,
					__AUTHOREMAIL,
					__COPYRIGHT,
					__AUTHORWEB,
					0,
					0,
#ifdef STATIC_RUNTIME_LIBRARY					
					{0x2fafc8da, 0x0b05, 0x40e9, {0xa1, 0xb4, 0x3b, 0x91, 0xfb, 0x19, 0xb7, 0x48}} //{2fafc8da-0b05-40e9-a1b4-3b91fb19b748}
#else
					{0x2facda05, 0x0edb, 0x424e, {0x80, 0x51, 0x04, 0x88, 0x17, 0xdc, 0xe2, 0x85}} //{2facda05-0edb-424e-8051-048817dce285}
#endif
};

OLD_MIRANDAPLUGININFO_SUPPORT;

extern "C" __declspec(dllexport) PLUGININFOEX *MirandaPluginInfoEx(DWORD mirandaVersion)
{
	if (mirandaVersion < (PLUGIN_MAKE_VERSION(0, 6, 0, 0)))
	{
		return NULL;
	}
		
	return &pluginInfo;
}

static const MUUID interfaces[] = {MIID_MIRPY, MIID_LAST};

extern "C" __declspec(dllexport) const MUUID *MirandaPluginInterfaces()
{
	return interfaces;
}

extern "C" __declspec(dllexport) int Load(PLUGINLINK *link)
{
	hConsoleIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CONSOLE));
	Log("%s", "Entering function " __FUNCTION__);
	pluginLink = link;
	
	Log("%s", "Initializing Miranda memory functions");
	InitializeMirandaMemFunctions();
	
	Log("%s", "Initializing events");
	InitEvents();
	
	Log("%s", "Initializing services");
	InitServices();
	
	Log("%s", "Creating critical section");
	InitializeCriticalSection(&mirPyCriticalSection);
	
	Log("%s", "Initializing python");
	InitPython();

	Log("%s", "Leaving function " __FUNCTION__);
	
	return 0;
}

extern "C" __declspec(dllexport) int Unload()
{
	Log("%s", "Entering function " __FUNCTION__);
	Log("%s", "Destroying python");
	DestroyPython();

	Log("%s", "Destroying events");
	DestroyEvents();
	
	Log("%s", "Destroying services");
	DestroyServices();
	
	Log("%s", "Destroying Miranda memory functions");
	DestroyMirandaMemFunctions();
	
	Log("%s", "Destroying critical section");
	DeleteCriticalSection(&mirPyCriticalSection);
	
	Log("%s", "Leaving function " __FUNCTION__);
	
	return 0;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	hInstance = hinstDLL;
	if (fdwReason == DLL_PROCESS_ATTACH)
		{
			DisableThreadLibraryCalls(hinstDLL);
			LogInit();
		}
		
  return TRUE;
}
