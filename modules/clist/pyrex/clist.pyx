ctypedef unsigned char BYTE
ctypedef unsigned long DWORD
ctypedef unsigned short WORD
ctypedef unsigned short WCHAR
ctypedef int (*MIRANDAHOOK)(long, long)

import threading

import MirPy

__clistLock = threading.RLock()

#from MirPy import CallService
#from MirPy import HookEvent
#from MirPy import UnhookEvent

STATUS_CONNECTING = 1
STATUS_OFFLINE    = 40071
STATUS_ONLINE			= 40072
STATUS_AWAY				= 40073
STATUS_DND				= 40074
STATUS_NA					= 40075
STATUS_OCCUPIED   = 40076
STATUS_FREECHAT   = 40077
STATUS_INVISILE   = 40078
STATUS_ONTHEPHONE = 40079
STATUS_OUTTOLUNCH = 40080
STATUS_IDLE				= 40081

cdef extern from "m_clist.h":
	ctypedef struct CLISTMENUITEM:
		int cbSize
		char *pszName
		DWORD flags
		int position
		long hIcon
		char *pszService
		char *pszPopupName
		int popupPosition
		DWORD hotKey
		char *pszContactOwner

def EventRemove(hContact, hEvent):
	"""Removes an event for a contact from the clist.
	hContact - the contact handle to remove the event for
	hEvent - the event handle to remove
	"""
	return MirPy.CallService("Clist/RemoveEvent", hContact, hEvent)
		
def SetStatusMode(int mode):
	"""SetStatusMode(int mode)
	Sets the global status to mode.
	"""
	return MirPy.CallService("CList/SetStatusMode", mode, 0)

def GetStatusMode():
	"""GetStatusMode().
	Returns the current global status mode. Returns one of the clist.STATUS_* constants. 
	"""
	return MirPy.CallService("CList/GetStatusMode", 0, 0)
	
def GetStatusDescription(int mode, int flags = 0):
	"""GetStatusDescription(int mode, int flags = 0).
	Returns the status description for the given mode. Returns None if the statums mode is unknown.
	"""
	cdef char *buff
	cdef long tmp 
	tmp = MirPy.CallService("CList/GetStatusModeDescription", mode, flags)
	buff = <char *> tmp
	if (buff == NULL):
		return None
	return buff
	
def GetContactDisplayName(long hContact, flags = 0):
	"""GetContactDisplayName(hContact, flags = 0).
	Returns the contact's display name.
	"""
	cdef char *buffer
	cdef long tmp
	tmp = MirPy.CallService("CList/GetContactDisplayName", hContact, flags)
	buffer = <char *> tmp
	return buffer

cdef AddMenuItem(char *menuType, char *name, int position, char *service, char *contactOwner, int flags, char *popupName, int popupPosition, int icon):
	cdef CLISTMENUITEM clmi
	clmi.cbSize = sizeof(CLISTMENUITEM)
	clmi.pszName = name
	clmi.flags = flags
	clmi.position = position
	clmi.hIcon = icon
	clmi.pszService = service
	clmi.pszPopupName = popupName
	clmi.popupPosition = popupPosition
	clmi.hotKey = 0
	clmi.pszContactOwner = contactOwner
	return MirPy.CallService(menuType, 0, <long> &clmi)
		
def AddMainMenuItem(name, position, service, flags = 0, popupName = None, popupPosition = 0, icon = 0):
	"""Adds a new menu item in miranda's main menu.
	AddMainMenuItem(name, position, service, flags = 0, popupName = None, popupPosition = 0, icon = 0)
		name - the name of the menu item.
		position - item position in menu
		service - service string. Will be called when the user selects the menu entry. wParam = 0, lParam = hwndContactList.
		flags - 1 = grayed, 2 = checked, keep untranslated = 1024, iconfromicolib = 2048.
		popupName - Name of the popup menu that this item is on. If this item is None then menu item is on the root of the menu.
		popupPosition - valid only if popupName != None. Position of the popup menu in the root menu.
		icon - icon handle. windows api stuff ...
	Returns a handle to the menu item. 
	"""
	cdef char *buffer
	buffer = NULL
	if (popupName is not None):
		buffer = popupName
	return AddMenuItem("CList/AddMainMenuItem", name, position, service, NULL, flags, buffer, popupPosition, icon)
	
def AddContactMenuItem(name, position, service, contactOwner = None, flags = 0, icon = 0):
	"""Adds a new menu item in a contact's menu.
	AddContactMenuItem(hContact, name, position, service, contactOwner = None, flags = 0, icon = 0)
		name - the name of the menu item.
		position - item position in menu
		service - service string. Will be called when the user selects the menu entry. wParam = contact handle, lParam = 0.
		contactOwner - the protocol for which the item will be shown. If contactOwner is None then the item will be shown for all protocols.
		flags - 1 = grayed, 2 = checked, hidden = 4, notoffline = 8 (won't appear for offline contacts), notonline = 16, notonlist = 32 (will appear for contacts that aren't on the user's list), notofflist = 64, keep untranslated = 1024, iconfromicolib = 2048.
		icon - icon handle. windows api stuff ...
	Returns a handle to the menu item.
	"""
	cdef char *buffer
	buffer = NULL
	if (contactOwner is not None):
		buffer = contactOwner
	return AddMenuItem("CList/AddContactMenuItem", name, position, service, buffer, flags, NULL, 0, icon)

