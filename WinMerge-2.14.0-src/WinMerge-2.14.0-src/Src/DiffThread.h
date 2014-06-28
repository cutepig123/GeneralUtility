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
 * @file  DiffThread.h
 *
 * @brief Declaration file for CDiffThread
 */
// ID line follows -- this is updated by SVN
// $Id: DiffThread.h 5576 2008-07-07 16:35:48Z jtuc $

#ifndef _DIFFTHREAD_H
#define _DIFFTHREAD_H

#include "diffcontext.h"

struct DiffFuncStruct;
class DiffThreadAbortable;

/**
 * @brief Class for threaded folder compare.
 * This class implements folder compare in two phases and in two threads:
 * - first thread collects items to compare to compare-time list
 *   (m_diffList).
 * - second threads compares items in the list.
 */
class CDiffThread
{
public:
	/** @brief Thread's states. */
	enum ThreadState
	{
		THREAD_NOTSTARTED = 0, /**< Thread not started, idle. */
		THREAD_COMPARING, /**< Thread running (comparing). */
		THREAD_COMPLETED, /**< Thread has completed its task. */
	};

// creation and use, called on main thread
	CDiffThread();
	~CDiffThread();
	void SetContext(CDiffContext * pCtx);
	UINT CompareDirectories(const String & dir1, const String & dir2, BOOL bRecursive);
	void SetHwnd(HWND hWnd);
	void SetMessageIDs(UINT updateMsg);
	void SetCompareSelected(bool bSelected = false);

// runtime interface for main thread, called on main thread
	UINT GetThreadState() const;
	void Abort() { m_bAborting = true; }
	bool IsAborting() const { return m_bAborting; }

// runtime interface for child thread, called on child thread
	bool ShouldAbort() const;

private:
	CDiffContext * m_pDiffContext; /**< Compare context storing results. */
	CWinThread * m_threads[2]; /**< Compare threads. */
	DiffFuncStruct * m_pDiffParm; /**< Structure for sending data to threads. */
	DiffThreadAbortable * m_pAbortgate;
	UINT m_msgUpdateUI; /**< UI-update message number */
	HWND m_hWnd; /**< Handle to folder compare GUI window */
	bool m_bAborting; /**< Is compare aborting? */
	bool m_bOnlyRequested; /**< Are we comparing only requested items (Update?) */
};

/**
 * @brief Structure used in sending data to the threads.
 * As thread functions have only one parameter we must pack all
 * the data we need inside structure.
 */
struct DiffFuncStruct
{
	CDiffContext * context; /**< Compare context. */
	UINT msgUIUpdate; /**< Windows message for updating GUI. */
	HWND hWindow; /**< Window getting status updates. */
	CDiffThread::ThreadState nThreadState; /**< Thread state. */
	BOOL bRecursive; /**< Is compare recursive (subfolders included)? */
	DiffThreadAbortable * m_pAbortgate; /**< Interface for aborting compare. */
	bool bOnlyRequested; /**< Compare only requested items? */
	HANDLE hSemaphore; /**< Semaphore for synchronizing threads. */

	DiffFuncStruct()
		: context(NULL)
		, msgUIUpdate(0)
		, hWindow(0)
		, nThreadState(CDiffThread::THREAD_NOTSTARTED)
		, bRecursive(FALSE)
		, m_pAbortgate(NULL)
		, bOnlyRequested(false)
		, hSemaphore(NULL)
		{}
};

// Thread functions
UINT DiffThreadCollect(LPVOID lpParam);
UINT DiffThreadCompare(LPVOID lpParam);

#endif /* _DIFFTHREAD_H */
