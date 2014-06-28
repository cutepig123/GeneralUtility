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
 * @file  DiffList.cpp
 *
 * @brief Implementation file for DiffList class
 */
// ID line follows -- this is updated by SVN
// $Id: DiffList.cpp 7418 2010-11-11 12:22:43Z gerundt $

#include "StdAfx.h"
#include "DiffList.h"
#include "DiffWrapper.h"
#include "coretools.h"

using std::swap;
using std::vector;

/**
 * @brief Swap diff sides.
 */
void DIFFRANGE::swap_sides()
{
	swap(begin0, begin1);
	swap(end0, end1);
	swap(dbegin0, dbegin1);
	swap(dend0, dend1);
	swap(blank0, blank1);
}

/**
 * @brief Default constructor, initialises difflist to 64 items.
 */
DiffList::DiffList()
: m_firstSignificant(-1)
, m_lastSignificant(-1)
{
	m_diffs.reserve(64); // Reserve some initial space to avoid allocations.
}

/**
 * @brief Removes all diffs from list.
 */
void DiffList::Clear()
{
	m_diffs.clear();
	m_firstSignificant = -1;
	m_lastSignificant = -1;
}

/**
 * @brief Returns count of items in diff list.
 * This function returns total amount of items (diffs) in list. So returned
 * count includes significant and non-significant diffs.
 * @note Use GetSignificantDiffs() to get count of non-ignored diffs.
 */
int DiffList::GetSize() const
{
	return m_diffs.size();
}

/**
 * @brief Returns count of significant diffs in the list.
 * This function returns total count of significant diffs in list. So returned
 * count doesn't include non-significant diffs.
 * @return Count of significant differences.
 */
int DiffList::GetSignificantDiffs() const
{
	int nSignificants = 0;
	const int nDiffCount = m_diffs.size();

	for (int i = 0; i < nDiffCount; i++)
	{
		const DIFFRANGE * dfi = DiffRangeAt(i);
		if (dfi->op != OP_TRIVIAL)
		{
			++nSignificants;
		}
	}
	return nSignificants;
}

/**
 * @brief Adds given diff to end of the list.
 * Adds given difference to end of the list (append).
 * @param [in] di Difference to add.
 */
void DiffList::AddDiff(const DIFFRANGE & di)
{
	DiffRangeInfo dri(di);

	// Allocate memory for new items exponentially
	if (m_diffs.size() == m_diffs.capacity())
		m_diffs.reserve(m_diffs.size() * 2);
	m_diffs.push_back(dri);
}

/**
 * @brief Checks if diff in given list index is significant or not.
 * @param [in] nDiff Index of DIFFRANGE to check.
 * @return TRUE if diff is significant, FALSE if not.
 */
bool DiffList::IsDiffSignificant(int nDiff) const
{
	const DIFFRANGE * dfi = DiffRangeAt(nDiff);
	if (dfi->op != OP_TRIVIAL)
		return true;
	else
		return false;
}

/**
 * @brief Get significant difference index of the diff.
 * This function returns the index of diff when only significant differences
 * are calculated.
 * @param [in] nDiff Index of difference to check.
 * @return Significant difference index of the diff.
 */
int DiffList::GetSignificantIndex(int nDiff) const
{
	int significants = -1;

	for (int i = 0; i <= nDiff; i++)
	{
		const DIFFRANGE * dfi = DiffRangeAt(i);
		if (dfi->op != OP_TRIVIAL)
		{
			++significants;
		}
	}
	return significants;
}

/**
 * @brief Returns copy of DIFFRANGE from diff-list.
 * @param [in] nDiff Index of DIFFRANGE to return.
 * @param [out] di DIFFRANGE returned (empty if error)
 * @return TRUE if DIFFRANGE found from given index.
 */
bool DiffList::GetDiff(int nDiff, DIFFRANGE & di) const
{
	const DIFFRANGE * dfi = DiffRangeAt(nDiff);
	if (!dfi)
	{
		DIFFRANGE empty;
		di = empty;
		return false;
	}
	di = *dfi;
	return true;
}

/**
 * @brief Return constant pointer to requested diff.
 * This function returns constant pointer to DIFFRANGE at given index.
 * @param [in] nDiff Index of DIFFRANGE to return.
 * @return Constant pointer to DIFFRANGE.
 */
const DIFFRANGE * DiffList::DiffRangeAt(int nDiff) const
{
	if (nDiff >= 0 && nDiff < m_diffs.size())
	{
		return &m_diffs[nDiff].diffrange;
	}
	else
	{
		ASSERT(0);
		return NULL;
	}
}

