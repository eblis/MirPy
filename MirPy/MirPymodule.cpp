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

static PyObject *ServiceNotFoundError; //Service not found exception
static PyObject *NoSuchEventError; //no such event exception
static PyObject *NoSuchCallbackForEventError; //no such callback for event exception
PyObject *PythonServicesPlaceholder; //where all python services are kept
PyObject *PythonRealServicesPlaceholder; //where all python services param are kept
PyObject *PythonEventsPlaceholder; //where all python event callbacks are kept
PyObject *MirPyVersion; //MirPy version

static PyObject *MirPy_ConsoleRestart(PyObject *self, PyObject *args)
{
	if (!PyArg_ParseTuple(args, ""))
		{
			return NULL;
		}
	int ret = ConsoleRestart();
	return Py_BuildValue("i", ret);
}

static PyObject *MirPy_ConsoleShow(PyObject *self, PyObject *args)
{
	int show = 1;
	if (!PyArg_ParseTuple(args, "|i", &show))
		{
			return NULL;
		}
	show = (show > 0) ? SW_SHOW : SW_HIDE;
	int ret = ConsoleShow(show);
	return Py_BuildValue("i", ret);
}

static PyObject *MirPy_ConsoleClear(PyObject *self, PyObject *args)
{
	if (!PyArg_ParseTuple(args, ""))
		{
			return NULL;
		}
	int ret = ConsoleClear();
	return Py_BuildValue("i", ret);
}

static PyObject *MirPy_ConsoleLog(PyObject *self, PyObject *args)
{
	EnterCriticalSection(&mirPyCriticalSection);
	PyGILState_STATE pyState = PyGILState_Ensure();
	char *message;
	long type = -LOG_UNKNOWN;
	if (!PyArg_ParseTuple(args, "s|l", &message, &type))
	{
		return NULL;
	}
	int ret = (type < 0) ? ConsoleLog(message, -type) : ConsoleLogColor(message, (COLORREF) type);
	PyGILState_Release(pyState);
	LeaveCriticalSection(&mirPyCriticalSection);
	
	return Py_BuildValue("i", ret);
}

static PyObject *MirPy_CallService(PyObject *self, PyObject *args)
{
	char *service = NULL;
	int cService = 0;
	WPARAM wParam;
	long lParam;

	char *szwParam = NULL;
	char *szlParam = NULL;	
	
	if (!PyArg_ParseTuple(args, "s#il", &service, &cService, &wParam, &lParam)) //check for int and long parameters
		{
			if (!PyArg_ParseTuple(args, "s#is", &service, &cService, &wParam, &szlParam)) //check for int and string parameters
			{
				if (!PyArg_ParseTuple(args, "s#sl", &service, &cService, &szwParam, &lParam)) //check for string and long parameters
				{
					if (!PyArg_ParseTuple(args, "s#ss", &service, &cService, &szwParam, &szlParam)) //check for string and string parameters
					{
						return NULL; //if no match was found then the parameters are not of the correct types
					}
					else{
						wParam = (WPARAM) szwParam;
						lParam = (LPARAM) szlParam;
					}
				}
				else{
					wParam = (WPARAM) szwParam;
				}
			}
			else{
				lParam = (LPARAM) szlParam;
			}
			
			PyErr_Clear(); //clear any exception that might have been raised
		}
	int ret = CallMirandaService(service, wParam, (LPARAM) lParam);
	if (ret == CALLSERVICE_NOTFOUND)
		{
			char buffer[1024];
			sprintf(buffer, "Service '%s' does not exist!", service);
			PyErr_SetString(ServiceNotFoundError, buffer);
			return NULL;
		}
		else{
			return Py_BuildValue("i", ret);
		}
}

static PyObject *MirPy_HookEvent(PyObject *self, PyObject *args)
{
	char *hookedEvent = NULL;
	MIRANDAHOOK callback = NULL;
	if (!PyArg_ParseTuple(args, "sl", &hookedEvent, &callback))
		{
			return NULL;
		}
	int ret = HookMirandaEvent(hookedEvent, callback);
	return Py_BuildValue("i", ret);
}

