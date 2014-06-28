; *** Inno Setup version 5.1.11+ Turkish messages ***
; Language     " T�k�e"               ::::::    Turkish
; Translate by " �eviren "            ::::::    Adil YILDIZ - Mehmet F. YUCE
; E-Mail       " Elektronik Posta "   ::::::    adil@kde.org.tr - mfyuce@yelkenbilgisayar.com
; Home Page    " Web Adresi "         ::::::    http://www.yildizyazilim.gen.tr - http://www.yelkenbilgisayar.com
;
; $jrsoftware: issrc/Files/Default.isl,v 1.66 2005/02/25 20:23:48 mlaan Exp $
[LangOptions]
LanguageName=T<00FC>rk<00E7>e
LanguageID=$041f
LanguageCodePage=1254
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
DialogFontName=
[Messages]

; *** Application titles
SetupAppTitle=Kur
SetupWindowTitle=%1 - Kur
UninstallAppTitle=Kald�r
UninstallAppFullTitle=%1 Kald�r

; *** Misc. common
InformationTitle=Bilgi
ConfirmTitle=Sorgu
ErrorTitle=Hata

; *** SetupLdr messages
SetupLdrStartupMessage=Bu kurulumda %1 program� y�klenecektir. Devam etmek istiyor musunuz?
LdrCannotCreateTemp=Ge�ici bir dosya olu�turulamad�. Kurulum iptal edildi
LdrCannotExecTemp=Ge�ici klas�rdeki dosya �al��t�r�lamad�. Kurulum iptal edildi

; *** Startup error messages
LastErrorMessage=%1.%n%nHata %2: %3
SetupFileMissing=%1 adl� dosya kurulum klas�r�nde bulunamad�. L�tfen sorunu d�zeltin veya program�n yeni bir kopyas�n� edinin.
SetupFileCorrupt=Kurulum dosyalar� bozulmu�. L�tfen program�n yeni bir kopyas�n� edinin.
SetupFileCorruptOrWrongVer=Kurulum dosyalar� bozulmu� veya kurulumun bu s�r�m� ile uyu�muyor olabilir. L�tfen sorunu d�zeltin veya program�n yeni bir kopyas�n� edinin.
NotOnThisPlatform=Bu program %1 �zerinde �al��t�r�lamaz.
OnlyOnThisPlatform=Bu program sadece %1 �zerinde �al��t�r�lmal�d�r.
OnlyOnTheseArchitectures=Bu program sadece a�a��daki mimarilere sahip Windows s�r�mlerinde �al���r:%n%n%1
MissingWOW64APIs=Kulland���n�z Windows s�r�m� Kur'un 64-bit y�kleme yapabilmesi i�in gerekli olan �zelliklere sahip de�il. Bu sorunu ortadan kald�rmak i�in l�tfen Service Pack %1 y�kleyin.
WinVersionTooLowError=Bu program� �al��t�rabilmek i�in %1 %2 s�r�m� veya daha sonras� y�kl� olmal�d�r.
WinVersionTooHighError=Bu program %1 %2 s�r�m� veya sonras�nda �al��maz.
AdminPrivilegesRequired=Bu program kurulurken y�netici olarak oturum a�m�� olman�z gereklidir.
PowerUserPrivilegesRequired=Bu program kurulurken Y�netici veya G�� Y�neticisi Grubu �yesi olarak giri� yapm�� olman�z gereklidir.
SetupAppRunningError=Kur %1 program�n�n �al��t���n� alg�lad�.%n%nL�tfen bu program�n �al��an b�t�n par�alar�n� �imdi kapatarak, devam etmek i�in Tamam'a veya ��kmak i�in �ptal'e t�klay�n.
UninstallAppRunningError=Kald�r %1 program�n�n �al��t���n� tespit etti.%n%nL�tfen bu program�n �al��an b�t�n par�alar�n� �imdi kapatarak, devam etmek i�in Tamam'a veya ��kmak i�in �ptal'e t�klay�n.

