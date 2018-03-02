ctypedef unsigned char BYTE
ctypedef unsigned long DWORD
ctypedef unsigned short WORD
ctypedef unsigned short WCHAR
ctypedef BYTE *PBYTE
ctypedef void *HANDLE
ctypedef int (*MIRANDAHOOK)(long, long)

import time
import threading

import MirPy
import database 
#from MirPy import CallService
#from MirPy import ServiceNotFoundError

cdef extern from "string.h":
	cdef extern char *strcpy(char *, char *)
	cdef extern char *strcat(char *, char *)

__messagingLock = threading.RLock()

class WindowEvent:
	"""Class that holds message window event data.
	DATA:
		hContact   - the contact that the window belongs to.
		hwndWindow - the handle of the window
		module     - used to get plugin type
		msgType    - one of the following value: opening, open, closing, close, custom
		flags      - used to indicate message direction for all event types except custom
	"""
	def __init__(self, hContact, hwndWindow, module, msgType, flags):
		self.hContact = hContact
		self.hwndWindow = hwndWindow
		self.module = module
		self.msgType = msgType
		self.flags = flags

cdef enum:
	MSG_WINDOW_EVT_OPENING = 1
	MSG_WINDOW_EVT_OPEN    = 2
	MSG_WINDOW_EVT_CLOSING = 3
	MSG_WINDOW_EVT_CLOSE   = 4
	MSG_WINDOW_EVT_CUSTOM  = 5

cdef extern from "malloc.h":
	cdef extern void *malloc(int)
	cdef extern void free(void *)

cdef extern from "m_message.h":
	ctypedef struct MessageWindowEventData:
		int cbSize
		HANDLE hContact
		unsigned long hwndWindow
		char* szModule
		unsigned int uType
		unsigned int uFlags
		void *local

cdef extern from "m_protocols.h":
	ctypedef struct CCSDATA:
		unsigned long hContact
		char *szProtoService
		long wParam
		long lParam
		
	ctypedef struct ACKDATA:
		int cbSize
		char *szModule
		HANDLE hContact
		int type
		int result
		HANDLE hProcess
		unsigned long lParam
		
class MessageNotSentException(Exception) : pass

acks = {}
	
cdef int CallContactService(unsigned long hContact, char *szProtoService, long wParam, long lParam):
	cdef CCSDATA ccs
	ccs.hContact = hContact
	ccs.szProtoService = szProtoService
	ccs.wParam = wParam
	ccs.lParam = lParam
	return MirPy.CallService("Proto/CallContactService", 0, <long> &ccs)
	
cdef int CallProtocolService(char *protocol, char *service, long wParam, long lParam):
	cdef char buffer[512]
	strcpy(buffer, protocol)
	strcat(buffer, service)
	return MirPy.CallService(buffer, wParam, lParam)

def ShowMessageWindow(long hContact, char *text = NULL):
	"""Shows the message window for a particular contact.
	ShowMessageWindow(hContact, text = None).
	"""
	cdef int res
	try:
		res = MirPy.CallService("SRMsg/LaunchMessageWindow", hContact, <long> text)
	except MirPy.ServiceNotFoundError, e:
		res = MirPy.CallService("SRMsg/SendCommand", hContact, <long> text) 
	return res
	
def GetMessageWindowApi():
	"""Returns the current message window api.
	Current version is 0.0.0.3.
	"""
	return MirPy.CallService("MessageAPI/WindowAPI", 0, 0)

def GetMessageWindowClass():
	"""Returns the message window class.
	"""
	cdef char buffer[2048]
	MirPy.CallService("MessageAPI/WindowClass", <long> buffer, 2048)
	return buffer
	
def SendMessage(hContact, char *message):
	"""Sends a message to a contact.
	SendMessage(hContact, message)
	"""
	hProcess = CallContactService(hContact, "/SendMsg", 0, <long> message)
	acks[hProcess] = message
	
def SetStatusMessage(char *protocol, char *statusMessage):
	"""Changes the status message for a protocol.
	SetStatusMessage(protocol, statusMessage)
	"""
	CallProtocolService(protocol, "/SetAwayMsg", 0, <long> statusMessage)
	

