# Microsoft Developer Studio Project File - Name="authSSP" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=authSSP - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "authSSP.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "authSSP.mak" CFG="authSSP - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "authSSP - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "authSSP - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "authSSP - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\Release"
# PROP Intermediate_Dir "..\..\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "AUTHSSP_EXPORTS" /D "_UNICODE" /D "UNICODE" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x813 /d "NDEBUG"
# ADD RSC /l 0x813 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib advapi32.lib ole32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib comctl32.lib advapi32.lib ole32.lib netapi32.lib delayimp.lib aclui.lib /nologo /subsystem:windows /dll /debug /machine:I386 /delayload:aclui.dll

!ELSEIF  "$(CFG)" == "authSSP - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\Debug"
# PROP Intermediate_Dir "..\..\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "AUTHSSP_EXPORTS" /D "_UNICODE" /D "UNICODE" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x813 /d "_DEBUG"
# ADD RSC /l 0x813 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib advapi32.lib ole32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib comctl32.lib advapi32.lib ole32.lib netapi32.lib delayimp.lib aclui.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept /delayload:aclui.dll

!ENDIF 

# Begin Target

# Name "authSSP - Win32 Release"
# Name "authSSP - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\authSSP.cpp
# End Source File
# Begin Source File

SOURCE=.\authSSP.def
# End Source File
# Begin Source File

SOURCE=.\EventLogging.cpp
# End Source File
# Begin Source File

SOURCE=.\GenClientServerContext.cpp
# End Source File
# Begin Source File

SOURCE=.\vncAccessControl.cpp
# End Source File
# Begin Source File

SOURCE=.\vncSecurityEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\vncSSP.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Auth_Seq.h
# End Source File
# Begin Source File

SOURCE=.\authSSP.h
# End Source File
# Begin Source File

SOURCE=.\EventLogging.h
# End Source File
# Begin Source File

SOURCE=.\GenClientServerContext.h
# End Source File
# Begin Source File

SOURCE=.\vncAccessControl.h
# End Source File
# Begin Source File

SOURCE=.\vncSecurityEditor.h
# End Source File
# Begin Source File

SOURCE=.\vncSecurityEditorProps.h
# End Source File
# Begin Source File

SOURCE=.\vncSSP.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\authSSP.rc
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# Begin Group "Custom"

# PROP Default_Filter "in"
# Begin Source File

SOURCE=.\buildtime.in

!IF  "$(CFG)" == "authSSP - Win32 Release"

# Begin Custom Build - Create Buildtime for VERSIONINFO
InputPath=.\buildtime.in
InputName=buildtime

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" FORCE
	cl /nologo /P /EP "$(InputPath)" && move /y "$(InputName).i" "$(InputName).h"

FORCE:

# End Custom Build

!ELSEIF  "$(CFG)" == "authSSP - Win32 Debug"

# Begin Custom Build - Create Buildtime for VERSIONINFO
InputPath=.\buildtime.in
InputName=buildtime

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" FORCE
	cl /nologo /P /EP "$(InputPath)" && move /y "$(InputName).i" "$(InputName).h"

FORCE:

# End Custom Build

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
