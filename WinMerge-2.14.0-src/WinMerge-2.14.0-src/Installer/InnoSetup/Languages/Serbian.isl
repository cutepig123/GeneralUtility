; *** Inno Setup version 5.1.11+ Serbian (Cyrillic) messages ***
;
; Vladimir Stefanovic, antivari@gmail.com, 18.10.2008
;
; Note: When translating this text, do not add periods (.) to the end of
; messages that didn't have them already, because on those messages Inno
; Setup adds the periods automatically (appending a period would result in
; two periods being displayed).

[LangOptions]
LanguageName=<0421><0440><043F><0441><043A><0438>
LanguageID=$0C1A
LanguageCodePage=1251
; If the language you are translating to requires special font faces or
; sizes, uncomment any of the following entries and change them accordingly.
;DialogFontName=
;DialogFontSize=8
;WelcomeFontName=Verdana
;WelcomeFontSize=12
;TitleFontName=Arial
;TitleFontSize=29
;CopyrightFontName=Arial
;CopyrightFontSize=8

[Messages]

; *** Application titles
SetupAppTitle=����������
SetupWindowTitle=���������� - %1
UninstallAppTitle=������������
UninstallAppFullTitle=%1 ������������

; *** Misc. common
InformationTitle=����������
ConfirmTitle=�������
ErrorTitle=������

; *** SetupLdr messages
SetupLdrStartupMessage=���� ��������� ���� ����������� %1. �� �� ������ �� ���������?
LdrCannotCreateTemp=��� ����� ��������� ���������� ��������. ���������� �� ���������
LdrCannotExecTemp=��� ����� ��������� �������� � ����������� ������������. ���������� �� ���������

; *** Startup error messages
LastErrorMessage=%1.%n%n������ %2: %3
SetupFileMissing=�������� %1 �������� � ������������� ������������. ������ ��� ��������� ������� ��� �������� ���� ����� ��������.
SetupFileCorrupt=������������ �������� �� ����������. ������ ��� �� �������� ���� ����� ��������.
SetupFileCorruptOrWrongVer=������������ �������� �� ����������, ��� ���� ���������� �� ���� ������� ����������. ������ ��� ��������� ������� ��� �������� ���� ����� ��������.
NotOnThisPlatform=��� ������� �� ��� ��������� �� %1.
OnlyOnThisPlatform=��� ������� �� ���� ��������� �� %1.
OnlyOnTheseArchitectures=��� ������� �� ���� ����������� ���� �� �������� Windows-� ������������ �� ������ ����������� �����������:%n%n%1
MissingWOW64APIs=������ Windows-� ��� ��������� �� ������ ��������� �������� �� ������������ ��������� �� ����� 64-����� ����������. �� �� ������ ��� �������, ������ ����������� Service P�ck %1.
WinVersionTooLowError=��� ������� ������� %1 ������ %2 ��� �����.
WinVersionTooHighError=��� ������� �� �� ���� ����������� �� %1 ������ %2 ��� �����.
AdminPrivilegesRequired=������ ���� �������� ��� ������������� �� �� ��� ����������� ��� �������.
PowerUserPrivilegesRequired=������ ���� �������� ��� ������������� ��� ��� ���� Power Users ����� ���� ����������� ��� �������.
SetupAppRunningError=���������� �� ������� �� �� %1 �������� ��������.%n%n������ �� ����� ��������� ��� ������ ��������, � ����� ���������� OK �� ��������, ��� Cancel �� ����������.
UninstallAppRunningError=������������ �� ������� �� �� %1 �������� ��������.%n%n������ �� ����� ��������� ��� ������ ��������, � ����� ���������� OK �� ��������, ��� Cancel �� ����������.

; *** Misc. errors
ErrorCreatingDir=���������� ��� ����� �� ������� ����������� "%1"
ErrorTooManyFilesInDir=��� ����� ��������� �������� � ������������ "%1" ���� ��� ������ ������� ��������

; *** Setup common messages
ExitSetupTitle=��������� ����������
ExitSetupMessage=���������� ��� ��������. ��� ���� ��������� ����������, ������� ��� ���� ����������.%n%n���������� ������ ��������� � �������� ����� ����� ��������.%n%n������ ����������?
AboutSetupMenuItem=&� ����������...
AboutSetupTitle=� ����������
AboutSetupMessage=%1 ������ %2%n%3%n%n%1 ������� ��������:%n%4
AboutSetupNote=
TranslatorNote=