/**
 * @brief Replaces diff in list in given index with given diff.
 * @param [in] nDiff Index (0-based) of diff to be replaced
 * @param [in] di Diff to put in list.
 * @return TRUE if index was valid and diff put to list.
 */
bool DiffList::SetDiff(int nDiff, const DIFFRANGE & di)
{
	if (nDiff < m_diffs.size())
	{
		m_diffs[nDiff] = di;
		return true;
	}
	else
		return false;
}

/**
 * @brief Checks if line is before, inside or after diff
 * @param [in] nLine Linenumber to text buffer (not "real" number)
 * @param [in] nDiff Index to diff table
 * @return -1 if line is before diff, 0 if line is in diff and
 * 1 if line is after diff.
 */
int DiffList::LineRelDiff(UINT nLine, UINT nDiff) const
{
	const DIFFRANGE * dfi = DiffRangeAt(nDiff);
	if (nLine < dfi->dbegin0)
		return -1;
	else if (nLine > dfi->dend0)
		return 1;
	else
		return 0;
}

/**
 * @brief Checks if line is inside given diff
 * @param [in] nLine Linenumber to text buffer (not "real" number)
 * @param [in] nDiff Index to diff table
 * @return TRUE if line is inside given difference.
 */
bool DiffList::LineInDiff(UINT nLine, UINT nDiff) const
{
	const DIFFRANGE * dfi = DiffRangeAt(nDiff);
	if (nLine >= dfi->dbegin0 && nLine <= dfi->dend0)
		return true;
	else
		return false;
}

/**
 * @brief Returns diff index for given line.
 * @param [in] nLine Linenumber, 0-based.
 * @return Index to diff table, -1 if line is not inside any diff.
 */
int DiffList::LineToDiff(UINT nLine) const
{
	const int nDiffCount = m_diffs.size();
	if (nDiffCount == 0)
		return -1;

	// First check line is not before first or after last diff
	if (nLine < DiffRangeAt(0)->dbegin0)
		return -1;
	if (nLine > DiffRangeAt(nDiffCount-1)->dend0)
		return -1;

	// Use binary search to search for a diff.
	int left = 0; // Left limit
	int middle = 0; // Compared item
	int right = nDiffCount - 1; // Right limit

	while (left <= right)
	{
		middle = (left + right) / 2;
		int result = LineRelDiff(nLine, middle);
		switch (result)
		{
		case -1: // Line is before diff in file
			right = middle - 1;
			break;
		case 0: // Line is in diff
			return middle;
			break;
		case 1: // Line is after diff in file
			left = middle + 1;
			break;
		default:
			_RPTF1(_CRT_ERROR, "Invalid return value %d from LineRelDiff(): "
				"-1, 0 or 1 expected!", result); 
			break;
		}
	}
	return -1;
}

/**
 * @brief Return previous diff from given line.
 * @param [in] nLine First line searched.
 * @param [out] nDiff Index of diff found.
 * @return TRUE if line is inside diff, FALSE otherwise.
 */
bool DiffList::GetPrevDiff(int nLine, int & nDiff) const
{
	bool bInDiff = true;
	int numDiff = LineToDiff(nLine);

	// Line not inside diff
	if (nDiff == -1)
	{
		bInDiff = false;
		for (int i = (int) m_diffs.size() - 1; i >= 0 ; i--)
		{
			if ((int)DiffRangeAt(i)->dend0 <= nLine)
			{
				numDiff = i;
				break;
			}
		}
	}
	nDiff = numDiff;
	return bInDiff;
}

/**
 * @brief Return next difference from given line.
 * This function finds next difference from given line. If line is inside
 * difference, that difference is returned. If next difference is not found
 * param @p nDiff is set to -1.
 * @param [in] nLine First line searched.
 * @param [out] nDiff Index of diff found.
 * @return TRUE if line is inside diff, FALSE otherwise.
 */
bool DiffList::GetNextDiff(int nLine, int & nDiff) const
{
	bool bInDiff = true;
	int numDiff = LineToDiff(nLine);

	// Line not inside diff
	if (numDiff == -1)
	{
		bInDiff = false;
		const int nDiffCount = m_diffs.size();
		for (int i = 0; i < nDiffCount; i++)
		{
			if ((int)DiffRangeAt(i)->dbegin0 >= nLine)
			{
				numDiff = i;
				break;
			}
		}
	}
	nDiff = numDiff;
	return bInDiff;
}

/**
 * @brief Check if diff-list contains significant diffs.
 * @return TRUE if list has significant diffs, FALSE otherwise.
 */
bool DiffList::HasSignificantDiffs() const
{
	if (m_firstSignificant == -1)
		return false;
	return true;
}

/**
 * @brief Return previous diff index from given line.
 * @param [in] nLine First line searched.
 * @return Index for next difference or -1 if no difference is found.
 */
