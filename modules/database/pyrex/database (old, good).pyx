from MirPy import CallService


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
	
cdef union DBVARIANTCHARVALUNION:
	char *pszVal
	int *pwszVal

cdef struct DBVARIANTCHARVALSTRUCT:
	DBVARIANTCHARVALUNION value
	
cdef struct DBVARIANTBLOBVALSTRUCT:
	int cpbVal
	unsigned char *pbVal
		
cdef union DBVARIANTVAL:
	unsigned char bVal
	char cVal
	int wVal
	short sVal
	unsigned long dVal
	long lVal
	DBVARIANTCHARVALSTRUCT charValue
	DBVARIANTBLOBVALSTRUCT blobValue
		
cdef struct DBVARIANT:
	char cType
	DBVARIANTVAL value

def GetProfileName():
	cdef char buf[1024]
	CallService("DB/GetProfileName", 1024, <long> buf)
	return buf

def GetProfilePath():
	cdef char buf[1024]
	CallService("DB/GetProfilePath", 1024, <long> buf)
	return buf