; *** Buttons
ButtonBack=< &�����
ButtonNext=&���� >
ButtonInstall=&���������
ButtonOK=��
ButtonCancel=��������
ButtonYes=&��
ButtonYesToAll=�� �� &���
ButtonNo=&��
ButtonNoToAll=�&� �� ���
ButtonFinish=&���������
ButtonBrowse=&�������...
ButtonWizardBrowse=�&������...
ButtonNewFolder=&������� ���� �����������

; *** "Select Language" dialog messages
SelectLanguageTitle=��������� ����� ����������
SelectLanguageLabel=��������� ����� ��� ������ �� ��������� �������� ����������:

; *** Common wizard text
ClickNext=���������� ���� �� ���������, ��� �������� �� ��������� ����������.
BeveledLabel=
BrowseDialogTitle=��������� �����������
BrowseDialogLabel=��������� ����� �� �������� ������������ �� �����, � ����� ���������� ��.
NewFolderName=���� �����������

; *** "Welcome" wizard page
WelcomeLabel1=���������� � [name] ������������ ���������
WelcomeLabel2=���� �� �� ����������� [name/ver] �� ��� �������.%n%n���������� �� �� ��������� ��� ����� �������� ��� ��������.

; *** "Password" wizard page
WizardPassword=�����
PasswordLabel1=��� ���������� �� �������� ������.
PasswordLabel3=������ ������� �����, � ����� ���������� ���� �� ��������. ������ ������ �� �� ������ � ���� ����� � ����� �����.
PasswordEditLabel=&�����:
IncorrectPassword=����� ��� ��� ������� ��� ��������. ������ �������� ������.

; *** "License Agreement" wizard
WizardLicense=������ � ��������
LicenseLabel=������ ��������� ������ ������ ����� ���������� ��� ��������.
LicenseLabel3=������ ��������� ������ � ��������, ��� �����. ������ ���������� ������ ���� ������� ��� �������� ����������.
LicenseAccepted=&��������� ������
LicenseNotAccepted=&�� ��������� ������

; *** "Information" wizard pages
WizardInfoBefore=����������
InfoBeforeLabel=������ ��������� ������ ������ ����� ���������� ��� ��������.
InfoBeforeClickLabel=���� ������ ������� �� ��������� ����������, ���������� ����.
WizardInfoAfter=����������
InfoAfterLabel=������ ��� ��������� ������ ������ ����� ���������� ��� ��������.
InfoAfterClickLabel=���� ������ ������� �� ��������� ����������, ���������� ����.

; *** "User Information" wizard page
WizardUserInfo=������ � ���������
UserInfoDesc=������ ������� ���� �������.
UserInfoName=&��������:
UserInfoOrg=&�����������:
UserInfoSerial=&������� ���:
UserInfoNameRequired=������ ������� ���.

; *** "Select Destination Location" wizard page
WizardSelectDir=��������� ��������� �������
SelectDirDesc=��� [name] ����� �� �� ���������?
SelectDirLabel3=���������� �� ��������� [name] � ������ �����������.
SelectDirBrowseLabel=�� ���������, ���������� ����. ��� ������ �� ��������� ���� ����� �����������, ���������� �������.
DiskSpaceMBLabel=�������� �� ������ [mb] MB ��������� �������� �� �����.
ToUNCPathname=������ �� ���������� �� ��� ���� � UNC ������. ��� ���������� �� ����������� ������� �� �����, �������� ��������� �� �������� ������ ����.
InvalidPath=������ ������� ���� ������ �� �������� �����; �� ������:%n%nC:\APP%n%n��� UNC ������ � ������:%n%n\\server\sh�re
InvalidDrive=���� ��� UNC ������ ��� ��� �������� �� ������ ��� ���� ��������. ������ ��������� ����� �����.
DiskSpaceWarningTitle=���� ������ �������� �� �����
DiskSpaceWarning=���������� ������� ������ %1 KB ��������� ��������, � �������� ���� ��� ���� %2 KB �� �����������.%n%n�� �� ���� ������ �� ���������?
DirNameTooLong=����� ������������ ��� ������ �� ����������.
InvalidDirName=����� ������������ ��� ��������.
BadDirName32=������ ������������ �� ���� ����� ���� ��� �� ������� �����:%n%n%1
DirExistsTitle=����������� ������
DirExists=�����������:%n%n%1%n%n�� ������. �� �� ���� ������ �� ������� ����������� � ����?
DirDoesntExistTitle=����������� �� ������
DirDoesntExist=�����������:%n%n%1%n%n�� ������. �� �� ������ �� �� ��������?

