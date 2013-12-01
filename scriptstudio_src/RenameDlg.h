#if !defined(AFX_RENAMEDLG_H__20040808_10D8_97E3_FA5B_0080AD509054__INCLUDED_)
#define AFX_RENAMEDLG_H__20040808_10D8_97E3_FA5B_0080AD509054__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CRenameDlg : public CDialogImpl<CRenameDlg>
{
public:
   enum { IDD = IDD_RENAME };

   CEdit m_ctrlName;
   CString m_sName;

   void Init(LPCTSTR pstrName)
   {
      m_sName = pstrName;
   }
   CString GetResult() const
   {
      return m_sName;
   }

   BEGIN_MSG_MAP(CRenameDlg)
      MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
      COMMAND_ID_HANDLER(IDOK, OnOK)
      COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
   END_MSG_MAP()

   LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      CenterWindow(GetParent());
      m_ctrlName = GetDlgItem(IDC_NAME);
      m_ctrlName.SetLimitText(63);
      m_ctrlName.SetWindowText(m_sName);
      return TRUE;
   }
   LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   {
      m_sName = CWindowText(m_ctrlName);
      if( m_sName.IsEmpty() ) {
         AtlMessageBox(m_hWnd, IDS_ERR_EMPTYNAME, IDS_CAPTION_ERROR, MB_ICONEXCLAMATION);
         return 0;
      }
      if( !::IsCharAlpha(m_sName[0]) ) {
         AtlMessageBox(m_hWnd, IDS_ERR_INVALIDNAME, IDS_CAPTION_ERROR, MB_ICONEXCLAMATION);
         return 0;
      }
      EndDialog(wID);
      return 0;
   }
   LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   {
      EndDialog(wID);
      return 0;
   }
};



#endif // !defined(AFX_RENAMEDLG_H__20040808_10D8_97E3_FA5B_0080AD509054__INCLUDED_)

