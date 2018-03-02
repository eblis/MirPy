ctypedef unsigned char BYTE
ctypedef unsigned long DWORD
ctypedef unsigned short WORD
ctypedef unsigned short WCHAR
ctypedef BYTE *PBYTE
ctypedef void * HANDLE

import MirPy
#from MirPy import CallService

cdef extern from "string.h":
	cdef extern char *strcpy(char *, char *)
	cdef extern char *strcat(char *, char *)

cdef extern from "m_popup.h":
	ctypedef struct POPUPDATA:
		long lchContact
		long lchIcon
		char *lpzContactName
		char *lpzText
		DWORD colorBack
		DWORD colorText
		void *PluginWindowProc
		void *PluginData


cdef DoShowPopup(char *title, char *message, long hIcon, DWORD colorBack, DWORD colorText, long contact):
	cdef POPUPDATA data
	data.lchContact = contact
	data.lchIcon = hIcon
	strcpy(data.lpzContactName, title)
	strcpy(data.lpzText, message)
	data.colorBack = <DWORD> colorBack
	data.colorText = <DWORD> colorText
	data.PluginWindowProc = NULL
	data.PluginData = NULL
	return MirPy.CallService("PopUp/AddPopUp", <long> &data, 0)

	
def ShowPopup(title, message, hIcon = 7, long contact = 0, colorBack = 0, colorText = 0):
	"""Shows a popup on screen
	ShowPopup(title, message, hIcon = 7, long contact = 0, colorBack = 0, colorText = 0)
	"""
	return DoShowPopup(title, message, hIcon, colorBack, colorText, contact)
