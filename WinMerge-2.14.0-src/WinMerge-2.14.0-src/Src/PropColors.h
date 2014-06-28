/** 
 * @file  PropColors.h
 *
 * @brief Declaration file for PropMergeColors propertyheet
 *
 */
// ID line follows -- this is updated by SVN
// $Id: PropColors.h 7501 2011-01-03 13:29:00Z gerundt $

#if !defined(AFX_PROPCOLORS_H__9C0A0F92_CDD6_40FE_BEFC_101B1E22FFDD__INCLUDED_)
#define AFX_PROPCOLORS_H__9C0A0F92_CDD6_40FE_BEFC_101B1E22FFDD__INCLUDED_


#include "ColorButton.h"
#include "OptionsPanel.h"

class COptionsMgr;

const int CustomColorsAmount = 16;

/** @brief Property page for colors options; used in options property sheet */
class PropMergeColors : public OptionsPanel
{
// Construction
public:
	PropMergeColors(COptionsMgr *optionsMgr);

// Implement IOptionsPanel
	virtual void ReadOptions();
	virtual void WriteOptions();
	
// Dialog Data
private:
	//{{AFX_DATA(PropMergeColors)
	enum { IDD = IDD_PROPPAGE_COLORS_WINMERGE };
	CColorButton	m_cTrivialDeleted;
	CColorButton	m_cTrivial;
	CColorButton	m_cSelDiffText;
	CColorButton	m_cDiffText;
	CColorButton	m_cSelDiffDeleted;
	CColorButton	m_cDiffDeleted;
	CColorButton	m_cSelDiff;
	CColorButton	m_cDiff;
	CColorButton	m_cTrivialText;
	CColorButton	m_cMoved;
	CColorButton	m_cMovedDeleted;
	CColorButton	m_cMovedText;
	CColorButton	m_cSelMoved;
	CColorButton	m_cSelMovedDeleted;
	CColorButton	m_cSelMovedText;
	CColorButton	m_cSelWordDiffText;
	CColorButton	m_cWordDiffText;
	CColorButton	m_cSelWordDiff;
	CColorButton	m_cWordDiff;
	//}}AFX_DATA


	COLORREF	m_clrDiff;
	COLORREF	m_clrSelDiff;
	COLORREF	m_clrDiffDeleted;
	COLORREF	m_clrSelDiffDeleted;
	COLORREF	m_clrDiffText;
	COLORREF	m_clrSelDiffText;
	COLORREF	m_clrTrivial;
	COLORREF	m_clrTrivialDeleted;
	COLORREF	m_clrTrivialText;
	COLORREF	m_clrMoved;
	COLORREF	m_clrMovedDeleted;
	COLORREF	m_clrMovedText;
	COLORREF	m_clrSelMoved;
	COLORREF	m_clrSelMovedDeleted;
	COLORREF	m_clrSelMovedText;
	COLORREF	m_clrWordDiff;
	COLORREF	m_clrWordDiffText;
	COLORREF	m_clrSelWordDiff;
	COLORREF	m_clrSelWordDiffText;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PropMergeColors)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	typedef enum { SET_DEFAULTS, WRITE_OPTIONS, READ_OPTIONS } OPERATION;

	void LoadCustomColors();
	void SaveCustomColors();
	void BrowseColor(CColorButton & colorButton, COLORREF & currentColor);
	void SerializeColors(OPERATION op);
	void SerializeColor(OPERATION op, CColorButton & btn, LPCTSTR optionName, COLORREF & color);

	COLORREF m_cCustColors[CustomColorsAmount];

	// Generated message map functions
	//{{AFX_MSG(PropMergeColors)
	afx_msg void OnDifferenceColor();
	afx_msg void OnDifferenceDeletedColor();
	afx_msg void OnSelDifferenceDeletedColor();
	afx_msg void OnSelDifferenceColor();
	afx_msg void OnDifferenceTextColor();
	afx_msg void OnSelDifferenceTextColor();
	afx_msg void OnTrivialDiffColor();
	afx_msg void OnTrivialDiffDeletedColor();
	afx_msg void OnTrivialDiffTextColor();
	afx_msg void OnMovedColor();
	afx_msg void OnMovedDeletedColor();
	afx_msg void OnMovedTextColor();
	afx_msg void OnSelMovedColor();
	afx_msg void OnSelMovedDeletedColor();
	afx_msg void OnSelMovedTextColor();
	afx_msg void OnWordDifferenceColor();
	afx_msg void OnSelWordDifferenceColor();
	afx_msg void OnWordDifferenceTextColor();
	afx_msg void OnSelWordDifferenceTextColor();
	afx_msg void OnDefaults();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPCOLORS_H__9C0A0F92_CDD6_40FE_BEFC_101B1E22FFDD__INCLUDED_)
