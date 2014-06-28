/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997  Dean P. Grimm
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  MainFrm.h
 *
 * @brief Declaration file for CMainFrame
 *
 */
// ID line follows -- this is updated by SVN
// $Id: MainFrm.h 7430 2010-11-17 13:42:16Z gerundt $

#if !defined(AFX_MAINFRM_H__BBCD4F8C_34E4_11D1_BAA6_00A024706EDC__INCLUDED_)
#define AFX_MAINFRM_H__BBCD4F8C_34E4_11D1_BAA6_00A024706EDC__INCLUDED_

#include <vector>
#include "ToolBarXPThemes.h"
#include "MDITabBar.h"
#include "OptionsMgr.h"
#include "VSSHelper.h"
#include "MergeCmdLineInfo.h"

/**
 * @brief Supported versioncontrol systems.
 */
enum
{
	VCS_NONE = 0,
	VCS_VSS4,
	VCS_VSS5,
	VCS_CLEARCASE,
};

/**
 * @brief Frame/View/Document types.
 */
enum FRAMETYPE
{
	FRAME_FOLDER, /**< Folder compare frame. */
	FRAME_FILE, /**< File compare frame. */
	FRAME_OTHER, /**< No frame? */
};

enum { WM_NONINTERACTIVE = 888 }; // timer value

class BCMenu;
class CDiffView;
class CDirView;
class CDirDoc;
class CMergeDoc;
class CHexMergeDoc;
class CMergeEditView;
class CMergeDiffDetailView;
class SyntaxColors;
class LineFiltersList;
class TempFile;
struct FileLocation;

// typed lists (homogenous pointer lists)
typedef CTypedPtrList<CPtrList, CMergeDoc *> MergeDocList;
typedef CTypedPtrList<CPtrList, CDirDoc *> DirDocList;
typedef CTypedPtrList<CPtrList, CHexMergeDoc *> HexMergeDocList;

class PackingInfo;
class CLanguageSelect;

/**
 * @brief Frame class containing save-routines etc
 */
class CMainFrame : public CMDIFrameWnd
{
	friend CLanguageSelect;
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:	
	BOOL m_bShowErrors; /**< Show folder compare error items? */
	LOGFONT m_lfDiff; /**< MergeView user-selected font */
	LOGFONT m_lfDir; /**< DirView user-selected font */
	static const TCHAR szClassName[];
// Operations
public:
	HMENU NewDirViewMenu();
	HMENU NewMergeViewMenu();
	HMENU NewHexMergeViewMenu();
	HMENU NewDefaultMenu(int ID = 0);
	HMENU GetScriptsSubmenu(HMENU mainMenu);
	HMENU GetPrediffersSubmenu(HMENU mainMenu);
	void UpdatePrediffersMenu();

	BOOL SyncFileToVCS(LPCTSTR pszDest,	BOOL &bApplyToAll, CString *psError);
	BOOL DoFileOpen(LPCTSTR pszLeft = NULL, LPCTSTR pszRight = NULL,
		DWORD dwLeftFlags = 0, DWORD dwRightFlags = 0, BOOL bRecurse = FALSE, CDirDoc *pDirDoc = NULL, CString prediffer = _T(""));
	int ShowMergeDoc(CDirDoc * pDirDoc, const FileLocation & filelocLeft,
		const FileLocation & filelocRight, DWORD dwLeftFlags = 0,
		DWORD dwRightFlags = 0, PackingInfo * infoUnpacker = NULL);
	void ShowHexMergeDoc(CDirDoc * pDirDoc,
		LPCTSTR pathLeft, LPCTSTR pathRight, BOOL bLeftRO, BOOL bRightRO);
	void UpdateResources();
	BOOL CreateBackup(BOOL bFolder, LPCTSTR pszPath);
	int HandleReadonlySave(CString& strSavePath, BOOL bMultiFile, BOOL &bApplyToAll);
	CString SetStatus(LPCTSTR status);
	void ClearStatusbarItemCount();
	void ApplyViewWhitespace();
	void SetEOLMixed(BOOL bAllow);
	void SelectFilter();
	void ShowVSSError(CException *e, LPCTSTR strItem);
	void ShowHelp(LPCTSTR helpLocation = NULL);
	void UpdateCodepageModule();
	void CheckinToClearCase(CString strDestinationPath);
	static void CenterToMainFrame(CDialog * dlg);
	static void SetMainIcon(CDialog * dlg);
	void StartFlashing();
	bool AskCloseConfirmation();
	BOOL DoOpenConflict(LPCTSTR conflictFile, bool checked = false);
	FRAMETYPE GetFrameType(const CFrameWnd * pFrame) const;

