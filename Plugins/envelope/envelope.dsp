# Microsoft Developer Studio Project File - Name="envelope" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=envelope - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "envelope.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "envelope.mak" CFG="envelope - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "envelope - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "envelope - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Plugins/envelope", MBDAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "envelope - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENVELOPE_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_X86_" /D "_WIN32" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LWO_EXPORTS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"R:\Plugins\XRayExport.p"
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "envelope - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENVELOPE_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "_X86_" /D "_WIN32" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LWO_EXPORTS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 d3dx.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"R:\Plugins\XRayExport.p" /pdbtype:sept
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "envelope - Win32 Release"
# Name "envelope - Win32 Debug"
# Begin Group "Export"

# PROP Default_Filter ""
# Begin Group "SceneScan"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\scenscan\objectdb.cpp
# End Source File
# Begin Source File

SOURCE=.\scenscan\objectdb.h
# End Source File
# Begin Source File

SOURCE=.\scenscan\ptsearch.cpp
# End Source File
# Begin Source File

SOURCE=.\scenscan\surface.cpp
# End Source File
# Begin Source File

SOURCE=.\scenscan\vecmat.cpp
# End Source File
# Begin Source File

SOURCE=.\scenscan\vecmat.h
# End Source File
# Begin Source File

SOURCE=.\scenscan\vmap.cpp
# End Source File
# End Group
# Begin Group "Editor"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\bone.cpp
# End Source File
# Begin Source File

SOURCE=.\bone.h
# End Source File
# Begin Source File

SOURCE=.\bone_lw.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportObject\EditMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportObject\EditMesh.h
# End Source File
# Begin Source File

SOURCE=.\ExportObject\EditMeshIO.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportObject\EditObject.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportObject\EditObject.h
# End Source File
# Begin Source File

SOURCE=.\ExportObject\EditObjectImport.cpp
# End Source File
# Begin Source File

SOURCE=.\Envelope.cpp
# End Source File
# Begin Source File

SOURCE=.\envelope.h
# End Source File
# Begin Source File

SOURCE=.\interp.cpp
# End Source File
# Begin Source File

SOURCE=.\motion.cpp
# End Source File
# Begin Source File

SOURCE=.\motion.h
# End Source File
# Begin Source File

SOURCE=.\motion_lw.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportObject\SceneClassList.h
# End Source File
# Begin Source File

SOURCE=.\ExportObject\SceneObject.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\envtest.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\LW_export_keys.cpp
# End Source File
# Begin Source File

SOURCE=.\LW_export_motion.cpp
# End Source File
# Begin Source File

SOURCE=.\LW_export_object.cpp
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# Begin Group "Plugin"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\serv.def
# End Source File
# Begin Source File

SOURCE=.\servmain.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Serverd.lib

!IF  "$(CFG)" == "envelope - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "envelope - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Server.lib

!IF  "$(CFG)" == "envelope - Win32 Release"

!ELSEIF  "$(CFG)" == "envelope - Win32 Debug"

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

SOURCE=.\_compressed_normal.cpp
# End Source File
# Begin Source File

SOURCE=.\_compressed_normal.h
# End Source File
# Begin Source File

SOURCE=.\_d3d_extensions.h
# End Source File
# Begin Source File

SOURCE=.\_Faabb.h
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

SOURCE=.\_l_vertex.h
# End Source File
# Begin Source File

SOURCE=.\_lit_vertex.h
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

SOURCE=.\_tl_vertex.h
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

SOURCE=.\_vertex.h
# End Source File
# Begin Source File

SOURCE=.\cpuid.cpp
# End Source File
# Begin Source File

SOURCE=.\cpuid.h
# End Source File
# Begin Source File

SOURCE=.\vector.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\FileSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\FileSystem.h
# End Source File
# Begin Source File

SOURCE=.\FixedVector.h
# End Source File
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
# Begin Source File

SOURCE=.\stdafx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# End Target
# End Project
