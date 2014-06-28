/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  DiffFileInfo.h
 *
 * @brief Declaration file for DiffFileInfo
 */
// ID line follows -- this is updated by SVN
// $Id: DiffFileInfo.h 5444 2008-06-07 06:48:49Z kimmov $

#ifndef _DIFF_FILE_INFO_H_INCLUDED
#define _DIFF_FILE_INFO_H_INCLUDED

#include "DirItem.h"
#include "FileTextEncoding.h"
#include "FileTextStats.h"

/**
 * @brief Class for fileflags and coding info.
 */
struct DiffFileFlags : public FileFlags
{
	/**
	* @brief Encodings supported.
	*/
	enum
	{ 
		UTF_8 = 0x1000,
		UCS_4 = 0x2000,
		UCS_2BE = 0x3000,
		UCS_2LE = 0x4000,
		coding_mask = 0x7000,
	};

	DWORD coding; /**< Coding info for item */
	DiffFileFlags() : coding(0) { }
	String ToString() const;
};


/**
 * @brief Information for file.
 * This class expands DirItem class with encoding information and
 * text stats information.
 * @sa DirItem.
 */
struct DiffFileInfo : public DirItem
{
// data
	bool bVersionChecked; /**< true if version string is up-to-date */
	DiffFileFlags flags; /**< file attributes */
	FileTextEncoding encoding; /**< unicode or codepage info */
	FileTextStats m_textStats; /**< EOL, zero-byte etc counts */

	// We could stash a pointer here to the parent DIFFITEM
	// but, I ran into trouble with, I think, the DIFFITEM copy constructor

// methods

	DiffFileInfo(): bVersionChecked(false) { }
	//void Clear();
	void ClearPartial();
	bool IsEditableEncoding() const;
};

#endif // _DIFF_FILE_INFO_H_INCLUDED