	static void OpenFileToExternalEditor(LPCTSTR file);

// Overrides
	virtual void GetMessageString(UINT nID, CString& rMessage) const;
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual void ActivateFrame(int nCmdShow = -1);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation methods
protected:
	virtual ~CMainFrame();
// Implementation in SourceControl.cpp
	void InitializeSourceControlMembers();
	BOOL SaveToVersionControl(CString& strSavePath);
// End SourceControl.cpp


// Public implementation data
public:
	BOOL m_bFirstTime; /**< If first time frame activated, get  pos from reg */
	CString m_strSaveAsPath; /**< "3rd path" where output saved if given */
	BOOL m_bEscShutdown; /**< If commandline switch -e given ESC closes appliction */
	VSSHelper m_vssHelper; /**< Helper class for VSS integration */
	SyntaxColors * GetMainSyntaxColors() { return m_pSyntaxColors; }
	BOOL m_bClearCaseTool; /**< WinMerge is executed as an external Rational ClearCase compare/merge tool. */
	BOOL m_bFlashing; /**< Window is flashing. */
	MergeCmdLineInfo::ExitNoDiff m_bExitIfNoDiff; /**< Exit if files are identical? */
	LineFiltersList *m_pLineFilters; /**< List of linefilters */

	/**
	 * @name Version Control System (VCS) integration.
	 */
	/*@{*/ 
protected:
	CString m_strVssUser; /**< Visual Source Safe User ID */
	CString m_strVssPassword; /**< Visual Source Safe Password */
	CString m_strVssDatabase; /**< Visual Source Safe database */
	CString m_strCCComment; /**< ClearCase comment */
public:
	BOOL m_bCheckinVCS;     /**< TRUE if files should be checked in after checkout */
	BOOL m_CheckOutMulti; /**< Suppresses VSS int. code asking checkout for every file */
	BOOL m_bVCProjSync; /**< VC project opened from VSS sync? */
	BOOL m_bVssSuppressPathCheck; /**< Suppresses VSS int code asking about different path */
	/*@}*/

	/**
	 * @name Textual labels/descriptors
	 * These descriptors overwrite dir/filename usually shown in headerbar
	 * and can be given from command-line. For example version control
	 * system can set these to "WinMerge v2.1.2.0" and "WinMerge 2.1.4.0"
	 * which is more pleasant and informative than temporary paths.
	 */
	/*@{*/ 
	String m_strDescriptions[2];
	/*@}*/

	/** @brief Possible toolbar image sizes. */
	enum TOOLBAR_SIZE
	{
		TOOLBAR_SIZE_16x16,
		TOOLBAR_SIZE_32x32,
	};

// Implementation data
protected:


	// control bar embedded members
	CStatusBar  m_wndStatusBar;
	CReBar m_wndReBar;
	ToolBarXPThemes m_wndToolBar;
	CMDITabBar m_wndTabBar;

	/** @brief Toolbar image table indexes. */
	enum TOOLBAR_IMAGES
	{
		TOOLBAR_IMAGES_ENABLED,
		TOOLBAR_IMAGES_DISABLED,
		TOOLBAR_IMAGES_COUNT
	};

	CImageList m_ToolbarImages[TOOLBAR_IMAGES_COUNT]; /**< Images for toolbar */

	enum
	{
		MENU_DEFAULT,
		MENU_MERGEVIEW,
		MENU_DIRVIEW,
		MENU_HEXMERGEVIEW,
		MENU_COUNT, // Add new items before this item
	};
	/**
	 * Menu frames - for which frame(s) the menu is.
	 */
	enum
	{
		MENU_MAINFRM = 0x000001,
		MENU_FILECMP = 0x000002,
		MENU_FOLDERCMP = 0x000004,
		MENU_ALL = MENU_MAINFRM | MENU_FILECMP | MENU_FOLDERCMP
	};

	/**
	 * A structure attaching a menu item, icon and menu types to apply to.
	 */
	struct MENUITEM_ICON
	{
		int menuitemID;   /**< Menu item's ID. */
		int iconResID;    /**< Icon's resource ID. */
		int menusToApply; /**< For which menus to apply. */
	};

	static const MENUITEM_ICON m_MenuIcons[];

