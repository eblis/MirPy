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

#include "services.h"

HWND hConsoleWnd = NULL;

HANDLE hsConsoleShow;

int InitServices()
{
	hsConsoleShow = CreateServiceFunction(MS_MIRPY_CONSOLE_SHOW, ConsoleShowService);
	
	return 0;
}

int DestroyServices()
{
	DestroyServiceFunction(hsConsoleShow);
	
	return 0;
}


int ConsoleShowService(WPARAM wParam, LPARAM lParam)
{
	if (!hConsoleWnd)
		{
			hConsoleWnd = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_CONSOLE), NULL, ConsoleDlgProc);
		}
	ShowWindow(hConsoleWnd, SW_SHOW);
	
	return 0;
}