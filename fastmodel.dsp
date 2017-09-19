# Microsoft Developer Studio Project File - Name="fastmodel" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=fastmodel - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "fastmodel.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "fastmodel.mak" CFG="fastmodel - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "fastmodel - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "fastmodel - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "fastmodel - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\cint_fastmodel" /I ".\\" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "FM_DEMO" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o /win32 "NUL"
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o /win32 "NUL"
# ADD BASE RSC /l 0x410 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x410 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 opengl32.lib glu32.lib Htmlhelp.lib Iphlpapi.lib /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "fastmodel - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /GX /ZI /Od /I "../cint_fastmodel" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "FM_DEMO" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o /win32 "NUL"
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o /win32 "NUL"
# ADD BASE RSC /l 0x410 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x410 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 opengl32.lib glu32.lib Htmlhelp.lib Iphlpapi.lib /nologo /subsystem:windows /profile /debug /machine:I386

!ENDIF 

# Begin Target

# Name "fastmodel - Win32 Release"
# Name "fastmodel - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Import"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FastCapLexer.cpp
# End Source File
# Begin Source File

SOURCE=.\FastCapLexer.h
# End Source File
# Begin Source File

SOURCE=.\FastHenryLexer.cpp
# End Source File
# Begin Source File

SOURCE=.\FastHenryLexer.h
# End Source File
# Begin Source File

SOURCE=.\ImportFastCap.cpp
# End Source File
# Begin Source File

SOURCE=.\ImportFastCap.h
# End Source File
# Begin Source File

SOURCE=.\ImportFastHenry.cpp
# End Source File
# Begin Source File

SOURCE=.\ImportFastHenry.h
# End Source File
# Begin Source File

SOURCE=.\ImportInColorView.cpp
# End Source File
# Begin Source File

SOURCE=.\ImportInColorView.h
# End Source File
# Begin Source File

SOURCE=.\ImportPatran.cpp
# End Source File
# Begin Source File

SOURCE=.\ImportPatran.h
# End Source File
# Begin Source File

SOURCE=.\lexer.cpp
# End Source File
# Begin Source File

SOURCE=.\lexer.h
# End Source File
# Begin Source File

SOURCE=.\PatranLexer.cpp
# End Source File
# Begin Source File

SOURCE=.\PatranLexer.h
# End Source File
# End Group
# Begin Group "DockableWnd"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DockableWnd\scbarcf.cpp
# End Source File
# Begin Source File

SOURCE=.\DockableWnd\scbarcf.h
# End Source File
# Begin Source File

SOURCE=.\DockableWnd\scbarg.cpp
# End Source File
# Begin Source File

SOURCE=.\DockableWnd\scbarg.h
# End Source File
# Begin Source File

SOURCE=.\DockableWnd\sizecbar.cpp
# End Source File
# Begin Source File

SOURCE=.\DockableWnd\sizecbar.h
# End Source File
# End Group
# Begin Group "BrowserWnd"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\browserSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\browserSheet.h
# End Source File
# Begin Source File

SOURCE=.\browserWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\browserWnd.h
# End Source File
# Begin Source File

SOURCE=.\ProjectCtrlPage.cpp
# End Source File
# Begin Source File

SOURCE=.\ProjectCtrlPage.h
# End Source File
# End Group
# Begin Group "IOWindow"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\outputEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\outputEdit.h
# End Source File
# Begin Source File

SOURCE=.\outputWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\outputWnd.h
# End Source File
# End Group
# Begin Group "Geometry"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Geometry.cpp
# End Source File
# Begin Source File

SOURCE=.\Geometry.h
# End Source File
# Begin Source File

SOURCE=.\Operation.cpp
# End Source File
# Begin Source File

SOURCE=.\Operation.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\CntrItem.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorEditView.cpp
# End Source File
# Begin Source File

SOURCE=.\ControlSolvers.cpp
# End Source File
# Begin Source File

