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

#include "PythonGlue.h"

static int pythonAvailable = 0;

DWORD dwMainId = NULL;

PyThreadState * mainThreadState = NULL;

int InitPython()
{
	Py_Initialize();
	PyEval_InitThreads();
	
	mainThreadState = PyThreadState_Get();
	
	//PyEval_ReleaseLock();
	
	dwMainId = GetCurrentThreadId();
	
	return 0;
}

void AddPath(char *buffer, char *path)
{
	strcat(buffer, "r\"");
	strcat(buffer, path);
	strcat(buffer, "\", ");
}

char *BuildPath(char *buffer, char *path, char *append)
{
	strcpy(buffer, path);
	int i = strlen(buffer);
	if (buffer[i - 1] != '\\')
		{
			if (append[0] != '\\')
				{
					strcat(buffer, "\\");
				}
		}
		else{
			if (append[0] == '\\')
				{
					buffer[i - 1] = '\0';
				}
		}
	strcat(buffer, append);
	return buffer;
}

void GetPath(HANDLE hFolder, char *buffer, int size, char *notFound)
{
	/*if ((hFolder) && (hFolder != (HANDLE) CALLSERVICE_NOTFOUND))
		{
			CallService(MS_FOLDERS_GET_PATH, (WPARAM) hFolder, (LPARAM) buffer);
		}
		else{
			strcpy(buffer, notFound);
		}*/
	FoldersGetCustomPath(hFolder, buffer, size, notFound);
}

void ShowPathInConsole(char *path)
{
	ConsoleLog("\t", LOG_INFO);
	ConsoleLog(path, LOG_INFO);
	ConsoleLog("\n", LOG_INFO);
}

int StartPython()
{
	pythonAvailable = 1;
	ConsoleRestart(); //show the console
	ConsoleLog(Translate("Loading MirPy ..."), LOG_MESSAGE);
	ConsoleLog("\n");
	char mirPath[1024];
	GetModuleFileName(NULL, mirPath, sizeof(mirPath));
  int i = strlen(mirPath) - 1;
	while (mirPath[i] != '\\')
		{
			mirPath[i--] = '\0';
		}
	char init[4096] = "import sys\nsys.path.extend([";
	char buffer[2048];
	char autoLoadPath[1024];
	char modulesPath[1024];
	char scriptsPath[1024];
	sprintf(buffer, "%s\n", Translate("Adding script folders to load path:"));
	ConsoleLog(buffer, LOG_INFO);
	AddPath(init, BuildPath(buffer, mirPath, "Plugins")); //add miranda\\plugins to the module search path (so it finds our dll)
	ShowPathInConsole(buffer);
	GetPath(hModulesFolder, modulesPath, 1024, "scripts\\MirPy\\modules");
	ShowPathInConsole(modulesPath);
	AddPath(init, modulesPath); //add the modules folder to the search path
	GetPath(hAutoloadFolder, autoLoadPath, 1024, "scripts\\MirPy\\autoload");
	ShowPathInConsole(autoLoadPath);
	AddPath(init, autoLoadPath); //add the autoload folder to the search path
	GetPath(hScriptsFolder, scriptsPath, 1024, "scripts\\MirPy");
	ShowPathInConsole(scriptsPath);
	AddPath(init, scriptsPath); //add the normal scripts folder to the search path
	i = strlen(init);
	while (init[--i] != '"') //remove last "
		{
			init[i] = '\0';
		}
	strcat(init, "])");
	
	//Py_VerboseFlag = 4; //do some logging :)
	
	int err = PyRun_SimpleString(init); 
	int ok = 1;
	if (err)
		{
			ConsoleLog(Translate("Error while adding script and module folders to python's search path."), LOG_ERROR);
			ConsoleLog("\n");
			ok = 0;
		}
		
	initMirPy();
	sprintf(init, "import MirPy\nMirPy.AutoLoadPath = r\"%s\"\nMirPy.ModulesPath = r\"%s\"\nMirPy.ScriptsPath = r\"%s\"", autoLoadPath, modulesPath, scriptsPath);
	err = PyRun_SimpleString(init);
	if (err)
		{
			ConsoleLog(Translate("Error while adding module path and autoload path variables to module MirPy."), LOG_ERROR);
			ConsoleLog("\n");
			ok = 0;
		}
	
	sprintf(buffer, Translate("Importing module '%s' (redirecting output buffers to console)."), "PyMir");
	strcat(buffer, "\n");
	ConsoleLog(buffer, LOG_INFO);
	strcpy(init, "import PyMir");
	err = PyRun_SimpleString(init);
	//err = (PyImport_ImportModule("PyMir") == NULL);
	if (err)
		{
			ConsoleLog(Translate("Error while trying to import module PyMir."), LOG_ERROR);
			ConsoleLog("\n");
			ok = 0;
		}
		
	sprintf(buffer, Translate("Loading modules found in autoload folder '%s'."), autoLoadPath);
	strcat(buffer, "\n");
	ConsoleLog(buffer, LOG_INFO);
	strcpy(init, "PyMir.AutoLoadScripts(globalVars = globals())");
	err = PyRun_SimpleString(init);
	if (err)
		{
			ConsoleLog(Translate("Error while trying to load modules in autoload folder."), LOG_ERROR);
			ConsoleLog("\n");
			ok = 0;
		}
	
	sprintf(buffer, Translate("MirPy version %d.%d.%d.%d loaded."), (VERSION >> 24) & 0xFF, (VERSION >> 16) & 0xFF, (VERSION >> 8) & 0xFF, (VERSION) & 0xFF);
	strcat(buffer, "\n");
	ConsoleLog(buffer, LOG_MESSAGE);
	if (!ok)
		{
			ConsoleLog(Translate("Errors encountered while trying to load MirPy. Make sure all the files are in the correct folders and that the files are using the correct case (MirPy.dll, not mirpy.dll)."), LOG_ERROR);
			ConsoleLog("\n");
			pythonAvailable = 0;
		}
	
	return 0;
}

