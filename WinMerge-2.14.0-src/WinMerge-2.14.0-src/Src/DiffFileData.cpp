/** 
 * @file DiffFileData.cpp
 *
 * @brief Code for DiffFileData class
 *
 * @date  Created: 2003-08-22
 */
// ID line follows -- this is updated by SVN
// $Id: DiffFileData.cpp 7513 2011-01-30 12:56:27Z jtuc $

#include "stdafx.h"
#include <io.h>
#include <tchar.h>
#include <shlwapi.h>
#include "coretypes.h"
#include "DiffItem.h"
#include "FileLocation.h"
#include "Diff.h"
#include "FileTransform.h"
#include "common/unicoder.h"
#include "AnsiConvert.h"
#include "DiffFileData.h"


static int f_defcp = 0; // default codepage

/**
 * @brief Simple initialization of DiffFileData
 * @note Diffcounts are initialized to invalid values, not zeros.
 */
DiffFileData::DiffFileData()
{
	m_inf = new file_data[2];
	int i = 0;
	for (i = 0; i < 2; ++i)
		memset(&m_inf[i], 0, sizeof(m_inf[i]));
	m_used = false;
	Reset();
	// Set default codepages
	for (i = 0; i < countof(m_FileLocation); ++i)
	{
		m_FileLocation[i].encoding.SetCodepage(f_defcp);
	}
}

/** @brief deallocate member data */
DiffFileData::~DiffFileData()
{
	Reset();
	delete [] m_inf;
}

/** @brief Allow caller to specify codepage to assume for all unknown files */
void DiffFileData::SetDefaultCodepage(int defcp)
{
	f_defcp = defcp;
}

/** @brief Open file descriptors in the inf structure (return false if failure) */
bool DiffFileData::OpenFiles(LPCTSTR szFilepath1, LPCTSTR szFilepath2)
{
	m_FileLocation[0].setPath(szFilepath1);
	m_FileLocation[1].setPath(szFilepath2);
	bool b = DoOpenFiles();
	if (!b)
		Reset();
	return b;
}

/** @brief stash away true names for display, before opening files */
void DiffFileData::SetDisplayFilepaths(LPCTSTR szTrueFilepath1, LPCTSTR szTrueFilepath2)
{
	m_sDisplayFilepath[0] = szTrueFilepath1;
	m_sDisplayFilepath[1] = szTrueFilepath2;
}


/** @brief Open file descriptors in the inf structure (return false if failure) */
bool DiffFileData::DoOpenFiles()
{
	Reset();

	for (int i = 0; i < 2; ++i)
	{
		// Fill in 8-bit versions of names for diffutils (WinMerge doesn't use these)
		// Actual paths are m_FileLocation[i].filepath
		// but these are often temporary files
		// Displayable (original) paths are m_sDisplayFilepath[i]
		m_inf[i].name = ansiconvert_SystemCP(m_sDisplayFilepath[i]);
		if (m_inf[i].name == NULL)
			return false;

		// Open up file descriptors
		// Always use O_BINARY mode, to avoid terminating file read on ctrl-Z (DOS EOF)
		// Also, WinMerge-modified diffutils handles all three major eol styles
		if (m_inf[i].desc == 0)
		{
			m_inf[i].desc = _topen(m_FileLocation[i].filepath.c_str(),
					O_RDONLY | O_BINARY, _S_IREAD);
		}
		if (m_inf[i].desc < 0)
			return false;

		// Get file stats (diffutils uses these)
		if (_fstat(m_inf[i].desc, &m_inf[i].stat) != 0)
		{
			return false;
		}
		
		if (string_compare_nocase(m_FileLocation[0].filepath,
				m_FileLocation[1].filepath) == 0)
		{
			m_inf[1].desc = m_inf[0].desc;
		}
	}

	m_used = true;
	return true;
}

