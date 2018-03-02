ctypedef unsigned char BYTE
ctypedef unsigned long DWORD
ctypedef unsigned short WORD
ctypedef unsigned short WCHAR
ctypedef BYTE *PBYTE
ctypedef int (*MIRANDAHOOK)(long, long)

import threading

import MirPy

__databaseLock = threading.RLock() 

#from MirPy import CallService
#from MirPy import HookEvent
#from MirPy import UnhookEvent

cdef extern from "malloc.h":
	cdef extern void *malloc(int)
	cdef extern void free(void *)
	
cdef extern from "memory.h":
	cdef extern  void *memcpy(void *dest, void *src, long count) 

cdef extern from "m_database.h":
	ctypedef struct DBVARIANT:
		char type
		BYTE bVal
		char cVal
		WORD wVal
		DWORD dVal
		short sVal
		char *pszVal
		WCHAR *pwszVal
		WORD cchVal
		WORD cpbVal
		BYTE *pbVal
	
	ctypedef struct DBCONTACTGETSETTING:
		char *szModule
		char *szSetting
		DBVARIANT *pValue
	
	ctypedef struct DBCONTACTWRITESETTING:
		char *szModule
		char *szSetting
		DBVARIANT value
		
	ctypedef struct DBEVENTINFO:
		int cbSize
		char *szModule
		DWORD timestamp
		DWORD flags
		WORD eventType
		DWORD cbBlob
		PBYTE pBlob

		

class SettingNotFoundError(Exception): pass
class InvalidEventHandleError(Exception): pass

class DBEvent:
	"""Class that holds event data.
	METHODS:
		__str__()   - returns module + timestamp + blob
	
	DATA:
		module    - the protocol module the event belongs to.
		timestamp - the timestamp of the event.
		flags     - event flags (DBEF_FIRST = 1, DBEF_SENT = 2, DBEF_READ = 4)
		eventType - the type of the event(EVENTTYPE_MESSAGE = 0, EVENTTYPE_URL = 1, EVENTTYPE_CONTACTS = 2, EVENTTYPE_ADDED = 1000, EVENTTYPE_AUTHREQUEST = 1001, EVENTTYPE_FILE = 1002, EVENTTYPE_STATUS = 25368 or custom)
		blob      - the event data (usually a string)
	"""
	def __init__(self, module, timestamp, flags, eventType, blob):
		self.module = module
		self.timestamp = timestamp
		self.flags = flags
		self.eventType = eventType
		self.blob = blob
		
	def __str__(self):
		buf = "Module = " + str(self.module) + "\nTimestamp = " + str(self.timestamp) + "\nBlob" + repr(self.blob)
		return buf

class DBWriteSetting:
	"""Class that holds a db contact write setting
	"""
	def __init__(self, module, setting, data):
		self.module = module
		self.setting = setting
		self.data = data
		
	def __str__(self):
		buf = "Module '%s', Setting: '%s', Data = '%s'" % (self.module, self.setting, str(self.data))
		return buf

cdef enum:
	DBVT_DELETED = 0
	DBVT_BYTE    = 1
	DBVT_WORD    = 2
	DBVT_DWORD   = 4
	DBVT_ASCIIZ  = 255
	DBVT_BLOB    = 254
	DBVT_UTF8    = 253
	DBVT_WCHAR   = 252
	DBVTF_VARIABLELENGTH = 0x80
	
cdef enum:
	DBEF_FIRST = 1
	DBEF_SENT  = 2
	DBEF_READ  = 4
	
cdef enum:
	EVENTTYPE_MESSAGE     = 0
	EVENTTYPE_URL         = 1
	EVENTTYPE_CONTACTS    = 2
	EVENTTYPE_ADDED       = 1000
	EVENTTYPE_AUTHREQUEST = 1001
	EVENTTYPE_FILE        = 1002
	EVENTTYPE_STATUS      = 25368