; *** Misc. errors
ErrorCreatingDir=Kur " %1 " klas�r� olu�turamad�.
ErrorTooManyFilesInDir=" %1 " klas�r� �ok fazla dosya i�erdi�inden burada bir dosya olu�turulamad�.

; *** Setup common messages
ExitSetupTitle=Kur'dan ��k
ExitSetupMessage=Kurulum tamamlanmad�. �imdi ��karsan�z program kurulmu� olmayacak.%n%nDaha sonra Kur'u tekrar �al��t�rarak kurulumu tamamlayabilirsiniz.%n%nKur'dan ��kmak istedi�inizden emin misiniz?
AboutSetupMenuItem=Kur H&akk�nda...
AboutSetupTitle=Kur Hakk�nda
AboutSetupMessage=%1 %2 s�r�m�%n%3%n%n%1 internet:%n%4
AboutSetupNote=
TranslatorNote=�eviri: Kaya Zeren%n

; *** Buttons
ButtonBack=< G&eri
ButtonNext=�&leri >
ButtonInstall=&Kur
ButtonOK=Tamam
ButtonCancel=�ptal
ButtonYes=E&vet
ButtonYesToAll=T�m�ne E&vet
ButtonNo=&Hay�r
ButtonNoToAll=T�m�ne Ha&y�r
ButtonFinish=&Son
ButtonBrowse=&G�zat...
ButtonWizardBrowse=G�za&t...
ButtonNewFolder=Ye&ni Klas�r Olu�tur

; *** "Select Language" dialog messages
SelectLanguageTitle=Kur Dilini Se�in
SelectLanguageLabel=L�tfen kurulum s�ras�nda kullanaca��n�z dili se�in:

; *** Common wizard text
ClickNext=Devam etmek i�in �leri'ye , ��kmak i�in �ptal 'e t�klay�n.
BeveledLabel=Inno Setup 5.1+ T�rk�e
BrowseDialogTitle=Klas�re G�zat
BrowseDialogLabel=A�a��daki listeden bir dizin se�ip, daha sonra Tamam'a t�klay�n.
NewFolderName=Yeni Klas�r

; *** "Welcome" wizard page
WelcomeLabel1=[name] Kurulum Sihirbaz�na Ho�geldiniz.
WelcomeLabel2=Kur �imdi [name/ver] program�n� bilgisayar�n�za y�kleyecek.%n%nDevam etmeden �nce �al��an di�er b�t�n programlar� kapatman�z �nerilir.

; *** "Password" wizard page
WizardPassword=Parola
PasswordLabel1=Bu kurulum parola korumal�d�r.
PasswordLabel3=L�tfen parolay� yaz�n. Daha sonra devam etmek i�in �leri'ye t�klay�n. L�tfen parolay� yazarken B�y�k-K���k harflere dikkat edin.
PasswordEditLabel=&Parola:
IncorrectPassword=Yazd���n�z parola hatal�. L�tfen tekrar deneyin.

; *** "License Agreement" wizard page
WizardLicense=Lisans Anla�mas�
LicenseLabel=Devam etmeden �nce a�a��daki �nemli bilgileri okuyun.
LicenseLabel3=L�tfen a�a��daki lisans anla�mas�n� okuyun. Kuruluma devam edebilmek i�in bu anla�man�n ko�ullar�n� kabul etmelisiniz.
LicenseAccepted=Anla�may� Kabul &Ediyorum.
LicenseNotAccepted=Anla�may� Kabul Et&miyorum.

; *** "Information" wizard pages
WizardInfoBefore=Bilgi
InfoBeforeLabel=Devam etmeden �nce a�a��daki �nemli bilgileri okuyun.
InfoBeforeClickLabel=Kur ile devam etmeye haz�r oldu�unuzda �leri'ye t�klay�n.
WizardInfoAfter=Bilgi
InfoAfterLabel=L�tfen devam etmeden �nce a�a��daki �nemli bilgileri okuyun.
InfoAfterClickLabel=Kurma i�lemine devam etmeye haz�r oldu�unuzda �leri'ye t�klay�n.

