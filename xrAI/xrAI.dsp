# Microsoft Developer Studio Project File - Name="xrAI" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=xrAI - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "xrAI.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "xrAI.mak" CFG="xrAI - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xrAI - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "xrAI - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/xrAI", KVDAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "xrAI - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "x:\"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G6 /Gr /MT /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib magicfm.lib /nologo /subsystem:windows /profile /machine:I386

!ELSEIF  "$(CFG)" == "xrAI - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "x:\"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "DEBUG" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib magicfm.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "xrAI - Win32 Release"
# Name "xrAI - Win32 Debug"
# Begin Group "Kernel"

# PROP Default_Filter ""
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\resource.rc
# End Source File
# End Group
# Begin Group "Math"

# PROP Default_Filter ""
# Begin Group "CPUID"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpuid.cpp
# End Source File
# Begin Source File

SOURCE=.\cpuid.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\_bitwise.h
# End Source File
# Begin Source File

SOURCE=.\_color.h
# End Source File
# Begin Source File

SOURCE=.\_compressed_normal.cpp
# End Source File
# Begin Source File

SOURCE=.\_compressed_normal.h
# End Source File
# Begin Source File

SOURCE=.\_d3d_extensions.h
# End Source File
# Begin Source File

SOURCE=.\_fbox.h
# End Source File
# Begin Source File

SOURCE=.\_fbox2.h
# End Source File
# Begin Source File

SOURCE=.\_fpoint.h
# End Source File
# Begin Source File

SOURCE=.\_frect.h
# End Source File
# Begin Source File

SOURCE=.\_intersection_quad.h
# End Source File
# Begin Source File

SOURCE=.\_ipoint.h
# End Source File
# Begin Source File

SOURCE=.\_irect.h
# End Source File
# Begin Source File

SOURCE=.\_math.cpp
# End Source File
# Begin Source File

SOURCE=.\_math.h
# End Source File
# Begin Source File

SOURCE=.\_matrix.h
# End Source File
# Begin Source File

SOURCE=.\_matrix33.h
# End Source File
# Begin Source File

SOURCE=.\_obb.h
# End Source File
# Begin Source File

SOURCE=.\_plane.h
# End Source File
# Begin Source File

SOURCE=.\_quaternion.cpp
# End Source File
# Begin Source File

SOURCE=.\_quaternion.h
# End Source File
# Begin Source File

SOURCE=.\_random.h
# End Source File
# Begin Source File

SOURCE=.\_sphere.cpp
# End Source File
# Begin Source File

SOURCE=.\_sphere.h
# End Source File
# Begin Source File

SOURCE=.\_std_extensions.h
# End Source File
# Begin Source File

SOURCE=.\_stl_extensions.h
# End Source File
# Begin Source File

SOURCE=.\_types.h
# End Source File
# Begin Source File

SOURCE=.\_vector2.h
# End Source File
# Begin Source File

SOURCE=.\_vector3d.h
# End Source File
# Begin Source File

SOURCE=.\_vector4.h
# End Source File
# Begin Source File

SOURCE=.\vector.h
# End Source File
# End Group
# Begin Group "Log"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\log.cpp
# End Source File
# Begin Source File

SOURCE=.\log.h
# End Source File
# End Group
# Begin Group "Debug"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DebugKernel.cpp
# End Source File
# Begin Source File

SOURCE=.\DebugKernel.h
# End Source File
# End Group
# Begin Group "Rapid"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cl_bboxcollide.cpp
# End Source File
# Begin Source File

SOURCE=.\cl_build.cpp
# End Source File
# Begin Source File

SOURCE=.\cl_collector.cpp
# End Source File
# Begin Source File

SOURCE=.\cl_collector.h
# End Source File
# Begin Source File

SOURCE=.\cl_collide.cpp
# End Source File
# Begin Source File

SOURCE=.\cl_defs.h
# End Source File
# Begin Source File

SOURCE=.\cl_intersect.cpp
# End Source File
# Begin Source File

SOURCE=.\cl_intersect.h
# End Source File
# Begin Source File

SOURCE=.\cl_moments.h
# End Source File
# Begin Source File

SOURCE=.\cl_overlap.cpp
# End Source File
# Begin Source File

SOURCE=.\cl_RAPID.cpp
# End Source File
# Begin Source File

SOURCE=.\cl_RAPID.h
# End Source File
# Begin Source File

SOURCE=.\cl_raypick.cpp
# End Source File
# End Group
# Begin Group "Generic"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\clsid.cpp
# End Source File
# Begin Source File

SOURCE=.\clsid.h
# End Source File
# Begin Source File

SOURCE=.\FixedVector.h
# End Source File
# Begin Source File

SOURCE=.\FTimer.h
# End Source File
# Begin Source File

SOURCE=.\Image.cpp
# End Source File
# Begin Source File

SOURCE=.\Image.h
# End Source File
# Begin Source File

SOURCE=.\light.h
# End Source File
# Begin Source File

SOURCE=.\std_classes.h
# End Source File
# Begin Source File

SOURCE=.\xr_func.h
# End Source File
# Begin Source File

SOURCE=.\xr_list.h
# End Source File
# End Group
# Begin Group "FS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FS.cpp
# End Source File
# Begin Source File

SOURCE=.\FS.h
# End Source File
# Begin Source File

SOURCE=.\LzHuf.cpp
# End Source File
# Begin Source File

SOURCE=.\lzhuf.h
# End Source File
# End Group
# Begin Group "Ini"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Xr_ini.cpp
# End Source File
# Begin Source File

SOURCE=.\xr_ini.h
# End Source File
# Begin Source File

SOURCE=.\xr_tokens.h
# End Source File
# Begin Source File

SOURCE=.\xr_trims.cpp
# End Source File
# Begin Source File

SOURCE=.\xr_trims.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\communicate.h
# End Source File
# Begin Source File

SOURCE=.\defines.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\xrAI.cpp
# End Source File
# Begin Source File

SOURCE=.\xrShaderTypes.h
# End Source File
# Begin Source File

SOURCE=.\xrSyncronize.h
# End Source File
# Begin Source File

SOURCE=.\xrThread.cpp
# End Source File
# Begin Source File

SOURCE=.\xrThread.h
# End Source File
# End Group
# Begin Group "Compiler"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\_buildnodes_view.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\compiler.cpp

!IF  "$(CFG)" == "xrAI - Win32 Release"

# ADD CPP /FAs

!ELSEIF  "$(CFG)" == "xrAI - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\compiler.h
# End Source File
# Begin Source File

SOURCE=.\compiler_build.cpp
# End Source File
# Begin Source File

SOURCE=.\compiler_cover.cpp
# End Source File
# Begin Source File

SOURCE=.\compiler_display.cpp
# End Source File
# Begin Source File

SOURCE=.\compiler_light.cpp
# End Source File
# Begin Source File

SOURCE=.\compiler_link.cpp
# End Source File
# Begin Source File

SOURCE=.\compiler_load.cpp
# End Source File
# Begin Source File

SOURCE=.\compiler_merge.cpp
# End Source File
# Begin Source File

SOURCE=.\compiler_save.cpp

!IF  "$(CFG)" == "xrAI - Win32 Release"

# ADD CPP /FAs

!ELSEIF  "$(CFG)" == "xrAI - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\compiler_smooth.cpp
# End Source File
# Begin Source File

SOURCE=.\motion_simulator.cpp
# End Source File
# Begin Source File

SOURCE=.\motion_simulator.h
# End Source File
# Begin Source File

SOURCE=.\xrLevel.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\todo.txt
# End Source File
# End Target
# End Project
