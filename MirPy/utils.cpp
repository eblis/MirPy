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

#include "utils.h"

int LogInit()
{
#ifdef _DEBUG
	FILE *fout = fopen(LOG_FILE, "wt");
	fclose(fout);
#endif
	return 0;
}

int Log(char *format, ...)
{
#ifdef _DEBUG
	char		str[4096];
	va_list	vararg;
	int tBytes;
	FILE *fout = fopen(LOG_FILE, "at");
	if (!fout)
		{
//			MessageBox(0, "can't open file", NULL, MB_OK);
			return -1;
		}

	va_start(vararg, format);
	
	tBytes = _vsnprintf(str, sizeof(str), format, vararg);
	if (tBytes > 0)
		{
			str[tBytes] = 0;
		}

	va_end(vararg);
	if (str[strlen(str) - 1] != '\n')
		{
			strcat(str, "\n");
		}
	fputs(str, fout);
	fclose(fout);
#endif
	return 0;
}

int Info(char *title, char *format, ...)
{
	char str[4096];
	va_list vararg;
	int tBytes;
	va_start(vararg, format);
	tBytes = _snprintf(str, sizeof(str), format, vararg);
	if (tBytes > 0)
		{
			str[tBytes] = 0;
		}
	va_end(vararg);
	return MessageBoxA(0, str, title, MB_OK | MB_ICONINFORMATION);		
}

char *StrCopy(char *source, int index, const char *what, int count)
{
	int i;
	for (i = 0; i < count; i++)
		{
			source[index + i] = what[i];
		}
	return source;
}

char *StrDelete(char *source, int index, int count)
{
	int len = strlen(source);
	int i;
	count = (count + index > len) ? len - index : count;
	for (i = index; i + count <= len; i++)
		{
			source[i] = source[i + count];
		}
	return source;
}


char *StrInsert(char *source, int index, const char *what)
{
	int whatLen = strlen(what);
	int sourceLen = strlen(source);
	int i;
	for (i = sourceLen; i >= index; i--)
		{
			source[i + whatLen] = source[i];
		}
	for (i = 0; i < whatLen; i++)
		{
			source[index + i] = what[i];
		}
	return source;
}

char *StrReplace(char *source, const char *what, const char *withWhat)
{
	char *pos;
	int whatLen = strlen(what);
	int withWhatLen = strlen(withWhat);
	int minLen;
	int index;
	
	while ((pos = strstr(source, what)))
		{
			minLen = min(whatLen, withWhatLen);
			StrCopy(source, pos - source, withWhat, minLen); 
			index = pos - source + minLen;
			if (whatLen > withWhatLen)
				{
					StrDelete(source, index, whatLen - withWhatLen);
				}
				else{
					if (whatLen < withWhatLen)
						{
							StrInsert(source, index, withWhat + minLen);
						}
				}
		}
	return source;
}

char *StrTrim(char *szText, const char *szTrimChars)
{
	int i = strlen(szText) - 1;
	while ((i >= 0) && (strchr(szTrimChars, szText[i])))
		{
			szText[i--] = '\0';
		}
	i = 0;
	while (((unsigned int )i < strlen(szText)) && (strchr(szTrimChars, szText[i])))
		{
			i++;
		}
	if (i)
		{
			StrDelete(szText, 0, i);
		}
	return szText;
}

void ScreenToClient(HWND hWnd, LPRECT rect)
{
	POINT pt;
	int cx = rect->right - rect->left;
	int cy = rect->bottom - rect->top;
	pt.x = rect->left;
	pt.y = rect->top;
	ScreenToClient(hWnd, &pt);
	rect->left = pt.x;
	rect->top = pt.y;
	rect->right = pt.x + cx;
	rect->bottom = pt.y + cy;
}

void AnchorMoveWindow(HWND window, const WINDOWPOS *parentPos, int anchors)
{
	RECT rParent;
	RECT rChild;
	
	if (parentPos->flags & SWP_NOSIZE)
		{
			return;
		}
	GetWindowRect(parentPos->hwnd, &rParent);
	rChild = AnchorCalcPos(window, &rParent, parentPos, anchors);
	MoveWindow(window, rChild.left, rChild.top, rChild.right - rChild.left, rChild.bottom - rChild.top, FALSE);
}

RECT AnchorCalcPos(HWND window, const RECT *rParent, const WINDOWPOS *parentPos, int anchors)
{
	RECT rChild;
	RECT rTmp;

	GetWindowRect(window, &rChild);
	ScreenToClient(parentPos->hwnd, &rChild);

	int cx = rParent->right - rParent->left;
	int cy = rParent->bottom - rParent->top;
	if ((cx == parentPos->cx) && (cy == parentPos->cy))
		{
			return rChild;
		}
	if (parentPos->flags & SWP_NOSIZE)
		{
			return rChild;
		}

	rTmp.left = parentPos->x - rParent->left;
	rTmp.right = (parentPos->x + parentPos->cx) - rParent->right;
	rTmp.bottom = (parentPos->y + parentPos->cy) - rParent->bottom;
	rTmp.top = parentPos->y - rParent->top;
	
	cx = (rTmp.left) ? -rTmp.left : rTmp.right;
	cy = (rTmp.top) ? -rTmp.top : rTmp.bottom;	
	
	rChild.right += cx;
	rChild.bottom += cy;
	//expanded the window accordingly, now we need to enforce the anchors
	if ((anchors & ANCHOR_LEFT) && (!(anchors & ANCHOR_RIGHT)))
		{
			rChild.right -= cx;
		}
	if ((anchors & ANCHOR_TOP) && (!(anchors & ANCHOR_BOTTOM)))
		{
			rChild.bottom -= cy;
		}
	if ((anchors & ANCHOR_RIGHT) && (!(anchors & ANCHOR_LEFT)))
		{
			rChild.left += cx;
		}
	if ((anchors & ANCHOR_BOTTOM) && (!(anchors & ANCHOR_TOP)))
		{
			rChild.top += cy;
		}
	return rChild;
}