int StopPython()
{
	char command[4096];
	ConsoleLog(Translate("Stopping python ... calling stop function StopMirPy() in module PyMir.StopMirPy"), LOG_MESSAGE);
	ConsoleLog("\n");
	strcpy(command, "import PyMir.StopMirPy\nPyMir.StopMirPy.StopMirPy()");
	int err = PyRun_SimpleString(command);
	
	return err;
}

int DestroyPython()
{
	StopPython();
	pythonAvailable = 0;
	Py_Finalize();
	
	if (hConsoleWnd)
	{
		DestroyWindow(hConsoleWnd);
	}
	
	dwMainId = NULL;
	
	return 0;
}

int IsPythonAvailable()
{
	return pythonAvailable;
}

int IsConsoleAvailable()
{
	return (hConsoleWnd != NULL);
}

int ConsoleRestart()
{
	if (hConsoleWnd)
		{
			DestroyWindow(hConsoleWnd);
			hConsoleWnd = NULL;
		}
	return ConsoleShowService(0, 0);
}

int ConsoleShow(int show)
{
	return ShowWindow(hConsoleWnd, show);
}

int ConsoleClear()
{
	HWND hLogWindow = GetDlgItem(hConsoleWnd, IDC_OUTPUT);
	return SendMessage(hLogWindow, WM_SETTEXT, 0, (LPARAM) "");
}

int ConsoleLog(char *message, int type)
{
	COLORREF color;
	switch (type)
		{
			case LOG_MESSAGE:
				{
					color = COLOR_MESSAGE;
					break;
				}
			case LOG_INFO:
				{
					color = COLOR_INFO;
					break;
				}
			case LOG_ERROR:
				{
					color = COLOR_ERROR;
					break;
				}
			case LOG_OUTPUT:
				{
					color = COLOR_OUTPUT;
					break;
				}
			default:
				{
					color = COLOR_UNKNOWN;
					break;
				}
		} 
	
	return ConsoleLogColor(message, color);
}

int ConsoleLogColor(char *message, COLORREF color)
{
	if (!IsConsoleAvailable())
	{
		return -1;
	}
	
	PLogMessage log = (PLogMessage) malloc(sizeof(TLogMessage));
	log->color = color;
	log->message = _strdup(message);
	
	PostMessage(hConsoleWnd, MIRPY_LOGMESSAGE, 0, (LPARAM) log);
	//SleepEx(0, TRUE);

	//HWND hLogWindow = GetDlgItem(hConsoleWnd, IDC_OUTPUT);
	//GETTEXTLENGTHEX textLen = {0}; textLen.flags = GTL_DEFAULT; textLen.codepage = CP_ACP;
	//DWORD size = SendMessage(hLogWindow, EM_GETTEXTLENGTHEX, (WPARAM) &textLen, 0);
	//SendMessage(hLogWindow, EM_SETSEL, size, size);
	//CHARFORMAT chf;
	//chf.cbSize = sizeof(CHARFORMAT);
	//SendMessage(hLogWindow, EM_GETCHARFORMAT, SCF_SELECTION, (LPARAM) &chf);
	//chf.dwMask = CFM_COLOR;
	//chf.dwEffects = 0;
	//chf.crTextColor = color;
	//SendMessage(hLogWindow, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) &chf);
	////SETTEXTEX st = {0};	st.codepage = CP_ACP;	st.flags = ST_SELECTION;
	//SendMessage(hLogWindow, EM_REPLACESEL, FALSE, (LPARAM) message);

	return 0;
}

int CallMirandaService(const char *service, WPARAM wParam, LPARAM lParam)
{
	return CallService(service, (WPARAM) wParam, (LPARAM) lParam); //uhh ohh, this is going to create some head aches ...
}

int HookMirandaEvent(char *service, MIRANDAHOOK callback)
{
	return (int) HookEvent(service, callback);
}

int UnhookMirandaEvent(HANDLE hookHandle)
{
	return UnhookEvent(hookHandle);
}

char *MirandaTranslate(const char *szEnglish)
{
	return Translate(szEnglish);
}

int CallPythonService(const char *service, WPARAM wParam, LPARAM lParam)
{
	PyObject *pyService;
	PyObject *pyRes;
	int res = CALLSERVICE_NOTFOUND;
	pyService = PyDict_GetItemString(PythonServicesPlaceholder, service);
	if (pyService)
		{
			pyRes = PyObject_CallFunction(pyService, "ii", wParam, lParam);
			if ((pyRes) && PyInt_Check(pyRes))
				{
					res = PyInt_AsLong(pyRes);
				}
				else{
					res = 0;
				}
		}
		else{
			res = CALLSERVICE_NOTFOUND;
		}
	return res;
}

int CallPythonServiceParam(PyObject *pyService, WPARAM wParam, LPARAM lParam)
{
	PyObject *pyRes = NULL;
	int res = 0;
	if (pyService)
	{
		pyRes = PyObject_CallFunction(pyService, "ii", wParam, lParam);
		if ((pyRes) && (PyInt_Check(pyRes)))
		{
			res = PyInt_AsLong(pyRes);
		}
		else{
			res = 0;
		}
	}
	else{
		res = CALLSERVICE_NOTFOUND;
	}
	
	return res;
}

int PythonServiceExists(const char *service)
{
	PyObject *callback = NULL;
	callback = PyDict_GetItemString(PythonServicesPlaceholder, service);
	
	return (callback != NULL);
}