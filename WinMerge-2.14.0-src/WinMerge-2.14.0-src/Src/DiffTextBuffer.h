/** 
 * @file  DiffTextBuffer.h
 *
 * @brief Declaration of CDiffTextBuffer class
 */
// ID line follows -- this is updated by SVN
// $Id: DiffTextBuffer.h 7489 2010-12-31 11:55:48Z gerundt $

#ifndef _DIFFTEXT_BUFFER_H_
#define _DIFFTEXT_BUFFER_H_

#include "GhostTextBuffer.h"
#include "FileTextEncoding.h"

class CMergedoc;
class PackingInfo;

/**
 * @brief Specialized buffer to save file data
 */
class CDiffTextBuffer : public CGhostTextBuffer
{
	friend class CMergeDoc;

private :
	CMergeDoc * m_pOwnerDoc; /**< Merge document owning this buffer. */
	int m_nThisPane; /**< Left/Right side */
	String m_strTempPath; /**< Temporary files folder. */
	int m_unpackerSubcode; /**< Plugin information. */
	bool m_bMixedEOL; /**< EOL style of this buffer is mixed? */

	/** 
	 * @brief Unicode encoding from ucr::UNICODESET.
	 *
	 * @note m_unicoding and m_codepage are indications of how the buffer is
	 * supposed to be saved on disk. In memory, it is invariant, depending on
	 * build:
	 * - ANSI: in memory it is CP_ACP/CP_THREAD_ACP 8-bit characters
	 * - Unicode: in memory it is wchars
	 */
	FileTextEncoding m_encoding;

	BOOL FlagIsSet(UINT line, DWORD flag) const;

public :
	CDiffTextBuffer(CMergeDoc * pDoc, int pane);

	void SetTempPath(const String &path);
	virtual void AddUndoRecord (BOOL bInsert, const CPoint & ptStartPos,
		const CPoint & ptEndPos, LPCTSTR pszText, int cchText,
		int nLinesToValidate, int nActionType = CE_ACTION_UNKNOWN,
		CDWordArray *paSavedRevisonNumbers = NULL);
	bool curUndoGroup();
	void ReplaceLine(CCrystalTextView * pSource, int nLine, LPCTSTR pchText,
		int cchText, int nAction =CE_ACTION_UNKNOWN);
	void ReplaceFullLine(CCrystalTextView * pSource, int nLine,
		const CString& strText, int nAction = CE_ACTION_UNKNOWN);

	int LoadFromFile(LPCTSTR pszFileName, PackingInfo * infoUnpacker,
		LPCTSTR filteredFilenames, BOOL & readOnly, CRLFSTYLE nCrlfStyle,
		const FileTextEncoding & encoding, CString &sError);
	int SaveToFile (LPCTSTR pszFileName, BOOL bTempFile, String & sError,
		PackingInfo * infoUnpacker = NULL, CRLFSTYLE nCrlfStyle = CRLF_STYLE_AUTOMATIC,
		BOOL bClearModifiedFlag = TRUE );
	ucr::UNICODESET getUnicoding() const { return m_encoding.m_unicoding; }
	void setUnicoding(ucr::UNICODESET value) { m_encoding.m_unicoding = value; }
	int getCodepage() const { return m_encoding.m_codepage; }
	void setCodepage(int value) { m_encoding.m_codepage = value; }
	const FileTextEncoding & getEncoding() const { return m_encoding; }
	bool IsMixedEOL() const { return m_bMixedEOL; }
	void SetMixedEOL(bool bMixed) { m_bMixedEOL = bMixed; }

	// If line has text (excluding eol), set strLine to text (excluding eol)
	BOOL GetLine(int nLineIndex, CString &strLine) const;

	// if line has any text (including eol), set strLine to text (including eol)
	BOOL GetFullLine(int nLineIndex, CString &strLine) const;

	virtual void SetModified (BOOL bModified = TRUE);
	void prepareForRescan();
	virtual void OnNotifyLineHasBeenEdited(int nLine);
	bool IsInitialized() const;
};

#endif // _DIFFTEXT_BUFFER_H_
