/** 
 * @file  paths.h
 *
 * @brief Declaration file for path routines
 */
// ID line follows -- this is updated by SVN
// $Id: paths.h 5301 2008-04-16 12:51:56Z kimmov $

#ifndef paths_h_included
#define paths_h_included

#include "UnicodeString.h"

/**
 * @brief Possible values when checking for file/folder existence.
 */
typedef enum
{
	DOES_NOT_EXIST, /**< File or folder does not exist. */
	IS_EXISTING_FILE, /**< It is existing file */
	IS_EXISTING_DIR, /**< It is existing folder */
} PATH_EXISTENCE;

bool paths_EndsWithSlash(LPCTSTR s);

PATH_EXISTENCE paths_DoesPathExist(LPCTSTR szPath);
LPCTSTR paths_FindFileName(LPCTSTR path);
void paths_normalize(String & sPath);
String paths_GetLongPath(LPCTSTR szPath, BOOL bExpandEnvs = TRUE);
bool paths_CreateIfNeeded(LPCTSTR szPath);
PATH_EXISTENCE GetPairComparability(LPCTSTR pszLeft, LPCTSTR pszRight);
BOOL paths_IsShortcut(LPCTSTR inPath);
String ExpandShortcut(const String &inFile);
String paths_ConcatPath(const String & path, const String & subpath);
String paths_GetParentPath(LPCTSTR path);
String paths_GetLastSubdir(const String & path);
BOOL paths_IsPathAbsolute(const String & path);
String paths_EnsurePathExist(const String & sPath);

#endif // paths_h_included
