/**
 *  @file TempFile.h
 *
 *  @brief Declaration of TempFile
 */
// ID line follows -- this is updated by SVN
// $Id: TempFile.h 4929 2008-01-18 20:03:57Z kimmov $

#ifndef _TEMP_FILE_
#define _TEMP_FILE_

/**
 * @brief A simple temporary file holder class.
 * This class creates and holds temporary file names. When instance
 * gets destroyed, the temporary file is also deleted.
 */
class TempFile
{
public:
	TempFile() {}
	~TempFile();
	void Create();
	String Create(LPCTSTR prefix = NULL);
	String CreateFromFile(LPCTSTR filepath, LPCTSTR prefix);
	String GetPath();
	bool Delete();

private:
	String m_path; /**< Temporary file path. */
};

#endif // _TEMP_FILE_
