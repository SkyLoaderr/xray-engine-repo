# Microsoft Developer Studio Project File - Name="shader" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=shader - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "shader.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "shader.mak" CFG="shader - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "shader - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "shader - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Plugins/shader", MBDAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "shader - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SHADER_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "_X86_" /D "_WIN32" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SHADER_EXPORTS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"r:\LW7\shader.p"
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "shader - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SHADER_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "_X86_" /D "_WIN32" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SHADER_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"shader.p" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "shader - Win32 Release"
# Name "shader - Win32 Debug"
# Begin Group "Shader"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\XRShaderDef.h
# End Source File
# Begin Source File

SOURCE=.\xrShaderLib.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\xrShaderLib.h
# End Source File
# Begin Source File

SOURCE=.\xrShaderTypes.h
# End Source File
# End Group
# Begin Group "Plugin"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\blotch.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\LW_Shader.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\LW_Shader.h
# End Source File
# Begin Source File

SOURCE=.\LW_ShaderDef.h
# End Source File
# Begin Source File

SOURCE=.\serv.def
# End Source File
# Begin Source File

SOURCE=.\servmain.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Serverd.lib

!IF  "$(CFG)" == "shader - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "shader - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Server.lib

!IF  "$(CFG)" == "shader - Win32 Release"

!ELSEIF  "$(CFG)" == "shader - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "XRay"

# PROP Default_Filter ""
# Begin Group "Math"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\_bitwise.h
# End Source File
# Begin Source File

SOURCE=.\_color.h
# End Source File
# Begin Source File

SOURCE=.\_Faabb.h
# End Source File
# Begin Source File

SOURCE=.\_fbox.h
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

SOURCE=.\_l_vertex.h
# End Source File
# Begin Source File

SOURCE=.\_lit_vertex.h
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

SOURCE=.\_quaternion.h
# End Source File
# Begin Source File

SOURCE=.\_tl_vertex.h
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

SOURCE=.\_vertex.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\FFileOps.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\FFileOps.h
# End Source File
# Begin Source File

SOURCE=.\FixedVector.h
# End Source File
# Begin Source File

SOURCE=.\LzHuf.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\lzhuf.h
# End Source File
# Begin Source File

SOURCE=.\math.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# Begin Source File

SOURCE=.\vector.h
# End Source File
# End Group
# End Target
# End Project
