; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=COwnerDrawButtonDlg
LastTemplate=CButton
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "OwnerDrawButton.h"

ClassCount=4
Class1=COwnerDrawButtonApp
Class2=COwnerDrawButtonDlg
Class3=CAboutDlg

ResourceCount=5
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_OWNERDRAWBUTTON_DIALOG
Resource4=IDD_ABOUTBOX (English (U.S.))
Class4=CMyClass
Resource5=IDD_OWNERDRAWBUTTON_DIALOG (English (U.S.))

[CLS:COwnerDrawButtonApp]
Type=0
HeaderFile=OwnerDrawButton.h
ImplementationFile=OwnerDrawButton.cpp
Filter=N

[CLS:COwnerDrawButtonDlg]
Type=0
HeaderFile=OwnerDrawButtonDlg.h
ImplementationFile=OwnerDrawButtonDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=COwnerDrawButtonDlg

[CLS:CAboutDlg]
Type=0
HeaderFile=OwnerDrawButtonDlg.h
ImplementationFile=OwnerDrawButtonDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Class=CAboutDlg


[DLG:IDD_OWNERDRAWBUTTON_DIALOG]
Type=1
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Class=COwnerDrawButtonDlg

[DLG:IDD_OWNERDRAWBUTTON_DIALOG (English (U.S.))]
Type=1
Class=COwnerDrawButtonDlg
ControlCount=4
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_COLOREDBUTTON,button,1342242827

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[CLS:CMyClass]
Type=0
HeaderFile=MyClass.h
ImplementationFile=MyClass.cpp
BaseClass=CButton
Filter=W
VirtualFilter=BWC

