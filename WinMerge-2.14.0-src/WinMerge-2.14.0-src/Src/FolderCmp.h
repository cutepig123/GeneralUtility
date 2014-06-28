/** 
 * @file  FolderCmp.h
 *
 * @brief Declaration file for FolderCmp
 */
// ID line follows -- this is updated by SVN
// $Id: FolderCmp.h 7492 2011-01-01 13:40:57Z gerundt $

#ifndef _FOLDERCMP_H_
#define _FOLDERCMP_H_

#include "DiffFileData.h"
#include "DiffUtils.h"
#include "ByteCompare.h"
#include "TimeSizeCompare.h"

class CDiffContext;
class PackingInfo;
class PrediffingInfo;

/**
 * @brief Holds plugin-related paths and information.
 */
struct PluginsContext
{
	String origFileName1;
	String origFileName2;
	String filepathUnpacked1;
	String filepathUnpacked2;
	String filepathTransformed1;
	String filepathTransformed2;
	PackingInfo * infoUnpacker;
	PrediffingInfo * infoPrediffer;
};

/**
 * @brief Class implementing file compare for folder compare.
 * This class implements (called from DirScan.cpp) compare of two files
 * during folder compare. The class implements both diffutils compare and
 * quick compare.
 */
class FolderCmp
{
public:
	FolderCmp();
	~FolderCmp();
	bool RunPlugins(CDiffContext * pCtxt, PluginsContext * plugCtxt, String &errStr);
	void CleanupAfterPlugins(PluginsContext *plugCtxt);
	UINT prepAndCompareTwoFiles(CDiffContext * pCtxt, DIFFITEM &di);

	int m_ndiffs;
	int m_ntrivialdiffs;

	DiffFileData m_diffFileData;
	CDiffContext * m_pCtx;

private:
	CompareEngines::DiffUtils *m_pDiffUtilsEngine;
	CompareEngines::ByteCompare *m_pByteCompare;
	CompareEngines::TimeSizeCompare *m_pTimeSizeCompare;
};


#endif // _FOLDERCMP_H_
