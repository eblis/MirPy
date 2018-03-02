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

#include "dlgHandlers.h"

#include "commctrl.h"
#include "richedit.h"

#define INPUT_SUBCLASS_ID 10001
#define MP_SPLITTERMOVED (WM_USER + 100)

static WNDPROC OldSplitterProc;
static WNDPROC OldInputEditProc;

void AddAnchorWindowToDeferList(HDWP &hdWnds, HWND window, RECT *rParent, WINDOWPOS *wndPos, int anchors)
{
	RECT rChild = AnchorCalcPos(window, rParent, wndPos, anchors);
	hdWnds = DeferWindowPos(hdWnds, window, HWND_NOTOPMOST, rChild.left, rChild.top, rChild.right - rChild.left, rChild.bottom - rChild.top, SWP_NOZORDER);
}

BOOL CALLBACK SplitterSubclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
		{
			case WM_NCHITTEST:
				{
					return HTCLIENT;
				}
			case WM_SETCURSOR:
				{
					RECT rc;
					GetClientRect(hWnd, &rc);
					SetCursor(rc.right > rc.bottom ? LoadCursor(NULL, IDC_SIZENS) : LoadCursor(NULL, IDC_SIZEWE));
					return TRUE;
				}
			case WM_LBUTTONDOWN:
				{
					SetCapture(hWnd);
					return 0;
				}
			case WM_MOUSEMOVE:
				{
					if (GetCapture() == hWnd)
						{
							RECT rc;
							GetClientRect(hWnd, &rc);
							SendMessage(GetParent(hWnd), MP_SPLITTERMOVED, GetMessagePos() + rc.bottom / 2, (LPARAM) hWnd);
						}
					return 0;
				}
			case WM_LBUTTONUP:
				{
					ReleaseCapture();
					PostMessage(GetParent(hWnd), WM_SIZE, 0, 0);
					return 0;
				}
		}
	return CallWindowProc(OldSplitterProc, hWnd, msg, wParam, lParam);
}

BOOL CALLBACK InputSubclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
		{
			case WM_KEYDOWN:
				{
					if ((wParam == VK_F5) || ((wParam == 'R') && (GetKeyState(VK_CONTROL) & 0x80)))
						{
							SendMessage(hConsoleWnd, WM_COMMAND, MAKEWPARAM(IDC_RUN, 1), 0);
						}
					break;
				}
		}
	return CallWindowProc(OldInputEditProc, hWnd, msg, wParam, lParam);
}

void SetWindowRect(HWND hWnd, RECT rect)
{
	SetWindowPos(hWnd, NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOOWNERZORDER | SWP_NOZORDER);
}

