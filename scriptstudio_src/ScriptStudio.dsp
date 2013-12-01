# Microsoft Developer Studio Project File - Name="ScriptStudio" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ScriptStudio - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ScriptStudio.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ScriptStudio.mak" CFG="ScriptStudio - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ScriptStudio - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ScriptStudio - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ScriptStudio - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "C:\Temp"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W4 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "STRICT" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x406 /d "NDEBUG"
# ADD RSC /l 0x406 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "ScriptStudio - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "C:\Temp"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "STRICT" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x406 /d "_DEBUG"
# ADD RSC /l 0x406 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "ScriptStudio - Win32 Release"
# Name "ScriptStudio - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Views"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\ExplorerView.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptView.cpp
# End Source File
# End Group
# Begin Group "Blocks"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Blocks\BlockDelay.cpp
# End Source File
# Begin Source File

SOURCE=.\Blocks\BlockFileClose.cpp
# End Source File
# Begin Source File

SOURCE=.\Blocks\BlockFileOpen.cpp
# End Source File
# Begin Source File

SOURCE=.\Blocks\BlockFileRead.cpp
# End Source File
# Begin Source File

SOURCE=.\Blocks\BlockFileWrite.cpp
# End Source File
# Begin Source File

SOURCE=.\Blocks\BlockInput.cpp
# End Source File
# Begin Source File

SOURCE=.\Blocks\BlockOutput.cpp
# End Source File
# Begin Source File

SOURCE=.\Blocks\BlockRoot.cpp
# End Source File
# Begin Source File

SOURCE=.\Blocks\BlockStart.cpp
# End Source File
# Begin Source File

SOURCE=.\Blocks\BlockStop.cpp
# End Source File
# Begin Source File

SOURCE=.\Blocks\BlockUserBlock.cpp
# End Source File
# Begin Source File

SOURCE=.\Blocks\BlockVariableChoose.cpp
# End Source File
# Begin Source File

SOURCE=.\Blocks\BlockVariableCompare.cpp
# End Source File
# Begin Source File

SOURCE=.\Blocks\BlockVariableCounter.cpp
# End Source File
# Begin Source File

SOURCE=.\Blocks\BlockVariableSet.cpp
# End Source File
# Begin Source File

SOURCE=.\Blocks\BlockVariableSplit.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\mainfrm.cpp
# End Source File
# Begin Source File

SOURCE=.\RoutePlanner.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptBlock.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptStudio.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptStudio.rc
# End Source File
# Begin Source File

SOURCE=.\Serializer.cpp
# End Source File
# Begin Source File

SOURCE=.\stdafx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "View Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\ExplorerView.h
# End Source File
# Begin Source File

SOURCE=.\ScriptView.h
# End Source File
# End Group
# Begin Group "Block Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Blocks\BlockDelay.h
# End Source File
# Begin Source File

SOURCE=.\Blocks\BlockFileClose.h
# End Source File
# Begin Source File

SOURCE=.\Blocks\BlockFileOpen.h
# End Source File
# Begin Source File

SOURCE=.\Blocks\BlockFileRead.h
# End Source File
# Begin Source File

SOURCE=.\Blocks\BlockFileWrite.h
# End Source File
# Begin Source File

SOURCE=.\Blocks\BlockInput.h
# End Source File
# Begin Source File

SOURCE=.\Blocks\BlockOutput.h
# End Source File
# Begin Source File

SOURCE=.\Blocks\BlockRoot.h
# End Source File
# Begin Source File

SOURCE=.\Blocks\BlockStart.h
# End Source File
# Begin Source File

SOURCE=.\Blocks\BlockStop.h
# End Source File
# Begin Source File

SOURCE=.\Blocks\BlockUserBlock.h
# End Source File
# Begin Source File

SOURCE=.\Blocks\BlockVariableChoose.h
# End Source File
# Begin Source File

SOURCE=.\Blocks\BlockVariableCompare.h
# End Source File
# Begin Source File

SOURCE=.\Blocks\BlockVariableCounter.h
# End Source File
# Begin Source File

SOURCE=.\Blocks\BlockVariableSet.h
# End Source File
# Begin Source File

SOURCE=.\Blocks\BlockVariableSplit.h
# End Source File
# End Group
# Begin Group "Dialog Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AboutDlg.h
# End Source File
# Begin Source File

SOURCE=.\RenameDlg.h
# End Source File
# End Group
# Begin Group "Misc Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\atlctrlsext.h
# End Source File
# Begin Source File

SOURCE=.\atlgdix.h
# End Source File
# Begin Source File

SOURCE=.\atlwinmisc.h
# End Source File
# Begin Source File

SOURCE=.\ToolCmdBar.h
# End Source File
# Begin Source File

SOURCE=.\WindowAnim.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\RoutePlanner.h
# End Source File
# Begin Source File

SOURCE=.\ScriptBlock.h
# End Source File
# Begin Source File

SOURCE=.\ScriptDoc.h
# End Source File
# Begin Source File

SOURCE=.\Serializer.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\blocks.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cur00001.cur
# End Source File
# Begin Source File

SOURCE=.\res\cur00002.cur
# End Source File
# Begin Source File

SOURCE=.\res\idi_drag.cur
# End Source File
# Begin Source File

SOURCE=.\res\idi_hand.cur
# End Source File
# Begin Source File

SOURCE=.\res\idi_nowa.cur
# End Source File
# Begin Source File

SOURCE=.\res\idi_plac.cur
# End Source File
# Begin Source File

SOURCE=.\res\insert.bmp
# End Source File
# Begin Source File

SOURCE=.\res\navigate.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ScriptStudio.exe.manifest
# End Source File
# Begin Source File

SOURCE=.\res\ScriptStudio.ico
# End Source File
# Begin Source File

SOURCE=.\res\ScriptStudiodoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\toolbar.bmp
# End Source File
# End Group
# End Target
# End Project

