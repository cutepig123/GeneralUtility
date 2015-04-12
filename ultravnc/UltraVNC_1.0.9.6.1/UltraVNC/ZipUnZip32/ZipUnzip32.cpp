/////////////////////////////////////////////////////////////////////////////
//  Copyright (C) 2002 Ultr@VNC Team Members. All Rights Reserved.
// 
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
//  USA.
//
// If the source code for the program is not available from the place from
// which you received this file, check 
// http://www.uvnc.com/
//
////////////////////////////////////////////////////////////////////////////
/*
  Copyright (c) 1990-2002 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2000-Apr-09 or later
  (the contents of which are also included in unzip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/

#include <memory.h>
#include <stdio.h>
#include <string.h>
#include "zipunzip32.h"

extern "C" {
#include "../zipunzip_src/zip20/zip.h"
#include "../zipunzip_src/unzip/unzip.h"
int   EXPENTRY ZpInit(LPZIPUSERFUNCTIONS lpZipUserFunc);
BOOL  EXPENTRY ZpSetOptions(LPZPOPT Opts);
int   EXPENTRY ZpArchive(ZCL C);
int WINAPI Wiz_SingleEntryUnzip(int ifnc, char **ifnv, int xfnc, char **xfnv,LPDCL lpDCL, LPUSERFUNCTIONS lpUserFunc);
}


////////////////////////////////////////////////////////////////////////
// ZIP32 DLL PART
/////////////////////////////////////////////////////////////////////////

//
//
//
CZipUnZip32::CZipUnZip32()
{
	m_ZpOpt.fSuffix = FALSE;        /* include suffixes (not yet implemented) */
	m_ZpOpt.fEncrypt = FALSE;       /* true if encryption wanted */
	m_ZpOpt.fSystem = TRUE;        /* true to include system/hidden files */
	m_ZpOpt.fVolume = FALSE;        /* true if storing volume label */
	m_ZpOpt.fExtra = FALSE;         /* true if including extra attributes */
	m_ZpOpt.fNoDirEntries = FALSE;  /* true if ignoring directory entries */
	m_ZpOpt.fVerbose = FALSE;       /* true if full messages wanted */
	m_ZpOpt.fQuiet = TRUE;         /* true if minimum messages wanted */
	m_ZpOpt.fCRLF_LF = FALSE;       /* true if translate CR/LF to LF */
	m_ZpOpt.fLF_CRLF = FALSE;       /* true if translate LF to CR/LF */
	m_ZpOpt.fJunkDir = FALSE;       /* true if junking directory names */
	m_ZpOpt.fGrow = FALSE;          /* true if allow appending to zip file */
	m_ZpOpt.fForce = FALSE;         /* true if making entries using DOS names */
	m_ZpOpt.fMove = FALSE;          /* true if deleting files added or updated */
	m_ZpOpt.fUpdate = FALSE;        /* true if updating zip file--overwrite only if newer */
	m_ZpOpt.fFreshen = FALSE;       /* true if freshening zip file--overwrite only */
	m_ZpOpt.fJunkSFX = FALSE;       /* true if junking sfx prefix*/
	m_ZpOpt.fLatestTime = FALSE;    /* true if setting zip file time to time of latest file in archive */
	m_ZpOpt.fComment = FALSE;       /* true if putting comment in zip file */
	m_ZpOpt.fOffsets = FALSE;       /* true if updating archive offsets for sfx files */
	m_ZpOpt.fDeleteEntries = FALSE; /* true if deleting files from archive */
	m_ZpOpt.fRecurse = 1;           /* subdir recursing mode: 1 = "-r", 2 = "-R" */
	m_ZpOpt.fRepair = 0;            /* archive repair mode: 1 = "-F", 2 = "-FF" */
	m_ZpOpt.Date = NULL;
	m_ZpOpt.fExcludeDate = FALSE;      /* Exclude files newer than specified date */
	m_ZpOpt.fIncludeDate = FALSE;      /* Include only files newer than specified date */ 
	m_ZpOpt.fLevel = '1';/* Not using, set to NULL pointer */
	m_ZpOpt.fPrivilege = FALSE;        /* Use privileges (WIN32 only) */
	m_ZpOpt.fEncryption = FALSE;       
}

CZipUnZip32::~CZipUnZip32()
{

}


// 
// Zip the given directory content
// 
bool CZipUnZip32::ZipDirectory(char* szRootDir, char* szDirectoryToZip, char* szZipFileName, bool fCompress)
{
	LPSTR szFileList = NULL;
	char **index, *sz;
	int retcode, i, cc;

	// Init the User Function struct
	m_hZUF = GlobalAlloc( GPTR, (DWORD)sizeof(ZIPUSERFUNCTIONS));
	if (!m_hZUF)
	{
		return false;
	}
	m_lpZipUserFunctions = (LPZIPUSERFUNCTIONS)GlobalLock(m_hZUF);
	if (!m_lpZipUserFunctions)
	{
		GlobalFree(m_hZUF);
		return false;
	}
	m_lpZipUserFunctions->print = DummyPrint;
	m_lpZipUserFunctions->password = DummyPassword;
	m_lpZipUserFunctions->comment = DummyComment;

	if (!ZpInit(m_lpZipUserFunctions))
		{
			FreeUpZipMemory();
			return false;
		}
							
	// Set the infos on the file (directory) to zip
	m_ZpZCL.argc = 1;      
	m_ZpZCL.lpszZipFN = szZipFileName;
	m_hFileList = GlobalAlloc( GPTR, 0x10000L);
	if ( m_hFileList )
	{
	  szFileList = (char far *)GlobalLock(m_hFileList);
	}
	index = (char **)szFileList; // WARNING: szFileList potentially a NULL ptr
	cc = (sizeof(char *) * m_ZpZCL.argc);
	sz = szFileList + cc;
	for (i = 0; i < m_ZpZCL.argc; i++)
	{
		cc = strlen(szDirectoryToZip);
		strcpy(sz, szDirectoryToZip);
		index[i] = sz;
		sz += (cc + 1);
	}
	m_ZpZCL.FNV = (char **)szFileList;

	if (fCompress)
		m_ZpOpt.fLevel = '1';
	else
		m_ZpOpt.fLevel = '0';
	m_ZpOpt.fTemp = FALSE;
	m_ZpOpt.szTempDir = "";
	m_ZpOpt.szRootDir = szRootDir;
	ZpSetOptions(&m_ZpOpt);//m_PZipSetOptions(&m_ZpOpt);

	// Zip the Directory
	retcode = ZpArchive(m_ZpZCL);//retcode = m_PZipArchive(m_ZpZCL);

	// Free the temp resources
	GlobalUnlock(m_hFileList);
	GlobalFree(m_hFileList);
	FreeUpZipMemory();

	return (retcode == 0);
}