; *** "User Information" wizard page
WizardUserInfo=Kullan�c� Bilgileri
UserInfoDesc=L�tfen bilgilerinizi yaz�n.
UserInfoName=K&ullan�c� Ad�:
UserInfoOrg=Ku&rum:
UserInfoSerial=Seri Numaras�:
UserInfoNameRequired=Bir ad yazmal�s�n�z.

; *** "Select Destination Directory" wizard page
WizardSelectDir=Kurulacak klas�r� se�iniz
SelectDirDesc=[name] hangi klas�re kurulsun?
SelectDirLabel3=Kur [name] program�n� a�a��daki klas�re kuracakt�r.
SelectDirBrowseLabel=Devam etmek i�in �leri'ye t�klay�n. Ba�ka bir klas�r se�mek istiyorsan�z, G�zat'a t�klay�n.
DiskSpaceMBLabel=Bu program en az [mb] MB disk alan� gerektirmektedir.
ToUNCPathname=Kur, UNC tipindeki klas�r yollar�na (�rn:\\yol vb.) kurulum yapamaz. E�er a� �zerine kurulum yapmaya �al���yorsan�z. Bir a� s�r�c�s� tan�tman�z gerekir.
InvalidPath=S�r�c� ad� ile birlikte tam yolu yazmal�s�n�z; �rne�in %nC:\APP%n%n veya bir UNC yolunu %n%n\\sunucu\payla��m%n%n �eklinde yazmal�s�n�z.
InvalidDrive=Se�ti�iniz s�r�c� bulunamad� veya ula��lam�yor. L�tfen ba�ka bir s�r�c� se�in.
DiskSpaceWarningTitle=Yetersiz Disk Alan�
DiskSpaceWarning=Kur i�in en az %1 KB kullan�labilir disk alan� gereklidir. Ancak se�ili diskte %2 KB bo� alan var.%n%nYine de devam etmek istiyor musunuz?
DirNameTooLong=Klas�r ad� veya yolu �ok uzun.
InvalidDirName=Klas�r ad� ge�ersiz.
BadDirName32=Klas�r ad� �u karakterlerden her hangi birini i�eremez:%n%n%1
DirExistsTitle=Klas�r Bulundu
DirExists=%n%n%1%n%n klas�r� zaten var. Yine de bu klas�re kurmak istedi�inizden emin misiniz?
DirDoesntExistTitle=Klas�r Bulunamad�
DirDoesntExist=%n%n%1%n%n klas�r� bulunamad�. Bu klas�r� olu�turmak ister misiniz?

; *** "Select Components" wizard page
WizardSelectComponents=Bile�enleri Se�in
SelectComponentsDesc=Hangi bile�enler kurulsun?
SelectComponentsLabel2=Kurmak istedi�iniz bile�enleri se�in; istemediklerinizi b�rak�n.Devam etmeye haz�r oldu�unuz zaman �leri'ye t�klay�n.
FullInstallation=Tam Kurulum
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=Normal Kurulum
CustomInstallation=�zel Kurulum
NoUninstallWarningTitle=Kurulu Bile�enler
NoUninstallWarning=Kur a�a��daki bile�enlerin kurulu oldu�unu alg�lad�:%n%n%1%n%nBu bile�enleri se�meyerek kald�rm�� olmayacaks�n�z.%n%nYine de devam etmek istiyor musunuz?
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=Se�ili bile�enler i�in en az [mb] MB disk alan� gereklidir.

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=Ek G�revleri Se�in
SelectTasksDesc=Hangi g�revler yerine getirilsin?
SelectTasksLabel2=[name] kurulurken istedi�iniz ek g�revleri se�ip �leri'ye t�klay�n.