###############################PROFILE FUNCTIONS########################################
	
def GetProfileName():
	"""Returns the name of the current running profile"""
	cdef char buf[1024]
	MirPy.CallService("DB/GetProfileName", 1024, <long> buf)
	return buf

def GetProfilePath():
	"""GetProfilePath().
	Returns the path of the current profile
	"""
	cdef char buf[1024]
	MirPy.CallService("DB/GetProfilePath", 1024, <long> buf)
	return buf
	
#################################GET SETTINGS FUNCTIONS#################################

cdef int GetContactSetting(long hContact, char *module, char *setting, DBVARIANT *dbVariant):
	cdef DBCONTACTGETSETTING cts
	cts.szModule = module
	cts.szSetting = setting
	cts.pValue = dbVariant
	return MirPy.CallService("DB/Contact/GetSettingStatic", hContact, <long> &cts)

def GetContactSettingByte(hContact, module, setting, errorValue):
	"""GetContactSettingByte(hContact, module, setting, errorValue).
	Retrieves the byte value for a given contact under module and setting. Module and setting are strings. If the value is not found errorValue will be returned
	"""
	cdef DBVARIANT data
	data.type = DBVT_BYTE
	cdef int res
	res = GetContactSetting(hContact, module, setting, &data)
	if (res != 0):
		return errorValue
	return data.bVal
	
def GetContactSettingWord(hContact, module, setting, errorValue):
	"""GetContactSettingWord(hContact, module, setting, errorValue).
	Retrieves the word value for a given contact. If the value is not found errorValue will be returned
	"""
	cdef DBVARIANT data
	data.type = DBVT_WORD
	cdef int res
	res = GetContactSetting(hContact, module, setting, &data)
	if (res != 0):
		return errorValue
	return data.wVal
	
def GetContactSettingDWord(hContact, module, setting, errorValue):
	"""GetContactSettingDWord(hContact, module, setting, errorValue).
	Retrieves the DWORD value for a given contact. If the value is not found errorValue will be returned
	"""
	cdef DBVARIANT data
	data.type = DBVT_DWORD
	cdef int res
	res = GetContactSetting(hContact, module, setting, &data)
	if (res != 0):
		return errorValue
	return data.dVal

def GetContactSettingString(hContact, module, setting, errorValue, size = 1024):
	"""GetContactSettingString(hContact, module, setting, errorValue, size = 1024).
	Retrieves the string value for a given contact. Only supports ansi strings currently. If the value is not found errorValue will be returned
	"""
	cdef DBVARIANT data
	data.type = DBVT_ASCIIZ
	data.cchVal = size
	cdef void *buffer
	buffer = malloc(size)
	data.pszVal = <char *> buffer
	cdef int res
	res = GetContactSetting(hContact, module, setting, &data)
	if (res != 0):
		return errorValue
	tmp = data.pszVal
	free(buffer)
	return tmp

#####################WRITE SETTINGS FUNCTION##################################

cdef int WriteContactSetting(long hContact, char *module, char *setting, DBVARIANT dbVariant):
	cdef DBCONTACTWRITESETTING cts
	cts.szModule = module
	cts.szSetting = setting
	cts.value = dbVariant
	return MirPy.CallService("DB/Contact/WriteSetting", hContact, <long> &cts)
	
def WriteContactSettingByte(hContact, module, setting, short value):
	"""WriteContactSettingByte(hContact, module, setting, value).
	Writes a byte value for the given contact under the module and setting given.
	"""
	cdef DBVARIANT data
	data.type = DBVT_BYTE
	data.bVal = value
	WriteContactSetting(hContact, module, setting, data)
	
def WriteContactSettingWord(hContact, module, setting, WORD value):
	"""WriteContactSettingWord(hContact, module, setting, value).
	Writes a word value for the given contact under the module and setting given.
	"""
	cdef DBVARIANT data
	data.type = DBVT_WORD
	data.wVal = value
	WriteContactSetting(hContact, module, setting, data)
	
