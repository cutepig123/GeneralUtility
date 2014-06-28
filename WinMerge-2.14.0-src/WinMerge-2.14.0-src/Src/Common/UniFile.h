/**
 *  @file   UniFile.h
 *  @author Perry Rapp, Creator, 2003-2006
 *  @date   Created: 2003-10
 *  @date   Edited:  2006-02-20 (Perry Rapp)
 *
 *  @brief  Declaration of Unicode file classes.
 */
// ID line follows -- this is updated by SVN
// $Id: UniFile.h 7480 2010-12-27 16:08:47Z gerundt $

#ifndef UniFile_h_included
#define UniFile_h_included

#include "unicoder.h"

/**
 * @brief Interface to file classes in this module
 */
class UniFile
{
public:

	/**
	 * @brief A struct for error message or error code.
	 */
	struct UniError
	{
		String apiname;
		int syserrnum; // valid if apiname nonempty
		String desc; // valid if apiname empty

		UniError();
		bool HasError() const;
		void ClearError();
		String GetError() const;
	};

	virtual ~UniFile() { }
	virtual bool OpenReadOnly(LPCTSTR filename) = 0;
	virtual void Close() = 0;
	virtual bool IsOpen() const = 0;

	virtual String GetFullyQualifiedPath() const = 0;
	virtual const UniError & GetLastUniError() const = 0;

	virtual bool IsUnicode() = 0;
	virtual bool ReadBom() = 0;
	virtual bool HasBom() = 0;
	virtual void SetBom(bool bom) = 0;

	virtual ucr::UNICODESET GetUnicoding() const = 0;
	virtual void SetUnicoding(ucr::UNICODESET unicoding) = 0;
	virtual int GetCodepage() const = 0;
	virtual void SetCodepage(int codepage) = 0;

public:
	virtual bool ReadString(String & line, bool * lossy) = 0;
	virtual bool ReadString(String & line, String & eol, bool * lossy) = 0;
	virtual int GetLineNumber() const = 0;
	virtual __int64 GetPosition() const = 0;
	virtual bool WriteString(const String & line) = 0;

	struct txtstats
	{
		int ncrs;
		int nlfs;
		int ncrlfs;
		int nzeros;
		INT64 first_zero; // byte offset, initially -1
		INT64 last_zero; // byte offset, initially -1
		int nlosses;
		txtstats() { clear(); }
		void clear() { ncrs = nlfs = ncrlfs = nzeros = nlosses = 0; first_zero = -1; last_zero = -1; }
	};
	virtual const txtstats & GetTxtStats() const = 0;
};

/**
 * @brief Local file access code used by both UniMemFile and UniStdioFile
 *
 * This class lacks an actual handle to a file
 */
class UniLocalFile : public UniFile
{
public:
	UniLocalFile();
	void Clear();

	virtual String GetFullyQualifiedPath() const { return m_filepath; }
	virtual const UniError & GetLastUniError() const { return m_lastError; }

	virtual ucr::UNICODESET GetUnicoding() const { return m_unicoding; }
	virtual void SetUnicoding(ucr::UNICODESET unicoding) { m_unicoding = unicoding; }
	virtual int GetCodepage() const { return m_codepage; }
	virtual void SetCodepage(int codepage) { m_codepage = codepage; }

	virtual int GetLineNumber() const { return m_lineno; }
	virtual const txtstats & GetTxtStats() const { return m_txtstats; }

	bool IsUnicode();

protected:
	virtual bool DoGetFileStatus();
	virtual void LastError(LPCTSTR apiname, int syserrnum);
	virtual void LastErrorCustom(LPCTSTR desc);

protected:
	int m_statusFetched; // 0 not fetched, -1 error, +1 success
	__int64 m_filesize;
	String m_filepath;
	String m_filename;
	int m_lineno; // current 0-based line of m_current
	UniError m_lastError;
	ucr::UNICODESET m_unicoding;
	int m_charsize; // 2 for UCS-2, else 1
	int m_codepage; // only valid if m_unicoding==ucr::NONE;
	txtstats m_txtstats;
	bool m_bom; /**< Did the file have a BOM when reading? */
	bool m_bUnicodingChecked; /**< Has unicoding been checked for the file? */
	bool m_bUnicode; /**< Is the file unicode file? */
};

/**
 * @brief Memory-Mapped disk file (read-only access)
 */
class UniMemFile : public UniLocalFile
{
	friend class UniMarkdownFile;
public:
	UniMemFile();
	virtual ~UniMemFile() { Close(); }

	virtual bool GetFileStatus();

	virtual bool OpenReadOnly(LPCTSTR filename);
	virtual bool Open(LPCTSTR filename);
	virtual bool Open(LPCTSTR filename, DWORD dwOpenAccess, DWORD dwOpenShareMode, DWORD dwOpenCreationDispostion, DWORD dwMappingProtect, DWORD dwMapViewAccess);
	void Close();
	virtual bool IsOpen() const;

	virtual bool ReadBom();
	virtual bool HasBom();
	virtual void SetBom(bool bom);

public:
	virtual bool ReadString(String & line, bool * lossy);
	virtual bool ReadString(String & line, String & eol, bool * lossy);
	virtual __int64 GetPosition() const { return m_current - m_base; }
	virtual bool WriteString(const String & line);

// Implementation methods
protected:
	virtual bool DoOpen(LPCTSTR filename, DWORD dwOpenAccess, DWORD dwOpenShareMode, DWORD dwOpenCreationDispostion, DWORD dwMappingProtect, DWORD dwMapViewAccess);

// Implementation data
private:
	HANDLE m_handle;
	HANDLE m_hMapping;
	LPBYTE m_base; // points to base of mapping
	LPBYTE m_data; // similar to m_base, but after BOM if any
	LPBYTE m_current; // current location in file
};

/**
 * @brief Regular buffered file (write-only access)
 * (ReadString methods have never been implemented,
 *  because UniMemFile above is good for reading)
 */
class UniStdioFile : public UniLocalFile
{
public:
	UniStdioFile();
	~UniStdioFile();

	virtual bool GetFileStatus();

	virtual bool OpenReadOnly(LPCTSTR filename);
	virtual bool OpenCreate(LPCTSTR filename);
	virtual bool OpenCreateUtf8(LPCTSTR filename);
	virtual bool Open(LPCTSTR filename, LPCTSTR mode);
	void Close();

	virtual bool IsOpen() const;

	virtual bool ReadBom();
	virtual bool HasBom();
	virtual void SetBom(bool bom);

protected:
	virtual bool ReadString(String & line, bool * lossy);
	virtual bool ReadString(String & line, String & eol, bool * lossy);

public:
	virtual __int64 GetPosition() const;

	virtual int WriteBom();
	virtual bool WriteString(const String & line);

// Implementation methods
protected:
	virtual bool DoOpen(LPCTSTR filename, LPCTSTR mode);
	virtual void LastError(LPCTSTR apiname, int syserrnum);
	virtual void LastErrorCustom(LPCTSTR desc);

// Implementation data
private:
	FILE * m_fp;
	__int64 m_data; // offset after any initial BOM
	void * m_pucrbuff;
};



#endif // UniFile_h_included