; *** "Ba�lat Men�s� Dizini Se�" sihirbaz sayfas�
WizardSelectProgramGroup=Ba�lat Men�s� Klas�r�n� Se�in
SelectStartMenuFolderDesc=Kur program k�sayollar�n� nereye yerle�tirsin?
SelectStartMenuFolderLabel3=Kur program�n k�sayollar�n� a�a��daki Ba�lat Men�s� klas�r�ne kuracak.
SelectStartMenuFolderBrowseLabel=Devam etmek i�in, �leri'ye t�klay�n. Ba�ka bir klas�r se�mek istiyorsan�z, G�zat'a t�klay�n.
MustEnterGroupName=Bir klas�r ad� yazmal�s�n�z.
GroupNameTooLong=Klas�r ad� veya yolu �ok uzun.
InvalidGroupName=Klas�r ad� ge�ersiz.
BadGroupName=Klas�r ad�, �u karakterlerden her hangi birini i�eremez:%n%n%1
NoProgramGroupCheck2=&Ba�lat men�s�nde k�sayol olu�turma

; *** "Ready to Install" wizard page
WizardReady=Y�kleme i�in haz�r
ReadyLabel1=Kur [name] program�n� bilgisayar�n�za kurmak i�in haz�r.
ReadyLabel2a=Kuruluma devam etmek i�in Kur'a, ayarlar�n�z� kontrol etmek veya de�i�tirmek i�in Geri'ye t�klay�n.
ReadyLabel2b=Kuruluma devam etmek i�in Kur'a t�klay�n.
ReadyMemoUserInfo=Kullan�c� Bilgisi:
ReadyMemoDir=Hedef Klas�r:
ReadyMemoType=Kurulum Tipi:
ReadyMemoComponents=Se�ili Bile�enler:
ReadyMemoGroup=Ba�lat Men�s� :
ReadyMemoTasks=Ek G�revler:

; *** "Kur Haz�rlan�yor" sihirbaz sayfas�
WizardPreparing=Kurulum Haz�rlan�yor
PreparingDesc=Kur [name] program�n� bilgisayar�n�za kurmak i�in haz�rlan�yor.
PreviousInstallNotCompleted=Bir �nceki Kur/Kald�r program�na ait i�lem tamamlanmam��.�nceki kurulum i�leminin tamamlanmas� i�in bilgisayar�n�z� yeniden ba�latmal�s�n�z.%n%nBilgisayar�n�z tekrar ba�lad�ktan sonra, Kur'u tekrar �al��t�rarak [name] program�n� kurma i�lemine devam edebilirsiniz.
CannotContinue=Kur devam edemiyor. L�tfen �ptal'e t�klay�p ��k�n.

; *** "Kuruluyor" sihirbaz
WizardInstalling=Kuruluyor
InstallingLabel=L�tfen [name] bilgisayar�n�za kurulurken bekleyin.

; *** "Setup Completed" wizard page
FinishedHeadingLabel=[name] Kur Sihirbaz� tamamlan�yor
FinishedLabelNoIcons=Kur [name] program�n� bilgisayar�n�za kurma i�lemini tamamlad�.
FinishedLabel=Kur [name] program�n� bilgisayar�n�za kurma i�lemini tamamlad�. Program y�klenen k�sayol simgelerine t�klanarak �al��t�r�labilir.
ClickFinish=Kur'dan ��kmak i�in Son'a t�klay�n.
FinishedRestartLabel=[name] program�n�n kurulumunu bitirmek i�in, Kur bilgisayar�n�z� yeniden ba�latacak. Bilgisayar�n�z �imdi yeniden ba�lat�ls�n m�?
FinishedRestartMessage=[name] kurulumunu bitirmek i�in, bilgisayar�n�z�n yeniden ba�lat�lmas� gerekiyor. %n%nBilgisayar�n�z yeniden ba�lat�ls�n m�?
ShowReadmeCheck=Beni Oku dosyas�n� okumak istiyorum.
YesRadio=&Evet, bilgisayar yeniden ba�lat�ls�n.
NoRadio=&Hay�r, daha sonra yeniden ba�lataca��m.
; used for example as 'Run MyProg.exe'
RunEntryExec=%1 uygulamas�n� �al��t�r
; used for example as 'View Readme.txt'
RunEntryShellExec=%1 dosyas�n� g�r�nt�le

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=Bir sonraki disketi tak�n.
SelectDiskLabel2=%1 numaral� disketi tak�p, Tamam'� t�klay�n.%n%nE�er dosyalar ba�ka bir yerde bulunuyor ise do�ru yolu yaz�n veya G�zat'� t�klay�n.
PathLabel=&Yol:
FileNotInDir2=" %1 " adl� dosya " %2 " dizininde bulunamad�. L�tfen do�ru diski veya dosyay� se�in.
SelectDirectoryLabel=L�tfen sonraki disketin yerini belirtin.

