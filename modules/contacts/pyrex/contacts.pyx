ctypedef unsigned char BYTE
ctypedef unsigned long DWORD
ctypedef unsigned short WORD

import MirPy
#from MirPy import CallService

cdef extern from "m_contacts.h":
	ctypedef struct CONTACTINFO:
		int cbSize
		BYTE dwFlag
		long hContact
		char *szProto
		BYTE type
		BYTE bVal
		WORD wVal
		DWORD dVal
		char *pszVal
		WORD cchVal



cdef enum:
	CNF_FIRSTNAME  = 1
	CNF_LASTNAME   = 2
	CNF_NICK       = 3
	CNF_CUSTOMNICK = 4
	CNF_EMAIL      = 5
	CNF_CITY       = 6
	CNF_STATE      = 7
	CNF_COUNTRY    = 8
	CNF_PHONE      = 9
	CNF_HOMEPAGE   = 10
	CNF_ABOUT      = 11
	CNF_GENDER     = 12
	CNF_AGE        = 13
	CNF_FIRSTLAST  = 14
	CNF_UNIQUEID   = 15
	CNF_DISPLAY    = 16
	CNF_DISPLAYNC  = 17
	CNF_UNICODE    = 0x80


cdef enum:
	CNTF_BYTE      = 1
	CNTF_WORD      = 2
	CNTF_DWORD     = 3
	CNTF_ASCIIZ    = 4
	
class Contact:
	"""Contains information about a contact
	firstName   - First name.
	lastName    - Last name.
	nick        - Contact's nick.
	customNick  - Contact's custom nick, clist nick.
	email       - Email (string or 0).
	city        - City (string or 0).
	state       - State (string or 0).
	country     - Country (string or 0).
	phone       - Contact's phone (string or 0).
	homepage    - Contact's home page (string or 0).
	about       - About info (string or 0).
	gender      - Contact's gender (char 'M', 'F' or 0)
	age         - Age (0 means unknown).
	protocol    - The protocol the contact is on (string or 0).
	uniqueID    - Contact's unique ID (depends on the protocol)
	"""
	def __init__(self):
		firstName = ""
		lastName = ""
		nick = ""
		customNick = ""
		email = 0
		city = 0
		state = 0
		country = 0
		phone = 0
		homepage = 0
		about = 0
		gender = 0
		age = 0
		protocol = 0
		uniqueID = 0

cdef GetInfo(CONTACTINFO ctInfo, flag):
	ctInfo.dwFlag = flag
	MirPy.CallService("Miranda/Contact/GetContactInfo", 0, <long> &ctInfo)
	if ctInfo.type == CNTF_BYTE:
		return ctInfo.bVal
	elif ctInfo.type == CNTF_WORD:
		return ctInfo.wVal
	elif ctInfo.type == CNTF_DWORD:
		return ctInfo.dVal
	elif ctInfo.type == CNTF_ASCIIZ:
		return ctInfo.pszVal
	return 0		

def GetContact(hContact):
	cdef char buffer[4096]
	buffer[0] = 0
	cdef CONTACTINFO ctInfo
	ctInfo.cbSize = sizeof(CONTACTINFO)
	ctInfo.hContact = hContact
	ctInfo.cchVal = 4096
	ctInfo.pszVal = buffer
	ctInfo.szProto = NULL
	
	contact = Contact()
	contact.firstName = GetInfo(ctInfo, CNF_FIRSTNAME)
	
	contact.lastName = GetInfo(ctInfo, CNF_LASTNAME)
	
	contact.nick = GetInfo(ctInfo, CNF_NICK)
	
	contact.customNick = GetInfo(ctInfo, CNF_CUSTOMNICK)
	
	contact.email = GetInfo(ctInfo, CNF_EMAIL)
	
	contact.city = GetInfo(ctInfo, CNF_CITY)
	
	contact.state = GetInfo(ctInfo, CNF_STATE)
	
	contact.country = GetInfo(ctInfo, CNF_COUNTRY)
	
	contact.phone = GetInfo(ctInfo, CNF_PHONE)
	
	contact.homepage = GetInfo(ctInfo, CNF_HOMEPAGE)
	
	contact.about = GetInfo(ctInfo, CNF_ABOUT)
	
	contact.gender = GetInfo(ctInfo, CNF_GENDER)
	
	ctInfo.dwFlag = CNF_AGE
	MirPy.CallService("Miranda/Contact/GetContactInfo", 0, <long> &ctInfo)
	if (ctInfo.type == CNTF_BYTE):
		contact.age = ctInfo.bVal
	else:
		contact.age = 0
	if (ctInfo.szProto != NULL):
		contact.protocol = ctInfo.szProto
	else:
		contact.protocol = 0
	
	contact.uniqueID = GetInfo(ctInfo, CNF_UNIQUEID)
	return contact
