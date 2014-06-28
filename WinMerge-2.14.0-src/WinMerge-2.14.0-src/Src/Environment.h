/** 
 * @file  Environment.h
 *
 * @brief Declaration file for Environment-related routines.
 */
// ID line follows -- this is updated by SVN
// $Id: Environment.h 5294 2008-04-14 06:41:02Z kimmov $

#ifndef _ENVIRONMENT_H_
#define _ENVIRONMENT_H_

LPCTSTR env_GetTempPath(int * pnerr = NULL);
String env_GetTempFileName(LPCTSTR lpPathName, LPCTSTR lpPrefixString,
		int * pnerr = NULL);

String env_GetWindowsDirectory();
String env_GetMyDocuments(HWND hWindow);

#endif // _ENVIRONMENT_H_