; *** Installation phase messages
SetupAborted=Kurulum tamamlanamad�.%n%nL�tfen sorunu d�zeltin veya Kur'u tekrar �al��t�r�n.
EntryAbortRetryIgnore=Tekrar denemek i�in "Tekrar Dene" ye, devam etmek i�in Yoksay'a, kurulumu iptal etmek i�in �ptal'e t�klay�n.

; *** Installation status messages
StatusCreateDirs=Klas�rler olu�turuluyor...
StatusExtractFiles=Dosyalar ��kart�l�yor...
StatusCreateIcons=Program k�sayollar� olu�turuluyor...
StatusCreateIniEntries=INI kay�tlar� olu�turuluyor...
StatusCreateRegistryEntries=Kay�t Defteri kay�tlar� olu�turuluyor...
StatusRegisterFiles=Dosyalar sisteme kaydediliyor...
StatusSavingUninstall=Kald�rma bilgileri kaydediliyor...
StatusRunProgram=Kurulum sonland�r�l�yor...
StatusRollback=De�i�iklikler geri al�n�yor...

; *** Misc. errors
ErrorInternal2=�� hata: %1
ErrorFunctionFailedNoCode=%1 ba�ar�s�z oldu.
ErrorFunctionFailed=%1 ba�ar�s�z oldu; kod  %2
ErrorFunctionFailedWithMessage=%1 ba�ar�s�z oldu ; kod  %2.%n%3
ErrorExecutingProgram=%1 adl� dosya �al��t�r�lamad�.

; *** Registry errors
ErrorRegOpenKey=A�a��daki Kay�t Defteri anahtar� a��l�rken hata olu�tu:%n%1\%2
ErrorRegCreateKey=A�a��daki Kay�t Defteri anahtar� olu�turulurken hata olu�tu:%n%1\%2
ErrorRegWriteKey=A�a��daki Kay�t Defteri anahtar�na yaz�l�rken hata olu�tu:%n%1\%2

; *** INI errors
ErrorIniEntry=" %1 " adl� dosyada INI kayd� yazma hatas�.

; *** File copying errors
FileAbortRetryIgnore=Yeniden denemek i�in "Yeniden Dene" ye, dosyay� atlamak i�in Yoksay'a (�nerilmez), Kurulumu iptal etmek i�in �ptal'e t�klay�n.
FileAbortRetryIgnore2=Yeniden denemek i�in "Yeniden Dene" ye, devam etmek i�in Yoksay'a (�nerilmez), Kurulumu �ptal etmek i�in �ptal'e t�klay�n.
SourceIsCorrupted=Kaynak Dosya Bozulmu�
SourceDoesntExist=%1 adl� kaynak dosya bulunamad�.
ExistingFileReadOnly=Dosya salt okunur.%n%nSalt okunur �zelli�ini kald�r�p yeniden denemek i�in Yeniden Dene'yi, dosyay� atlamak i�in Yoksay'�, kurulumu iptal etmek i�in �ptal'i t�klay�n.
ErrorReadingExistingDest=Dosyay� okurken bir hata olu�tu :
FileExists=Dosya zaten var.%n%nKurulum'un �zerine yazmas�n� ister misiniz?
ExistingFileNewer=Kurulum'un y�klemek istedi�i dosyadan daha yeni bir dosya var. Bu dosyay� koruman�z �nerilir.%n%nVar olan dosya korunsun mu?
ErrorChangingAttr=Var olan dosyan�n �zelli�i de�i�tirilirken bir hata olu�tu:
ErrorCreatingTemp=Hedef klas�rde dosya olu�turulurken bir hata olu�tu:
ErrorReadingSource=Kaynak dosya okunurken bir hata olu�tu:
ErrorCopying=Bir dosya kopyalan�rken bir hata olu�tu:
ErrorReplacingExistingFile=Var olan dosya de�i�tirilirken bir hata olu�tu:
ErrorRestartReplace=Yeniden ba�lat de�i�tir ba�ar�s�z oldu:
ErrorRenamingTemp=Hedef klas�rde bulunan dosyan�n ad� de�i�tirilirken hata oldu:
ErrorRegisterServer=%1 adl� DLL/OCX sisteme tan�t�lamad�.
ErrorRegSvr32Failed=RegSvr32 ��k�� hatas� %1 ile ba�ar�s�z oldu
ErrorRegisterTypeLib=%1 adl� tip k�t�phanesi sisteme tan�t�lamad�

