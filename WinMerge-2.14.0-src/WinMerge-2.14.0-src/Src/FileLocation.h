/** 
 * @file FileLocation.h
 *
 * @brief Declaration for FileLocation class.
 *
 */
// ID line follows -- this is updated by SVN
// $Id: FileLocation.h 5444 2008-06-07 06:48:49Z kimmov $

#ifndef FileLocation_included
#define FileLocation_included

#include "FileTextEncoding.h"

/**
 * @brief A structure containing file's path and encoding information.
 */
struct FileLocation
{
	String filepath; /**< Full path for the file. */
	FileTextEncoding encoding; /**< Encoding info for the file. */

	/**
	 * The default constructor.
	 */
	FileLocation()
	{
	}

	/**
	 * The constructor taking a path as a parameter.
	 * @param [in] path Full path for the file.
	 */
	FileLocation(LPCTSTR path) : filepath(path)
	{
	}

// Methods

	/**
	 * Set the file path.
	 * @param [in] sFilePath Full path for the file.
	 */
	void setPath(const String & sFilePath) { this->filepath = sFilePath; }

// Copy constructor & copy operator should do the obvious correct things
};

#endif // FileLocation_included