BOOL CALLBACK ConsoleDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_INITDIALOG:
		{
			TranslateDialogDefault(hWnd);
			//static HANDLE hAccelerators = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_HOTKEYS));
			OldSplitterProc = (WNDPROC) SetWindowLong(GetDlgItem(hWnd, IDC_SPLITTER), GWL_WNDPROC, (LONG) SplitterSubclassProc);
			OldInputEditProc = (WNDPROC) SetWindowLong(GetDlgItem(hWnd, IDC_INPUT), GWL_WNDPROC, (LONG) InputSubclassProc);
			SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM) hConsoleIcon);
			//SendMessage(GetDlgItem(hWnd, IDC_INPUT), EM_SETEVENTMASK, 0, ENM_KEYEVENTS);
			
			return TRUE;
			break;
		}
			
		case WM_DESTROY:
		{
			hConsoleWnd = NULL;
			
			break;
		}
			
		case WM_CLOSE:
		{
			//DestroyWindow(hWnd);
			ShowWindow(hWnd, SW_HIDE);
			
			break;
		}
		
		case MIRPY_LOGMESSAGE:
		{
			PLogMessage log = (PLogMessage) lParam;
			if (log)
			{
				HWND hLogWindow = GetDlgItem(hWnd, IDC_OUTPUT);
				GETTEXTLENGTHEX textLen = {0}; textLen.flags = GTL_DEFAULT; textLen.codepage = CP_ACP;
				DWORD size = SendMessage(hLogWindow, EM_GETTEXTLENGTHEX, (WPARAM) &textLen, 0);
				SendMessage(hLogWindow, EM_SETSEL, size, size);
				CHARFORMAT chf;
				chf.cbSize = sizeof(CHARFORMAT);
				SendMessage(hLogWindow, EM_GETCHARFORMAT, SCF_SELECTION, (LPARAM) &chf);
				chf.dwMask = CFM_COLOR;
				chf.dwEffects = 0;
				chf.crTextColor = log->color;
				SendMessage(hLogWindow, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) &chf);
				//SETTEXTEX st = {0};	st.codepage = CP_ACP;	st.flags = ST_SELECTION;
				SendMessage(hLogWindow, EM_REPLACESEL, FALSE, (LPARAM) log->message);
				
				if (log->message) { free(log->message); }
				
				free(log);
			}
			
			break;
		}
			
		case MP_SPLITTERMOVED:
		{
			RECT oRect, iRect, sRect;
			HWND hOutput = GetDlgItem(hWnd, IDC_OUTPUT);
			HWND hInput = GetDlgItem(hWnd, IDC_INPUT);
			HWND hSplitter = GetDlgItem(hWnd, IDC_SPLITTER);
			GetWindowRect(hOutput, &oRect);
			GetWindowRect(hInput, &iRect);
			GetWindowRect(hSplitter, &sRect);
			int x, y;
			x = LOWORD(wParam);
			y = HIWORD(wParam);
			int diff = y - sRect.top;
			if ((int) iRect.bottom - (iRect.top + diff) < 20)
			{
				diff = 0;//50 - (iRect.bottom - iRect.top);
			}
			if ((int) (oRect.bottom + diff) - oRect.top < 20)
			{
				diff = 0;
			}
			oRect.bottom += diff;
			iRect.top += diff;
			OffsetRect(&sRect, 0, diff);
			ScreenToClient(hWnd, &sRect);
			ScreenToClient(hWnd, &iRect);
			ScreenToClient(hWnd, &oRect);
			SetWindowRect(hOutput, oRect);
			SetWindowRect(hInput, iRect);
			SetWindowRect(hSplitter, sRect);
			
			break;
		}
			
		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case IDC_CLOSE:
				{
					SendMessage(hWnd, WM_CLOSE, 0, 0);
					
					break;
				}
					
				case IDC_RUN:
				{
					char *buffer;
					int size = SendMessage(GetDlgItem(hWnd, IDC_INPUT), WM_GETTEXTLENGTH, 0, 0);
					size++;
					buffer = (char *) malloc(size);
					SendMessage(GetDlgItem(hWnd, IDC_INPUT), WM_GETTEXT, (WPARAM) size, (LPARAM) buffer);
					buffer[size - 1] = '\0';
					StrReplace(buffer, "\r\n", "\n");
					OutputDebugString(buffer);
					PyRun_SimpleString(buffer);
					free(buffer);
					
					break;
				}
			}
				
			break;
		}
			
		case WM_WINDOWPOSCHANGING:
		{
			HDWP hdWnds = BeginDeferWindowPos(4);
			RECT rParent;
			WINDOWPOS *wndPos = (WINDOWPOS *) lParam;
			GetWindowRect(hWnd, &rParent);
			int height = rParent.bottom - rParent.top;
			if (wndPos->cx < MIN_CONSOLE_WIDTH)
			{
				wndPos->cx = MIN_CONSOLE_WIDTH;
			}
			if (wndPos->cy < height) //window is shrinking
			{
				RECT rInput, rOutput;
				GetWindowRect(GetDlgItem(hWnd, IDC_INPUT), &rInput);
				GetWindowRect(GetDlgItem(hWnd, IDC_OUTPUT), &rOutput);
				int diff = height - wndPos->cy;
				int hOutput = rOutput.bottom - rOutput.top - diff;
				int hInput = rInput.bottom - rInput.top - diff;
				if ((hOutput < 30) || (hInput < 30))
				{
					wndPos->cy = height;
				}
			}
			
			AddAnchorWindowToDeferList(hdWnds, GetDlgItem(hWnd, IDC_OUTPUT), &rParent, wndPos, ANCHOR_TOP | ANCHOR_LEFT | ANCHOR_RIGHT);
			AddAnchorWindowToDeferList(hdWnds, GetDlgItem(hWnd, IDC_INPUT), &rParent, wndPos, ANCHOR_ALL);
			AddAnchorWindowToDeferList(hdWnds, GetDlgItem(hWnd, IDC_RUN), &rParent, wndPos, ANCHOR_LEFT | ANCHOR_BOTTOM);
			AddAnchorWindowToDeferList(hdWnds, GetDlgItem(hWnd, IDC_CLOSE), &rParent, wndPos, ANCHOR_RIGHT | ANCHOR_BOTTOM);
			AddAnchorWindowToDeferList(hdWnds, GetDlgItem(hWnd, IDC_SPLITTER), &rParent, wndPos, ANCHOR_RIGHT | ANCHOR_LEFT | ANCHOR_TOP);
			
			EndDeferWindowPos(hdWnds);
			
			break;
		}
			
/*			case WM_NOTIFY:
			{
				MSGFILTER *msgFilter = (MSGFILTER *) lParam;
				switch(msgFilter->nmhdr.idFrom)
					{
						case IDC_INPUT:
							{
								switch (msgFilter->nmhdr.code)
									{
										case EN_MSGFILTER:
											{
												switch (msgFilter->msg)
													{
														case WM_SYSKEYDOWN:
															{
																if (msgFilter->wParam == 'R')
																{
//																if (GetKeyState(VK_CONTROL))
//																{
																		SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDC_RUN, 0), 0);
//																} 
																}
																break;
															}
													}
												break;
											}
									}
								break;
							}
					}
				break;
			}*/
			
		default:
		{
		
			break;
		}
	}
		
	return 0;
}

LRESULT CALLBACK InputEditDlgProc(HWND, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIDSubclass, DWORD_PTR data)
{
	return 0;
}