; *** Post-installation errors
ErrorOpeningReadme=Beni Oku dosyas� a��l�rken hata olu�tu.
ErrorRestartingComputer=Kurulum bilgisayar� yeniden ba�latamad�. L�tfen kendiniz yeniden ba�lat�n.

; *** Uninstaller messages
UninstallNotFound=%1 adl� dosya bulunamad�. Kald�rma program� �al��t�r�lamad�.
UninstallOpenError="%1" dosyas� a��lam�yor. Kald�rma program� �al��t�r�lamad�.
UninstallUnsupportedVer=%1 adl� Kald�r bilgi dosyas� kald�rma program�n�n bu s�r�m� ile uyu�muyor. Kald�rma program� �al��t�r�lamad�.
UninstallUnknownEntry=Kald�r Bilgi dosyas�ndaki %1 adl� sat�r anla��lamad�
ConfirmUninstall=%1 ve bile�enlerini kald�rmak istedi�inizden emin misiniz?
UninstallOnlyOnWin64=Bu kurulum sadece 64-bit Windows �zerinden kald�r�labilir.
OnlyAdminCanUninstall=Bu kurulum sadece y�netici yetkisine sahip kullan�c�lar taraf�ndan kald�rabilir.
UninstallStatusLabel=L�tfen %1 program� bilgisayar�n�zdan kald�r�l�rken bekleyin...
UninstalledAll=%1 program� bilgisayar�n�zdan tamamen kald�r�ld�.
UninstalledMost=%1 program�n�n kald�r�lma i�lemi sona erdi.%n%nBaz� bile�enler kald�r�lamad�. Bu dosyalar� kendiniz silebilirsiniz.
UninstalledAndNeedsRestart=%1 program�n�n kald�r�lmas� tamamland�, Bilgisayar�n�z� yeniden ba�latmal�s�n�z.%n%n�imdi yeniden ba�lat�ls�n m�?
UninstallDataCorrupted="%1" adl� dosya bozuk. . Kald�rma program� �al��t�r�lamad�.

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=Payla��lan dosya kald�r�ls�n m�?
ConfirmDeleteSharedFile2=Sistemde payla��lan baz� dosyalar�n art�k hi�bir program taraf�ndan kullan�lmad���n� belirtiyor. Kald�r bu payla��lan dosyalar� silsin mi?%n%n Bu dosya baz� programlar tafar�ndan kullan�l�yorsa ve silinmesini isterseniz, s�z konusu programalar d�zg�n �al��mayabilir. Emin de�ilseniz, Hay�r'a t�klay�n. Bu dosyalar�n sisteminizde durmas� hi�bir zarar vermez.
SharedFileNameLabel=Dosya Ad�:
SharedFileLocationLabel=Yol:
WizardUninstalling=Kald�rma Durumu
StatusUninstalling=%1 Kald�r�l�yor...

; The custom messages below aren't used by Setup itself, but if you make
; use of them in your scripts, you'll want to translate them.

