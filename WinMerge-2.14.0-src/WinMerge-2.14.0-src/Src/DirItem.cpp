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
/** 
 * @file  DirItem.cpp
 *
 * @brief Implementation for DirItem routines
 */
// ID line follows -- this is updated by SVN
// $Id: DirItem.cpp 7491 2010-12-31 13:37:55Z gerundt $

#include <windows.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <tchar.h>
#include "UnicodeString.h"
#include "coretools.h"
#include "DirItem.h"

/**
 * @brief Set filename and path for the item.
 * @param [in] fullpath Full path to file to set to item.
 */
void DirItem::SetFile(String fullPath)
{
	String ext;
	SplitFilename(fullPath.c_str(), &path, &filename, &ext);
	filename += _T(".");
	filename += ext;
}

/**
 * @brief Update fileinfo from given file.
 * This function updates file's information from given item. Function
 * does not set filename and path.
 * @param [in] sFilePath Full path to file/directory to update
 * @return TRUE if information was updated (item was found).
 */
BOOL DirItem::Update(const String &sFilePath)
{
	struct _stati64 fstats;
	__int64 mtime64 = 0;
	BOOL retVal = FALSE;

	size = -1;
	flags.reset();
	mtime = 0;

	if (_tstati64(sFilePath.c_str(), &fstats) == 0)
	{
		// There can be files without modification date.
		// Then we must use creation date. Of course we assume
		// creation date then exists...
		if (fstats.st_mtime == 0)
			mtime64 = fstats.st_ctime;
		else
			mtime64 = fstats.st_mtime;

		// No size for directory ( size remains as -1)
		if ((fstats.st_mode & _S_IFDIR) == 0)
			size = fstats.st_size;

		flags.attributes = GetFileAttributes(sFilePath.c_str());

		retVal = TRUE;
	}
	mtime = mtime64;
	return retVal;
}

/**
 * @brief Clears FileInfo data.
 */
/*void DirItem::Clear()
{
	ClearPartial();
	filename.erase();
	path.erase();
}*/

/**
 * @brief Clears FileInfo data except path/filename.
 */
void DirItem::ClearPartial()
{
	ctime = 0;
	mtime = 0;
	size = -1;
	bIsDir = false;
	version.Clear();
	flags.reset();
}
