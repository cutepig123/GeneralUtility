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
 * @file UnicodeString.h
 *
 * @brief Unicode string based on std::wstring.
 *
 */
// ID line follows -- this is updated by SVN
// $Id: UnicodeString.h 7447 2010-11-24 07:58:17Z gerundt $

#ifndef _UNICODE_STRING_
#define _UNICODE_STRING_

#include <string>

#define std_tchar(type) std::w##type

typedef std_tchar(string) String;

String string_makelower(const String &str);

void string_replace(String &target, const String &find, const String &replace);

// Comparing
int string_compare_nocase(const String &str1, const String &str2);

// Trimming
String string_trim_ws(const String & str);
String string_trim_ws_begin(const String & str);
String string_trim_ws_end(const String & str);

// Formatting
String string_format(const TCHAR *fmt, ...);

#endif // _UNICODE_STRING_