static PyObject *MirPy_UnhookEvent(PyObject *self, PyObject *args)
{
	HANDLE hookHandle = NULL;
	if (!PyArg_ParseTuple(args, "l", &hookHandle))
		{
			return NULL;
		}
	int ret = UnhookMirandaEvent(hookHandle);
	return Py_BuildValue("i", ret);
}

static PyObject *MirPy_Translate(PyObject *self, PyObject *args)
{
	char *buffer;
	if (!PyArg_ParseTuple(args, "s", &buffer))
		{
			return NULL;
		}
	char *ret = MirandaTranslate(buffer);
	return Py_BuildValue("s", ret);
}

static PyObject *MirPy_Malloc(PyObject *self, PyObject *args)
{
	long size;
	if (!PyArg_ParseTuple(args, "l", &size))
		{
			return NULL;
		}
	void *block = malloc(size);
	if (!block)
		{//not enough mem
			PyErr_NoMemory();
			return NULL;
		}
	return Py_BuildValue("l", block);
}

static PyObject *MirPy_Realloc(PyObject *self, PyObject *args)
{
	void *block;
	long size;
	if (!PyArg_ParseTuple(args, "ll", &block, &size))
		{
			return NULL;
		}
	block = realloc(block, size);
	if (!block)
		{
			PyErr_NoMemory();
			return NULL;
		}
	return Py_BuildValue("l", block);
}

static PyObject *MirPy_Free(PyObject *self, PyObject *args)
{
	void *block;
	if (!PyArg_ParseTuple(args, "l", &block))
		{
			return NULL;
		}
	free(block);
	Py_RETURN_NONE;
}

static PyObject *MirPy_MirandaMalloc(PyObject *self, PyObject *args)
{
	long size;
	if (!PyArg_ParseTuple(args, "l", &size))
		{
			return NULL;
		}
	void *block = MirandaMalloc(size);
	if (!block)
		{//not enough mem
			PyErr_NoMemory();
			return NULL;
		}
	return Py_BuildValue("l", block);
}

static PyObject *MirPy_MirandaRealloc(PyObject *self, PyObject *args)
{
	void *block;
	long size;
	if (!PyArg_ParseTuple(args, "ll", &block, &size))
		{
			return NULL;
		}
	block = MirandaRealloc(block, size);
	if (!block)
		{
			PyErr_NoMemory();
			return NULL;
		}
	return Py_BuildValue("l", block);
}

static PyObject *MirPy_MirandaFree(PyObject *self, PyObject *args)
{
	void *block;
	if (!PyArg_ParseTuple(args, "l", &block))
		{
			return NULL;
		}
	MirandaFree(block);
	Py_RETURN_NONE;
}

static PyObject *MirPy_CreateService(PyObject *self, PyObject *args)
{
	char *service = NULL;
	PyObject *callback = NULL;
	if (!PyArg_ParseTuple(args, "sO", &service, &callback))
		{
			return NULL;
		}
	if (!PyCallable_Check(callback))
		{
			char buffer[4096];
			sprintf(buffer, "'%s' is not callable", "callback");
			PyErr_SetString(PyExc_TypeError, buffer);
			return NULL;
		}
	int res = PyDict_SetItemString(PythonServicesPlaceholder, service, callback);
	return (res < 0) ? NULL : Py_BuildValue("i", res);
}

static PyObject *MirPy_CreateRealService(PyObject *self, PyObject *args)
{
	char *service = NULL;
	PyObject *callback = NULL;
	if (!PyArg_ParseTuple(args, "sO", &service, &callback))
	{
		return NULL;
	}
	
	if (!PyCallable_Check(callback))
	{
			char buffer[4096];
			sprintf(buffer, "'%s' is not callable", "callback");
			PyErr_SetString(PyExc_TypeError, buffer);
			
			return NULL;
	}
	
	int res = PyDict_SetItemString(PythonServicesPlaceholder, service, callback);
	
	HANDLE hService = CreateServiceFunctionParam(service, MirPyCallServiceParam, (LPARAM) callback);
	int err = -1;
	if (hService > 0)
	{
		err = PyDict_SetItem(PythonRealServicesPlaceholder, callback, PyInt_FromLong((long) hService));
	}
	else{
		char buffer[4096];
		sprintf(buffer, "Failed to register real service '%s'", service);
		PyErr_SetString(PyExc_TypeError, buffer);
	}
	
	return ((res < 0) || (err < 0)) ? NULL : Py_BuildValue("i", res);
}