###############HOOKABLE EVENTS FUNCTIONS#############################

HookedMessagingEvents = {}
HookedMessagingEventsCallbacks = {}

def __CallEventCallbacks(event, wParam, lParam):
	res = 0
	__messagingLock.acquire()
	list = HookedMessagingEventsCallbacks[event]
	for callback in list:
		try:
			res = callback(wParam, lParam)
		except Exception, e:
			print "Error calling '%s' (%s)" % (callback.__name__, e)
		if (res != None) and (res != 0):
			break
	if res == None:
		res = 0
	__messagingLock.release()
	
	return res

def HookMessagingEvent(event, callback):
	"""Hooks a messaging event.
	HookMessagingEvent(event, callback)
		event - the event string. To find out all available events type "print messaging.HookedMessagingEvents.keys()" (do not modify this dictionary !!!)
		callback - python function that gets called when the event occurs. The declaration is OnEvent(wParam, lParam); the actual parameters depend on the event.	
	"""
	if HookedMessagingEvents.has_key(event):
		HookedMessagingEventsCallbacks[event].append(callback)
	else:
		
		raise MirPy.NoSuchEventError("The event '%s' was not found." % event)

def UnhookMessagingEvent(event, callback):
	"""Unhooks a previously hooked messaging event.
	UnhookMessagingEvent(event, callback)
		event - the event to unhook. Must have been previously hooked with a call to HookMessagingEvent.
		callback - the event callback.
	Both parameters are required because the same callback could be used to hook multiple events.
	"""
	if HookedMessagingEvents.has_key(event):
		try:
			HookedMessagingEventsCallbacks[event].remove(callback)
		except Exception, e:
			raise MirPy.NoSuchCallbackForEventError("The event '%s' has no callback '%s'" % (event, callback))
	else:
		raise MirPy.NoSuchEventError("The event '%s' was not found." % event)

cdef int OnWindowEvent(long wParam, long lParam):
	cdef MessageWindowEventData *eventData
	eventData = <MessageWindowEventData *> lParam
	cdef long hContact
	hContact = <long> eventData.hContact
	msgType = "custom"
	if eventData.uType == MSG_WINDOW_EVT_OPENING:
		msgType = "opening"
	elif eventData.uType == MSG_WINDOW_EVT_OPEN:
		msgType = "open"
	elif eventData.uType == MSG_WINDOW_EVT_CLOSING:
		msgType = "closing"
	elif eventData.uType == MSG_WINDOW_EVT_CLOSE:
		msgType = "close"

	event = WindowEvent(hContact, eventData.hwndWindow, eventData.szModule, msgType, eventData.uFlags)
	return __CallEventCallbacks("MessageAPI/WindowEvent", wParam, event)


cdef int OnProtocolAck(long wParam, long lParam):
	cdef ACKDATA *ack
	
	result = 0
	ack = <ACKDATA *> lParam
	
	hContact = <unsigned long> ack.hContact
	event = database.DBEvent(ack.szModule, time.time(), 2, 0, ack.lParam)
	result = __CallEventCallbacks("Proto/Ack", wParam, event)
	
	if ack.type == 0: #message ack
		for hProcess in acks.keys():
			if (<unsigned long> ack.hProcess == hProcess):
				if ack.result == 0: #success
					message = acks[hProcess]
					event.blob = message
					event.cbBlob = len(message)
					
					database.EventAdd(hContact, event)
					
					del(acks[hProcess]) #remove the processed ack
					
	return result

cdef __messagingAddEventHook(event, MIRANDAHOOK function):
	handle = MirPy.HookEvent(event, <long> function)
	HookedMessagingEvents[event] = handle
	HookedMessagingEventsCallbacks[event] = []
	
def __HookMessagingEvents():
	__messagingAddEventHook("MessageAPI/WindowEvent", OnWindowEvent)
	__messagingAddEventHook("Proto/Ack", OnProtocolAck)

def __UnhookMessagingEvents():
	for handle in HookedMessagingEvents.values():
		MirPy.UnhookEvent(handle) 

def OnMirPyClose():
	print "Unhooking Messaging events"
	__UnhookMessagingEvents()
	
__HookMessagingEvents() #hook the events