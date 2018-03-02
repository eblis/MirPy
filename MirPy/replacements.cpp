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

#include "replacements.h"

MIRANDACALLSERVICE realCallService = NULL;
MIRANDACALLSERVICE realCallServiceSync = NULL;
MIRANDASERVICEEXISTS realServiceExists = NULL;

static CRITICAL_SECTION csServices;

int HookReplacementFunctions()
{
	InitializeCriticalSection(&csServices);

	Log("Hooking real CallService(). Replacing 0x%p with 0x%p", pluginLink->CallService, MirPyCallServiceReplacement);
	
	realCallService = pluginLink->CallService;
	realCallServiceSync = pluginLink->CallServiceSync;
	realServiceExists = pluginLink->ServiceExists;
	
	pluginLink->CallService = MirPyCallServiceReplacement;
	pluginLink->CallServiceSync = MirPyCallServiceSyncReplacement;
	pluginLink->ServiceExists = MirPyServiceExists;

	return 0;	
}

int UnhookReplacementFunctions()
{
	Log("Unhooking real CallService(). Replacing 0x%p with 0x%p", pluginLink->CallService, realCallService);
	if (realCallService)
	{
		pluginLink->CallService = realCallService;
	}
		
	if (realCallServiceSync)
	{
		pluginLink->CallServiceSync = realCallServiceSync;
	}
		
	if (realServiceExists)
	{
		pluginLink->ServiceExists = realServiceExists;
	}

	DeleteCriticalSection(&csServices);
	
	return 0;
}


int MirPyCallServiceReplacement(const char *service, WPARAM wParam, LPARAM lParam)
{
	int res = realCallService(service, wParam, lParam);
	if ((res == CALLSERVICE_NOTFOUND) && (IsPythonAvailable()) && (service))
		{
			EnterCriticalSection(&csServices);
			
			res = CallPythonService(service, wParam, lParam);
			
			LeaveCriticalSection(&csServices);
		}
	
	return res;
}

int MirPyCallServiceSyncReplacement(const char *service, WPARAM wParam, LPARAM lParam)
{
	int res = realCallServiceSync(service, wParam, lParam);
	if ((res == CALLSERVICE_NOTFOUND) && (IsPythonAvailable()) && (service))
		{
			EnterCriticalSection(&csServices);
			
			res = CallPythonService(service, wParam, lParam);
			
			LeaveCriticalSection(&csServices);
		}
	
	return res;
}

int MirPyServiceExists(const char *service)
{
	int exists = realServiceExists(service);
	if ((!exists) && (service))
	{
		exists = PythonServiceExists(service);
	}
	
	return exists;
}

int MirPyCallServiceParam(WPARAM wParam, LPARAM lParam, LPARAM pyCallback)
{
	int res = CALLSERVICE_NOTFOUND;
	PyObject *service = (PyObject *) pyCallback;
	if ((service) && (PyCallable_Check(service)))
	{
		res = CallPythonServiceParam(service, wParam, lParam);
	}
	
	return res;
}