; *** "Select Components" wizard page
WizardSelectComponents=��������� ����������
SelectComponentsDesc=��� ���������� ���� �����������?
SelectComponentsLabel2=��������� ���������� ��� ������ �� �����������; �������� ���������� ��� �� ������ �� �����������. ���������� ���� ���� ������ ������� �� ���������.
FullInstallation=���� ����������
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=��������� ����������
CustomInstallation=���������� ���� ������ ����������
NoUninstallWarningTitle=���������� ������
NoUninstallWarning=���������� �� ������� �� ������ ���������� �� ������ �� ����� ��������:%n%n%1%n%n����������� ���� ���������� �� ��� ��������.%n%n�� �� ���� ������ �� ���������?
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=�������� �������� ������ �������� ������ [mb] MB �������� �� �����.

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=��������� ������� �������
SelectTasksDesc=����� ������� ������� �� ��� �������� �������?
SelectTasksLabel2=��������� ������� ������� ��� ������ �� ���������� [name] �����, � ����� ���������� ����.

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=��������� ����������� �� ����� ����
SelectStartMenuFolderDesc=��� ������ �� ���������� ������� ������� �� �������?
SelectStartMenuFolderLabel3=���������� �� ��������� ������� �� ������� � ������� ������������ ����� �����.
SelectStartMenuFolderBrowseLabel=�� ���������, ���������� ����. ��� ������ �� ��������� ���� ����� �����������, ���������� �������.
MustEnterGroupName=������ ������� ����� ������������.
GroupNameTooLong=����� ������������ ��� ������ �� ����������.
InvalidGroupName=����� ������������ ��� ��������.
BadGroupName=����� ������������ �� ��� ����� ���� ��� �� ������� �����:%n%n%1
NoProgramGroupCheck2=&���� �� ������ ����������� � ����� �����

; *** "Ready to Install" wizard page
WizardReady=���������� �� �������
ReadyLabel1=���������� �� ������� �� ������� [name] �� ��� �������.
ReadyLabel2a=���������� ��������� �� ��������� �� �����������, ��� ���������� ����� ��� ������ �� ������ ���������� ��� ��������� ���� ����������.
ReadyLabel2b=���������� ��������� �� ��������� �� �����������.
ReadyMemoUserInfo=������ � ���������:
ReadyMemoDir=��������� �������:
ReadyMemoType=��� ����������:
ReadyMemoComponents=�������� ����������:
ReadyMemoGroup=����������� ����� �����:
ReadyMemoTasks=������� �������:

; *** "Preparing to Install" wizard page
WizardPreparing=�������� �� ����������
PreparingDesc=���������� �� �������� �� ������� [name] �� ��� �������.
PreviousInstallNotCompleted=����������/�������� ���������� �������� ��� ��������. �������� �� �� ����������� ��� ������� �� �� �� ���������� ��������.%n%n����� ������������ ��������, ��������� ������ ���������� [name] �� �� ��� �� ��������.
CannotContinue=���������� �� �� ���� ���������. ������ ���������� �������� �� �������.

; *** "Installing" wizard page
WizardInstalling=�����������
InstallingLabel=������ �������� ��� �� [name] ��������� �� ��� �������.

; *** "Setup Completed" wizard page
FinishedHeadingLabel=[name] - ��������� ����������
FinishedLabelNoIcons=���������� �������� [name] �� ��� ������� �� ��������.
FinishedLabel=���������� �������� [name] �� ��� ������� �� ��������. ������� ������ ��������� ����� ���������� �����.
ClickFinish=���������� ��������� �� �������.
FinishedRestartLabel=�� �� �� ���������� [name] ��������, ���� �� ������������ �������. �� �� ������ �� �� ����������� �����?
FinishedRestartMessage=�� �� �� ���������� [name] ��������, ���� �� ������������ �������.%n%n�� �� ������ �� �� ����������� �����?
ShowReadmeCheck=��, ����� �� �������� README ��������
YesRadio=&��, �������� ������� �����
NoRadio=&��, ������������ ������� ������
; used for example as 'Run MyProg.exe'
RunEntryExec=������� %1
; used for example as 'View Readme.txt'
RunEntryShellExec=������� %1

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=���������� �� �������� ������� ����
SelectDiskLabel2=������ ������� ���� %1 � ���������� ��.%n%n��� �� �������� �� ���� ����� ���� ������� � ����� ������ ������������ ���� ��� �� ����� ���������, ������� ���������� ������ ��� ���������� �������.
PathLabel=&������:
FileNotInDir2=�������� "%1" �� �� ���� ������� � "%2". ������ ������� ����� ���� ��� ��������� ����� �����������.
SelectDirectoryLabel=������ ��������� ������� �������� �����.