def WriteContactSettingDWord(hContact, module, setting, DWORD value):
	"""WriteContactSettingDWord(hContact, module, setting, value).
	Writes a DWORD value for the given contact under the module and setting given.
	"""
	cdef DBVARIANT data
	data.type = DBVT_DWORD
	data.dVal = value
	WriteContactSetting(hContact, module, setting, data)
	
def WriteContactSettingString(hContact, module, setting, char *value):
	"""WriteContactSettingString(hContact, module, setting, value).
	Writes a string value for the given contact under the module and setting given.
	"""
	cdef DBVARIANT data
	data.type = DBVT_ASCIIZ
	data.pszVal = value
	WriteContactSetting(hContact, module, setting, data)
	
def DeleteContactSetting(long hContact, char *module, char *setting):
	"""DeleteContactSetting(long hContact, module, setting).
	Deletes a contact setting.
	"""
	cdef DBCONTACTGETSETTING cts
	cts.szModule = module
	cts.szSetting = setting
	cts.pValue = NULL
	return MirPy.CallService("DB/Contact/DeleteSetting", hContact, <long> &cts)

#####################CONTACTS RELATED FUNCTIONS##############################
	
def ContactGetCount():
	"""ContactGetCount().
	Returns the number of contacts in the database
	"""
	return MirPy.CallService("DB/Contact/GetCount", 0, 0)
	
def ContactFindFirst():
	"""ContactFindFirst
	Returns the handle of the first contact in the database.
	"""
	return MirPy.CallService("DB/Contact/FindFirst", 0, 0)

def ContactFindNext(hContact):
	"""ContactFindNext(hContact).
	Finds the next contact after hContact in the database and returns it.
	"""	
	return MirPy.CallService("DB/Contact/FindNext", hContact, 0)
	
def ContactDelete(hContact):
	"""ContactDelete(hContact).
	Deletes the contact hContact from the database and all events and settings associated with it.
	"""
	return MirPy.CallService("DB/Contact/Delete", hContact, 0)
	
def ContactAdd():
	"""ContactAdd().
	Adds a new contact to the database and returns the handle for it."""
	return MirPy.CallService("DB/Contact/Add", 0, 0)
	
def ContactIs(hContact):
	"""ContactIs(hContact).
	Checks to see if hContact is a valid contact handle.
	Returns 1 if it's a valid handle, 0 otherwise."""
	return MirPy.CallService("DB/Contact/Is", hContact, 0)

####################EVENTS RELATED FUNCTIONS##############################3
	
def EventGetCount(hContact):
	"""EventGetCount(hContact).
	Returns the number of events in the chain belonging to hContact
	"""
	return MirPy.CallService("DB/Event/GetCount", hContact, 0)
	
def EventDelete(hContact, hDbEvent):
	"""EventDelete(hContact, hDbEvent).
	Deletes a particular event from a contact's event chain.
	"""
	return MirPy.CallService("DB/Event/Delete", hContact, hDbEvent)

def EventGet(hDbEvent):
	"""EventGet(hDbEvent).
	Returns the event data for a given event handle. It will return an instance of DBEvent.
	The following DBEvent members will be set: module, timestamp, flags, eventType and blob.
	"""
	if hDbEvent == 0:
		raise InvalidEventHandleError("Event handle %d is invalid, it cannot be 0 (NULL)." % hDbEvent)
		
	size = MirPy.CallService("DB/Event/GetBlobSize", hDbEvent, 0)
	size = size + 4
	cdef void *buffer
	buffer = malloc(size)
	cdef DBEVENTINFO dbEvent
	dbEvent.cbSize = sizeof(DBEVENTINFO)
	dbEvent.cbBlob = size
	dbEvent.pBlob = <PBYTE> buffer
	MirPy.CallService("DB/Event/Get", hDbEvent, <long> &dbEvent)
	message = <char *> dbEvent.pBlob
	event = DBEvent(dbEvent.szModule, dbEvent.timestamp, dbEvent.flags, dbEvent.eventType, message)
	free(buffer)
	return event

