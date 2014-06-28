/**
 * @file  ByteCompare.h
 *
 * @brief Declaration file for ByteCompare
 */
// ID line follows -- this is updated by SVN
// $Id: ByteCompare.h 7327 2010-10-20 20:25:08Z gerundt $

#ifndef _BYTE_COMPARE_H_
#define _BYTE_COMPARE_H_

#include "FileTextStats.h"

class CompareOptions;
class QuickCompareOptions;
class IAbortable;
struct FileLocation;
struct file_data;

namespace CompareEngines
{

/**
 * @brief A quick compare -compare method implementation class.
 * This compare method compares files in small blocks. Code assumes block size
 * is in range of 32-bit int-type.
 */
class ByteCompare
{
public:
	ByteCompare();
	~ByteCompare();

	bool SetCompareOptions(const CompareOptions & options);
	void ClearCompareOptions();
	void SetAdditionalOptions(bool stopAfterFirstDiff);
	void SetAbortable(const IAbortable * piAbortable);
	void SetFileData(int items, file_data *data);
	int CompareFiles(FileLocation *location);
	void GetTextStats(int side, FileTextStats *stats);

private:
	QuickCompareOptions *m_pOptions; /**< Compare options for diffutils. */
	IAbortable * m_piAbortable;
	file_data * m_inf; /**< Compared files data (for diffutils). */
	FileTextStats m_textStats[2];

};

} // namespace CompareEngines

#endif // _BYTE_COMPARE_H_