; *** Installation phase messages
SetupAborted=���������� ��� ��������.%n%n������ ��������� ������� � ��������� ���������� ������.
EntryAbortRetryIgnore=���������� Retry �� ������� ������, Ignore �� ��������� � ������ ������, ��� Abort �� ��������� ����������.

; *** Installation status messages
StatusCreateDirs=������ ������������...
StatusExtractFiles=����������� ��������...
StatusCreateIcons=��������� �������...
StatusCreateIniEntries=��������� INI �����...
StatusCreateRegistryEntries=��������� Registry �����...
StatusRegisterFiles=���������� ��������...
StatusSavingUninstall=������� �������� �� ������������...
StatusRunProgram=���������� ����������...
StatusRollback=����������� ��������� ������ � ������� �� ��������� �����...

; *** Misc. errors
ErrorInternal2=������� ������: %1
ErrorFunctionFailedNoCode=%1 ��� ������
ErrorFunctionFailed=%1 ��� ������; ��� %2
ErrorFunctionFailedWithMessage=%1 ��� ������; ��� %2.%n%3
ErrorExecutingProgram=��� ����� ��������� ��������:%n%1

; *** Registry errors
ErrorRegOpenKey=������ ��� �������� Registry ����:%n%1\%2
ErrorRegCreateKey=������ ��� ��������� Registry ����:%n%1\%2
ErrorRegWriteKey=������ ��� ����� Registry ����:%n%1\%2

; *** INI errors
ErrorIniEntry=������ ��� ����� � INI �������� "%1".

; *** File copying errors
FileAbortRetryIgnore=���������� Retry �� ������� ������, Ignore �� ���������� ��� �������� (��� ������������), ��� Abort �� ��������� ����������.
FileAbortRetryIgnore2=���������� Retry �� ������� ������, Ignore �� ��������� � ������ ������ (��� ������������), ��� Abort �� ��������� ����������.
SourceIsCorrupted=������� �������� �� ����������
SourceDoesntExist=������� �������� "%1" �� ������
ExistingFileReadOnly=������� �������� �� �������� '���� �� ������'.%n%n���������� Retry �� �������� ������� '���� �� ������' � ������� ������, Ignore �� ���������� ��� ��������, ��� Abort �� ��������� ����������.
ErrorReadingExistingDest=����� �� �� ������ �������� ������� ������ ������ ��������:
FileExists=�������� �� ������.%n%n�� �� ������ �� �� ���������� ������?
ExistingFileNewer=������� �������� �� ����� �� ��� ��� ���������� ����� �� �������. ���������� �� �� �������� ������� ��������.%n%n�� �� ������ �� �������� ������� ��������?
ErrorChangingAttr=����� �� �� ������ �������� ������� ������� �������� �� ������ ��������:
ErrorCreatingTemp=����� �� �� ������ �������� ������� ������ �������� � ���������� ������������:
ErrorReadingSource=����� �� �� ������ �������� ������� ������ ������� ��������:
ErrorCopying=����� �� �� ������ �������� ������� �������� ��������:
ErrorReplacingExistingFile=����� �� �� ������ �������� ������� ������ ������� ��������:
ErrorRestartReplace=RestartReplace ��� �����:
ErrorRenamingTemp=D���� �� �� ������ �������� ������� ������� ������ �������� � ���������� ������������:
ErrorRegisterServer=��� ����� ������������ DLL/OCX: %1
ErrorRegSvr32Failed=RegSvr32 ��� ������� �������, ������ %1
ErrorRegisterTypeLib=��� ������ ������������ 'type libr�ry': %1

; *** Post-installation errors
ErrorOpeningReadme=����� �� �� ������ �������� �������� README ��������.
ErrorRestartingComputer=���������� ��� ������ �� ��������� �������. ������ ��� ������� �� ����.