def EventAdd(hContact, event):
	"""EventAdd(hContact, event)
	event must be of type DBEvent and it must also have a member called cbBlob which holds the size of blob data.
	Adds an event to the database. The event structure must be filled correctly.
	"""
	cdef DBEVENTINFO e
	cdef PBYTE buffer
	cdef char * tmp
	e.cbSize = sizeof(DBEVENTINFO)
	e.szModule = event.module
	e.timestamp = event.timestamp
	e.flags = event.flags
	e.eventType = event.eventType
	
	size = (event.cbBlob + 1)
	buffer = <PBYTE> malloc(size + 2)
	tmp = event.blob
	memcpy(buffer, tmp, size)
	buffer[size] = buffer[size + 1] = 0
	e.pBlob = buffer
	e.cbBlob = size
	print size
	res = MirPy.CallService("DB/Event/Add", hContact, <long> &e)
	free(buffer)
	return res  
	
def EventMarkRead(hContact, hDbEvent):
	"""EventMarkRead(hContact, hDbEvent)
	Marks event as read.
	"""
	return MirPy.CallService("DB/Event/MarkRead", hContact, hDbEvent)
	
def EventGetContact(hDbEvent):
	"""EventGetContact(hDbEvent).
	Returns the contact handle the event belongs to.
	"""
	return MirPy.CallService("DB/Event/GetContact", hDbEvent, 0)
	
def EventFindFirst(hContact):
	"""EventFindFirst(hContact).
	Returns the handle of the first event associated with the contact or 0 if no such event exists.
	"""
	return MirPy.CallService("DB/Event/FindFirst", hContact, 0)
	
def EventFindFirstUnread(hContact):
	"""EventFindFirstUnread(hContact).
	Returns the handle of the first unread event associated with the contact or 0 if no such event exists.
	"""
	return MirPy.CallService("DB/Event/FindFirstUnread", hContact, 0)

def EventFindLast(hContact):
	"""EventFindLast(hContact).
	Returns the handle to the last event associated with the contact or 0 if no such event exists.
	"""
	return MirPy.CallService("DB/Event/FindLast", hContact, 0)
	
def EventFindNext(hDbEvent):
	"""EventFindNext(hDbEvent).
	Returns the handle of the event after hDbEvent or 0 if no other event exists.
	"""
	return MirPy.CallService("DB/Event/FindNext", hDbEvent, 0)
	
def EventFindPrev(hDbEvent):
	"""EventFindPrev(hDbEvent).
	Returns the handle of the event before hDbEvent or 0 if no other event exists.
	"""
	return MirPy.CallService("DB/Event/FindPrev", hDbEvent, 0)

###############HOOKABLE EVENTS FUNCTIONS#############################
HookedDatabaseEvents = {}
HookedDatabaseEventsCallbacks = {}

def __CallEventCallbacks(event, wParam, lParam):
	res = 0
	__databaseLock.acquire()
	list = HookedDatabaseEventsCallbacks[event]
	for callback in list:
		try:
			res = callback(wParam, lParam)
		except Exception, e:
			print "Error calling '%s' (%s)" % (callback.__name__, e)
			res = 0
		if (res != None) and (res != 0):
			break
			
	if res == None: #if no return code was given assume 0
		res = 0
	__databaseLock.release()
	
	return res

def HookDatabaseEvent(event, callback):
	"""Hooks a database event.
	HookDatabaseEvent(event, callback)
		event - the event string. To find out all available events type "print database.HookedDatabaseEvents.keys()" (do not modify this dictionary !!!)
		callback - python function that gets called when the event occurs. The declaration is OnEvent(wParam, lParam); the actual parameters depend on the event.	
	"""
	if HookedDatabaseEvents.has_key(event):
		HookedDatabaseEventsCallbacks[event].append(callback)
	else:
		raise MirPy.NoSuchEventError("The event '%s' was not found." % event)

