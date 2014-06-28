/** 
 * @file  PropSyntaxColors.h
 *
 * @brief Declaration file for PropSyntaxColors propertyheet
 *
 */
// ID line follows -- this is updated by SVN
// $Id: PropSyntaxColors.h 7501 2011-01-03 13:29:00Z gerundt $

#ifndef _PROP_SYNTAX_COLORS_H_
#define _PROP_SYNTAX_COLORS_H_

#include "ColorButton.h"
#include "OptionsPanel.h"

class SyntaxColors;

const int NumCustomColors = 16;

class PropSyntaxColors : public OptionsPanel
{
	DECLARE_DYNAMIC(PropSyntaxColors)

// Construction & Destruction
public:
	PropSyntaxColors(COptionsMgr *optionsMgr, SyntaxColors *pColors);

// Implement IOptionsPanel
	virtual void ReadOptions();
	virtual void WriteOptions();

// Dialog Data
public:

private:
	enum { IDD = IDD_PROPPAGE_COLORS_SYNTAX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedEcolorKeywords();
	afx_msg void OnBnClickedEcolorFunctions();
	afx_msg void OnBnClickedEcolorComments();
	afx_msg void OnBnClickedEcolorNumbers();
	afx_msg void OnBnClickedEcolorOperators();
	afx_msg void OnBnClickedEcolorStrings();
	afx_msg void OnBnClickedEcolorPreprocessor();
	afx_msg void OnBnClickedEcolorUser1();
	afx_msg void OnBnClickedEcolorUser2();
	afx_msg void OnBnClickedEcolorsBdefaults();
	afx_msg void OnBnClickedEcolorKeywordsBold();
	afx_msg void OnBnClickedEcolorFunctionsBold();
	afx_msg void OnBnClickedEcolorCommentsBold();
	afx_msg void OnBnClickedEcolorNumbersBold();
	afx_msg void OnBnClickedEcolorOperatorsBold();
	afx_msg void OnBnClickedEcolorStringsBold();
	afx_msg void OnBnClickedEcolorPreprocessorBold();
	afx_msg void OnBnClickedEcolorUser1Bold();
	afx_msg void OnBnClickedEcolorUser2Bold();

	void LoadCustomColors();
	void SaveCustomColors();
	void BrowseColorAndSave(CColorButton & colorButton, int colorIndex);
	int GetCheckVal(UINT nColorIndex);
	void UpdateBoldStatus(CButton &btn, UINT colorIndex);

	DECLARE_MESSAGE_MAP()
	
private:
	CColorButton m_btnKeywordsText;
	CColorButton m_btnFunctionsText;
	CColorButton m_btnCommentsText;
	CColorButton m_btnNumbersText;
	CColorButton m_btnOperatorsText;
	CColorButton m_btnStringsText;
	CColorButton m_btnPreprocessorText;
	CColorButton m_btnUser1Text;
	CColorButton m_btnUser2Text;
	int m_nKeywordsBold;
	int m_nFunctionsBold;
	int m_nCommentsBold;
	int m_nNumbersBold;
	int m_nOperatorsBold;
	int m_nStringsBold;
	int m_nPreProcessorBold;
	int m_nUser1Bold;
	int m_nUser2Bold;
	CButton m_btnKeywordsBold;
	CButton m_btnFunctionsBold;
	CButton m_btnCommentsBold;
	CButton m_btnNumbersBold;
	CButton m_btnOperatorsBold;
	CButton m_btnStringsBold;
	CButton m_btnPreProcessorBold;
	CButton m_btnUser1Bold;
	CButton m_btnUser2Bold;

	SyntaxColors *m_pTempColors;
	COLORREF m_cCustColors[COLORINDEX_COUNT];
};
#endif // _PROP_SYNTAX_COLORS_H_