static PyObject *MirPy_DestroyService(PyObject *self, PyObject *args)
{
	char *service = NULL;
	if (!PyArg_ParseTuple(args, "s", &service))
		{
			return NULL;
		}
	PyObject *callback = PyDict_GetItemString(PythonServicesPlaceholder, service);
	if (callback)
		{
			PyDict_DelItemString(PythonServicesPlaceholder, service);
		}
		else{
			char buffer[4096];
			sprintf(buffer, "Service '%s' is not known. Are you sure you created it ?", service);
			PyErr_SetString(ServiceNotFoundError, buffer);
			
			return NULL;
		}
	Py_RETURN_NONE;
}

static PyObject *MirPy_DestroyRealService(PyObject *self, PyObject *args)
{
	char *service = NULL;
	if (!PyArg_ParseTuple(args, "s", &service))
	{
		return NULL;
	}
	PyObject *callback = PyDict_GetItemString(PythonServicesPlaceholder, service);
	if (callback)
	{
		PyObject *handle = PyDict_GetItem(PythonRealServicesPlaceholder, callback);
		if (handle)
		{
			HANDLE hService = (HANDLE) PyInt_AsLong(handle);
			DestroyServiceFunction(hService);
		}
		PyDict_DelItemString(PythonServicesPlaceholder, service);
	}
	else{
			char buffer[4096];
			sprintf(buffer, "Service '%s' is not known. Are you sure you created it ?", service);
			PyErr_SetString(ServiceNotFoundError, buffer);
			
			return NULL;
	}
	
	Py_RETURN_NONE;
}

static PyObject *MirPy_ServiceExists(PyObject *self, PyObject *args)
{
	char *service = NULL;
	if (!PyArg_ParseTuple(args, "s", &service))
	{
		return NULL;
	}
	
	int exists = ServiceExists(service);
	if (exists)
	{
		Py_RETURN_TRUE;
	}
	else{
		Py_RETURN_FALSE;
	}
}

