# Microsoft Developer Studio Project File - Name="xrCDB" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=xrCDB - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "xrCDB.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "xrCDB.mak" CFG="xrCDB - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xrCDB - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "xrCDB - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/xr_3da/xrCDB", XKFAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "xrCDB - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "x:\game\"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir "x:\game\"
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "XRCDB_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G6 /Gz /MD /W3 /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "XRCDB_EXPORTS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386

!ELSEIF  "$(CFG)" == "xrCDB - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "x:\game\"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir "x:\game\"
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "XRCDB_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /G6 /Gz /MDd /W3 /Gm /Gi /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "XRCDB_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "xrCDB - Win32 Release"
# Name "xrCDB - Win32 Debug"
# Begin Group "Kernel"

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

SOURCE=.\FixedVector.h
# End Source File
# Begin Source File

SOURCE=.\Frustum.cpp
# End Source File
# Begin Source File

SOURCE=.\Frustum.h
# End Source File
# Begin Source File

SOURCE=.\vector.h
# End Source File
# End Group
# Begin Group "Core"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cl_raypick.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\xrCDB.cpp
# End Source File
# Begin Source File

SOURCE=.\xrCDB.h
# End Source File
# Begin Source File

SOURCE=.\xrCDB_box.cpp

!IF  "$(CFG)" == "xrCDB - Win32 Release"

# ADD CPP /FAs

!ELSEIF  "$(CFG)" == "xrCDB - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\xrCDB_frustum.cpp

!IF  "$(CFG)" == "xrCDB - Win32 Release"

# ADD CPP /FAs

!ELSEIF  "$(CFG)" == "xrCDB - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\xrCDB_ray.cpp

!IF  "$(CFG)" == "xrCDB - Win32 Release"

# ADD CPP /FAs

!ELSEIF  "$(CFG)" == "xrCDB - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "Opcode"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\OPC_AABB.cpp
# End Source File
# Begin Source File

SOURCE=.\OPC_AABB.h
# End Source File
# Begin Source File

SOURCE=.\OPC_AABBCollider.cpp
# End Source File
# Begin Source File

SOURCE=.\OPC_AABBCollider.h
# End Source File
# Begin Source File

SOURCE=.\OPC_AABBTree.cpp
# End Source File
# Begin Source File

SOURCE=.\OPC_AABBTree.h
# End Source File
# Begin Source File

SOURCE=.\OPC_BoundingSphere.h
# End Source File
# Begin Source File

SOURCE=.\OPC_BoxBoxOverlap.h
# End Source File
# Begin Source File

SOURCE=.\OPC_BVTCache.h
# End Source File
# Begin Source File

SOURCE=.\OPC_Collider.cpp
# End Source File
# Begin Source File

SOURCE=.\OPC_Collider.h
# End Source File
# Begin Source File

SOURCE=.\OPC_Common.cpp
# End Source File
# Begin Source File

SOURCE=.\OPC_Common.h
# End Source File
# Begin Source File

SOURCE=.\OPC_Container.cpp
# End Source File
# Begin Source File

SOURCE=.\OPC_Container.h
# End Source File
# Begin Source File

SOURCE=.\OPC_FPU.h
# End Source File
# Begin Source File

SOURCE=.\OPC_Matrix3x3.cpp
# End Source File
# Begin Source File

SOURCE=.\OPC_Matrix3x3.h
# End Source File
# Begin Source File

SOURCE=.\OPC_Matrix4x4.cpp
# End Source File
# Begin Source File

SOURCE=.\OPC_Matrix4x4.h
# End Source File
# Begin Source File

SOURCE=.\OPC_MemoryMacros.h
# End Source File
# Begin Source File

SOURCE=.\OPC_Model.cpp
# End Source File
# Begin Source File

SOURCE=.\OPC_Model.h
# End Source File
# Begin Source File

SOURCE=.\OPC_OBB.cpp
# End Source File
# Begin Source File

SOURCE=.\OPC_OBB.h
# End Source File
# Begin Source File

SOURCE=.\OPC_OBBCollider.cpp
# End Source File
# Begin Source File

SOURCE=.\OPC_OBBCollider.h
# End Source File
# Begin Source File

SOURCE=.\OPC_OptimizedTree.cpp
# End Source File
# Begin Source File

SOURCE=.\OPC_OptimizedTree.h
# End Source File
# Begin Source File

SOURCE=.\OPC_Plane.cpp
# End Source File
# Begin Source File

SOURCE=.\OPC_Plane.h
# End Source File
# Begin Source File

SOURCE=.\OPC_PlanesAABBOverlap.h
# End Source File
# Begin Source File

SOURCE=.\OPC_PlanesCollider.cpp
# End Source File
# Begin Source File

SOURCE=.\OPC_PlanesCollider.h
# End Source File
# Begin Source File

SOURCE=.\OPC_PlanesTriOverlap.h
# End Source File
# Begin Source File

SOURCE=.\OPC_Point.cpp
# End Source File
# Begin Source File

SOURCE=.\OPC_Point.h
# End Source File
# Begin Source File

SOURCE=.\OPC_Preprocessor.h
# End Source File
# Begin Source File

SOURCE=.\OPC_Ray.cpp
# End Source File
# Begin Source File

SOURCE=.\OPC_Ray.h
# End Source File
# Begin Source File

SOURCE=.\OPC_RayAABBOverlap.h
# End Source File
# Begin Source File

SOURCE=.\OPC_RayCollider.cpp
# End Source File
# Begin Source File

SOURCE=.\OPC_RayCollider.h
# End Source File
# Begin Source File

SOURCE=.\OPC_RayTriOverlap.h
# End Source File
# Begin Source File

SOURCE=.\OPC_Settings.h
# End Source File
# Begin Source File

SOURCE=.\OPC_SphereAABBOverlap.h
# End Source File
# Begin Source File

SOURCE=.\OPC_SphereCollider.cpp
# End Source File
# Begin Source File

SOURCE=.\OPC_SphereCollider.h
# End Source File
# Begin Source File

SOURCE=.\OPC_SphereTriOverlap.h
# End Source File
# Begin Source File

SOURCE=.\OPC_TreeBuilders.cpp
# End Source File
# Begin Source File

SOURCE=.\OPC_TreeBuilders.h
# End Source File
# Begin Source File

SOURCE=.\OPC_TreeCollider.cpp
# End Source File
# Begin Source File

SOURCE=.\OPC_TreeCollider.h
# End Source File
# Begin Source File

SOURCE=.\OPC_Triangle.cpp
# End Source File
# Begin Source File

SOURCE=.\OPC_Triangle.h
# End Source File
# Begin Source File

SOURCE=.\OPC_TriBoxOverlap.h
# End Source File
# Begin Source File

SOURCE=.\OPC_TriTriOverlap.h
# End Source File
# Begin Source File

SOURCE=.\OPC_Types.h
# End Source File
# Begin Source File

SOURCE=.\OPC_VolumeCollider.cpp
# End Source File
# Begin Source File

SOURCE=.\OPC_VolumeCollider.h
# End Source File
# Begin Source File

SOURCE=.\Opcode.cpp
# End Source File
# Begin Source File

SOURCE=.\Opcode.h
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Group
# End Target
# End Project