	BCMenu * m_pMenus[MENU_COUNT]; /**< Menus for different views */
	SyntaxColors *m_pSyntaxColors; /**< Syntax color container */
	std::vector<TempFile*> m_tempFiles; /**< List of possibly needed temp files. */

// Generated message map functions
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	void GetFontProperties();
	//{{AFX_MSG(CMainFrame)
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg LRESULT OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu) ;
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnOptionsShowDifferent();
	afx_msg void OnOptionsShowIdentical();
	afx_msg void OnOptionsShowUniqueLeft();
	afx_msg void OnOptionsShowUniqueRight();
	afx_msg void OnOptionsShowBinaries();
	afx_msg void OnOptionsShowSkipped();
	afx_msg void OnUpdateOptionsShowdifferent(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOptionsShowidentical(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOptionsShowuniqueleft(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOptionsShowuniqueright(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOptionsShowBinaries(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOptionsShowSkipped(CCmdUI* pCmdUI);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnFileOpen();
	afx_msg void OnHelpGnulicense();
	afx_msg void OnOptions();
	afx_msg void OnViewSelectfont();
	afx_msg void OnUpdateViewSelectfont(CCmdUI* pCmdUI);
	afx_msg void OnViewUsedefaultfont();
	afx_msg void OnUpdateViewUsedefaultfont(CCmdUI* pCmdUI);
	afx_msg void OnHelpContents();
	afx_msg void OnUpdateHelpContents(CCmdUI* pCmdUI);
	afx_msg void OnClose();
	afx_msg void OnViewWhitespace();
	afx_msg void OnUpdateViewWhitespace(CCmdUI* pCmdUI);
	afx_msg void OnToolsGeneratePatch();
	afx_msg void OnDropFiles(HDROP dropInfo);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnUpdatePluginUnpackMode(CCmdUI* pCmdUI);
	afx_msg void OnPluginUnpackMode(UINT nID);
	afx_msg void OnUpdatePluginPrediffMode(CCmdUI* pCmdUI);
	afx_msg void OnPluginPrediffMode(UINT nID);
	afx_msg void OnUpdateReloadPlugins(CCmdUI* pCmdUI);
	afx_msg void OnReloadPlugins();
	afx_msg void OnSaveConfigData();
	afx_msg void OnFileNew();
	afx_msg void OnToolsFilters();
	afx_msg void OnDebugLoadConfig();
	afx_msg void OnHelpMerge7zmismatch();
	afx_msg void OnUpdateHelpMerge7zmismatch(CCmdUI* pCmdUI);
	afx_msg void OnViewStatusBar();
	afx_msg void OnViewToolbar();
	afx_msg void OnUpdateViewTabBar(CCmdUI* pCmdUI);
	afx_msg void OnViewTabBar();
	afx_msg void OnUpdateResizePanes(CCmdUI* pCmdUI);
	afx_msg void OnResizePanes();
	afx_msg void OnFileOpenproject();
	afx_msg LRESULT OnCopyData(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnWindowCloseAll();
	afx_msg void OnUpdateWindowCloseAll(CCmdUI* pCmdUI);
	afx_msg void OnSaveProject();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnDebugResetOptions();
	afx_msg void OnToolbarNone();
	afx_msg void OnUpdateToolbarNone(CCmdUI* pCmdUI);
	afx_msg void OnToolbarSmall();
	afx_msg void OnUpdateToolbarSmall(CCmdUI* pCmdUI);
	afx_msg void OnToolbarBig();
	afx_msg void OnUpdateToolbarBig(CCmdUI* pCmdUI);
	afx_msg BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHelpReleasenotes();
	afx_msg void OnHelpTranslations();
	afx_msg void OnFileOpenConflict();
	afx_msg void OnPluginsList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void addToMru(LPCTSTR szItem, LPCTSTR szRegSubKey, UINT nMaxItems = 20);
	const MergeDocList &GetAllMergeDocs();
	const DirDocList &GetAllDirDocs();
	const HexMergeDocList &GetAllHexMergeDocs();
	BOOL IsComparing();
	void RedisplayAllDirDocs();
	CMergeDoc * GetMergeDocToShow(CDirDoc * pDirDoc, BOOL * pNew);
	CHexMergeDoc * GetHexMergeDocToShow(CDirDoc * pDirDoc, BOOL * pNew);
	CDirDoc * GetDirDocToShow(BOOL * pNew);
	void ShowFontChangeMessage();
	void OpenFileOrUrl(LPCTSTR szFile, LPCTSTR szUrl);
	BOOL CreateToobar();
	CMergeEditView * GetActiveMergeEditView();
	void LoadToolbarImages();
	HMENU NewMenu( int view, int ID );
};

CMainFrame * GetMainFrame(); // access to the singleton main frame object
SyntaxColors * GetMainSyntaxColors(); // access to the singleton set of syntax colors

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__BBCD4F8C_34E4_11D1_BAA6_00A024706EDC__INCLUDED_)
