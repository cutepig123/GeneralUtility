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
 * @file  Utf8FileDetect.h
 *
 * @brief Declaration function detecting UTF-8 files.
 */
// ID line follows -- this is updated by SVN
// $Id: Utf8FileDetect.h 4959 2008-01-26 00:05:00Z kimmov $

#ifndef _UTF8FILEDETECT_H_
#define _UTF8FILEDETECT_H_

bool CheckForInvalidUtf8(LPBYTE pBuffer, int size);

#endif // _UTF8FILEDETECT_H_