; *** Uninstaller messages
UninstallNotFound=�������� "%1" �� ������. ������������� ��� ������.
UninstallOpenError=�������� "%1" �� �� ���� ��������. ������������� ��� ������
UninstallUnsupportedVer=�������������� Log �������� "%1" �� � ������ ��� �� ��������� ��� ������ �����������. ������������� ��� ������
UninstallUnknownEntry=�������� ���� (%1) �� ������ � �������������� Log ��������
ConfirmUninstall=�� �� ��� ������� �� ������ �� ������� �������� %1 � ��� ������ ����������?
UninstallOnlyOnWin64=��� ������� �� ���� ������������� ���� �� 64-������ Windows-�.
OnlyAdminCanUninstall=��� ������� ���� ������������� ���� �������� �� ����������������� �������.
UninstallStatusLabel=������ �������� ��� %1 �� ���� ������ �� ����� ��������.
UninstalledAll=%1 j� ������� ������ �� ����� ��������.
UninstalledMost=%1 ������������ �� ��������.%n%n���� ������ �� ���� ����� ��������. ��� �� ���� �������� �����.
UninstalledAndNeedsRestart=�� �������� ������������ %1, ��� ������� �� ���� ������������.%n%n�� �� ������ �� �� ����������� �����?
UninstallDataCorrupted="%1" �������� �� �������. ������������� ��� ������

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=������ ��������� ��������?
ConfirmDeleteSharedFile2=������ ������� �� ������ ��������� �������� ���� �� ������� �� ����� �������. �� �� ������ �� ������������ ������ ��� ��������� ��������?%n%n��� ���� �������� � ���� ������� ��� ��������, ����� ��� �������� �������������. ��� ����� �������, ��������� No. �������� ��� �������� ��� ������������� ������� ����� �������.
SharedFileNameLabel=����� ��������:
SharedFileLocationLabel=������:
WizardUninstalling=����� ������������
StatusUninstalling=������������� %1...

; The custom messages below aren't used by Setup itself, but if you make
; use of them in your scripts, you'll want to translate them.

[CustomMessages]
;Inno Setup Built-in Custom Messages
NameAndVersion=%1 ������ %2
AdditionalIcons=������� �����:
OptionalFeatures=Optional Features:
CreateDesktopIcon=������� &Desktop �����
CreateQuickLaunchIcon=������� &Quick Launch �����
ProgramOnTheWeb=%1 �� ���������
UninstallProgram=������������ %1
LaunchProgram=������� %1
AssocFileExtension=&�������� %1 �� %2 ����� ��������
AssocingFileExtension=������������ %1 �� %2 ����� ��������...

;Things we can also localize
CompanyName=Thingamahoochie Software

;Types
TypicalInstallation=Typical Installation
FullInstallation=Full Installation
CompactInstallation=Compact Installation
CustomInstallation=Custom Installation

;Components
AppCoreFiles=WinMerge Core Files
Filters=Filters
Plugins=Plugins

;Localization Components
Languages=Languages
BasqueLanguage=Basque menus and dialogs
BulgarianLanguage=Bulgarian menus and dialogs
CatalanLanguage=Catalan menus and dialogs
ChineseSimplifiedLanguage=Chinese (Simplified) menus and dialogs
ChineseTraditionalLanguage=Chinese (Traditional) menus and dialogs
CroatianLanguage=Croatian menus and dialogs
CzechLanguage=Czech menus and dialogs
DanishLanguage=Danish menus and dialogs
DutchLanguage=Dutch menus and dialogs
FrenchLanguage=French menus and dialogs
GalicianLanguage=Galician menus and dialogs
GermanLanguage=German menus and dialogs
GreekLanguage=Greek menus and dialogs
HungarianLanguage=Hungarian menus and dialogs
ItalianLanguage=Italian menus and dialogs
JapaneseLanguage=Japanese menus and dialogs
KoreanLanguage=Korean menus and dialogs
NorwegianLanguage=Norwegian menus and dialogs
PersianLanguage=Persian menus and dialogs
PolishLanguage=Polish menus and dialogs
PortugueseBrazilLanguage=Portuguese (Brazilian) menus and dialogs
PortugueseLanguage=Portuguese menus and dialogs
RomanianLanguage=Romanian menus and dialogs
RussianLanguage=Russian menus and dialogs
SerbianLanguage=Serbian menus and dialogs
SlovakLanguage=Slovak menus and dialogs
SlovenianLanguage=Slovenian menus and dialogs
SpanishLanguage=Spanish menus and dialogs
SwedishLanguage=Swedish menus and dialogs
TurkishLanguage=Turkish menus and dialogs
UkrainianLanguage=Ukrainian menus and dialogs

;Tasks
ExplorerContextMenu=&Enable Explorer context menu integration
IntegrateTortoiseCVS=Integrate with &TortoiseCVS
IntegrateTortoiseSVN=Integrate with T&ortoiseSVN
IntegrateClearCase=Integrate with Rational &ClearCase
AddToPath=&Add WinMerge folder to your system path

;Icon Labels
ReadMe=Read Me
UsersGuide=User's Guide
ViewStartMenuFolder=&View the WinMerge Start Menu Folder

;Code Dialogs
DeletePreviousStartMenu=The installer has detected that you changed the location of your start menu from "%s" to "%s". Would you like to delete the previous start menu folder?

; Project file description
ProjectFileDesc=WinMerge Project file
