/** 
 * @file  MovedLines.cpp
 *
 * @brief Implementation of MovedLines class.
 */

// Disable VC6's "identifier was truncated..." warning. It is meaningless.
#if _MSC_VER == 1200
	#pragma warning(disable: 4786)
#endif
#include <map>
#include "MovedLines.h"

using std::map;

/**
 * @brief clear the lists of moved blocks.
 */
void MovedLines::Clear()
{
	m_moved0.clear();
	m_moved1.clear();
}

/**
 * @brief Add moved block to the list.
 * @param [in] side1 First side we are mapping.
 * @param [in] line1 Linenumber in side first side.
 * @param [in] line2 Linenumber in second side.
 */
void MovedLines::Add(ML_SIDE side1, unsigned int line1,	unsigned int line2)
{
	map<int, int> * list;
	if (side1 == SIDE_LEFT)
		list = &m_moved0;
	else
		list = &m_moved1;

	(*list)[line1] = line2;
}

/**
 * @brief Check if line is in moved block.
 * @param [in] line Linenumber to check.
 * @param [in] side Side of the linenumber.
 */
int MovedLines::LineInBlock(unsigned int line, ML_SIDE side)
{
	if (side == SIDE_LEFT)
		return SecondSideInMovedBlock(line);
	else
		return FirstSideInMovedBlock(line);
}

/**
 * @brief Get right->left info for a moved line (real line number)
 */
int MovedLines::FirstSideInMovedBlock(unsigned int secondSideLine)
{
	map<int, int>::const_iterator iter;
	iter = m_moved1.find(secondSideLine);
	if (iter != m_moved1.end())
		return iter->second;
	else
		return -1;
}

/**
 * @brief Get left->right info for a moved line (real line number)
 */
int MovedLines::SecondSideInMovedBlock(unsigned int firstSideLine)
{
	map<int, int>::const_iterator iter;
	iter = m_moved0.find(firstSideLine);
	if (iter != m_moved0.end())
		return iter->second;
	else
		return -1;
}