static PyMethodDef MirPyMethods[] = {
	{"ConsoleRestart", MirPy_ConsoleRestart, METH_VARARGS, "ConsoleRestart().\nDestroys the console window and then creates it again."},
	{"ConsoleClear", MirPy_ConsoleClear, METH_VARARGS, "ConsoleClear().\nClears the log window."},
	{"ConsoleShow", MirPy_ConsoleShow, METH_VARARGS, "ConsoleShow(show = 1)\nShows or hides the console. If show > 0 then the console window will be shown otherwise it will be hidden."},
	{"ConsoleLog", MirPy_ConsoleLog, METH_VARARGS, "ConsoleLog(message, type = LOG_UNKNOWN).\nLogs a message to the console. Takes a string and an optional long parameter.\nThe long parameter specifies the color if it's bigger than 0 or specifies a type if it's lower than 0."},
	{"CallService", MirPy_CallService, METH_VARARGS, "CallService(service, wParam, lParam).\nCalls a miranda service and returns the result. Takes 3 params: the service to call\nthe wParam of the service and the lParam. wParam and lParam can be either integers or strings. Use with care!"},
	{"HookEvent", MirPy_HookEvent, METH_VARARGS, "HookEvent(event, callback).\nHooks a miranda event and returns the handle of the hook. Takes 2 params: the event string and the callback method (int (*callback)(param1, param2)."},
	{"UnhookEvent", MirPy_UnhookEvent, METH_VARARGS, "UnhookEvent(handle).\nUnhooks a previously hooked miranda event. Takes 1 param, the event handle."},
	{"Translate", MirPy_Translate, METH_VARARGS, "Translate(message).\nTranslates a string into the user's locale."},
	{"Malloc", MirPy_Malloc, METH_VARARGS, "Malloc(size).\nAllocates a block of memory.\nTakes only one param, the length of the block as a python long and returns a pointer (python long) to the block of memory."},
	{"Realloc", MirPy_Realloc, METH_VARARGS, "Realloc(buffer, newSize).\nReallocates a block of memory.\nTakes 2 parameters, the block of memory to reallocate (python long) and the new size (python long). Returns the new address of the reallocated block of memory as a python long object."},
	{"Free", MirPy_Free, METH_VARARGS, "Free(buffer).\nFrees a block of memory allocated with either Malloc or Realloc.\nTakes one parameter, the block to deallocate as a python long. Returns None."},
	{"MirandaMalloc", MirPy_MirandaMalloc, METH_VARARGS, "MirandaMalloc(size).\nSame as Malloc but uses miranda's version of the function."},
	{"MirandaRealloc", MirPy_MirandaRealloc, METH_VARARGS, "MirandaRealloc(buffer, newSize).\nSame as Realloc but uses miranda's version of the function."},	
	{"MirandaFree", MirPy_MirandaFree, METH_VARARGS, "MirandaFree(buffer).\nSame as Free but uses miranda's version of the function."},
	{"CreateService", MirPy_CreateService, METH_VARARGS, "CreateService(serviceName, callback).\nCreates a new service that can be called by any miranda plugin.\nThe callback function takes exactly 2 parameters.\nBe careful because most plugins are in C and you can't ask them to pass complicated structures because they don't know how.\nYou should only use the values to pass simple integer values."},
	{"DestroyService", MirPy_DestroyService, METH_VARARGS, "DestroyService(serviceName).\nDestroys a service previously created with CreateService.\nYou should always destroy the services you've created when you no longer want to accept requests."},
	{"ServiceExists", MirPy_ServiceExists, METH_VARARGS, "ServiceExists(serviceName).\nChecks to see if a Miranda service exists. It will check for both native services and, if no native service is found, it will also check for a python service with that name. The native ServiceExists() methos is also aware of python services."},
	{"CreateRealService", MirPy_CreateRealService, METH_VARARGS, "CreateRealService(}, (serviceName, callback).\nCreates a new fully functional service that can be called by any miranda plugin. This type of service can also be called from within Miranda's core.\nThe callback function takes exactly 2 parameters.\nBe careful because most plugins are in C and you can't ask them to pass complicated structures because they don't know how.\nYou should only use the values to pass simple integer values."},
	{"DestroyRealService", MirPy_DestroyRealService, METH_VARARGS, "DestroyRealService(serviceName).\nDestroys a service previously created with CreateRealService.\nYou should always destroy the services you've created when you no longer want to accept requests."},
	{NULL, NULL, 0, NULL} //sentinel
};

PyMODINIT_FUNC initMirPy()
{
	PyObject *module;
	module = Py_InitModule("MirPy", MirPyMethods);
	
	MirPyVersion = PyString_FromFormat("%d.%d.%d.%d", (VERSION >> 24) & 0xFF, (VERSION >> 16) & 0xFF, (VERSION >> 8) & 0xFF, (VERSION) & 0xFF);

	ServiceNotFoundError = PyErr_NewException("MirPy.ServiceNotFoundError", NULL, NULL);
	NoSuchEventError = PyErr_NewException("MirPy.NoSuchEventError", PyExc_Exception, NULL);
	NoSuchCallbackForEventError = PyErr_NewException("MirPy.NoSuchCallbackForEventError", PyExc_Exception, NULL);
	PythonServicesPlaceholder = PyDict_New();
	PythonRealServicesPlaceholder = PyDict_New();
	PythonEventsPlaceholder = PyDict_New();
	Py_INCREF(ServiceNotFoundError);
	Py_INCREF(NoSuchEventError);
	Py_INCREF(NoSuchCallbackForEventError);
	Py_INCREF(PythonServicesPlaceholder);
	Py_INCREF(PythonRealServicesPlaceholder);
	Py_INCREF(PythonEventsPlaceholder);
	Py_INCREF(MirPyVersion);
	
	PyModule_AddObject(module, "ServiceNotFoundError", ServiceNotFoundError);
	PyModule_AddObject(module, "NoSuchEventError", NoSuchEventError);
	PyModule_AddObject(module, "NoSuchCallbackForEventError", NoSuchCallbackForEventError);
	PyModule_AddObject(module, "PythonServices", PythonServicesPlaceholder);
	PyModule_AddObject(module, "PythonServicesParam", PythonRealServicesPlaceholder);
	PyModule_AddObject(module, "PythonEventCallbacks", PythonEventsPlaceholder);
	PyModule_AddObject(module, "Version", MirPyVersion);
}