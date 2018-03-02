ctypedef unsigned char BYTE
ctypedef unsigned long DWORD
ctypedef unsigned short WORD
ctypedef unsigned short WCHAR
ctypedef BYTE *PBYTE
ctypedef void *HANDLE
ctypedef int (*MIRANDAHOOK)(long, long)

import MirPy
#from MirPy import CallService
#from MirPy import ServiceNotFoundError

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

cdef extern from "string.h":
	cdef extern char *strcpy(char *, char *)
	cdef extern char *strcat(char *, char *)

cdef extern from "m_icq.h":
	ctypedef struct ICQ_CUSTOM_STATUS:
		int cbSize
		int flags
		int *status
		char *pszName
		char *pszMessage
		unsigned int *wParam
		long *lParam
		
class ExtendedStatus:
	"""Contains information about an extended status.
	flags     - status flags
	status    - actual extended status
	name      - status name
	message   - status message
	wParam    - extra data - currently not used
	lParam    - extra data - currently not used  
	"""
	def __init__(self):
		flags = 0
		status = 0
		name = ""
		message = ""
		wParam = None
		lParam = None

class ProtocolNotFoundError(Exception): pass

def CallProtocolService(char *protocol, char *service, long wParam, long lParam):
	cdef char buffer[512]
	upper = protocol.upper()
	strcpy(buffer, upper)
	strcat(buffer, service)
	res = 0
	try:
		res = MirPy.CallService(buffer, wParam, lParam)
	except MirPy.ServiceNotFoundError, e:
		raise ProtocolNotFoundError("Protocol '" + upper + "' was not found.")
	return res	 
	
def GetStatus(char *protocol):
	"""GetStatus(protocol)
	Returns the status of the given protocol. Returns one of the status.STATUS_* constants.
	"""
	return CallProtocolService(protocol, "/GetStatus", 0, 0)

def SetStatus(char *protocol, long mode):
	"""SetStatus(protocol mode)
	Sets the protocol status to mode.
	Returns 0 on success, nonzero on failure
	"""
	return CallProtocolService(protocol, "/SetStatus", mode, 0)

def SetStatusMessage(char *protocol, long mode, char *statusMessage):
	"""SetStatusMessage(protocol, mode, statusMessage)
	Changes the status and status message for a protocol.
	Returns 0 on success, nonzero on failure. 
	"""
	return CallProtocolService(protocol, "/SetAwayMsg", mode, <long> statusMessage)

def GetXStatus(char *protocol):
	"""GetXStatus(protocol)
	Returns the extended status mode (valid values are 1 to 32).
	"""
	cdef char *title
	cdef char *msg
	title = NULL
	msg = NULL
	return CallProtocolService(protocol, "/GetXStatus", <long> &title, <long> &msg)
	
def SetXStatus(char *protocol, int xMode):
	"""SetXStatus(protocol, xMode)
	Sets the protocol extended status to xMode. xMode valid values are from 1 to 32
	Returns the extended status mode set or 0 on error.
	"""
	return CallProtocolService(protocol, "/SetXStatus", xMode, 0)

def GetXStatusEx(char *protocol, long hContact):
	"""GetXStatusEx(protocol, hContact)
	Returns the current extended status. Return value is of type ExtendedStatus.
	"""
	cdef ICQ_CUSTOM_STATUS ext
	cdef int tmpStatus
	cdef char name[2048]
	cdef char message[2048]
	
	name[0] = message[0] = 0	
	
	wParam = lParam = tmpStatus = 0
	ext.cbSize = sizeof(ICQ_CUSTOM_STATUS)
	ext.flags = 7 #status, name and message valid
	ext.wParam = NULL
	ext.lParam = NULL
	ext.status = &tmpStatus
	ext.pszName = name
	ext.pszMessage = message
	
	CallProtocolService(protocol, "/GetXStatusEx", hContact, <long> &ext)
	xStatus = ExtendedStatus()
	xStatus.flags = ext.flags
	xStatus.wParam = None
	xStatus.lParam = None
	xStatus.status = tmpStatus
	xStatus.name = name
	xStatus.message = message
		
	return xStatus

def SetXStatusEx(char *protocol, xMode, xName, xMessage):
	"""SetXStatusEx(protocol, xMode, name, statusMessage)
	Changes the extended status to xMode. 'xName' is the name of the extended status and 'xMessage' is the status message. 
	"""
	cdef ICQ_CUSTOM_STATUS ext
	cdef int tmpStatus

	tmpStatus = xMode
	ext.cbSize = sizeof(ICQ_CUSTOM_STATUS)
	ext.flags = 7
	ext.wParam = NULL
	ext.lParam = NULL
	ext.status = &tmpStatus
	ext.pszName = xName
	ext.pszMessage = xMessage
	
	return CallProtocolService(protocol, "/SetXStatusEx", 0, <long> &ext)	