/**
 * @file  ClipBoard.h
 *
 * @brief ClipBoard helper functions definitions.
 */
// ID line follows -- this is updated by SVN
// $Id: ClipBoard.h 5545 2008-06-30 01:45:21Z marcelgosselin $

#ifndef _CLIPBOARD_H_
#define _CLIPBOARD_H_

#include "UnicodeString.h"

bool PutToClipboard(const String & text, HWND currentWindowHandle);
bool GetFromClipboard(String & text, HWND currentWindowHandle);
bool TextInClipboard();

#endif // _CLIPBOARD_H_