SOURCE=.\DebugHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\ErrorHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\FastCapView.cpp
# End Source File
# Begin Source File

SOURCE=.\FastHenryView.cpp
# End Source File
# Begin Source File

SOURCE=.\fastmodel.cpp
# End Source File
# Begin Source File

SOURCE=.\fastmodel.rc
# End Source File
# Begin Source File

SOURCE=.\fastmodelDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\fastmodelView.cpp
# End Source File
# Begin Source File

SOURCE=.\FileChangeEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\Global.cpp
# End Source File
# Begin Source File

SOURCE=.\IntroDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\OGLViewProps.cpp
# End Source File
# Begin Source File

SOURCE=.\OpenGlPrintView.cpp
# End Source File
# Begin Source File

SOURCE=.\OpenGLView.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\CntrItem.h
# End Source File
# Begin Source File

SOURCE=.\ColorEditView.h
# End Source File
# Begin Source File

SOURCE=.\ControlSolvers.h
# End Source File
# Begin Source File

SOURCE=.\DebugHandler.h
# End Source File
# Begin Source File

SOURCE=.\ErrorHandler.h
# End Source File
# Begin Source File

SOURCE=.\FastCapView.h
# End Source File
# Begin Source File

SOURCE=.\FastHenryView.h
# End Source File
# Begin Source File

SOURCE=.\fastmodel.h
# End Source File
# Begin Source File

SOURCE=.\fastmodelDoc.h
# End Source File
# Begin Source File

SOURCE=.\fastmodelView.h
# End Source File
# Begin Source File

SOURCE=.\FileChangeEvent.h
# End Source File
# Begin Source File

SOURCE=.\Global.h
# End Source File
# Begin Source File

SOURCE=.\IntroDialog.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\OGLViewProps.h
# End Source File
# Begin Source File

SOURCE=.\OpenGlPrintView.h
# End Source File
# Begin Source File

SOURCE=.\OpenGlView.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Res\controltabs.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cur00001.cur
# End Source File
# Begin Source File

SOURCE=.\res\cur00002.cur
# End Source File
# Begin Source File

SOURCE=.\res\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\res\fastmodel.ico
# End Source File
# Begin Source File

SOURCE=.\res\fastmodel.rc2
# End Source File
# Begin Source File

SOURCE=.\res\fastmodelDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_fast.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_fastcap.ico
# End Source File
# Begin Source File

SOURCE=.\RES\rotate_far.cur
# End Source File
# Begin Source File

SOURCE=.\RES\rotate_near.cur
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\zoom.cur
# End Source File
# End Group
# Begin Group "Help Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\HLP\fastmodel.hhp

!IF  "$(CFG)" == "fastmodel - Win32 Release"

# Begin Custom Build
OutDir=.\Release
InputPath=.\HLP\fastmodel.hhp
InputName=fastmodel

"$(OutDir)\$(InputName).chm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	hhc.exe .\hlp\$(InputName).hhp 
	if exist "hlp\$(InputName).chm" copy "hlp\$(InputName).chm" $(OutDir) 
	echo off 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "fastmodel - Win32 Debug"

# Begin Custom Build
OutDir=.\Debug
InputPath=.\HLP\fastmodel.hhp
InputName=fastmodel

"$(OutDir)\$(InputName).chm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	hhc.exe .\hlp\$(InputName).hhp 
	if exist "hlp\$(InputName).chm" copy "hlp\$(InputName).chm" $(OutDir) 
	echo off 
	
# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\History\History.txt
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
# Section fastmodel : {00000942-0010-0240-2700-270013001E00}
# 	1:17:ID_INDICATOR_DATE:105
# 	2:2:BH:
# 	2:17:ID_INDICATOR_DATE:ID_INDICATOR_DATE
# End Section
# Section fastmodel : {00000000-0000-0000-0000-000000000000}
# 	1:23:CG_IDR_POPUP_MAIN_FRAME:103
# 	1:28:CG_IDR_POPUP_FAST_MODEL_VIEW:104
# End Section
