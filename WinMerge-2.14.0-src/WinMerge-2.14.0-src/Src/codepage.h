/**
 * @file  codepage.h
 */
// RCS ID line follows -- this is updated by SVN
// $Id: codepage.h 5759 2008-08-08 04:01:13Z marcelgosselin $

#ifndef __CODEPAGE_H__
#define __CODEPAGE_H__

void updateDefaultCodepage(int cpDefaultMode, int customCodepage);
int getDefaultCodepage();

bool isCodepageInstalled(int codepage);

#endif //__CODEPAGE_H__
