MirPy Scripting Plugin v.0.1.3.8
	by Cristian Libotean
	copyright © 2006-2010 Cristian Libotean

This plugin enables you to create python scripts that interact with Miranda.
You need to have python installed in order for the plugin to work. You can get
the latest version of python here: http://www.python.org/.


It comes with a number of modules that you can use in your python scripts.
	MirPy - this is a builtin module inside the MirPy plugin. Is is used by most (if not all)
	  other modules. To find out what functions it defines you can write "import MirPy <\n> print dir(MirPy)"
		inside the console and execute it. This module logs text to the console and provides functions to
		allocate/reallocate/free memory from inside python - you should only use these functions while building extension modules.
	PyMir - this module needs to be located in "miranda\scripts\MirPy\" (an option to change the folder will probably be added later).
		This module is used to redirect the standard output, standard error to the console so you can see the output of the 
		scripts and/or if they encountered errors. You can modify this module but you should only do so if you know what you're doing.
	
	Extension modules: These modules make the connection between miranda and python and allow you to use the services provided by
		other plugins.
	database.pyd - this module allows you to use the services provided by the database plugin. If you want to see what the module
		defines you can just write "help(database)" in the console window and run it (don't forget to import database first).
		You can also use the builtin help command to find out information about a certain function i.e.: help(database.ContactGetCount).
	contacts.pyd - this module allows you to use services related to contacts. dir() and help() also apply to this module.
		
Don't use MirPy.Malloc, MirPy.Realloc, MirPy.Free unless you really know what you're doing !!! Python doesn't need these functions !!!
(they're only there for extension modules to use). If you want to interact with miranda use the relevant extension module(s).
		 
To find help on a particular module, function, class use the python builtin command help.
(i.e. if you want to learn about the module MirPy just do a print help(MirPy) in the console.)
You can also use dir() on variables. For example, in the contacts module you can use contact=GetContact(<contact handle>) and then
use dir(contact) to see what attributes the contact class has.

Currently you can write python scripts in the console window and it will execute them once you hit Run.
There's no way yet to run script files from the console (except for python's builtin function execfile() ).

The services you create in Python are callable from both C code and Python scripts. How to create services:
"
import MirPy

def MyService(wParam, lParam):
  print "MyService(" + str(wParam) + ", " + str(lParam) + ")"

MirPy.CreateService("MirPy/Test", MyService)
MirPy.CallService("MirPy/Test", 2, 3)
"

How to hook events:
"
import database

def OnSettingChanged(wParam, lParam):
  print "OnSettingChanged(" + str(wParam) + ", " + str(lParam) + ")"

database.HookDatabaseEvent("DB/Contact/SettingChanged", OnSettingChanged)
#to unhook the event use database.UnhookDatabaseEvent("DB/Contact/SettingChanged", OnSettingChanged)

-----------------------------

import clist

def OnStatusChange(wParam, lParam):
  print "OnStatusChange(" + str(wParam) + ", " + str(lParam) + ")"

clist.HookClistEvent("CList/StatusModeChange", OnStatusChange)
#to unhook the event use clist.UnhookClistEvent("CList/StatusModeChange", OnStatusChange)
"


Changes:

+ : new feature
* : changed
! : bufgix
- : feature removed or disabled because of pending bugs

v. 0.1.3.8 - 2010/08/02
	* Link against Python 2.7.

v. 0.1.3.7 - 2008/10/30
	* Use Pyrex 0.9.8.5 to build modules
	* Link against Python 2.6.

v. 0.1.3.6 - 2008/05/29
	* Use Pyrex 0.9.8.2 to build modules
	! Fixed undefined strcpy and strcat methods in pyrex modules.

v. 0.1.3.5 - 2008/01/30
	+ Added protocol module
	+ Added clist.RemoveEvent method
	+ Added gen_docs.py script created by skipass
	+ Added documentation created by gen_docs.py
	! The event handler for DB/Event/Deleted now returns the value of the callbacks

v. 0.1.3.4 - 2008/01/24
	* Changed beta versions server.

v. 0.1.3.3 - 2007/10/07
	! Fixed crash when using service functions with NULL as the service name.

v. 0.1.3.2 - 2007/09/26
	! Fix for crash on exit (now unhooks functions on preshutdown instead of shutdown event).

v. 0.1.3.1 - 2007/06/12
	+ Added support for real services which can be called from Miranda's core.
	To create real services you need to call CreateRealService() and DestroyRealService().

v. 0.1.3.0 - 2007/06/07
	! Fixes for event callbacks that don't return anything (previously only module database had the fix, now all modules have it)
	! The current thread will now be locked before calling any callback functions.
	! Several fixes for status module (GetXStatus, SetXStatus, GetXStatusEx).
	+ Added MirPy.ServiceExists function.
	* MirPy.CallService now supports string parameters as well as integer ones. You can pass either an integer or a python string to either wParam or lParam (or both). Be careful !!! If you pass the wrong parameter types Miranda could crash.
	+ Added clist.AddStatusMenuItem
	! Fixed clist.AddContactMenuItem when contactOwner was not None.
	+ Added clist.OnPreBuildContactMenu event

v. 0.1.2.1 - 2007/06/02
	! Version number was wrong

v. 0.1.2.0 - 2007/06/01
	+ Added Miranda Misc module
	+ Added MirPy version variable in module MirPy

v. 0.1.1.1 - 2007/05/28
	! If event callbacks don't return anything assume 0

v. 0.1.1.0 - 2007/05/27
	+ Added option to add events to the database.
	+ Sending messages now listens to acks from protocol (python modules can now hook "Proto/Ack" event).
	+ If the message was delivered it will be stored in the database; otherwise the message will not be stored.

v. 0.1.0.3 - 2007/04/03
	+ Added status and xStatus support.

v. 0.1.0.2 - 2007/03/20
	* Change to compensate for a translation bug in Miranda's core.

v. 0.1.0.1 - 2007/03/20
	+ More translatable strings.

v. 0.1.0.0 - 2007/03/13
	! Logging to console from multiple threads will not hang the application anymore.

v. 0.0.2.1 - 2007/03/09
	* Changed icon (icon provided by Faith Healer).

v. 0.0.2.0 - 2007/03/07
	+ Added UUID ( {2fafc8da-0b05-40e9-a1b4-3b91fb19b748} Static link, {2facda05-0edb-424e-8051-048817dce285} Dynamic link)
	+ Added MIRPY interface.

v. 0.0.1.9 - 2007/01/31
	* Changed beta URL.

v. 0.0.1.8 - 2007/01/28
	+ Added window event to messaging module.
	* Moved some of the module exceptions to MirPy module instead of having each module define them.

v. 0.0.1.7 - 2007/01/07
	+ New version resource file.
	* Requires at least Miranda 0.6. (Removed hack for Miranda 0.5).

v. 0.0.1.6 - 2006/11/26
	+ Added dll version info.

v. 0.0.1.5 - 2006/11/17
	! Fixed AutoLoad.py script file (thx tooru)
	! Fixed clist module (thx tooru).
	! Fixed exception on MirPy shutdown in database and clist modules.
	* Changed the way event return codes are handled - any non zero value will stop calling other event callbacks (before only the value 1 would stop calling the other callbacks).

v. 0.0.1.4 - 2006/11/14
	! Last version broke PyMir module, now it's fixed.

v. 0.0.1.3 - 2006/11/05
	! Fixed autoloaded modules not appearing in globals()

v. 0.0.1.2 - 2006/10/02
	! Fixed crash on exit when console window was visible.
	! Fix for multiple plugins hooking CallService().

v. 0.0.1.1 - 2006/09/25
	+ Updater support (beta versions) (again)
	* MirPy built against Python 2.5
	* Compatible with Python 2.5 (which doesn't load .dll modules anymore)
	! Plugin won't crash if Python wasn't successfully loaded.

v. 0.0.1.0 - 2006/09/10
	+ Rebased dll (0x2F500000)
	* Use big icon
	* Dll file moved to plugins folder to make the folder structure easier to understand.

v. 0.0.0.7 - 2006/08/31
	* Scripts can now create services again which are callable from C and Python (example in readme).
	+ Added UnhookDatabaseEvent and UnhookClistEvent.
	+ Added help for HookDatabaseEvent and HookClistEvent.
	+ Added popup colors.
	! Fixed HookClistEvent.
	+ Added examples in readme that explain how to hook events and how to create services.
	! Fixed crash in database.EventGet for hEvent = 0

v. 0.0.0.6 - 2006/05/17
	!Destroy services the proper way.
	
v. 0.0.0.5 - 2006/03/30
	*Support for folders plugin new api.
	+Added CreateService and DestroyService functions - these require the latest nightly build, #50+ (support is currently removed in the core).
	+Added function to add menu items to module clist - requires CreateService.
	+Added new normal scripts folder.

v. 0.0.0.4 - 2006/03/26
	!Escaped space in updater url string (static build couldn't be updated because of this).
	+Added error messages to load sequence.
	+Made strings printed during load sequence translateable.
	+Added hotkeys to run scripts - Ctrl + R and F5.
	!HideConsole.py included in the static build as well.
	
v. 0.0.0.3 - 2006/03/26
	+Added messaging module
	
v. 0.0.0.2 - 2006/03/06
	+Added clist module
	+Added Translate function to module MirPy
	
v. 0.0.0.1 - 2006/x/x
2006/03/04
	+Added updater support
	
2006/03/03
	+Added database events. Now you can hook the events provided in m_database.h.
	+Added support for folders plugin
	+Added autoload scripts support
	+Added popups module
	+Added MirPyClose event. If you want your script to get notified when MirPy is closing (you need to free some data)
	you only need to create a python function called OnMirPyClose() and it will be called automatically.

2006/02/28
	+Added miranda's version of malloc(), realloc() and free()


	First build, released on miranda's forums.

******Translateable strings****** (updated for version 0.1.0.1)
Loading MirPy ...
Adding script folders to load path:
Importing module '%s' (redirecting output buffers to console).\n
Loading modules found in autoload folder '%s'.\n
Error while adding script and module folders to python's search path.
Error while adding module path and autoload path variables to module MirPy.
Error while trying to import module PyMir.
Error while trying to load modules in autoload folder.
MirPy version %d.%d.%d.%d loaded.
Errors encountered while trying to load MirPy. Make sure all the files are in the correct folders and that the files are using the correct case (MirPy.dll, not mirpy.dll).
Stopping python ... calling stop function StopMirPy() in module PyMir.StopMirPy

;From PyMir scripts
Importing module '%s' ...
Module '%s%s' will not be loaded
Module '%s' is not a valid python module ... ignoring autoload
Finished autoloading modules ...\n


MirPy copyright:

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


Python copyright:

Copyright (c) 2001-2005 Python Software Foundation.
All Rights Reserved.
Copyright (c) 2000 BeOpen.com.
All Rights Reserved.
Copyright (c) 1995-2001 Corporation for National Research Initiatives.
All Rights Reserved.
Copyright (c) 1991-1995 Stichting Mathematisch Centrum, Amsterdam.
All Rights Reserved.


PSF LICENSE AGREEMENT FOR PYTHON 2.4.2

This LICENSE AGREEMENT is between the Python Software Foundation (``PSF''), and the Individual or Organization (``Licensee'') accessing
and otherwise using Python 2.4.2 software in source or binary form and its associated documentation.

Subject to the terms and conditions of this License Agreement, PSF hereby grants Licensee a nonexclusive, royalty-free, world-wide license
to reproduce, analyze, test, perform and/or display publicly, prepare derivative works, distribute, and otherwise use Python 2.4.2 alone or
in any derivative version, provided, however, that PSF's License Agreement and PSF's notice of copyright, i.e.,
``Copyright © 2001-2004 Python Software Foundation; All Rights Reserved'' are retained in Python 2.4.2 alone or in any
derivative version prepared by Licensee. 

In the event Licensee prepares a derivative work that is based on or incorporates Python 2.4.2 or any part thereof, and wants to make
the derivative work available to others as provided herein, then Licensee hereby agrees to include in any such work a brief summary of
the changes made to Python 2.4.2. 

PSF is making Python 2.4.2 available to Licensee on an ``AS IS'' basis. PSF MAKES NO REPRESENTATIONS OR WARRANTIES, EXPRESS OR IMPLIED.
BY WAY OF EXAMPLE, BUT NOT LIMITATION, PSF MAKES NO AND DISCLAIMS ANY REPRESENTATION OR WARRANTY OF MERCHANTABILITY OR FITNESS FOR ANY
PARTICULAR PURPOSE OR THAT THE USE OF PYTHON 2.4.2 WILL NOT INFRINGE ANY THIRD PARTY RIGHTS. 

PSF SHALL NOT BE LIABLE TO LICENSEE OR ANY OTHER USERS OF PYTHON 2.4.2 FOR ANY INCIDENTAL, SPECIAL, OR CONSEQUENTIAL DAMAGES OR LOSS AS A
RESULT OF MODIFYING, DISTRIBUTING, OR OTHERWISE USING PYTHON 2.4.2, OR ANY DERIVATIVE THEREOF, EVEN IF ADVISED OF THE POSSIBILITY THEREOF. 

This License Agreement will automatically terminate upon a material breach of its terms and conditions. 

Nothing in this License Agreement shall be deemed to create any relationship of agency, partnership, or joint venture between PSF and Licensee.
This License Agreement does not grant permission to use PSF trademarks or trade name in a trademark sense to endorse or promote products or services
of Licensee, or any third party. 

By copying, installing or otherwise using Python 2.4.2, Licensee agrees to be bound by the terms and conditions of this License Agreement. 