def AddStatusMenuItem(name, position, service, contactOwner = None, flags = 0, icon = 0):
	"""Adds a new menu item in the status menu.
	AddStatusMenuItem(hContact, name, position, service, contactOwner = None, flags = 0, icon = 0)
		name - the name of the menu item.
		position - item position in menu
		service - service string. Will be called when the user selects the menu entry. wParam = contact handle, lParam = 0.
		contactOwner - the protocol for which the item will be shown. If contactOwner is None then the item will be shown for all protocols.
		flags - 1 = grayed, 2 = checked, hidden = 4, notoffline = 8 (won't appear for offline contacts), notonline = 16, notonlist = 32 (won't appear for contacts that aren't on the user's list), notofflist = 64, keep untranslated = 1024, iconfromicolib = 2048.
		icon - icon handle. windows api stuff ...
	Returns a handle to the menu item.
	"""
	cdef char *buffer
	buffer = NULL
	if (contactOwner is not None):
		buffer = contactOwner
	return AddMenuItem("CList/AddStatusMenuItem", name, position, service, buffer, flags, NULL, 0, icon)

def ModifyMenuItem(hMenuItem, name, flags = None, icon = None, hotkey = None):
	"""Modifies a menu item. Parameters that have the value None will not be changed.
		hMenuItem - the handle of the menu item as returned by one of the Add*MenuItem methods.
		name - new menu item name.
		flags - new flags for the menu item.
		icon - new menu icon. windows api stuff ...
		hotkey - new menu item hotkey.
	Returns 0 on success, nonzero on failure.
	"""
	cdef CLISTMENUITEM clmi
	clmi.cbSize = sizeof(CLISTMENUITEM)
	clmi.flags = clmi.position = clmi.popupPosition = clmi.hotKey = clmi.hIcon = 0
	clmi.pszName = clmi.pszService = clmi.pszPopupName = clmi.pszContactOwner = NULL

	if (flags != None):
		clmi.flags = flags
		clmi.flags = clmi.flags | 0x40000000
	if (name != None):
		clmi.flags = clmi.flags | 0x80000000
		clmi.pszName = name
	if (icon != None):
		clmi.flags = clmi.flags | 0x20000000
		clmi.hIcon = icon
	if (hotkey != None):
		clmi.flags = clmi.flags | 0x10000000
		clmi.hotKey = hotkey
	
	return MirPy.CallService("CList/ModifyMenuItem", hMenuItem, <long> &clmi)


###############HOOKABLE EVENTS FUNCTIONS#############################

HookedClistEvents = {}
HookedClistEventsCallbacks = {}

def __CallEventCallbacks(event, wParam, lParam):
	res = 0
	__clistLock.acquire()
	list = HookedClistEventsCallbacks[event]
	for callback in list:
		try:
			res = callback(wParam, lParam)
		except Exception, e:
			print "Error calling '%s' (%s)" % (callback.__name__, e)
		if (res != None) and (res != 0):
			break
	if res == None:
		res = 0
	__clistLock.release()
	
	return res

def HookClistEvent(event, callback):
	"""Hooks a clist event.
	HookClistEvent(event, callback)
		event - the event string. To find out all available events type "print clist.HookedClistEvents.keys()" (do not modify this dictionary !!!)
		callback - python function that gets called when the event occurs. The declaration is OnEvent(wParam, lParam); the actual parameters depend on the event.
	"""
	if HookedClistEvents.has_key(event):
		HookedClistEventsCallbacks[event].append(callback)
	else:
		raise MirPy.NoSuchEventError("The event '%s' was not found." % event)
		
def UnhookClistEvent(event, callback):
	"""Unhooks a previously hooked clist event.
	UnhookClistEvent(event, callback)
		event - the event to unhook. Must have been previously hooked with a call to HookClistEvent.
		callback - the event callback.
	Both parameters are required because the same callback could be used to hook multiple events.
	"""
	if HookedClistEvents.has_key(event):
		try:
			HookedClistEventsCallbacks[event].remove(callback)
		except Exception, e:
			raise MirPy.NoSuchCallbackForEventError("The event '%s' has no callback '%s'" % (event, callback))
	else:
		raise MirPy.NoSuchEventError("The event '%s' was not found." % event)	
	
		
cdef int OnEventStatusChange(long wParam, long lParam):
	if lParam == 0:
		tmp = None
	else:
		tmp = <char *> lParam
	__CallEventCallbacks("CList/StatusModeChange", wParam, tmp)
	
cdef int OnPreBuildContactMenu(long wParam, long lParam):
	__CallEventCallbacks("CList/PreBuildContactMenu", wParam, lParam)
	
cdef __clistAddEventHook(event, MIRANDAHOOK function):
	handle = MirPy.HookEvent(event, <long> function)
	HookedClistEvents[event] = handle
	HookedClistEventsCallbacks[event] = []
	
def __HookClistEvents():
	__clistAddEventHook("CList/StatusModeChange", OnEventStatusChange)
	__clistAddEventHook("CList/PreBuildContactMenu", OnPreBuildContactMenu)

def __UnhookClistEvents():
	for handle in HookedClistEvents.values():
		MirPy.UnhookEvent(handle) 
	
def OnMirPyClose():
	print "Unhooking Clist events"
	__UnhookClistEvents()
	
__HookClistEvents() #hook the events		 
