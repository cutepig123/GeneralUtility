/** 
 * @file  PropArchive.h
 *
 * @brief Declaration file for PropArchive propertyheet
 *
 */
// ID line follows -- this is updated by SVN
// $Id: PropArchive.h 7501 2011-01-03 13:29:00Z gerundt $

#ifndef _PROP_ARCHIVE_H_
#define _PROP_ARCHIVE_H_

#include "StatLink.h"
#include "OptionsPanel.h"

class COptionsMgr;

/** 
 * @brief Class for Archive Support options page.
 */
class PropArchive : public OptionsPanel
{
public:
	PropArchive(COptionsMgr *optionsMgr);

// Implement IOptionsPanel
	virtual void ReadOptions();
	virtual void WriteOptions();

// Dialog Data
	enum { IDD = IDD_PROP_ARCHIVE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	void UpdateControls();

	afx_msg void OnEnableClicked();

	DECLARE_MESSAGE_MAP()
public:
	CStaticLink m_wwwLink;
	BOOL m_bEnableSupport;
	int m_nInstallType;
	BOOL m_bProbeType;
};
#endif // _PROP_ARCHIVE_H_