[CustomMessages]
;Inno Setup Built-in Custom Messages
NameAndVersion=%1 %2 s�r�m�
AdditionalIcons=Ek simgeler:
OptionalFeatures=�ste�e Ba�l� �zellikler:
CreateDesktopIcon=Masa�st� simg&esi olu�tur
CreateQuickLaunchIcon=H�zl� Ba�lat simgesi &olu�tur
ProgramOnTheWeb=%1 Web Sitesi
UninstallProgram=%1 program�n� kald�r
LaunchProgram=%1 program�n� �al��t�r
AssocFileExtension=%2 dosya uzant�lar�n� %1 ile ili�kilendir
AssocingFileExtension=%2 dosya uzant�lar� %1 ile ili�kilendiriliyor...

;Things we can also localize
CompanyName=Thingamahoochie Yaz�l�m

;Types
TypicalInstallation=Tipik Kurulum
FullInstallation=Tam Kurulum
CompactInstallation=K���k Kurulum
CustomInstallation=�zel Kurulum

;Components
AppCoreFiles=WinMerge �ekirdek Dosyalar�
Filters=S�zge�ler
Plugins=Eklentiler

;Localization Components
Languages=Diller
BasqueLanguage=Basque menus and dialogs
BulgarianLanguage=Bulgarca men� ve pencereler
CatalanLanguage=Katalanca men� ve pencereler
ChineseSimplifiedLanguage=�ince (Basitle�tirilmi�) men� ve pencereler
ChineseTraditionalLanguage=�ince (Geleneksel) men� ve pencereler
CroatianLanguage=H�rvat�a men� ve pencereler
CzechLanguage=�ekce men� ve pencereler
DanishLanguage=Danimarkaca men� ve pencereler
DutchLanguage=Felemenk men� ve pencereler
FrenchLanguage=Frans�zca men� ve pencereler
GalicianLanguage=Gali�ce men� ve pencereler
GermanLanguage=Almanca men� ve pencereler
GreekLanguage=Yunanca men� ve pencereler
HungarianLanguage=Macarca men� ve pencereler
ItalianLanguage=Italyanca men� ve pencereler
JapaneseLanguage=Japonca men� ve pencereler
KoreanLanguage=Korece men� ve pencereler
NorwegianLanguage=Norve�ce men� ve pencereler
PersianLanguage=Persian menus and dialogs
PolishLanguage=Leh�e men� ve pencereler
PortugueseBrazilLanguage=Portekizce (Brezilya) men� ve pencereler
PortugueseLanguage=Portekizce men� ve pencereler
RomanianLanguage=Romence men� ve pencereler
RussianLanguage=Rus�a men� ve pencereler
SerbianLanguage=Serbian menus and dialogs
SlovakLanguage=Slovak�a men� ve pencereler
SlovenianLanguage=Slovence men� ve pencereler
SpanishLanguage=�spanyolca men� ve pencereler
SwedishLanguage=�sve�ce men� ve pencereler
TurkishLanguage=T�rk�e men� ve pencereler
UkrainianLanguage=Ukraynaca men� ve pencereler

;Tasks
ExplorerContextMenu=&Windows gezgini sa� t�k men� b�t�nle�mesini etkinle�tir
IntegrateTortoiseCVS=&TortoiseCVS ile b�t�nle�
IntegrateTortoiseSVN=T&ortoiseSVN ile b�t�nle�
IntegrateClearCase=Rational &ClearCase ile b�t�nle�
AddToPath=Win&Merge klas�r�n� sistem yoluna ekle

;Icon Labels
ReadMe=Beni Oku
UsersGuide=Kullan�c� Kitab�
ViewStartMenuFolder=&WinMerge'i Ba�lang�� Men�s� Klas�r�nde G�ster

;Code Dialogs
DeletePreviousStartMenu=The installer has detected that you changed the location of your start menu from "%s" to "%s". Would you like to delete the previous start menu folder?

; Project file description
ProjectFileDesc=WinMerge Proje dosyas�
