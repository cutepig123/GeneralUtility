/** 
 * @file  Environment.cpp
 *
 * @brief Environment related routines.
 */
// ID line follows -- this is updated by SVN
// $Id: Environment.cpp 5953 2008-09-14 14:24:23Z sdottaka $

#include "stdafx.h"
#include "paths.h"
#include "Environment.h"
#include "OptionsDef.h"
#include "Merge.h"

// Static string used by paths_GetTempPath() for storing temp path.
static String strTempPath;

/** 
 * @brief Get folder for temporary files.
 * This function returns system temp folder.
 * @param [out] pnerr Error code if erorr happened.
 * @return Temp path, or empty string if error happened.
 * @note Temp path is cached after first call.
 * @todo Should we return NULL for error case?
 */
LPCTSTR env_GetTempPath(int * pnerr)
{
	if (strTempPath.empty())
	{
		if (GetOptionsMgr()->GetBool(OPT_USE_SYSTEM_TEMP_PATH))
		{
			int cchTempPath = GetTempPath(0, 0);
			strTempPath.resize(cchTempPath - 1);
			if (!GetTempPath(cchTempPath, &*strTempPath.begin()))
			{
				int err = GetLastError();
				if (pnerr)
					*pnerr = err;
#ifdef _DEBUG
				String sysErr = GetSysError(err); // for debugging
#endif
				return strTempPath.c_str(); // empty
			}
		}
		else
		{
			strTempPath = GetOptionsMgr()->GetString(OPT_CUSTOM_TEMP_PATH);
			if (!paths_EndsWithSlash(strTempPath.c_str()))
				strTempPath += '\\';
		}
		strTempPath = paths_GetLongPath(strTempPath.c_str());
	}
	return strTempPath.c_str();
}

/**
 * @brief Get filename for temporary file.
 * @param [in] lpPathName Temporary file folder.
 * @param [in] lpPrefixString Prefix to use for filename.
 * @param [out] pnerr Error code if error happened.
 * @return Full path for temporary file or empty string if error happened.
 */
String env_GetTempFileName(LPCTSTR lpPathName, LPCTSTR lpPrefixString, int * pnerr)
{
	TCHAR buffer[MAX_PATH] = {0};
	if (_tcslen(lpPathName) > MAX_PATH-14)
		return _T(""); // failure
	int rtn = GetTempFileName(lpPathName, lpPrefixString, 0, buffer);
	if (!rtn)
	{
		int err = GetLastError();
		if (pnerr)
			*pnerr = err;
#ifdef _DEBUG
		String sysErr = GetSysError(err); // for debugging
#endif
		return _T("");
	}
	return buffer;
}

/**
 * @brief Get Windows directory.
 * @return Windows directory.
 */
String env_GetWindowsDirectory()
{
	TCHAR buf[MAX_PATH] = {0};
	GetWindowsDirectory(buf, MAX_PATH);
	return buf;
}

/**
 * @brief Return User's My Documents Folder.
 * This function returns full path to User's My Documents -folder.
 * @param [in] hWindow Parent window.
 * @return Full path to My Documents -folder.
 */
String env_GetMyDocuments(HWND hWindow)
{
	LPITEMIDLIST pidl;
	LPMALLOC pMalloc;
	String path;

	HRESULT rv = SHGetSpecialFolderLocation(hWindow, CSIDL_PERSONAL, &pidl);
	if (rv == S_OK)
	{
		TCHAR szPath[MAX_PATH] = {0};
		if (SHGetPathFromIDList(pidl, szPath))
		{
			path = szPath;
		}

		SHGetMalloc(&pMalloc);
		pMalloc->Free(pidl);
		pMalloc->Release();
	}
	return path;
}
