/////////////////////////////////////////////////////////////////////////////
//  Copyright (C) 2002-2010 Ultr@VNC Team Members. All Rights Reserved.
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
//  USA.
//
// If the source code for the program is not available from the place from
// which you received this file, check 
// http://www.uvnc.com/
//
////////////////////////////////////////////////////////////////////////////

#ifndef _VIDEOD_H
#define _VIDEOD_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <windows.h>

#include <tchar.h>
#include <winbase.h>
#include <winreg.h>
//#include <StrSafe.h>

#define MAXCHANGES_BUF 2000

#define OSVISTA 6
#define OSWINXP 5
#define OSWIN2000 5
#define OSWIN2003 5
#define OSOLD 4
#define OSWINXP64 7

#define SCREEN_SCREEN 11
#define BLIT 12
#define SOLIDFILL 13
#define BLEND 14
#define TRANS 15
#define PLG 17
#define TEXTOUT 18

typedef BOOL (WINAPI* pEnumDisplayDevices)(PVOID,DWORD,PVOID,DWORD);
typedef LONG (WINAPI* pChangeDisplaySettingsExA)(LPCSTR,LPDEVMODEA,HWND,DWORD,LPVOID);
typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
typedef struct _CHANGES_RECORD
{
	ULONG type;  //screen_to_screen, blit, newcache,oldcache
	RECT rect;	
	POINT point;
}CHANGES_RECORD;
typedef CHANGES_RECORD *PCHANGES_RECORD;
typedef struct _CHANGES_BUF
	{
	 ULONG counter;
	 CHANGES_RECORD pointrect[MAXCHANGES_BUF];
	}CHANGES_BUF;
typedef CHANGES_BUF *PCHANGES_BUF;

class VIDEODRIVER
{
public:
	VIDEODRIVER();
	void VIDEODRIVER_start(int x,int y,int w,int h);
	void VIDEODRIVER_Stop();
	virtual ~VIDEODRIVER();
	BOOL HardwareCursor();
	BOOL NoHardwareCursor();

	ULONG oldaantal;
	PCHAR mypVideoMemory;
	PCHAR myframebuffer;
	PCHANGES_BUF mypchangebuf;
	BOOL blocked;
	DWORD shared_buffer_size;
	

protected:
	int OSVersion();
	bool Mirror_driver_attach_XP(int x,int y,int w,int h);
	void Mirror_driver_detach_XP();
	bool Mirror_driver_Vista(DWORD dwAttach,int x,int y,int w,int h);
	PCHAR VideoMemory_GetSharedMemory(void);
	void VideoMemory_ReleaseSharedMemory(PCHAR pVideoMemory);
	HDC GetDcMirror();

	int OSVER;
	
};

#endif