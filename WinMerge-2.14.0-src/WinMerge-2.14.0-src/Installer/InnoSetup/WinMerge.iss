; ID line follows -- this is updated by SVN
; $Id: WinMerge.iss 7580M 2013-02-02 21:37:16Z (local) $
;
;           Programmed by:  Christian Blackburn, Christian List, Kimmo Varis,
;                 Purpose:  The is the Inno Setup installation script for distributing our WinMerge application.
; Tools Needed To Compile:  Inno Setup 5.1.7+ (http://www.jrsoftware.org/isdl.php), Inno Setup QuickStart Pack 5.1.7+(http://www.jrsoftware.org/isdl.php)
;                           note: the versions of Inno Setup and the QuickStart Pack should be identical to ensure proper function
;Directly Dependant Files:  Because this is an installer. It would be difficult to list and maintain each of the files referenced
;                           throughout the script in the header.  If you search this plain text script for a particular file in our Subversio repository and it
;                           doesn't appear then this script is not directly dependant on that file.
;Compilation Instructions:  1.  Open this file in Inno Setup or ISTool
;                           2.  Make sure Compression=LZMA/Ultra, InternalCompressLevel=Ultra, and SolidCompression=True these values are lowered during
;                               development to speed up compilation, however at release we want the intaller to be as strong as possible.
;                           3. Check all files are present:
;                                   -From ISTool Click
;                           4.  Compile the script: "Project" --> "Verify Files..."
;                                   -From Inno Setup "Click "Build" --> "Compile"
;                                   -From ISTool Click "Project" --> "Compile Setup"
;                           5.  The compiled installer will appear in the \InnoSetup\Output\ directory at currently should be around 1.5MBs in size.
;
; Installer To Do List:
; #  We need to unregister, and delete the ShellExtension Dll if the user doesn't want it, during installation

; #  Display integration options in gray rather than hiding them if the user doesn't have the application in question installed
; #  We need to ask those that have the RCLLocalization.dll in their plugins folder if they actually want it, their answer will need to be stored in the registry
; #  Write code to detect "\Programs\WinMerge\WinMerge" type start menu installs
;
; Things that make the user's life easier:
; #  Create instructions and a sample language file using the Inno Setup Translator Tool (http://www2.arnes.si/~sopjsimo/translator.html)
; #  Add "WinMerge is running would you like to close it now?" support with programmatic termination
;     -Note: We'll need to add a declares statement to our ISX code so that we can use FindWindowEx directly or a mutex search or two
; #  Rather than requiring users to restart we could just kill all intances of Explorer.exe, but we'll need to prompt the user first and restart it
;    once the ShellExtension.dll file has been added or removed.
;
; Non-Essential Features:
; #  See about getting a higher resolution copy of the Users's Guide.ico source art from somebody (A 32x32, and or 48x48 would be nice)
; #  Using the registry set the order our icons appear within their group in the start menu.:
;      1.  WinMerge
;      2.  Read Me
;      3.  Users's Guide
;      4.  WinMerge on the Web
;      5.  Uninstall WinMerge
; #  Create the ability to install to two start menu groups simultaneously
;
; Not yet possible (Limited by Inno Setup):
; #  While uninstalling prompt the user as to whether or not they'd like to remove their WinMerge preferences too?

#define AppVersion "2.14.0"
#define FriendlyAppVersion Copy(GetFileVersion(SourcePath + "\..\..\Build\MergeUnicodeRelease\WinMergeU.exe"), 1, 5)

; Runtime files installers
#define RuntimesX86Installer "..\..\..\Runtimes\vcredist_x86.exe"
#define RuntimesX64Installer "..\..\..\Runtimes\vcredist_x64.exe"


[Setup]
AppName=WinMerge
AppVersion={#AppVersion}
AppVerName=WinMerge {#AppVersion}
AppPublisher=Thingamahoochie Software
AppPublisherURL=http://WinMerge.org/
AppSupportURL=http://WinMerge.org/
AppUpdatesURL=http://WinMerge.org/

; Installer executable's version resource info
VersionInfoCompany=http://winmerge.org
VersionInfoDescription=WinMerge Installer
VersionInfoVersion={#AppVersion}

;This is in case an older version of the installer happened to be
DirExistsWarning=no

;Tells the installer to only display a select language dialog if the an exact match wasn't found
ShowLanguageDialog=auto

DefaultDirName={pf}\WinMerge
DefaultGroupName=WinMerge
DisableStartupPrompt=true
AllowNoIcons=true
InfoBeforeFile=..\..\Docs\users\GPL.rtf
InfoAfterFile=..\..\Docs\users\ReadMe.txt

OutputBaseFilename=WinMerge-{#AppVersion}-Setup

;This must be admin to install C++ Runtimes and shell extension
PrivilegesRequired=admin

;Windows 2000 or later required
MinVersion=0,5.0

UninstallDisplayIcon={app}\{code:ExeName}

;Artwork References
WizardImageFile=Art\Large Logo.bmp
WizardSmallImageFile=Art\Small Logo.bmp
WizardImageStretch=false

;It is confusing, if Setup/Uninstall use the same icon like WinMerge!
;SetupIconFile=..\..\src\res\Merge.ico

;Compression Parameters
;Please note while Compression=lzma/ultra and InternalCompressLevel=Ultra are better than max
;they also require 320 MB of memory for compression. If you're system has at least 256MB RAM then by all
;means set it to ultra before compilation
Compression=lzma/ultra
InternalCompressLevel=ultra
SolidCompression=true

; Update file associations for shell (project files)
ChangesAssociations=true
; Updates PATH
ChangesEnvironment=true
OutputDir=..\..\Build
AlwaysShowComponentsList=true


[Languages]
;Inno Setup's Native Language
Name: English; MessagesFile: Languages\English.isl

;Localizations:
Name: Basque; MessagesFile: Languages\Basque.isl; InfoAfterFile: ..\..\Docs\Users\Languages\ReadMe-Basque.txt
Name: Bulgarian; MessagesFile: Languages\Bulgarian.isl; InfoAfterFile: ..\..\Docs\Users\Languages\ReadMe-Bulgarian.txt
Name: Catalan; MessagesFile: Languages\Catalan.isl; InfoAfterFile: ..\..\Docs\Users\Languages\ReadMe-Catalan.txt
Name: Chinese_Simplified; MessagesFile: Languages\Chinese_Simplified.isl; InfoAfterFile: ..\..\Docs\Users\Languages\ReadMe-ChineseSimplified.txt
Name: Chinese_Traditional; MessagesFile: Languages\Chinese_Traditional.isl; InfoAfterFile: ..\..\Docs\Users\Languages\ReadMe-ChineseTraditional.txt
Name: Croatian; MessagesFile: Languages\Croatian.isl; InfoAfterFile: ..\..\Docs\Users\Languages\ReadMe-Croatian.txt
Name: Czech; MessagesFile: Languages\Czech.isl
Name: Danish; MessagesFile: Languages\Danish.isl
Name: Dutch; MessagesFile: Languages\Dutch.isl; InfoAfterFile: ..\..\Docs\Users\Languages\ReadMe-Dutch.txt
Name: French; MessagesFile: Languages\French.isl; InfoAfterFile: ..\..\Docs\Users\Languages\ReadMe-French.txt
Name: Galician; MessagesFile: Languages\Galician.isl; InfoAfterFile: ..\..\Docs\Users\Languages\ReadMe-Galician.txt
Name: German; MessagesFile: Languages\German.isl
Name: Greek; MessagesFile: Languages\Greek.isl; InfoAfterFile: ..\..\Docs\Users\Languages\ReadMe-Greek.txt
Name: Hungarian; MessagesFile: Languages\Hungarian.isl
Name: Italian; MessagesFile: Languages\Italian.isl
Name: Japanese; MessagesFile: Languages\Japanese.isl; InfoAfterFile: ..\..\Docs\Users\Languages\ReadMe-Japanese.txt
Name: Korean; MessagesFile: Languages\Korean.isl
Name: Norwegian; MessagesFile: Languages\Norwegian.isl
Name: Persian; MessagesFile: Languages\Persian.isl
Name: Polish; MessagesFile: Languages\Polish.isl
Name: Portuguese; MessagesFile: Languages\Portuguese.isl
Name: PortugueseBrazilian; MessagesFile: Languages\Brazilian_Portuguese.isl; InfoAfterFile: ..\..\Docs\Users\Languages\ReadMe-Brazilian.txt
Name: Romanian; MessagesFile: Languages\Romanian.isl; InfoAfterFile: ..\..\Docs\Users\Languages\ReadMe-Romanian.txt
Name: Russian; MessagesFile: Languages\Russian.isl
Name: Serbian; MessagesFile: Languages\Serbian.isl; InfoAfterFile: ..\..\Docs\Users\Languages\ReadMe-Serbian.txt
Name: Slovak; MessagesFile: Languages\Slovak.isl
Name: Slovenian; MessagesFile: Languages\Slovenian.isl
Name: Spanish; MessagesFile: Languages\Spanish.isl; InfoAfterFile: ..\..\Docs\Users\Languages\ReadMe-Spanish.txt
Name: Swedish; MessagesFile: Languages\Swedish.isl; InfoAfterFile: ..\..\Docs\Users\Languages\ReadMe-Swedish.txt
Name: Turkish; MessagesFile: Languages\Turkish.isl; InfoAfterFile: ..\..\Docs\Users\Languages\ReadMe-Turkish.txt
Name: Ukrainian; MessagesFile: Languages\Ukrainian.isl; InfoAfterFile: ..\..\Docs\Users\Languages\ReadMe-Ukrainian.txt


[Messages]
English.FinishedLabel=Setup has finished installing WinMerge on your computer.
English.SetupAppTitle=Setup - WinMerge {#AppVersion}
English.WizardInfoBefore=License Agreement
English.InfoBeforeLabel=GNU General Public License


[Types]
Name: typical; Description: {cm:TypicalInstallation}
Name: full; Description: {cm:FullInstallation}
Name: compact; Description: {cm:CompactInstallation}
Name: custom; Description: {cm:CustomInstallation}; Flags: iscustom


[Components]
; Executable, libraries, documentation
Name: Core; Description: {cm:AppCoreFiles}; Types: full custom typical compact; Flags: fixed

Name: filters; Description: {cm:Filters}; Flags: disablenouninstallwarning; Types: full typical
Name: Plugins; Description: {cm:Plugins}; Flags: disablenouninstallwarning; Types: full

;Language components
Name: Languages; Description: {cm:Languages}; Flags: disablenouninstallwarning
Name: Languages\Basque; Description: {cm:BasqueLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Basque
Name: Languages\Basque; Description: {cm:BasqueLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Basque

Name: Languages\Bulgarian; Description: {cm:BulgarianLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Bulgarian
Name: Languages\Bulgarian; Description: {cm:BulgarianLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Bulgarian

Name: Languages\Catalan; Description: {cm:CatalanLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Catalan
Name: Languages\Catalan; Description: {cm:CatalanLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Catalan

Name: Languages\Chinese_Simplified; Description: {cm:ChineseSimplifiedLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Chinese_Simplified
Name: Languages\Chinese_Simplified; Description: {cm:ChineseSimplifiedLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Chinese_Simplified

Name: Languages\Chinese_Traditional; Description: {cm:ChineseTraditionalLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Chinese_Traditional
Name: Languages\Chinese_Traditional; Description: {cm:ChineseTraditionalLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Chinese_Traditional

Name: Languages\Croatian; Description: {cm:CroatianLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Croatian
Name: Languages\Croatian; Description: {cm:CroatianLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Croatian

Name: Languages\Czech; Description: {cm:CzechLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Czech
Name: Languages\Czech; Description: {cm:CzechLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Czech

Name: Languages\Danish; Description: {cm:DanishLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Danish
Name: Languages\Danish; Description: {cm:DanishLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Danish

Name: Languages\Dutch; Description: {cm:DutchLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Dutch
Name: Languages\Dutch; Description: {cm:DutchLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Dutch

Name: Languages\French; Description: {cm:FrenchLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not French
Name: Languages\French; Description: {cm:FrenchLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: French

Name: Languages\Galician; Description: {cm:GalicianLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Galician
Name: Languages\Galician; Description: {cm:GalicianLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Galician

Name: Languages\German; Description: {cm:GermanLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not German
Name: Languages\German; Description: {cm:GermanLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: German

Name: Languages\Greek; Description: {cm:GreekLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Greek
Name: Languages\Greek; Description: {cm:GreekLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Greek

Name: Languages\Hungarian; Description: {cm:HungarianLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Hungarian
Name: Languages\Hungarian; Description: {cm:HungarianLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Hungarian

Name: Languages\Italian; Description: {cm:ItalianLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Italian
Name: Languages\Italian; Description: {cm:ItalianLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Italian

Name: Languages\Japanese; Description: {cm:JapaneseLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Japanese
Name: Languages\Japanese; Description: {cm:JapaneseLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Japanese

Name: Languages\Korean; Description: {cm:KoreanLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Korean
Name: Languages\Korean; Description: {cm:KoreanLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Korean

Name: Languages\Norwegian; Description: {cm:NorwegianLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Norwegian
Name: Languages\Norwegian; Description: {cm:NorwegianLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Norwegian

Name: Languages\Persian; Description: {cm:PersianLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Persian
Name: Languages\Persian; Description: {cm:PersianLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Persian

Name: Languages\Polish; Description: {cm:PolishLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Polish
Name: Languages\Polish; Description: {cm:PolishLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Polish

Name: Languages\Portuguese; Description: {cm:PortugueseLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Portuguese
Name: Languages\Portuguese; Description: {cm:PortugueseLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Portuguese

Name: Languages\PortugueseBrazilian; Description: {cm:PortugueseBrazilLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not PortugueseBrazilian
Name: Languages\PortugueseBrazilian; Description: {cm:PortugueseBrazilLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: PortugueseBrazilian

Name: Languages\Romanian; Description: {cm:RomanianLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Romanian
Name: Languages\Romanian; Description: {cm:RomanianLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Romanian

Name: Languages\Russian; Description: {cm:RussianLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Russian
Name: Languages\Russian; Description: {cm:RussianLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Russian

Name: Languages\Serbian; Description: {cm:SerbianLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Serbian
Name: Languages\Serbian; Description: {cm:SerbianLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Serbian

Name: Languages\Slovak; Description: {cm:SlovakLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Slovak
Name: Languages\Slovak; Description: {cm:SlovakLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Slovak

Name: Languages\Slovenian; Description: {cm:SlovenianLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Slovenian
Name: Languages\Slovenian; Description: {cm:SlovenianLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Slovenian

Name: Languages\Spanish; Description: {cm:SpanishLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Spanish
Name: Languages\Spanish; Description: {cm:SpanishLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Spanish

Name: Languages\Swedish; Description: {cm:SwedishLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Swedish
Name: Languages\Swedish; Description: {cm:SwedishLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Swedish

Name: Languages\Turkish; Description: {cm:TurkishLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Turkish
Name: Languages\Turkish; Description: {cm:TurkishLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Turkish

Name: Languages\Ukrainian; Description: {cm:UkrainianLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Ukrainian
Name: Languages\Ukrainian; Description: {cm:UkrainianLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Ukrainian


[Tasks]
Name: ShellExtension; Description: {cm:ExplorerContextMenu}; GroupDescription: {cm:OptionalFeatures}
Name: modifypath; Description: {cm:AddToPath}; GroupDescription: {cm:OptionalFeatures}; Flags: unchecked
Name: TortoiseCVS; Description: {cm:IntegrateTortoiseCVS}; GroupDescription: {cm:OptionalFeatures}; Check: TortoiseCVSInstalled
Name: TortoiseSVN; Description: {cm:IntegrateTortoiseSVN}; GroupDescription: {cm:OptionalFeatures}; Check: TortoiseSVNInstalled; MinVersion: 0,5.0.2195sp3
Name: ClearCase; Description: {cm:IntegrateClearCase}; GroupDescription: {cm:OptionalFeatures}; Check: ClearCaseInstalled
Name: desktopicon; Description: {cm:CreateDesktopIcon}; GroupDescription: {cm:AdditionalIcons}; Flags: unchecked
Name: quicklaunchicon; Description: {cm:CreateQuickLaunchIcon}; GroupDescription: {cm:AdditionalIcons}; OnlyBelowVersion: 0,6.1

[InstallDelete]
; Diff.txt is a file left over from previous versions of WinMerge (before version 2.0), we just delete it to be nice.
Type: files; Name: {app}\Diff.txt

;All of these files are removed so if the user upgrades their operating system or changes their language selections only the
;necessary files will be left in the installation folder
;Another reason these files might be strays is if a user extracted one of the experimental builds such as:
;WinMerge.{#AppVersion}-exe.7z.
Name: {app}\WinMerge.exe; Type: files
Name: {app}\WinMergeU.exe; Type: files; MinVersion: 0, 4

;Remove manifest files as we don't need them in 2.6.2 / 2.7.1.1 or later
Name: {app}\WinMerge.exe.manifest; Type: files
Name: {app}\WinMergeU.exe.manifest; Type: files

;This won't work, because the file has to be unregistered, and explorer closed, first.
;Name: {app}\ShellExtension.dll; Type: files; Check: TaskDisabled('ShellExtension')

; Remove existing .lang files - we don't need them anymore as we are
; using PO files now.
Name: {app}\Languages\MergeBrazilian.lang; Type: files; Check: not IsComponentSelected('PortugueseBrazilian')
Name: {app}\Languages\MergeBulgarian.lang; Type: files; Check: not IsComponentSelected('Bulgarian')
Name: {app}\Languages\MergeCatalan.lang; Type: files; Check: not IsComponentSelected('Catalan')
Name: {app}\Languages\MergeChineseSimplified.lang; Type: files; Check: not IsComponentSelected('Chinese_Simplified')
Name: {app}\Languages\MergeChineseTraditional.lang; Type: files; Check: not IsComponentSelected('Chinese_Traditional')
Name: {app}\Languages\MergeCzech.lang; Type: files; Check: not IsComponentSelected('Czech')
Name: {app}\Languages\MergeDanish.lang; Type: files; Check: not IsComponentSelected('Danish')
Name: {app}\Languages\MergeDutch.lang; Type: files; Check: not IsComponentSelected('Dutch')
Name: {app}\Languages\MergeFrench.lang; Type: files; Check: not IsComponentSelected('French')
Name: {app}\Languages\MergeGerman.lang; Type: files; Check: not IsComponentSelected('German')
Name: {app}\Languages\MergeHungarian.lang; Type: files; Check: not IsComponentSelected('Hungarian')
Name: {app}\Languages\MergeItalian.lang; Type: files; Check: not IsComponentSelected('Italian')
Name: {app}\Languages\MergeJapanese.lang; Type: files; Check: not IsComponentSelected('Japanese')
Name: {app}\Languages\MergeKorean.lang; Type: files; Check: not IsComponentSelected('Korean')
Name: {app}\Languages\MergeNorwegian.lang; Type: files; Check: not IsComponentSelected('Norwegian')
Name: {app}\Languages\MergePersian.lang; Type: files; Check: not IsComponentSelected('Persian')
Name: {app}\Languages\MergePolish.lang; Type: files; Check: not IsComponentSelected('Polish')
Name: {app}\Languages\MergePortuguese.lang; Type: files; Check: not IsComponentSelected('Portuguese')
Name: {app}\Languages\MergeSlovak.lang; Type: files; Check: not IsComponentSelected('Slovak')
Name: {app}\Languages\MergeSpanish.lang; Type: files; Check: not IsComponentSelected('Spanish')
Name: {app}\Languages\MergeRussian.lang; Type: files; Check: not IsComponentSelected('Russian')
Name: {app}\Languages\MergeSerbian.lang; Type: files; Check: not IsComponentSelected('Serbian')
Name: {app}\Languages\MergeSwedish.lang; Type: files; Check: not IsComponentSelected('Swedish')
Name: {app}\Languages\MergeTurkish.lang; Type: files; Check: not IsComponentSelected('Turkish')
Name: {app}\MergePlugins\list.txt; Type: files; Check: not IsComponentSelected('Plugins')

;Removes the user's guide icon if the user deselects the user's guide component.
Name: {group}\{cm:UsersGuide}.lnk; Type: files; Check: not IsComponentSelected('Docs')

;This removes the quick launch icon in case the user chooses not to install it after previously having it installed
Name: {userappdata}\Microsoft\Internet Explorer\Quick Launch\WinMerge.lnk; Type: files; Check: not IsTaskSelected('QuickLauchIcon')
Name: {commonappdata}\Microsoft\Internet Explorer\Quick Launch\WinMerge.lnk; Type: files; Check: not IsTaskSelected('QuickLauchIcon')

;This removes the desktop icon in case the user chooses not to install it after previously having it installed
Name: {commondesktop}\WinMerge.lnk; Type: files; Check: not IsTaskSelected('DesktopIcon')

;Removes the Uninstall icon from the start menu...
Name: {group}\{cm:UninstallProgram,WinMerge}.lnk; Type: files;

;Remove ANSI executable link from start menu for NT-based Windows versions
;This was installed earlier, but not anymore.
Name: {group}\WinMerge (ANSI).lnk; Type: files; MinVersion: 0,4
Name: {app}\Docs; Type: filesandordirs

Name: {app}\MergePlugins\editor addin.sct; Type: Files; Check: not IsComponentSelected('Plugins')
Name: {app}\MergePlugins\insert datetime.sct; Type: Files; Check: not IsComponentSelected('Plugins')
Name: {app}\MergePlugins; Type: DirIfEmpty; Check: not IsComponentSelected('Plugins')

Name: {app}\Filters\ADAMulti.flt; Type: Files; Check: not IsComponentSelected('Filters')
Name: {app}\Filters\ASPNET.flt; Type: Files; Check: not IsComponentSelected('Filters')
Name: {app}\Filters\CSharp_loose.flt; Type: Files; Check: not IsComponentSelected('Filters')
Name: {app}\Filters\Delphi.flt; Type: Files; Check: not IsComponentSelected('Filters')
Name: {app}\Filters\MASM.flt; Type: Files; Check: not IsComponentSelected('Filters')
Name: {app}\Filters\Merge_GnuC_loose.flt; Type: Files; Check: not IsComponentSelected('Filters')
Name: {app}\Filters\Merge_VC_loose.flt; Type: Files; Check: not IsComponentSelected('Filters')
Name: {app}\Filters\Merge_VB_loose.flt; Type: Files; Check: not IsComponentSelected('Filters')
Name: {app}\Filters\SourceControl.flt; Type: Files; Check: not IsComponentSelected('Filters')
Name: {app}\Filters\Symbian.flt; Type: Files; Check: not IsComponentSelected('Filters')
Name: {app}\Filters\XML_html.flt; Type: Files; Check: not IsComponentSelected('Filters')
Name: {app}\Filters\FileFilter.tmpl; Type: Files; Check: not IsComponentSelected('Filters')
Name: {app}\Filters; Type: DirIfEmpty; Check: not IsComponentSelected('Filters')



[Dirs]
;The always uninstall flag tells the uninstaller to remove the folder if it's empty regardless of whether or not it existed prior to the installation
Name: {app}; Flags: uninsalwaysuninstall


[Files]
; WinMerge itself
Source: ..\..\Build\MergeUnicodeRelease\WinMergeU.exe; DestDir: {app}; Flags: promptifolder; Components: Core

; List of installed files
Source: ..\..\Docs\Users\Files.txt; DestDir: {app}; Flags: promptifolder; Components: Core

; Microsoft runtime libraries installer (C-runtimes, MFC)
Source: {#RuntimesX86Installer}; DestDir: {tmp}; Flags: ignoreversion; Components: Core; AfterInstall: RuntimesInstaller
Source: {#RuntimesX64Installer}; DestDir: {tmp}; Flags: ignoreversion; Components: Core; Check: IsWin64; AfterInstall: RuntimesX64Installer

; Shell extension
Source: ..\..\Build\ShellExtension\unicode release mindependency\ShellExtensionU.dll; DestDir: {app}; Flags: regserver uninsrestartdelete restartreplace promptifolder; Check: not IsWin64
; 64-bit version of ShellExtension
Source: ..\..\Build\ShellExtension\x64 release\ShellExtensionX64.dll; DestDir: {app}; Flags: regserver uninsrestartdelete restartreplace promptifolder 64bit; MinVersion: 0,5.01.2600; Check: IsWin64

; Expat dll
Source: ..\..\Build\expat\libexpat.dll; DestDir: {app}; Flags: promptifolder; Components: Core

; PCRE dll
Source: ..\..\Build\pcre\pcre.dll; DestDir: {app}; Flags: promptifolder; Components: Core

; MergeLang.dll - translation helper dll
Source: ..\..\Build\MergeUnicodeRelease\MergeLang.dll; DestDir: {app}; Flags: promptifolder; Components: Core

; Language files
Source: ..\..\Src\Languages\Basque.po; DestDir: {app}\Languages; Components: Languages\Basque; Flags: ignoreversion comparetimestamp
Source: ..\..\Docs\Users\Languages\ReadMe-Basque.txt; DestDir: {app}\Docs; Components: Languages\Basque
Source: ..\..\Src\Languages\Brazilian.po; DestDir: {app}\Languages; Components: Languages\PortugueseBrazilian; Flags: ignoreversion comparetimestamp
Source: ..\..\Docs\Users\Languages\ReadMe-Brazilian.txt; DestDir: {app}\Docs; Components: Languages\PortugueseBrazilian
Source: ..\..\Src\Languages\Bulgarian.po; DestDir: {app}\Languages; Components: Languages\Bulgarian; Flags: ignoreversion comparetimestamp
Source: ..\..\Docs\Users\Languages\ReadMe-Bulgarian.txt; DestDir: {app}\Docs; Components: Languages\Bulgarian
Source: ..\..\Src\Languages\Catalan.po; DestDir: {app}\Languages; Components: Languages\Catalan; Flags: ignoreversion comparetimestamp
Source: ..\..\Docs\Users\Languages\ReadMe-Catalan.txt; DestDir: {app}\Docs; Components: Languages\Catalan
Source: ..\..\Src\Languages\ChineseSimplified.po; DestDir: {app}\Languages; Components: Languages\Chinese_Simplified; Flags: ignoreversion comparetimestamp
Source: ..\..\Docs\Users\Languages\ReadMe-ChineseSimplified.txt; DestDir: {app}\Docs; Components: Languages\Chinese_Simplified
Source: ..\..\Src\Languages\ChineseTraditional.po; DestDir: {app}\Languages; Components: Languages\Chinese_Traditional; Flags: ignoreversion comparetimestamp
Source: ..\..\Docs\Users\Languages\ReadMe-ChineseTraditional.txt; DestDir: {app}\Docs; Components: Languages\Chinese_Traditional
Source: ..\..\Src\Languages\Croatian.po; DestDir: {app}\Languages; Components: Languages\Croatian; Flags: ignoreversion comparetimestamp
Source: ..\..\Docs\Users\Languages\ReadMe-Croatian.txt; DestDir: {app}\Docs; Components: Languages\Croatian
Source: ..\..\Src\Languages\Czech.po; DestDir: {app}\Languages; Components: Languages\Czech; Flags: ignoreversion comparetimestamp
Source: ..\..\Src\Languages\Danish.po; DestDir: {app}\Languages; Components: Languages\Danish; Flags: ignoreversion comparetimestamp
Source: ..\..\Src\Languages\Dutch.po; DestDir: {app}\Languages; Components: Languages\Dutch; Flags: ignoreversion comparetimestamp
Source: ..\..\Docs\Users\Languages\ReadMe-Dutch.txt; DestDir: {app}\Docs; Components: Languages\Dutch
Source: ..\..\Src\Languages\French.po; DestDir: {app}\Languages; Components: Languages\French; Flags: ignoreversion comparetimestamp
Source: ..\..\Docs\Users\Languages\ReadMe-French.txt; DestDir: {app}\Docs; Components: Languages\French
Source: ..\..\Src\Languages\Galician.po; DestDir: {app}\Languages; Components: Languages\Galician; Flags: ignoreversion comparetimestamp
Source: ..\..\Docs\Users\Languages\ReadMe-Galician.txt; DestDir: {app}\Docs; Components: Languages\Galician
Source: ..\..\Src\Languages\German.po; DestDir: {app}\Languages; Components: Languages\German; Flags: ignoreversion comparetimestamp
Source: ..\..\Src\Languages\Greek.po; DestDir: {app}\Languages; Components: Languages\Greek; Flags: ignoreversion comparetimestamp
Source: ..\..\Docs\Users\Languages\ReadMe-Greek.txt; DestDir: {app}\Docs; Components: Languages\Greek
Source: ..\..\Src\Languages\Hungarian.po; DestDir: {app}\Languages; Components: Languages\Hungarian; Flags: ignoreversion comparetimestamp
Source: ..\..\Src\Languages\Italian.po; DestDir: {app}\Languages; Components: Languages\Italian; Flags: ignoreversion comparetimestamp
Source: ..\..\Src\Languages\Japanese.po; DestDir: {app}\Languages; Components: Languages\Japanese; Flags: ignoreversion comparetimestamp
Source: ..\..\Docs\Users\Languages\ReadMe-Japanese.txt; DestDir: {app}\Docs; Components: Languages\Japanese
Source: ..\..\Src\Languages\Korean.po; DestDir: {app}\Languages; Components: Languages\Korean; Flags: ignoreversion comparetimestamp
Source: ..\..\Src\Languages\Norwegian.po; DestDir: {app}\Languages; Components: Languages\Norwegian; Flags: ignoreversion comparetimestamp
Source: ..\..\Src\Languages\Persian.po; DestDir: {app}\Languages; Components: Languages\Persian; Flags: ignoreversion comparetimestamp
Source: ..\..\Src\Languages\Polish.po; DestDir: {app}\Languages; Components: Languages\Polish; Flags: ignoreversion comparetimestamp
Source: ..\..\Src\Languages\Portuguese.po; DestDir: {app}\Languages; Components: Languages\Portuguese; Flags: ignoreversion comparetimestamp
Source: ..\..\Src\Languages\Romanian.po; DestDir: {app}\Languages; Components: Languages\Romanian; Flags: ignoreversion comparetimestamp
Source: ..\..\Docs\Users\Languages\ReadMe-Romanian.txt; DestDir: {app}\Docs; Components: Languages\Romanian
Source: ..\..\Src\Languages\Russian.po; DestDir: {app}\Languages; Components: Languages\Russian; Flags: ignoreversion comparetimestamp
Source: ..\..\Src\Languages\Serbian.po; DestDir: {app}\Languages; Components: Languages\Serbian; Flags: ignoreversion comparetimestamp
Source: ..\..\Docs\Users\Languages\ReadMe-Serbian.txt; DestDir: {app}\Docs; Components: Languages\Serbian
Source: ..\..\Src\Languages\Slovak.po; DestDir: {app}\Languages; Components: Languages\Slovak; Flags: ignoreversion comparetimestamp
Source: ..\..\Src\Languages\Slovenian.po; DestDir: {app}\Languages; Components: Languages\Slovenian; Flags: ignoreversion comparetimestamp
Source: ..\..\Src\Languages\Spanish.po; DestDir: {app}\Languages; Components: Languages\Spanish; Flags: ignoreversion comparetimestamp
Source: ..\..\Docs\Users\Languages\ReadMe-Spanish.txt; DestDir: {app}\Docs; Components: Languages\Spanish
Source: ..\..\Src\Languages\Swedish.po; DestDir: {app}\Languages; Components: Languages\Swedish; Flags: ignoreversion comparetimestamp
Source: ..\..\Docs\Users\Languages\ReadMe-Swedish.txt; DestDir: {app}\Docs; Components: Languages\Swedish
Source: ..\..\Src\Languages\Turkish.po; DestDir: {app}\Languages; Components: Languages\Turkish; Flags: ignoreversion comparetimestamp
Source: ..\..\Docs\Users\Languages\ReadMe-Turkish.txt; DestDir: {app}\Docs; Components: Languages\Turkish
Source: ..\..\Src\Languages\Ukrainian.po; DestDir: {app}\Languages; Components: Languages\Ukrainian; Flags: ignoreversion comparetimestamp
Source: ..\..\Docs\Users\Languages\ReadMe-Ukrainian.txt; DestDir: {app}\Docs; Components: Languages\Ukrainian

Source: ..\..\Filters\*.flt; DestDir: {app}\Filters; Flags: sortfilesbyextension comparetimestamp ignoreversion; Components: filters
Source: ..\..\Filters\FileFilter.tmpl; DestDir: {app}\Filters; Flags: sortfilesbyextension comparetimestamp ignoreversion; Components: filters

;Documentation
Source: ..\..\Docs\Users\ReadMe.txt; DestDir: {app}\Docs; Flags: comparetimestamp ignoreversion promptifolder; Components: Core
Source: ..\..\Docs\Users\Contributors.txt; DestDir: {app}; Flags: comparetimestamp ignoreversion promptifolder; Components: Core
Source: ..\..\Docs\Users\ReleaseNotes.html; DestDir: {app}\Docs; Flags: comparetimestamp ignoreversion promptifolder; Components: Core
Source: ..\..\Docs\Users\ChangeLog.txt; DestDir: {app}\Docs; Flags: comparetimestamp ignoreversion promptifolder; Components: Core
Source: ..\..\Build\Manual\htmlhelp\WinMerge.chm; DestDir: {app}\Docs\; Flags: overwritereadonly uninsremovereadonly; Components: Core

;Plugins
;Please note IgnoreVersion and CompareTimeStamp are to instruct the installer to not not check for version info and go straight to comparing modification dates
Source: ..\..\Plugins\dlls\editor addin.sct; DestDir: {app}\MergePlugins; Flags: IgnoreVersion CompareTimeStamp; Components: Plugins
Source: ..\..\Plugins\dlls\insert datetime.sct; DestDir: {app}\MergePlugins; Flags: IgnoreVersion CompareTimeStamp; Components: Plugins
Source: ..\..\Plugins\dlls\*.dll; DestDir: {app}\MergePlugins; Flags: promptifolder; Components: Plugins


[Icons]
;Start Menu Icons
Name: {group}\WinMerge; Filename: {app}\{code:ExeName}
Name: {group}\{cm:ReadMe}; Filename: {app}\Docs\ReadMe.txt; IconFileName: {win}\NOTEPAD.EXE
Name: {group}\{cm:UsersGuide}; Filename: {app}\Docs\WinMerge.chm
Name: {group}\{cm:ProgramOnTheWeb,WinMerge}; Filename: http://winmerge.org/

;Link to translated ReadMe in Start Menu
Name: {group}\{cm:ReadMe}; Filename: {app}\Docs\ReadMe-Basque.txt; IconFileName: {win}\NOTEPAD.EXE; Languages: Basque
Name: {group}\{cm:ReadMe}; Filename: {app}\Docs\ReadMe-Brazilian.txt; IconFileName: {win}\NOTEPAD.EXE; Languages: PortugueseBrazilian
Name: {group}\{cm:ReadMe}; Filename: {app}\Docs\ReadMe-Bulgarian.txt; IconFileName: {win}\NOTEPAD.EXE; Languages: Bulgarian
Name: {group}\{cm:ReadMe}; Filename: {app}\Docs\ReadMe-Catalan.txt; IconFileName: {win}\NOTEPAD.EXE; Languages: Catalan
Name: {group}\{cm:ReadMe}; Filename: {app}\Docs\ReadMe-ChineseSimplified.txt; IconFileName: {win}\NOTEPAD.EXE; Languages: Chinese_Simplified
Name: {group}\{cm:ReadMe}; Filename: {app}\Docs\ReadMe-ChineseTraditional.txt; IconFileName: {win}\NOTEPAD.EXE; Languages: Chinese_Traditional
Name: {group}\{cm:ReadMe}; Filename: {app}\Docs\ReadMe-Croatian.txt; IconFileName: {win}\NOTEPAD.EXE; Languages: Croatian
Name: {group}\{cm:ReadMe}; Filename: {app}\Docs\ReadMe-Dutch.txt; IconFileName: {win}\NOTEPAD.EXE; Languages: Dutch
Name: {group}\{cm:ReadMe}; Filename: {app}\Docs\ReadMe-French.txt; IconFileName: {win}\NOTEPAD.EXE; Languages: French
Name: {group}\{cm:ReadMe}; Filename: {app}\Docs\ReadMe-Galician.txt; IconFileName: {win}\NOTEPAD.EXE; Languages: Galician
Name: {group}\{cm:ReadMe}; Filename: {app}\Docs\ReadMe-Greek.txt; IconFileName: {win}\NOTEPAD.EXE; Languages: Greek
Name: {group}\{cm:ReadMe}; Filename: {app}\Docs\ReadMe-Japanese.txt; IconFileName: {win}\NOTEPAD.EXE; Languages: Japanese
Name: {group}\{cm:ReadMe}; Filename: {app}\Docs\ReadMe-Romanian.txt; IconFileName: {win}\NOTEPAD.EXE; Languages: Romanian
Name: {group}\{cm:ReadMe}; Filename: {app}\Docs\ReadMe-Serbian.txt; IconFileName: {win}\NOTEPAD.EXE; Languages: Serbian
Name: {group}\{cm:ReadMe}; Filename: {app}\Docs\ReadMe-Spanish.txt; IconFileName: {win}\NOTEPAD.EXE; Languages: Spanish
Name: {group}\{cm:ReadMe}; Filename: {app}\Docs\ReadMe-Swedish.txt; IconFileName: {win}\NOTEPAD.EXE; Languages: Swedish
Name: {group}\{cm:ReadMe}; Filename: {app}\Docs\ReadMe-Turkish.txt; IconFileName: {win}\NOTEPAD.EXE; Languages: Turkish
Name: {group}\{cm:ReadMe}; Filename: {app}\Docs\ReadMe-Ukrainian.txt; IconFileName: {win}\NOTEPAD.EXE; Languages: Ukrainian

;Desktop Icon
Name: {commondesktop}\WinMerge; Filename: {app}\{code:ExeName}; Tasks: desktopicon

;Quick Launch Icon
Name: {userappdata}\Microsoft\Internet Explorer\Quick Launch\WinMerge; Filename: {app}\{code:ExeName}; Tasks: quicklaunchicon

[Registry]
Root: HKCU; Subkey: Software\Thingamahoochie; Flags: uninsdeletekeyifempty
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge; Flags: uninsdeletekey
Root: HKLM; Subkey: Software\Thingamahoochie; Flags: uninsdeletekeyifempty
Root: HKLM; Subkey: Software\Thingamahoochie\WinMerge; Flags: uninsdeletekey

;Add Project file extension (.WinMerge) to known extensions and
;set WinMerge to open those files
;set Notepad to edit project files
Root: HKCR; Subkey: .WinMerge; ValueType: String; ValueData: WinMerge.Project.File; Flags: uninsdeletekey
Root: HKCR; Subkey: WinMerge.Project.File; ValueType: String; ValueData: {cm:ProjectFileDesc}; Flags: uninsdeletekey
Root: HKCR; Subkey: WinMerge.Project.File\shell\open\command; ValueType: String; ValueData: """{app}\{code:ExeName}"" ""%1"""; Flags: uninsdeletekey
Root: HKCR; Subkey: WinMerge.Project.File\shell\edit\command; ValueType: String; ValueData: """NOTEPAD.EXE"" ""%1"""; Flags: uninsdeletekey
Root: HKCR; Subkey: WinMerge.Project.File\DefaultIcon; ValueType: String; ValueData: """{app}\{code:ExeName}"",1"; Flags: uninsdeletekey

; delete obsolete values
;In Inno Setup Version 4.18 ValueData couldn't be null and compile,
;if this is fixed in a later version feel free to remove the parameter
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge\Settings; ValueType: none; ValueName: LeftMax; Flags: deletevalue
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge\Settings; ValueType: none; ValueName: DirViewMax; Flags: deletevalue

;This removes the key that remembers which messageboxes to hide from the user, this is because the text of that message
;can change and make it more clear as to the user why they might want to pay attention to a particular dialog and also
;because a particular message might be added or removed and a new message might occupy a previous message's ID number
Root: HKLM; Subkey: Software\Thingamahoochie\WinMerge\MessageBoxes; ValueType: none; Flags: deletekey

Root: HKCR; SubKey: Directory\Shell\WinMerge\command; ValueType: none; Flags: deletekey noerror
Root: HKCR; SubKey: Directory\Shell\WinMerge; ValueType: none; Flags: deletekey noerror

;Adds "Start Menu" --> "Run" Support for WinMerge
;TODO: Deinstall WinMerge.exe paths?
Root: HKLM; Subkey: Software\Microsoft\Windows\CurrentVersion\App Paths\WinMerge.exe; ValueType: none; Flags: uninsdeletekey
Root: HKLM; Subkey: Software\Microsoft\Windows\CurrentVersion\App Paths\WinMergeU.exe; ValueType: none; Flags: uninsdeletekey
Root: HKLM; SubKey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\WinMerge.exe; ValueType: string; ValueName: ; ValueData: {app}\{code:ExeName}
Root: HKLM; SubKey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\WinMergeU.exe; ValueType: string; ValueName: ; ValueData: {app}\{code:ExeName}

;Registry Keys for use by ShellExtension.dll
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge; ValueType: string; ValueName: Executable; ValueData: {app}\{code:ExeName}

;Enables or disables the Context Menu preference based on what the user selects during install
;Initially the Context menu is explicitly disabled:
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge; ValueType: dword; ValueName: ContextMenuEnabled; ValueData: 0; Check: not IsTaskSelected('ShellExtension')

;If the user chose to use the context menu then we re-enable it.  This is necessary so it'll turn on and off not just on.
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge; ValueType: dword; ValueName: ContextMenuEnabled; ValueData: 1; Tasks: ShellExtension; Check: not ShellMenuEnabled()

;If WinMerge.exe is installed then we'll automatically configure WinMerge as the differencing application
Root: HKCU; SubKey: Software\TortoiseCVS; ValueType: string; ValueName: External Diff Application; ValueData: {app}\{code:ExeName}; Flags: uninsdeletevalue; Tasks: TortoiseCVS
Root: HKCU; SubKey: Software\TortoiseCVS; ValueType: dword; ValueName: DiffAsUnicode; ValueData: $00000001; Flags: uninsdeletevalue; Tasks: TortoiseCVS
Root: HKCU; SubKey: Software\TortoiseCVS\Prefs\External Diff Application; ValueType: string; ValueName: _; ValueData: {app}\{code:ExeName}; Flags: uninsdeletevalue dontcreatekey; Tasks: TortoiseCVS
Root: HKCU; SubKey: Software\TortoiseCVS\Prefs\External Diff2 Params; ValueType: string; ValueName: _; ValueData: """%1"" ""%2"""; Flags: uninsdeletevalue dontcreatekey; Tasks: TortoiseCVS

;Tells TortoiseCVS to use WinMerge as its differencing application (this happens whether or not Tortoise is current installed, that way
;if it is installed at a later date this will automatically support it)
Root: HKCU; SubKey: Software\TortoiseCVS; ValueType: string; ValueName: External Merge Application; ValueData: {app}\{code:ExeName}; Flags: uninsdeletevalue; Tasks: TortoiseCVS
Root: HKCU; SubKey: Software\TortoiseCVS; ValueType: dword; ValueName: MergeAsUnicode; ValueData: $00000001; Flags: uninsdeletevalue; Tasks: TortoiseCVS
Root: HKCU; SubKey: Software\TortoiseCVS\Prefs\External Merge Application; ValueType: string; ValueName: _; ValueData: {app}\{code:ExeName}; Flags: uninsdeletevalue dontcreatekey; Tasks: TortoiseCVS
Root: HKCU; SubKey: Software\TortoiseCVS\Prefs\External Merge2 Params; ValueType: string; ValueName: _; ValueData: """%mine"" ""%yours"""; Flags: uninsdeletevalue dontcreatekey; Tasks: TortoiseCVS

;Set WinMerge as TortoiseSVN diff tool
Root: HKCU; SubKey: Software\TortoiseSVN; ValueType: string; ValueName: Diff; ValueData: {app}\{code:ExeName} -e -ub -dl %bname -dr %yname %base %mine; Flags: uninsdeletevalue; Tasks: TortoiseSVN

;Whatever the user chooses at the [Select Setup Language] dialog should also determine what language WinMerge will start up in
;(unless the user already has a startup language specified)
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $0000042d; Flags: deletevalue; Languages: Basque
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000402; Flags: deletevalue; Languages: Bulgarian
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000403; Flags: deletevalue; Languages: Catalan
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000804; Flags: deletevalue; Languages: Chinese_Simplified
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000404; Flags: deletevalue; Languages: Chinese_Traditional
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $0000041a; Flags: deletevalue; Languages: Croatian
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000405; Flags: deletevalue; Languages: Czech
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000406; Flags: deletevalue; Languages: Danish
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000413; Flags: deletevalue; Languages: Dutch
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000409; Flags: deletevalue; Languages: English
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $0000040c; Flags: deletevalue; Languages: French
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000456; Flags: deletevalue; Languages: Galician
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000407; Flags: deletevalue; Languages: German
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000408; Flags: deletevalue; Languages: Greek
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $0000040e; Flags: deletevalue; Languages: Hungarian
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000410; Flags: deletevalue; Languages: Italian
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000411; Flags: deletevalue; Languages: Japanese
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000412; Flags: deletevalue; Languages: Korean
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000414; Flags: deletevalue; Languages: Norwegian
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000429; Flags: deletevalue; Languages: Persian
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000415; Flags: deletevalue; Languages: Polish
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000816; Flags: deletevalue; Languages: Portuguese
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000416; Flags: deletevalue; Languages: PortugueseBrazilian
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000418; Flags: deletevalue; Languages: Romanian
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000419; Flags: deletevalue; Languages: Russian
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000c1a; Flags: deletevalue; Languages: Serbian
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $0000041b; Flags: deletevalue; Languages: Slovak
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000424; Flags: deletevalue; Languages: Slovenian
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000c0a; Flags: deletevalue; Languages: Spanish
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $0000041D; Flags: deletevalue; Languages: Swedish
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $0000041f; Flags: deletevalue; Languages: Turkish
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000422; Flags: deletevalue; Languages: Ukrainian


[Run]
;This will no longer appear unless the user chose to make a start menu group in the first place
Filename: {win}\Explorer.exe; Description: {cm:ViewStartMenuFolder}; Parameters: """{group}"""; Flags: waituntilidle postinstall skipifsilent unchecked; Check: GroupCreated

Filename: {app}\{code:ExeName}; Description: {cm:LaunchProgram, WinMerge}; Flags: nowait postinstall skipifsilent runmaximized


[UninstallDelete]
;Remove 7-zip integration dlls possibly installed (by hand or using separate installer)
Name: {app}\Merge7z*.dll; Type: files
Name: {app}\7zip_pad.xml; Type: files
Name: {app}\Codecs; Type: filesandordirs
Name: {app}\Formats; Type: filesandordirs
Name: {app}\Lang; Type: filesandordirs

Name: {group}; Type: dirifempty
Name: {app}; Type: dirifempty


[Code]

{Runs the runtime file installer}
{Command line used is documented in:
http://blogs.msdn.com/b/astebner/archive/2006/08/23/715755.aspx
http://blogs.msdn.com/astebner/archive/2007/02/07/update-regarding-silent-install-of-the-vc-8-0-runtime-vcredist-packages.aspx
}
procedure RuntimesInstaller();
var
    ResultCode: Integer;
begin
    Exec(ExpandConstant('{tmp}\vcredist_x86.exe'), '/q:a /c:"msiexec /i vcredist.msi /qn /l*v ' + GetTempDir() + '\vcredist_x86.log"', ExpandConstant('{tmp}'), SW_SHOW, ewWaitUntilTerminated, ResultCode)
    if ResultCode <> 0 then
      begin
        Exec(ExpandConstant('{tmp}\vcredist_x86.exe'), '/q:a /c:"VCREDI~3.EXE /q:a /c:""msiexec /i vcredist.msi /qn"" "', ExpandConstant('{tmp}'), SW_SHOW, ewWaitUntilTerminated, ResultCode)
      end
end;

{Runs the runtime file installer for X64}
{Command line used is documented in:
http://blogs.msdn.com/b/astebner/archive/2006/08/23/715755.aspx
http://blogs.msdn.com/astebner/archive/2007/02/07/update-regarding-silent-install-of-the-vc-8-0-runtime-vcredist-packages.aspx
}
procedure RuntimesX64Installer();
var
    ResultCode: Integer;
begin
    Exec(ExpandConstant('{tmp}\vcredist_x64.exe'), '/q:a /c:"msiexec /i vcredist.msi /qn /l*v ' + GetTempDir() + '\vcredist_x64.log"', ExpandConstant('{tmp}'), SW_SHOW, ewWaitUntilTerminated, ResultCode)
    if ResultCode <> 0 then
      begin
        Exec(ExpandConstant('{tmp}\vcredist_x64.exe'), '/q:a /c:"VCREDI~2.EXE /q:a /c:""msiexec /i vcredist.msi /qn"" "', ExpandConstant('{tmp}'), SW_SHOW, ewWaitUntilTerminated, ResultCode)
      end
end;

{Determines whether or not the user chose to create a start menu}
Function GroupCreated(): boolean;
Var
    {Stores the path of the start menu group Inno Setup may have created}
    strGroup_Path: string;
Begin
    {Saves the path that Inno Setup intended to create the start menu group at}
    strGroup_Path := ExpandConstant('{group}');

    {If the start menu path isn't blank then..}
    if strGroup_Path <> '' Then
        Begin
            {If the user choose to create the start menu then this folder will exist.
            If the folder exists then GroupCreated = True otherwise it does not.}
            Result := DirExists(strGroup_Path)
        end
    else
        {Since the start menu path is null, we know that the user chose not to create a
        start menu group (note in Inno Setup 4.18 this didn't yet work, but I'm sure it will in the future}
        Result := False;

     {Debug

    If DirExists(strGroup_Path) = True Then
        Msgbox('The group "' + ExpandConstant('group') + '" was found', mbInformation, mb_ok)
    Else
        Msgbox('The group "' + ExpandConstant('group') + '" doesn''t exist.', mbInformation, mb_ok); }
End;


{Returns the appropriate name of the .EXE being installed}
Function ExeName(Unused: string): string;
Begin
  Result := 'WinMergeU.exe';
End;

Function FixVersion(strInput: string): string;
{Returns a version with four segments A.B.C.D}
Var
  {Stores the number of periods found within the version string}
  intPeriods: integer;

  {Creates a counter}
  i: integer;

  {Generates the string to be returned to the user}
  strVersion: string;
Begin

  {Creates a copy of the input string before we tear it apart}
  strVersion := strInput;

  {Until strInput is empty do..}
  While Length(strInput) > 0 do
    Begin
      {if the first character of the input string is a period then}
      If Copy(strInput, 1, 1) = '.' Then

        {Incriment the number of periods found}
        intPeriods := intPeriods + 1;

      {Remove the first character from the Input string}
      strInput := Copy(strINput, 2, length(strINput));
    End;

  {For every period shy of 3 do..}
  For i := 1 to 3 - intperiods do

    {Add a '.0' to the version string}
    strVersion := strVersion + '.0';

  {Returns the Version string with the correct number of segments}
  Result := strVersion;

End;

Function RemoveLeadingZeros(strInput: string): string;
{Removes the leading zeros if any from a numeric string}
Begin

  {While the first character of the string is a zero}
  While Copy(strInput, 1, 1) = '0' Do
    begin

    {Removes one leading zero from the input string}
      strInput := Copy(strInput, 2, Length(strINput));
    end;

  {Returns the formatted string}
  Result := strInput;

End;


{Returns whether or not the version string detected is meets the version number requirement}
Function VersionAtLeast(strVersion_Installed: string; intMajor: integer; intMinor: integer; intRevision: integer; intBuild: integer): boolean;
Var

  {Stores the Major of the Version installed (X.0.0.0)}
  intMajor_Installed: Integer;

  {Stores the Minor of the Version installed (1.X.0.0)}
  intMinor_Installed: Integer;

  {Stores the Revision of the Version installed (1.0.X.0)}
  intRevision_Installed: Integer;

  {Stores the Revision of the Version installed (1.0.0.X)}
  intBuild_Installed: Integer;

  {Stores the last valid character of a particular segment (Major, Minor, Revision) of the Version string}
  intEnd_Pos: Integer;

begin
  {Debug
  Msgbox('The version installed is ' + strVersion_Installed + ' and the required version is ' + IntToStr(intMajor) + '.' + IntToStr(intMinor) + '.' + IntToStr(intRevision) + '.' + IntToStr(intBuild), mbINformation, mb_OK)
        }

  {Makes sure the version string contains four numberic segments 5.2 ---> 5.2.0.0}
  strVersion_Installed := FixVersion(strVersion_Installed);

  {If the version number is empty then quit the function}
  if strVersion_Installed = '' Then
    begin
      Result := False;

      {Stops analyzing the version installed and returns that the version installed was inadequate}
      exit;
    end;

  {Starts detecting the Major value of the Version Installed}

  {Sets the end position equal to one character before the first period in the version number}
  intEnd_Pos := Pos('.', strVersion_Installed) -1

  {Sets the major version equal to all character before the first period }
  intMajor_installed := StrToIntDef(RemoveLeadingZeros(Copy(strVersion_Installed, 1, intEnd_Pos)), 0);

  {Debug
  msgbox('The Major version installed is ' + IntToStr(intMajor_installed) + ' and the required Major is ' + IntToStr(intMajor) + '.', mbInformation, MB_OK)
        }

  {If the Major Version Installed is greater than the required value then...}
  if intMajor_Installed > intMajor Then
    begin
      {Returns that the version number was adequate, since it actually exceeded the requirement}
      Result := True;

        {Debug
        msgbox(IntToStr(intMajor_installed) + ' > ' +  IntToStr(intMajor), mbInformation, MB_OK)
        }

      {Stops analyzing the version number since we already know it met the requirement}
      exit;
    end;

  {If the Major version installed is less than the requirement then...}
  If intMajor_Installed < intMajor Then
    begin
      {Debug
       msgbox(IntToStr(intMajor_installed) + ' < ' +  IntToStr(intMajor), mbInformation, MB_OK)
       }

      Result := False;

      {Stops analyzing the version number since we already know it's inadequate and returns False (inadequate)}
      exit;
    end;


  {Starts detecting the Minor version of the Version Installed}

  {Modifies strVersion_Installed removing the first period and everything prior to it (Removes the Major Version)}
  strVersion_Installed := Copy(strVersion_Installed, intEnd_Pos + 2, Length(strVersion_Installed));

  {Sets the end position equal to one character before the first period in the version number}
  intEnd_Pos := Pos('.', strVersion_Installed) -1

  {Sets the Minor version equal to all character before the first period }
	intMinor_installed := StrToIntDef(RemoveLeadingZeros(Copy(strVersion_Installed, 1, intEnd_Pos)), 0)

	{Debug
  msgbox('The Minor version installed is ' + IntToStr(intMinor_installed) + ' and the required Minor is ' + IntToStr(intMinor) + '.', mbInformation, MB_OK)
        }

	{If the Minor Version Installed is greater than the required value then...}
	If intMinor_Installed > intMinor Then
    begin
      {Returns that the version number was adequate}
      Result := True;

      {Stops further analyzation of the version number}
      exit
    end;

  {If the minor installed is less than what was required}
  If intMinor_Installed < intMinor Then
    Begin
      Result := False;

      {Returns that the version installed did not meet the required value and stops analyzing the version number}
      exit;
    end;


  {Starts Detecting the Revision of the Version Installed}

	{Modifies strVersion_Installed removing the first period and everything prior to it (Removes the Minor Version)}
	strVersion_Installed := Copy(strVersion_Installed, intEnd_Pos + 2, Length(strVersion_Installed));

	{Sets the last character of the Revision to last character before the first period}
	intEnd_Pos := Pos('.', strVersion_Installed) -1

	{Sets the Revision Installed equal to everything before the first period}
	intRevision_Installed := strToIntDef(RemoveLeadingZeros(Copy(strVersion_Installed, 1, intEnd_Pos)), 0);

	{Debug
  msgbox('The Revision version installed is ' + IntToStr(intRevision_installed) + ' and the required Revision is ' + IntToStr(intRevision) + '.', mbInformation, MB_OK)
        }

	{If the Revision Installed is greater than the required value then...}
  If intRevision_Installed > intRevision Then
    begin
      {Return that the version installed was adequate}
      Result := True;

      {Stops further analyzation of the version number}
      exit
    end;

  {If the revision installed did not meet the requirement then...}
  If intRevision_Installed < intRevision Then
    begin
      Result := False;

    {Return that the version number failed to meet the requirement and stops further analyzation of the version number}
      exit;
    end;

  {Start Detection the Build Installed}

	{Modifies strVersion_Installed removing the first period and everything prior to it (Removes the Revision) leaving behind only the build number}
	strVersion_Installed := Copy(strVersion_Installed, intEnd_Pos + 2, Length(strVersion_Installed));

	{Set the build installed = to what's left of the strVersion_Installed text}
	intBuild_installed := strToIntDef(RemoveLeadingZeros(strVersion_Installed), 0);

	{Debug
  msgbox('The Build version installed is ' + IntToStr(intBuild_installed) + ' and the required Build is ' + IntToStr(intBuild) + '.', mbInformation, MB_OK)
        }

	{If the build installed is greater than or equal to the requirement then...}
	if intBuild_installed >= intBuild Then

	 {Report that the version installed was adequate}
    Result := True
  else

    {Reports the inadequacy of the version installed and seases processing }
    Result := False;
end;

{Returns whether or not the version of particular file is at least equal to requested value}
Function FileVersionAtLeast(strFile_Path: string; intMajor: integer; intMinor: integer; intRevision: integer; intBuild: integer): boolean;
  Var
  {Stores the version of the file to be compared}
  strVersion: string;
Begin
  {Gets the version number of the specified file}
  GetVersionNumbersString(strFile_Path, strVersion)

  {File Version at least is the result of the VersionAtLeast Determination}
  Result := VersionAtLeast(strVersion, intMajor, intMinor, intRevision, intBuild);


  {Debug: If you'd like to debug with a messagebox then un rem this
	If Result = True then

	 Msgbox('The version of ' + strFile_path + ' required is "' + IntToStr(intMajor) + '.' + IntToStr(intMinor) + '.' + IntToStr(intRevision) + '.' + IntToStr(intBuild) + '". The version found was "'  + strVersion + '.  The version detected met the required value.', mbInformation, MB_OK)
	else
	 Msgbox('The version of ' + strFile_path + ' required is "' + IntToStr(intMajor) + '.' + IntToStr(intMinor) + '.' + IntToStr(intRevision) + '.' + IntToStr(intBuild) + '". The version found was "' + strVersion + '.  The version detected did not meet the required value.', mbInformation, MB_OK);
    }
end;

{Determines whether or not TortoiseCVS is installed}
Function TortoiseCVSInstalled(): boolean;
Begin
	{This absolutely must remain as \CustomIcons, because our application used to create some TortoiseCVS keys even if the application wasn't installed!}
    Result := RegKeyExists(HKEY_LOCAL_MACHINE, 'SOFTWARE\TortoiseCVS\CustomIcons') or RegKeyExists(HKEY_LOCAL_MACHINE, 'SOFTWARE\TortoiseCVS\Icons');
End;

{Determines whether or not TortoiseSVN is installed}
Function TortoiseSVNInstalled(): boolean;
Begin
    Result := RegKeyExists(HKEY_LOCAL_MACHINE, 'SOFTWARE\TortoiseSVN') or RegKeyExists(HKEY_LOCAL_MACHINE, 'SOFTWARE\Classes\Directory\Background\shellex\ContextMenuHandlers\TortoiseSVN');
End;

Function OldGroup(): string;
Begin
    {Stores where in \All Users\Programs\ our start menu used to be located}
     RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\WinMerge_is1', 'Inno Setup: Icon Group', Result)
End;

Procedure DeletePreviousStartMenu();
Var
        strOld: string;
        strNew: string;
        strMessage: string;
        strShortcut: string;
Begin
    {Detects the previous start menu group's path, if any}
    strOld := OldGroup();

    {Detects the current start menu group's path, if any (not creating a group is a valid option)}
    strNew := ExpandConstant('{group}');

    {removes the start menu portion of the path from the group path making it match the format of strOld}
    StringChange(strNew, ExpandConstant('{commonprograms}\'), '')

    {if the user does have a previous start menu location then..}
    If strOld <> '' Then
        Begin
            {If the current and previous start menu locations are different then...}
            If Uppercase(strOld) <> UpperCase(strNew) Then
                Begin
                    strMessage := ExpandConstant('{cm:DeletePreviousStartMenu}');
                    strMessage := Format(strMessage, [strOld, strNew]);

                    {Display a dialog asking the user if they'd like to delete the previous start menu group}
                    {If they'd like to delete the previous start menu group then...}
                    If Msgbox(strMessage, mbConfirmation, mb_YesNo) = mrYes Then
                        Begin
                            strOld := ExpandConstant('{commonprograms}\') + strOld;
                            {Remove old start menu}
                            DelTree(strOld, True, True, True);
                        End;
                End;
        End;
End;

{This event procedure is queed each time the user changes pages within the installer}
Procedure CurPageChanged(CurPage: integer);
Begin
    {if the installer reaches the file copy page then...}
    If CurPage = wpInstalling Then
            {Delete the previous start menu group if the location has changed since the last install}
            DeletePreviousStartMenu;
End;

// Checks if context menu is already enabled for shell extension
// If so, we won't overwrite its existing value in [Registry] section
Function ShellMenuEnabled(): Boolean;
Begin
  If RegValueExists(HKCU, 'Software\Thingamahoochie\WinMerge', 'ContextMenuEnabled') Then
    result := True
  Else
    result := False;
End;

{Replace one occurrence of OldStr in Str with NewStr}
Function ReplaceSubString(Src: string; OldStr: string; NewStr: string) : string;
Var
    OldStrStartAt: Integer;

Begin
    OldStrStartAt := Pos(OldStr, Src);
    if OldStrStartAt > 0 then
    begin
        {Remove old string}
        Delete(Src, OldStrStartAt, Length(OldStr));
        {Insert new string}
        Insert(NewStr, Src, OldStrStartAt);
    end;
    Result := Src;
End;

{Returns WinMerge installed exeutable file name}
Function WinMergeExeName(): string;
Var
	Unused: String;

Begin
    Result := ExpandConstant('{app}\') + ExeName(Unused);
End;

{Returns ClearCase external tools configuration file name}
Function ClearCaseMapFile(): string;
Begin
    if not RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Rational Software\', 'RSINSTALLDIR', Result) then
    begin
        Result := {pf} + '\Rational\';
    end;
    Result := Result + 'ClearCase\lib\mgrs\map';
End;

{Determines whether or not Rational ClearCase is installed}
Function ClearCaseInstalled(): boolean;
Begin
    Result := FileExists(ClearCaseMapFile());
End;

{Intergrate WinMerge as ClearCase external diff tool}
Procedure IntegrateClearCase(OldExe: String; NewExe: String);
Var
    MapFile: TStringList;
    FileName: String;
    I: Integer;
    BakFile: String;

Begin
    FileName := ClearCaseMapFile();
    {Create a backup file of the original file first time it gets modified}
    BakFile := FileName + '.original';
    if not FileExists(BakFile) then
    begin
        FileCopy(FileName, BakFile, True);
    end;
    MapFile := TStringList.Create();
    {Read the entire map file to a string list}
    MapFile.LoadFromFile(FileName);
    if MapFile.Count > 0 then
    begin
        for I := 0 to MapFile.Count do
        begin
            {Search for the 'text_file_delta xcompare ...' line}
			if (MapFile.Strings[I][1] <> ';') and (Pos('text_file_delta', MapFile.Strings[I]) > 0) and (Pos('xcompare', MapFile.Strings[I]) > 0) then
			begin
				{Replace old executable name with a new executable name}
				MapFile.Strings[I] := ReplaceSubString(MapFile.Strings[I], OldExe, NewExe);
			    break;
			end;
		end;
		{ Save the modified file. }
		MapFile.SaveToFile(FileName);
	end;
End;

// Add WinMerge to system path.
// This requires certain order of things to work:
// #1 ModPathDir function must be first (it gets called by others)
// #2 include of modpath.iss so modpath code gets included
// #3 CurStepChanged and CurUninstallStepChanged procedures as they call
//    ModPath (in modpath.iss)
function ModPathDir(): TArrayOfString;
var
    Dir:	TArrayOfString;
begin
    setArrayLength(Dir, 1)
	Dir[0] := ExpandConstant('{app}');
	Result := Dir;
end;

#include "modpath.iss"

procedure CurStepChanged(CurStep: TSetupStep);
Begin
    if CurStep = ssPostInstall then
    begin
		if IsTaskSelected('modifypath') then
			ModPath();
        if IsTaskSelected('ClearCase') then
        begin
            IntegrateClearCase('..\..\bin\cleardiffmrg.exe', WinMergeExeName());
        end;
    end;
End;

Procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
var
	appdir:			String;
	selectedTasks:	String;
Begin
	appdir := ExpandConstant('{app}')
	if CurUninstallStep = usUninstall then begin
		if LoadStringFromFile(appdir + '\uninsTasks.txt', selectedTasks) then
			if Pos('modifypath', selectedTasks) > 0 then
				ModPath();
		DeleteFile(appdir + '\uninsTasks.txt')
	end;

    if CurUninstallStep = usPostUninstall then
    begin
      if ClearCaseInstalled() then
        IntegrateClearCase(WinMergeExeName(), '..\..\bin\cleardiffmrg.exe');
    end;
End;