int DiffList::PrevSignificantDiffFromLine(UINT nLine) const
{
	int nDiff = -1;

	for (int i = m_diffs.size() - 1; i >= 0 ; i--)
	{
		const DIFFRANGE * dfi = DiffRangeAt(i);
		if (dfi->op != OP_TRIVIAL && dfi->dend0 <= nLine)
		{
			nDiff = i;
			break;
		}
	}
	return nDiff;
}

/**
 * @brief Return next diff index from given line.
 * @param [in] nLine First line searched.
 * @return Index for previous difference or -1 if no difference is found.
 */
int DiffList::NextSignificantDiffFromLine(UINT nLine) const
{
	int nDiff = -1;
	const int nDiffCount = m_diffs.size();

	for (int i = 0; i < nDiffCount; i++)
	{
		const DIFFRANGE * dfi = DiffRangeAt(i);
		if (dfi->op != OP_TRIVIAL && dfi->dbegin0 >= nLine)
		{
			nDiff = i;
			break;
		}
	}
	return nDiff;
}

/**
 * @brief Construct the doubly-linked chain of significant differences
 */
void DiffList::ConstructSignificantChain()
{
	m_firstSignificant = -1;
	m_lastSignificant = -1;
	int prev = -1;
	// must be called after diff list is entirely populated
    for (vector<DiffRangeInfo>::size_type i = 0; i < m_diffs.size(); ++i)
	{
		if (m_diffs[i].diffrange.op == OP_TRIVIAL)
		{
			m_diffs[i].prev = -1;
			m_diffs[i].next = -1;
		}
		else
		{
			m_diffs[i].prev = prev;
			if (prev != -1)
				m_diffs[prev].next = i;
			prev = i;
			if (m_firstSignificant == -1)
				m_firstSignificant = i;
			m_lastSignificant = i;
		}
	}
}

/**
 * @brief Return index to first significant difference.
 * @return Index of first significant difference.
 */
int DiffList::FirstSignificantDiff() const
{
	return m_firstSignificant;
}

/**
 * @brief Return index of next significant diff.
 * @param [in] nDiff Index to start looking for next diff.
 * @return Index of next significant difference.
 */
int DiffList::NextSignificantDiff(int nDiff) const
{
	return m_diffs[nDiff].next;
}

/**
 * @brief Return index of previous significant diff.
 * @param [in] nDiff Index to start looking for previous diff.
 * @return Index of previous significant difference.
 */
int DiffList::PrevSignificantDiff(int nDiff) const
{
	return m_diffs[nDiff].prev;
}

/**
 * @brief Return index to last significant diff.
 * @return Index of last significant difference.
 */
int DiffList::LastSignificantDiff() const
{
	return m_lastSignificant;
}

/**
 * @brief Return pointer to first significant diff.
 * @return Constant pointer to first significant difference.
 */
const DIFFRANGE * DiffList::FirstSignificantDiffRange() const
{
	if (m_firstSignificant == -1)
		return NULL;
	return DiffRangeAt(m_firstSignificant);
}

/**
 * @brief Return pointer to last significant diff.
 * @return Constant pointer to last significant difference.
 */
const DIFFRANGE * DiffList::LastSignificantDiffRange() const
{
	if (m_lastSignificant == -1)
		return NULL;
	return DiffRangeAt(m_lastSignificant);
}

/**
 * @brief Swap sides in diffrange.
 */
void DiffList::Swap()
{
	vector<DiffRangeInfo>::iterator iter = m_diffs.begin();
	while (iter != m_diffs.end())
	{
		(*iter).diffrange.swap_sides();
		++iter;
	}
}

/**
 * @brief Count number of lines to add to sides (because of synch).
 * @param [out] nLeftLines Number of lines to add to left side.
 * @param [out] nRightLines Number of lines to add to right side.
 */
void DiffList::GetExtraLinesCounts(int &nLeftLines, int &nRightLines)
{
	nLeftLines = 0;
	nRightLines = 0;
	int nDiffCount = GetSize();

	for (int nDiff = 0; nDiff < nDiffCount; ++nDiff)
	{
		DIFFRANGE curDiff;
		VERIFY(GetDiff(nDiff, curDiff));

		// this guarantees that all the diffs are synchronized
		ASSERT(curDiff.begin0 + nLeftLines == curDiff.begin1 + nRightLines);
		int nline0 = curDiff.end0 - curDiff.begin0 + 1;
		int nline1 = curDiff.end1 - curDiff.begin1 + 1;
		int nextra = nline0 - nline1;

		if (nextra > 0)
			nRightLines += nextra;
		else
			nLeftLines -= nextra;
	}
}
