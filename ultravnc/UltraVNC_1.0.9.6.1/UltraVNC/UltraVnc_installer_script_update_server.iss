#define AppName "UltraVNC"
#define AppID "Ultravnc2"


[Setup]
AppName=UltraVNC
AppVerName=UltraVNC 1.0.9.6
AppVersion=1.0.9.6
VersionInfoVersion=1.0.9.6
AppPublisher=uvnc bvba
AppCopyright=UltraVnc Team
AppPublisherURL={cm:PublisherURL}
AppSupportURL={cm:SupportURL}
AppUpdatesURL={cm:UpdatesURL}
DefaultDirName={pf}\{cm:MyAppName}
DefaultGroupName={cm:MyAppName}
WindowVisible=false
DisableStartupPrompt=true
DisableReadyPage=false
ChangesAssociations=true
MinVersion=0,5.0.2195
PrivilegesRequired=admin
AppID={#AppID}
UninstallRestartComputer=true
DirExistsWarning=no
OutputDir=setupfile
OutputBaseFilename=UltraVNC_1.0.9.6_update
BackColorDirection=lefttoright
UserInfoPage=false
ShowLanguageDialog=yes
LanguageDetectionMethod=uilanguage
AllowUNCPath=false
WindowShowCaption=false
WindowStartMaximized=false
WindowResizable=false
Compression=lzma/ultra
AlwaysRestart=false
VersionInfoDescription={#AppName} Setup
WizardImageBackColor=clWhite
WizardImageStretch=false
SetupIconFile=UltraVNC.ico
WizardImageFile=UltraVNC-splash.bmp
WizardSmallImageFile=UltraVNC-logo.bmp
InfoAfterFile=Readme.txt
InfoBeforeFile=Whatsnew.txt
LicenseFile=Licence.txt
InternalCompressLevel=ultra
SolidCompression=true
SignTool=signing
VersionInfoCompany=uvnc bvba
VersionInfoCopyright=UltraVnc Team
VersionInfoProductName=UltraVnc 
VersionInfoProductVersion=1.0.9.6
UninstallDisplayName=UltraVnc

[Languages]
Name: en; MessagesFile: compiler:Default.isl
Name: de; MessagesFile: compiler:Languages\german.isl
Name: fr; MessagesFile: compiler:Languages\french.isl

[CustomMessages]
en.MyAppName={#AppName}
en.MyAppVerName={#AppName} %1
en.firewall=Configuring Windows firewall...
en.SupportURL=http://forum.ultravnc.info
en.UpdatesURL=http://www.uvnc.com
en.PublisherURL=http://www.uvnc.com

de.MyAppName={#AppName}
de.MyAppVerName={#AppName} %1
de.firewall=Die Windows Firewall wird konfiguriert...
de.SupportURL=http://forum.ultravnc.info
de.UpdatesURL=http://www.uvnc.com
de.PublisherURL=http://www.uvnc.com

fr.MyAppName={#AppName}
fr.MyAppVerName={#AppName} %1
fr.firewall=Configuration du Pare Feu Windows...
fr.SupportURL=http://forum.ultravnc.info
fr.UpdatesURL=http://www.uvnc.com
fr.PublisherURL=http://www.uvnc.com

en.FullInstall=Full installation
de.FullInstall=Vollst�ndige Installation
fr.FullInstall=Installation compl�te

en.CustomInstall=Custom installation
de.CustomInstall=Benutzerdefinierte Installation
fr.CustomInstall=Installation personnalis�e

en.ServerOnly=UltraVNC Server Only
de.ServerOnly=Nur UltraVNC Server
fr.ServerOnly=Seulement UltraVNC Server
en.ServerOnlyS=UltraVNC Server Only   "silent"
de.ServerOnlyS=Nur UltraVNC Server  "silent"
fr.ServerOnlyS=Seulement UltraVNC Server  "mode silencieux"

en.ViewerOnly=UltraVNC Viewer Only
de.ViewerOnly=Nur UltraVNC Viewer
fr.ViewerOnly=Seulement UltraVNC Viewer

en.InstallService=&Register %1 as a system service
de.InstallService=%1 als System-Dienst &registrieren
fr.InstallService=&Installation et enregistrement d'%1 comme service syst�me

en.ServerConfig=Server configuration:
de.ServerConfig=Server Konfiguration:
fr.ServerConfig=Configuration serveur:

en.StartService=&Start or restart %1 service
de.StartService=%1 Dienst (erneut) &starten
fr.StartService=&D�marrer ou red�marrer %1 le service

en.CreateDesktopIcons=Create %1 &desktop icons
de.CreateDesktopIcons=%1 &Desktop-Symbole anlegen
fr.CreateDesktopIcons=Cr�er les ic�nes d'%1 sur le &bureau

en.Starting=Starting %1 service...
de.Starting=%1 Dienst starten...
fr.Starting=D�marrage du service %1...

en.Stopping=Stopping %1 service...
de.Stopping=%1 Dienst stoppen...
fr.Stopping=Arr�t du service %1...

en.Removing=Removing %1 service...
de.Removing=%1 Dienst entfernen...
fr.Removing=Suppression du service %1...

en.Registering=Registering %1 service...
de.Registering=%1 Dienst registrieren...
fr.Registering=Enregistrement service %1...

en.Passwd=Check set initial password...
de.Passwd=Check set initial password...
fr.Passwd=V�rification du mot de passe initial...

[Types]
Name: full; Description: {cm:FullInstall}
Name: server; Description: {cm:ServerOnly}
Name: server_silent; Description: {cm:ServerOnlyS}
Name: viewer; Description: {cm:ViewerOnly}
;Name: custom; Description: {cm:CustomInstall}; Flags: iscustom

[Components]
Name: UltraVNC_Server_S; Description: UltraVNC Server Silent; Types: server_silent; Flags: disablenouninstallwarning
Name: UltraVNC_Server; Description: UltraVNC Server; Types: full server; Flags: disablenouninstallwarning
Name: UltraVNC_Viewer; Description: UltraVNC Viewer; Types: full viewer; Flags: disablenouninstallwarning

[Tasks]
Name: installservice; Description: {cm:InstallService,UltraVNC Server}; GroupDescription: {cm:ServerConfig}; Components: UltraVNC_Server UltraVNC_Server_S; MinVersion: 0,1; Check: isTaskChecked('installservice')
Name: installservice; Description: {cm:InstallService,UltraVNC Server}; GroupDescription: {cm:ServerConfig}; Components: UltraVNC_Server UltraVNC_Server_S; MinVersion: 0,1; Flags: unchecked; Check: not(isTaskChecked('installservice'))
Name: startservice; Description: {cm:StartService,UltraVNC}; GroupDescription: {cm:ServerConfig}; Components: UltraVNC_Server UltraVNC_Server_S; MinVersion: 0,1; Check: isTaskChecked('startservice')
Name: startservice; Description: {cm:StartService,UltraVNC}; GroupDescription: {cm:ServerConfig}; Components: UltraVNC_Server UltraVNC_Server_S; MinVersion: 0,1; Flags: unchecked; Check: not(isTaskChecked('startservice'))
Name: desktopicon; Description: {cm:CreateDesktopIcons,UltraVNC}; Components: UltraVNC_Viewer UltraVNC_Server UltraVNC_Server_S; Check: isTaskChecked('desktopicon')
Name: desktopicon; Description: {cm:CreateDesktopIcons,UltraVNC}; Components: UltraVNC_Viewer UltraVNC_Server UltraVNC_Server_S; Flags: unchecked; Check: not(isTaskChecked('desktopicon'))
Name: associate; Description: {cm:AssocFileExtension,UltraVNC Viewer,.vnc}; Components: UltraVNC_Viewer; Check: isTaskChecked('associate')
Name: associate; Description: {cm:AssocFileExtension,UltraVNC Viewer,.vnc}; Components: UltraVNC_Viewer; Flags: unchecked; Check: not(isTaskChecked('associate'))
Components: UltraVNC_Server_S UltraVNC_Server; Name: startwinvnc; Description: Start server app after install; Flags: unchecked; 


[Files]
; component independent files
Source: isxdl.dll; Flags: dontcopy
Source: UltraVNC.ico; Flags: dontcopy
Source: WizModernSmallImage-IS.bmp; Flags: dontcopy

Source: check_install.exe; DestDir: {app}; Components: UltraVNC_Server_S; BeforeInstall: StopVNC_S; Flags: restartreplace
Source: check_install.exe; DestDir: {app}; Components: UltraVNC_Server; BeforeInstall: StopVNC; Flags: restartreplace
Source: Whatsnew.txt; DestDir: {app}
Source: Licence.txt; DestDir: {app}
Source: Readme.txt; DestDir: {app}
; server files
; winvnc.exe needs to be first here because it triggers stopping WinVNC service/app.
Source: winvnc.exe; DestDir: {app}; Components: UltraVNC_Server UltraVNC_Server_S; Flags: restartreplace ignoreversion replacesameversion; Check: Can_cont(); MinVersion: ,5.1.2600;
Source: sas.dll; DestDir: {app}; Components: UltraVNC_Server UltraVNC_Server_S; Flags: restartreplace ignoreversion replacesameversion; Check: Can_cont(); MinVersion: ,6.0.6000; OnlyBelowVersion: 0,6.1.7600; 
Source: w2k\winvnc.exe; DestDir: {app}; Components: UltraVNC_Server UltraVNC_Server_S; Flags: restartreplace ignoreversion replacesameversion; Check: Can_cont(); MinVersion: ,5.0.2195; OnlyBelowVersion: 0,5.1.2600; 
Source: vncviewer.exe; DestDir: {app}; Components: UltraVNC_Viewer; Flags: restartreplace ignoreversion replacesameversion; MinVersion: ,5.1.2600; 
Source: w2k\vncviewer.exe; DestDir: {app}; Components: UltraVNC_Viewer; Flags: restartreplace ignoreversion replacesameversion; MinVersion: ,5.0.2195; OnlyBelowVersion: 0,5.1.2600; 
Source: schook.dll; DestDir: {app}; Components: UltraVNC_Server UltraVNC_Server_S; Flags: restartreplace ignoreversion replacesameversion; Check: Can_cont(); MinVersion: ,5.1.2600; 
Source: w2k\schook.dll; DestDir: {app}; Components: UltraVNC_Server UltraVNC_Server_S; Flags: restartreplace ignoreversion replacesameversion; Check: Can_cont(); MinVersion: ,5.0.2195; OnlyBelowVersion: 0,5.1.2600; 


Source: plugins_multi_thread\SecureVNCPlugin.dsm; DestDir: {app}; Flags: restartreplace

Source: uvnc_settings.exe; DestDir: {app}; Flags: ignoreversion replacesameversion




[Icons]
Name: {userdesktop}\UltraVNC Server; Filename: {app}\winvnc.exe; Components: UltraVNC_Server UltraVNC_Server_S; IconIndex: 0; Tasks: desktopicon; Check: Can_cont()
Name: {userdesktop}\UltraVNC Viewer; Filename: {app}\vncviewer.exe; IconIndex: 0; Components: UltraVNC_Viewer; Tasks: desktopicon
Name: {group}\UltraVNC Viewer; FileName: {app}\vncviewer.exe; WorkingDir: {app}; IconIndex: 0; Components: UltraVNC_Viewer
Name: {group}\UltraVNC Server; FileName: {app}\WinVNC.exe; WorkingDir: {app}; Components: UltraVNC_Server UltraVNC_Server_S; IconIndex: 0; Check: Can_cont()


Name: {group}\UltraVNC Viewer\Run UltraVNC Viewer (Listen Mode); FileName: {app}\vncviewer.exe; Parameters: -listen; WorkingDir: {app}; IconIndex: 0; Components: UltraVNC_Viewer
Name: {group}\UltraVNC Viewer\Show UltraVNC Viewer Help; FileName: {app}\vncviewer.exe; Parameters: -help; WorkingDir: {app}; IconIndex: 0; Components: UltraVNC_Viewer

Name: {group}\UltraVNC Server\Install WinVNC Service; FileName: {app}\WinVNC.exe; Parameters: -install; WorkingDir: {app}; Components: UltraVNC_Server UltraVNC_Server_S; Check: Can_cont()
Name: {group}\UltraVNC Server\Remove WinVNC Service; FileName: {app}\WinVNC.exe; Parameters: -uninstall; WorkingDir: {app}; Components: UltraVNC_Server UltraVNC_Server_S; Check: Can_cont()
Name: {group}\UltraVNC Server\Start WinVNC Service; FileName: {app}\WinVNC.exe; Parameters: -startservice; WorkingDir: {app}; Components: UltraVNC_Server UltraVNC_Server_S; Check: Can_cont()
Name: {group}\UltraVNC Server\Stop WinVNC Service; FileName: {app}\WinVNC.exe; Parameters: -stopservice; WorkingDir: {app}; Components: UltraVNC_Server UltraVNC_Server_S; Check: Can_cont()

Name: {group}\Edit Settings; FileName: {app}\uvnc_settings.exe; WorkingDir: {app}; Components: UltraVNC_Server UltraVNC_Server_S; Check: Can_cont()

[Registry]
Root: HKCR; Subkey: .vnc; ValueType: string; ValueName: ; ValueData: VncViewer.Config; Flags: uninsdeletevalue; Tasks: associate
Root: HKCR; Subkey: VncViewer.Config; ValueType: string; ValueName: ; ValueData: VNCviewer Config File; Flags: uninsdeletekey; Tasks: associate
Root: HKCR; Subkey: VncViewer.Config\DefaultIcon; ValueType: string; ValueName: ; ValueData: {app}\vncviewer.exe,0; Tasks: associate
Root: HKCR; Subkey: VncViewer.Config\shell\open\command; ValueType: string; ValueName: ; ValueData: """{app}\vncviewer.exe"" -config ""%1"""; Tasks: associate

[Run]
Filename: {app}\WinVNC.exe; Components: UltraVNC_Server UltraVNC_Server_S; Check: Can_cont(); Tasks: startwinvnc; Flags: NoWait; 
Filename: {app}\WinVNC.exe; Parameters: -install; Flags: runhidden; Components: UltraVNC_Server UltraVNC_Server_S; Tasks: installservice; StatusMsg: {cm:Registering, UltraVNC}; Check: Can_cont()
Filename: net; Parameters: start uvnc_service; Flags: runhidden; Components: UltraVNC_Server UltraVNC_Server_S; Tasks: startservice; StatusMsg: {cm:Starting,UltraVNC}
filename: {sys}\netsh; Parameters: firewall add portopening TCP 5900 vnc5900; StatusMsg: {cm:firewall}; Flags: runhidden; MinVersion: 0,5.01; Components: UltraVNC_Server UltraVNC_Server_S; Check: Can_cont()
Filename: {sys}\netsh; Parameters: firewall add portopening TCP 5800 vnc5800; StatusMsg: {cm:firewall}; Flags: runhidden; MinVersion: 0,5.01; Components: UltraVNC_Server UltraVNC_Server_S; Check: Can_cont()
Filename: {sys}\netsh; Parameters: "firewall add allowedprogram ""{app}\vncviewer.exe"" ""vncviewer.exe"" ENABLE ALL"; StatusMsg: {cm:firewall}; Flags: runhidden; MinVersion: 0,5.01; Components: UltraVNC_Viewer

[UninstallRun]
Filename: net; Parameters: stop uvnc_service; Flags: runhidden; RunOnceId: StopVncService; Components: UltraVNC_Server UltraVNC_Server_S; Tasks: startservice; StatusMsg: {cm:Stopping, UltraVNC}
Filename: {app}\WinVNC.exe; Parameters: -uninstall; Flags: runhidden; RunOnceId: RemoveVncService; Components: UltraVNC_Server UltraVNC_Server_S; Tasks: installservice; StatusMsg: {cm:Removing,UltraVNC}
Filename: {sys}\netsh; Parameters: firewall delete portopening TCP 5900 vnc5900; StatusMsg: {cm:firewall}; Flags: runhidden; MinVersion: 0,5.01; Components: UltraVNC_Server UltraVNC_Server_S
Filename: {sys}\netsh; Parameters: firewall delete portopening TCP 5800 vnc5800; StatusMsg: {cm:firewall}; Flags: runhidden; MinVersion: 0,5.01; Components: UltraVNC_Server UltraVNC_Server_S
Filename: {sys}\netsh; Parameters: "firewall delete allowedprogram program=""{app}\vncviewer.exe"""; StatusMsg: {cm:firewall}; Flags: runhidden; MinVersion: 0,5.01; Components: UltraVNC_Viewer

[_ISTool]
UseAbsolutePaths=true
[Code]
var
  SelectedTasks: String;
  ConCont: Integer;
  ISset: Boolean;

function isxdl_SetOption(Option, Value: PChar): Integer;
external 'isxdl_SetOption@files:isxdl.dll stdcall';

function GetModuleHandle(lpModuleName: LongInt): LongInt;
external 'GetModuleHandleA@kernel32.dll stdcall';
function ExtractIcon(hInst: LongInt; lpszExeFileName: PChar; nIconIndex: LongInt): LongInt;
external 'ExtractIconA@shell32.dll stdcall';
function DrawIconEx(hdc: LongInt; xLeft, yTop: Integer; hIcon: LongInt; cxWidth, cyWidth: Integer; istepIfAniCur: LongInt; hbrFlickerFreeDraw, diFlags: LongInt): LongInt;
external 'DrawIconEx@user32.dll stdcall';
function DestroyIcon(hIcon: LongInt): LongInt;
external 'DestroyIcon@user32.dll stdcall';


const
  DI_NORMAL = 3;

function InitializeSetup(): Boolean;
begin
  Result := True;
  ISset := False;
end;


function Can_cont(): Boolean;
begin
if ConCont <> 5 then
Result := false;
if ConCont = 5  then
  Result := true;
end;

procedure StopVNC();
begin
  if UsingWinNT() = True then
  ExtractTemporaryFile('check_install.exe');
  if Exec(ExpandConstant('{tmp}\check_install.exe'), '', '', SW_HIDE, ewWaitUntilTerminated, ConCont) then
  begin
    Log('Checking system status');
  end
  else begin
    Log('Checking system status');
  end;
end;

procedure StopVNC_S();
begin
  if UsingWinNT() = True then
  ExtractTemporaryFile('check_install.exe');
  if Exec(ExpandConstant('{tmp}\check_install.exe'), 'silent', '', SW_HIDE, ewWaitUntilTerminated, ConCont) then
  begin
    Log('Checking system status');
  end
  else begin
    Log('Checking system status');
  end;
end;

function IsTaskChecked(Taskname: String): Boolean;
begin
  Log('SelectedTasks='+SelectedTasks);
  if CompareStr(SelectedTasks, '?') <> 0 then
    Result := (Pos(Taskname, SelectedTasks) > 0)
  else
  begin
    // default if not set through inf file
    Result := false;
    case Taskname of
    'desktopicon':
      Result := true;
    'associate':
      Result := true;
  end;
  end;
end;

function IsVista():Boolean;
var
  Version: TWindowsVersion;
begin
  Result:=False;
  GetWindowsVersionEx(Version);
  if Version.NTPlatform and (Version.Major = 6) then Result:=True;
end;

[Dirs]

[InnoIDE_Settings]
LogFileOverwrite=false