/** @brief Clear inf structure to pristine */
void DiffFileData::Reset()
{
	ASSERT(m_inf);
	// If diffutils put data in, have it cleanup
	if (m_used)
	{
		cleanup_file_buffers(m_inf);
		m_used = false;
	}
	// clean up any open file handles, and zero stuff out
	// open file handles might be leftover from a failure in DiffFileData::OpenFiles
	for (int i = 0; i < 2; ++i)
	{
		if (m_inf[1].desc == m_inf[0].desc)
		{
			m_inf[1].desc = 0;
		}
		free((void *)m_inf[i].name);
		m_inf[i].name = NULL;

		if (m_inf[i].desc > 0)
		{
			_close(m_inf[i].desc);
		}
		m_inf[i].desc = 0;
		memset(&m_inf[i], 0, sizeof(m_inf[i]));
	}
}

/** @brief detect unicode file and quess encoding */
DiffFileData::UniFileBom::UniFileBom(int fd)
{
	size = 0;
	unicoding = ucr::NONE;
	if (fd == -1) 
		return;
	long tmp = _lseek(fd, 0, SEEK_SET);

	const int max_size = 8 * 1024;
	unsigned char* buffer = new unsigned char[max_size];
	if (buffer == NULL)
		return;

	int bytes = _read(fd, buffer, max_size);
	bom = false;
	unicoding = ucr::DetermineEncoding(buffer, bytes, &bom);
	switch (unicoding)
	{
		case ucr::UCS2LE:
		case ucr::UCS2BE:
			size = 2;
			break;
		case ucr::UTF8:
			if (bom)
				size = 3;
			else
				size = 0;
			break;
		default:
			break;
	}

	delete[] buffer;
	_lseek(fd, tmp, SEEK_SET);
}

/**
 * @brief Invoke appropriate plugins for prediffing
 * return false if anything fails
 * caller has to DeleteFile filepathTransformed, if it differs from filepath
 */
bool DiffFileData::Filepath_Transform(FileLocation & fpenc,
	const String & filepath, String & filepathTransformed,
	LPCTSTR filteredFilenames, PrediffingInfo * infoPrediffer)
{
	BOOL bMayOverwrite = FALSE; // temp variable set each time it is used

	if (fpenc.encoding.m_unicoding && fpenc.encoding.m_unicoding != ucr::UCS2LE)
	{
		// second step : normalize Unicode to OLECHAR (most of time, do nothing)
		// (OLECHAR = UCS-2LE in Windows)
		bMayOverwrite = (filepathTransformed != filepath); // may overwrite if we've already copied to temp file
		if (!FileTransform_NormalizeUnicode(filepathTransformed, bMayOverwrite))
			return false;
	}

	// Note: filepathTransformed may be in UCS-2 (if toUtf8), or it may be raw encoding (if !Utf8)
	// prediff plugins must handle both

	// third step : prediff (plugins)
	bMayOverwrite = (filepathTransformed != filepath); // may overwrite if we've already copied to temp file
	if (infoPrediffer->bToBeScanned)
	{
		// FileTransform_Prediffing tries each prediffer for the pointed out filteredFilenames
		// if a prediffer fails, we consider it is not the good one, that's all
		// FileTransform_Prediffing returns FALSE only if the prediffer works, 
		// but the data can not be saved to disk (no more place ??)
		if (FileTransform_Prediffing(filepathTransformed, filteredFilenames, infoPrediffer, bMayOverwrite) 
				== FALSE)
			return false;
	}
	else
	{
		// this can failed if the pointed out prediffer has a problem
		if (FileTransform_Prediffing(filepathTransformed, *infoPrediffer, bMayOverwrite) 
				== FALSE)
			return false;
	}

	if (fpenc.encoding.m_unicoding)
	{
		// fourth step : prepare for diffing
		// may overwrite if we've already copied to temp file
		BOOL bMayOverwrite = 0 != lstrcmpi(filepathTransformed.c_str(), filepath.c_str());
		if (!FileTransform_UCS2ToUTF8(filepathTransformed, bMayOverwrite))
			return false;
	}
	return true;
}
