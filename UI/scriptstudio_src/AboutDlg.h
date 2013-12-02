#if !defined(AFX_ABOUTDLG_H__31989C75_659B_41E6_B8F8_16AE31733F92__INCLUDED_)
#define AFX_ABOUTDLG_H__31989C75_659B_41E6_B8F8_16AE31733F92__INCLUDED_


class CAboutDlg : public CDialogImpl<CAboutDlg>
{
public:
   enum { IDD = IDD_ABOUTBOX };

   BEGIN_MSG_MAP(CAboutDlg)
      MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
      COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
      COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
   END_MSG_MAP()

   LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      CenterWindow(GetParent());
      return TRUE;
   }
   LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   {
      EndDialog(wID);
      return 0;
   }
};


#endif // !defined(AFX_ABOUTDLG_H__31989C75_659B_41E6_B8F8_16AE31733F92__INCLUDED_)