def UnhookDatabaseEvent(event, callback):
	"""Unhooks a previously hooked database event.
	UnhookDatabaseEvent(event, callback)
		event - the event to unhook. Must have been previously hooked with a call to HookDatabaseEvent.
		callback - the event callback.
	Both parameters are required because the same callback could be used to hook multiple events.
	"""
	if HookedDatabaseEvents.has_key(event):
		try:
			HookedDatabaseEventsCallbacks[event].remove(callback)
		except Exception, e:
			raise MirPy.NoSuchCallbackForEventError("The event '%s' has no callback '%s'" % (event, callback))
	else:
		raise MirPy.NoSuchEventError("The event '%s' was not found." % event)	
	

cdef int OnEventAdded(long wParam, long lParam):
	return __CallEventCallbacks("DB/Event/Added", wParam, lParam)
	
cdef int OnEventFilterAdd(long wParam, long lParam):
	cdef DBEVENTINFO *dbEvent
	dbEvent = <DBEVENTINFO *> lParam
	cdef char *buffer
	buffer = <char *> dbEvent.pBlob
	event = DBEvent(dbEvent.szModule, dbEvent.timestamp, dbEvent.flags, dbEvent.eventType, buffer)
	return __CallEventCallbacks("DB/Event/FilterAdd", wParam, event)
	
cdef int OnEventDeleted(long wParam, long lParam):
	return __CallEventCallbacks("DB/Event/Deleted", wParam, lParam)
	
cdef int OnContactAdded(long wParam, long lParam):
	return __CallEventCallbacks("DB/Contact/Added", wParam, lParam)
	
cdef int OnContactDeleted(long wParam, long lParam):
	return __CallEventCallbacks("DB/Contact/Deleted", wParam, lParam)
	
cdef int OnSettingChanged(long wParam, long lParam):
	cdef DBCONTACTWRITESETTING *dbWriteSetting
	cdef DBVARIANT data
	dbWriteSetting = <DBCONTACTWRITESETTING *> lParam
	data = dbWriteSetting.value
	
	if (data.type == DBVT_BYTE):
		tmp = data.bVal
	elif (data.type == DBVT_WORD):
		tmp = data.wVal
	elif (data.type == DBVT_DWORD):
		tmp = data.dVal
	elif (data.type == DBVT_ASCIIZ):
		tmp = data.pszVal
	else:
		tmp = None
	cdef char *module
	cdef char *setting
	module = dbWriteSetting.szModule
	setting = dbWriteSetting.szSetting
	value = DBWriteSetting(module, setting, tmp)
		
	return __CallEventCallbacks("DB/Contact/SettingChanged", wParam, value)

cdef void __dbAddEventHook(event, MIRANDAHOOK function):
	handle = MirPy.HookEvent(event, <long> function)
	HookedDatabaseEvents[event] = handle
	HookedDatabaseEventsCallbacks[event] = []
	
def __HookDatabaseEvents():
	__dbAddEventHook("DB/Event/Added", OnEventAdded)
	__dbAddEventHook("DB/Event/FilterAdd", OnEventFilterAdd)
	__dbAddEventHook("DB/Event/Deleted", OnEventDeleted)
	__dbAddEventHook("DB/Contact/Added", OnContactAdded)
	__dbAddEventHook("DB/Contact/Deleted", OnContactDeleted)
	__dbAddEventHook("DB/Contact/SettingChanged", OnSettingChanged)
	
def __UnhookDatabaseEvents():
	for handle in HookedDatabaseEvents.values():
		MirPy.UnhookEvent(handle)
	
def OnMirPyClose():
	print "Unhooking Database events"
	__UnhookDatabaseEvents()
	
__HookDatabaseEvents() #hook all the events	