void CZipUnZip32::FreeUpZipMemory(void)
{
	if (m_hZUF)
	{
		GlobalUnlock(m_hZUF);
		GlobalFree(m_hZUF);
	}
}

int WINAPI DummyPassword(LPSTR p, int n, LPCSTR m, LPCSTR name)
{
	return 1;
}

int WINAPI DummyPrint(char far *buf, unsigned long size)
{
	return (unsigned int) size;
}


int WINAPI DummyComment(char far *szBuf)
{
	szBuf[0] = '\0';
	return 1;
} 


/////////////////////////////////////////////////////////////////////////
// END OF ZIP32 DLL PART
/////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////
// UNZIP32 DLL PART
/////////////////////////////////////////////////////////////////////////

bool CZipUnZip32::UnZipDirectory(char* szRootDir, char* szZipFileName)
{
	int exfc, infc;
	char **exfv, **infv;
	int retcode;
	
	m_hDCL = GlobalAlloc( GPTR, (DWORD)sizeof(DCL));
	if (!m_hDCL)
	{
		return false;
	}
	m_lpDCL = (LPDCL)GlobalLock(m_hDCL);
	if (!m_lpDCL)
	{
		GlobalFree(m_hDCL);
		return false;
	}
	
	m_hUF = GlobalAlloc( GPTR, (DWORD)sizeof(USERFUNCTIONS));
	if (!m_hUF)
	{
		GlobalUnlock(m_hDCL);
		GlobalFree(m_hDCL);
		return false;
	}
	m_lpUserFunctions = (LPUSERFUNCTIONS)GlobalLock(m_hUF);
	
	if (!m_lpUserFunctions)
	{
		GlobalUnlock(m_hDCL);
		GlobalFree(m_hDCL);
		GlobalFree(m_hUF);
		return false;
	}
	
	m_lpUserFunctions->password = password;
	m_lpUserFunctions->print = DisplayBuf;
	m_lpUserFunctions->sound = NULL;
	m_lpUserFunctions->replace = GetReplaceDlgRetVal;
	m_lpUserFunctions->SendApplicationMessage = ReceiveDllMessage;
	
	m_lpDCL->ncflag = 0; // Write to stdout if true 
	m_lpDCL->fQuiet = 2; // We want all messages. 1 = fewer messages,2 = no messages 
	m_lpDCL->ntflag = 0; // test zip file if true 
	m_lpDCL->nvflag = 0; // give a verbose listing if true 
	m_lpDCL->nzflag = 0; // display a zip file comment if true 
	m_lpDCL->ndflag = 1; // Recreate directories != 0, skip "../" if < 2
	m_lpDCL->naflag = 0; // Do not convert CR to CRLF 
	m_lpDCL->nfflag = 0; // Do not freshen existing files only 
	m_lpDCL->noflag = 1; // Over-write all files if true
	m_lpDCL->ExtractOnlyNewer = 0; // Do not extract only newer
	m_lpDCL->PromptToOverwrite = 0; // "Overwrite all" selected -> no query mode
	m_lpDCL->lpszZipFN = szZipFileName; // The archive name 
	m_lpDCL->lpszExtractDir = szRootDir; // The directory to extract to. This is set to NULL if you are extracting to the current directory.
	
	infc = exfc = 0;
	infv = exfv = NULL;

	// Unzip the directory content
	retcode = Wiz_SingleEntryUnzip(	infc,
											infv,
											exfc,
											exfv,
											m_lpDCL,
											m_lpUserFunctions
										);
	
	FreeUpUnzipMemory();
 	return (retcode == 0);
}

int WINAPI GetReplaceDlgRetVal(char *filename)
{
	return 1;
}

void CZipUnZip32::FreeUpUnzipMemory(void)
{
	if (m_hDCL)
	{
		GlobalUnlock(m_hDCL);
		GlobalFree(m_hDCL);
	}
	if (m_hUF)
	{
		GlobalUnlock(m_hUF);
		GlobalFree(m_hUF);
	}
}

void WINAPI ReceiveDllMessage(	unsigned long ucsize, unsigned long csiz,
								unsigned cfactor,
								unsigned mo, unsigned dy, unsigned yr, unsigned hh, unsigned mm,
								char c, LPSTR filename, LPSTR methbuf, unsigned long crc, char fCrypt)
{
}

int WINAPI password(char *p, int n, const char *m, const char *name)
{
	return 1;
}

int WINAPI DisplayBuf(LPSTR buf, unsigned long size)
{
	return (unsigned int) size;
}
