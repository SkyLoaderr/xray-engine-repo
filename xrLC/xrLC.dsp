# Microsoft Developer Studio Project File - Name="xrLC" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=xrLC - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "xrLC.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "xrLC.mak" CFG="xrLC - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xrLC - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "xrLC - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/xrLC", ZHCAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "xrLC - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G6 /Gr /MD /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /profile /machine:I386 /out:"x:\xrLC.exe"

!ELSEIF  "$(CFG)" == "xrLC - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /Gi /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"x:\xrLC.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "xrLC - Win32 Release"
# Name "xrLC - Win32 Debug"
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

SOURCE=.\vector.h
# End Source File
# End Group
# Begin Group "Kernel"

# PROP Default_Filter ""
# Begin Group "Res"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\resource.rc
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
# Begin Source File

SOURCE=.\clsid.cpp
# End Source File
# Begin Source File

SOURCE=.\clsid.h
# End Source File
# Begin Source File

SOURCE=.\defines.h
# End Source File
# Begin Source File

SOURCE=.\FTimer.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\xr_list.h
# End Source File
# Begin Source File

SOURCE=.\xrLC.cpp
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
# Begin Group "Internal"

# PROP Default_Filter ""
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
# Begin Group "Targa"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\tga.cpp
# End Source File
# Begin Source File

SOURCE=.\tga.h
# End Source File
# End Group
# Begin Group "ILoader"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Image.cpp
# End Source File
# Begin Source File

SOURCE=.\Image.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\FixedVector.h
# End Source File
# Begin Source File

SOURCE=.\vbm.h
# End Source File
# Begin Source File

SOURCE=.\xr_func.h
# End Source File
# Begin Source File

SOURCE=.\xrFace.cpp
# End Source File
# Begin Source File

SOURCE=.\xrFace.h
# End Source File
# Begin Source File

SOURCE=.\xrIsect.h
# End Source File
# Begin Source File

SOURCE=.\xrLoadSurface.cpp
# End Source File
# Begin Source File

SOURCE=.\xrUVpoint.h
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

!IF  "$(CFG)" == "xrLC - Win32 Release"

# ADD CPP /FAs

!ELSEIF  "$(CFG)" == "xrLC - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "External"

# PROP Default_Filter ""
# Begin Group "Communication"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\b_globals.h
# End Source File
# Begin Source File

SOURCE=.\communicate.h
# End Source File
# End Group
# Begin Group "NV_Strips"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\NV_Library\NvTriStrip.cpp
# End Source File
# Begin Source File

SOURCE=.\NV_Library\NvTriStrip.h
# End Source File
# Begin Source File

SOURCE=.\NV_Library\NvTriStripObjects.cpp
# End Source File
# Begin Source File

SOURCE=.\NV_Library\NvTriStripObjects.h
# End Source File
# Begin Source File

SOURCE=.\NV_Library\VertexCache.cpp
# End Source File
# Begin Source File

SOURCE=.\NV_Library\VertexCache.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\FreeImage.h
# End Source File
# Begin Source File

SOURCE=.\light.h
# End Source File
# Begin Source File

SOURCE=.\progmesh.h
# End Source File
# Begin Source File

SOURCE=.\std_classes.h
# End Source File
# Begin Source File

SOURCE=.\TextureParams.h
# End Source File
# Begin Source File

SOURCE=.\xrDXTC.h
# End Source File
# Begin Source File

SOURCE=.\xrLevel.h
# End Source File
# Begin Source File

SOURCE=.\xrOcclusion.h
# End Source File
# Begin Source File

SOURCE=.\xrSpherical.h
# End Source File
# End Group
# Begin Group "Deflector"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\xrDeflectoL_Direct.cpp
# End Source File
# Begin Source File

SOURCE=.\xrDeflector.cpp
# End Source File
# Begin Source File

SOURCE=.\xrDeflector.h
# End Source File
# Begin Source File

SOURCE=.\xrDeflectorDisplay.cpp
# End Source File
# Begin Source File

SOURCE=.\xrDeflectorL_Radiosity.cpp
# End Source File
# Begin Source File

SOURCE=.\xrDeflectorLight.cpp
# End Source File
# End Group
# Begin Group "Shaders"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\XRShaderDef.h
# End Source File
# Begin Source File

SOURCE=.\xrShaderLib.cpp
# End Source File
# Begin Source File

SOURCE=.\xrShaderLib.h
# End Source File
# Begin Source File

SOURCE=.\xrShaderTypes.h
# End Source File
# End Group
# Begin Group "XRAY"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\fmesh.cpp
# End Source File
# Begin Source File

SOURCE=.\Fmesh.h
# End Source File
# Begin Source File

SOURCE=.\OGF_Face.cpp
# End Source File
# Begin Source File

SOURCE=.\OGF_Face.h
# End Source File
# Begin Source File

SOURCE=.\OGF_Face_Save.cpp
# End Source File
# Begin Source File

SOURCE=.\OGF_Face_Stripify.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Build.cpp
# End Source File
# Begin Source File

SOURCE=.\Build.h
# End Source File
# Begin Source File

SOURCE=.\Sector.cpp
# End Source File
# Begin Source File

SOURCE=.\Sector.h
# End Source File
# Begin Source File

SOURCE=.\xrBuildCForm.cpp
# End Source File
# Begin Source File

SOURCE=.\xrBuildRapidModel.cpp
# End Source File
# Begin Source File

SOURCE=.\xrCalcNormals.cpp
# End Source File
# Begin Source File

SOURCE=.\xrDetailPatch.cpp
# End Source File
# Begin Source File

SOURCE=.\xrFlex2OGF.cpp
# End Source File
# Begin Source File

SOURCE=.\xrHierrarhy.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\xrLight.cpp
# End Source File
# Begin Source File

SOURCE=.\xrLight_Implicit.cpp
# End Source File
# Begin Source File

SOURCE=.\xrMergeGeometry.cpp
# End Source File
# Begin Source File

SOURCE=.\xrMergeLM.cpp
# End Source File
# Begin Source File

SOURCE=.\xrPreOptimize.cpp
# End Source File
# Begin Source File

SOURCE=.\xrPVS.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\xrResolveMaterials.cpp
# End Source File
# Begin Source File

SOURCE=.\xrSaveLights.cpp
# End Source File
# Begin Source File

SOURCE=.\xrSaveOGF.cpp
# End Source File
# Begin Source File

SOURCE=.\xrSectors.cpp
# End Source File
# Begin Source File

SOURCE=.\xrSoftenLights.cpp
# End Source File
# Begin Source File

SOURCE=.\xrT_Junction.cpp
# End Source File
# Begin Source File

SOURCE=.\xrTesselate.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\xrUVmapping.cpp
# End Source File
# Begin Source File

SOURCE=.\xrVis.cpp
# PROP Exclude_From_Build 1
# End Source File
# End Target
# End Project
