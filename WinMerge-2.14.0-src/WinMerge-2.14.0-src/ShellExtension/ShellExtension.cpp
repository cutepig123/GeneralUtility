/////////////////////////////////////////////////////////////////////////////
// ShellExtension.cpp : Implementation of DLL Exports.
//
/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or (at
//    your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
// Look at http://www.codeproject.com/shell/ for excellent guide
// to Windows Shell programming by Michael Dunn.
//
// $Id: ShellExtension.cpp 7314 2010-10-18 09:16:32Z gerundt $

// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL,
//      run nmake -f ShellExtensionps.mk in the project directory.

#include "stdafx.h"
#include "resource.h"
#include <initguid.h>
#include "ShellExtension.h"

#include "ShellExtension_i.c"
#include "WinMergeShell.h"
#include "RegKey.h"

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_WinMergeShell, CWinMergeShell)
END_OBJECT_MAP()

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		_Module.Init(ObjectMap, hInstance, &LIBID_SHELLEXTENSIONLib);
		DisableThreadLibraryCalls(hInstance);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
		_Module.Term();
	return TRUE;    // ok
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void)
{
	return (_Module.GetLockCount() == 0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
	// If we're on NT, add ourselves to the list of approved shell extensions.

	// Note that you should *NEVER* use the overload of CRegKey::SetValue with
	// 4 parameters.  It lets you set a value in one call, without having to
	// call CRegKey::Open() first.  However, that version of SetValue() has a
	// bug in that it requests KEY_ALL_ACCESS to the key.  That will fail if the
	// user is not an administrator.  (The code should request KEY_WRITE, which
	// is all that's necessary.)

	// Read also:
	// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/shellcc/
	// platform/shell/programmersguide/shell_int/shell_int_extending/
	// extensionhandlers/shell_ext.asp

	DWORD dwWinVersionPacked = GetVersion();

	// Parse the GetVersion result
	bool is9x = ((dwWinVersionPacked & 0x80000000UL) == 1);
	DWORD dwWindowsMajorVersion = (LOBYTE(LOWORD(dwWinVersionPacked)));
	DWORD dwWindowsMinorVersion = (HIBYTE(LOWORD(dwWinVersionPacked)));
	// Win 9x does not have build numbers
	DWORD dwBuild = (is9x ? 0 : (DWORD)(HIWORD(dwWinVersionPacked)));

	// Special code for Windows NT 4.0 only
	if (!is9x && dwWindowsMajorVersion == 4)
	{
		CRegKeyEx reg;
		LONG lRet;

		lRet = reg.Open(HKEY_LOCAL_MACHINE,
				_T("Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved"));

		if (ERROR_SUCCESS != lRet)
			return E_ACCESSDENIED;

		lRet = reg.WriteString(_T("WinMerge_Shell Extension"),
				_T("{4E716236-AA30-4C65-B225-D68BBA81E9C2}"));

		if (ERROR_SUCCESS != lRet)
			return E_ACCESSDENIED;
	}

	// registers object, typelib and all interfaces in typelib
	return _Module.RegisterServer(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	// If we're on NT, remove ourselves from the list of approved shell extensions.
	// Note that if we get an error along the way, I don't bail out since I want
	// to do the normal ATL unregistration stuff too.

	DWORD dwWinVersion = GetVersion();
	if (((dwWinVersion & 0x80000000UL) == 0) && LOWORD(dwWinVersion) == 4)
	{
		CRegKey reg;
		LONG lRet;

		lRet = reg.Open(HKEY_LOCAL_MACHINE,
				_T("Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved"),
				KEY_SET_VALUE);

		if (ERROR_SUCCESS == lRet)
		{
			lRet = reg.DeleteValue(_T("{4E716236-AA30-4C65-B225-D68BBA81E9C2}"));
		}
	}

	return _Module.UnregisterServer(TRUE);
}


