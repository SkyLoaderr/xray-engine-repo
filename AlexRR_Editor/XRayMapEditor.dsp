# Microsoft Developer Studio Project File - Name="XRayMapEditor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=XRayMapEditor - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "XRayMapEditor.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "XRayMapEditor.mak" CFG="XRayMapEditor - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "XRayMapEditor - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "XRayMapEditor - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/XRayMapEditor (Gold edition)", KLAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "XRayMapEditor - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /G6 /Gr /Zp16 /MT /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 dxguid.lib ddraw.lib winmm.lib comctl32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "XRayMapEditor - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"pch.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 dxguid.lib ddraw.lib winmm.lib opengl32.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "XRayMapEditor - Win32 Release"
# Name "XRayMapEditor - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Group "Startup"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\XRayMapEditor.cpp
# End Source File
# End Group
# Begin Group "UI"

# PROP Default_Filter ""
# Begin Group "Bars"

# PROP Default_Filter ""
# Begin Group "ViewButtons"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\UI_PControl.cpp
# End Source File
# Begin Source File

SOURCE=.\UI_PControl.h
# End Source File
# Begin Source File

SOURCE=.\UI_RControl.cpp
# End Source File
# Begin Source File

SOURCE=.\UI_RControl.h
# End Source File
# Begin Source File

SOURCE=.\UI_SControl.cpp
# End Source File
# Begin Source File

SOURCE=.\UI_SControl.h
# End Source File
# Begin Source File

SOURCE=.\UI_ViewControl.cpp
# End Source File
# Begin Source File

SOURCE=.\UI_ViewControl.h
# End Source File
# End Group
# Begin Group "ToolOptions"

# PROP Default_Filter ""
# Begin Group "Add Action Bars"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\TB_AddLandscape.cpp
# End Source File
# Begin Source File

SOURCE=.\TB_AddLandscape.h
# End Source File
# Begin Source File

SOURCE=.\TB_AddLight.cpp
# End Source File
# Begin Source File

SOURCE=.\TB_AddLight.h
# End Source File
# Begin Source File

SOURCE=.\TB_AddObject.cpp
# End Source File
# Begin Source File

SOURCE=.\TB_AddObject.h
# End Source File
# Begin Source File

SOURCE=.\TB_AddSndPlane.cpp
# End Source File
# Begin Source File

SOURCE=.\TB_AddSndPlane.h
# End Source File
# Begin Source File

SOURCE=.\TB_AddSound.cpp
# End Source File
# Begin Source File

SOURCE=.\TB_AddSound.h
# End Source File
# End Group
# Begin Group "Select Action Bars"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\TB_SelLandscape.cpp
# End Source File
# Begin Source File

SOURCE=.\TB_SelLandscape.h
# End Source File
# Begin Source File

SOURCE=.\TB_SelLight.cpp
# End Source File
# Begin Source File

SOURCE=.\TB_SelLight.h
# End Source File
# Begin Source File

SOURCE=.\TB_SelObject.cpp
# End Source File
# Begin Source File

SOURCE=.\TB_SelObject.h
# End Source File
# Begin Source File

SOURCE=.\TB_SelSndPlane.cpp
# End Source File
# Begin Source File

SOURCE=.\TB_SelSndPlane.h
# End Source File
# Begin Source File

SOURCE=.\TB_SelSound.cpp
# End Source File
# Begin Source File

SOURCE=.\TB_SelSound.h
# End Source File
# End Group
# Begin Group "Move Action Bars"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\TB_MoveLandscape.cpp
# End Source File
# Begin Source File

SOURCE=.\TB_MoveLandscape.h
# End Source File
# Begin Source File

SOURCE=.\TB_MoveObject.cpp
# End Source File
# Begin Source File

SOURCE=.\TB_MoveObject.h
# End Source File
# Begin Source File

SOURCE=.\TB_MoveSndPlane.cpp
# End Source File
# Begin Source File

SOURCE=.\TB_MoveSndPlane.h
# End Source File
# Begin Source File

SOURCE=.\TB_MoveSound.h
# End Source File
# End Group
# Begin Group "Rotate Action Bars"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\TB_RotateLandscape.cpp
# End Source File
# Begin Source File

SOURCE=.\TB_RotateLandscape.h
# End Source File
# Begin Source File

SOURCE=.\TB_RotateObject.cpp
# End Source File
# Begin Source File

SOURCE=.\TB_RotateObject.h
# End Source File
# Begin Source File

SOURCE=.\TB_RotateSndPlane.cpp
# End Source File
# Begin Source File

SOURCE=.\TB_RotateSndPlane.h
# End Source File
# End Group
# Begin Group "Scale Action Bars"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\TB_ScaleLandscape.cpp
# End Source File
# Begin Source File

SOURCE=.\TB_ScaleLandscape.h
# End Source File
# Begin Source File

SOURCE=.\TB_ScaleObject.cpp
# End Source File
# Begin Source File

SOURCE=.\TB_ScaleObject.h
# End Source File
# Begin Source File

SOURCE=.\TB_ScaleSndPlane.cpp
# End Source File
# Begin Source File

SOURCE=.\TB_ScaleSndPlane.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\TB_Main.cpp
# End Source File
# Begin Source File

SOURCE=.\TB_Main.h
# End Source File
# End Group
# Begin Group "BarsBase"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\UI_BottomBar.cpp
# End Source File
# Begin Source File

SOURCE=.\UI_LeftBar.cpp
# End Source File
# Begin Source File

SOURCE=.\UI_TopBar.cpp
# End Source File
# End Group
# End Group
# Begin Group "Viewport"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\UI_D3D.cpp
# End Source File
# Begin Source File

SOURCE=.\UI_Mouse.cpp
# End Source File
# Begin Source File

SOURCE=.\UI_VisWindow.cpp
# End Source File
# End Group
# Begin Group "Base"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\UI_Drawing.cpp
# End Source File
# Begin Source File

SOURCE=.\UI_Main.cpp
# End Source File
# Begin Source File

SOURCE=.\UI_Main.h
# End Source File
# Begin Source File

SOURCE=.\UI_Window.cpp
# End Source File
# End Group
# Begin Group "Mouse"

# PROP Default_Filter ""
# Begin Group "Add Action"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MSC_AddLandscape.cpp
# End Source File
# Begin Source File

SOURCE=.\MSC_AddLandscape.h
# End Source File
# Begin Source File

SOURCE=.\MSC_AddLight.cpp
# End Source File
# Begin Source File

SOURCE=.\MSC_AddLight.h
# End Source File
# Begin Source File

SOURCE=.\MSC_AddObject.cpp
# End Source File
# Begin Source File

SOURCE=.\MSC_AddObject.h
# End Source File
# Begin Source File

SOURCE=.\MSC_AddSndPlane.cpp
# End Source File
# Begin Source File

SOURCE=.\MSC_AddSndPlane.h
# End Source File
# Begin Source File

SOURCE=.\MSC_AddSound.cpp
# End Source File
# Begin Source File

SOURCE=.\MSC_AddSound.h
# End Source File
# End Group
# Begin Group "Select Action"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MSC_SelLandscape.cpp
# End Source File
# Begin Source File

SOURCE=.\MSC_SelLandscape.h
# End Source File
# Begin Source File

SOURCE=.\MSC_SelLight.cpp
# End Source File
# Begin Source File

SOURCE=.\MSC_SelLight.h
# End Source File
# Begin Source File

SOURCE=.\MSC_SelObject.cpp
# End Source File
# Begin Source File

SOURCE=.\MSC_SelObject.h
# End Source File
# Begin Source File

SOURCE=.\MSC_SelSndPlane.cpp
# End Source File
# Begin Source File

SOURCE=.\MSC_SelSndPlane.h
# End Source File
# Begin Source File

SOURCE=.\MSC_SelSound.cpp
# End Source File
# Begin Source File

SOURCE=.\MSC_SelSound.h
# End Source File
# End Group
# Begin Group "Move Action"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MSC_MoveLandscape.cpp
# End Source File
# Begin Source File

SOURCE=.\MSC_MoveLandscape.h
# End Source File
# Begin Source File

SOURCE=.\MSC_MoveLight.cpp
# End Source File
# Begin Source File

SOURCE=.\MSC_MoveLight.h
# End Source File
# Begin Source File

SOURCE=.\MSC_MoveObject.cpp
# End Source File
# Begin Source File

SOURCE=.\MSC_MoveObject.h
# End Source File
# Begin Source File

SOURCE=.\MSC_MovePivot.cpp
# End Source File
# Begin Source File

SOURCE=.\MSC_MovePivot.h
# End Source File
# Begin Source File

SOURCE=.\MSC_MoveSndPlane.cpp
# End Source File
# Begin Source File

SOURCE=.\MSC_MoveSndPlane.h
# End Source File
# Begin Source File

SOURCE=.\MSC_MoveSound.cpp
# End Source File
# Begin Source File

SOURCE=.\MSC_MoveSound.h
# End Source File
# End Group
# Begin Group "Rotate Action"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MSC_RotateLandscape.cpp
# End Source File
# Begin Source File

SOURCE=.\MSC_RotateLandscape.h
# End Source File
# Begin Source File

SOURCE=.\MSC_RotateObject.cpp
# End Source File
# Begin Source File

SOURCE=.\MSC_RotateObject.h
# End Source File
# Begin Source File

SOURCE=.\MSC_RotateSndPlane.cpp
# End Source File
# Begin Source File

SOURCE=.\MSC_RotateSndPlane.h
# End Source File
# End Group
# Begin Group "Scale Action"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MSC_ScaleLandscape.cpp
# End Source File
# Begin Source File

SOURCE=.\MSC_ScaleLandscape.h
# End Source File
# Begin Source File

SOURCE=.\MSC_ScaleObject.cpp
# End Source File
# Begin Source File

SOURCE=.\MSC_ScaleObject.h
# End Source File
# Begin Source File

SOURCE=.\MSC_ScaleSndPlane.cpp
# End Source File
# Begin Source File

SOURCE=.\MSC_ScaleSndPlane.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\MSC_List.cpp
# End Source File
# Begin Source File

SOURCE=.\MSC_List.h
# End Source File
# Begin Source File

SOURCE=.\MSC_Main.cpp
# End Source File
# Begin Source File

SOURCE=.\MSC_Main.h
# End Source File
# End Group
# Begin Group "SmallDialogs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\UI_DialogEnterPivot.cpp
# End Source File
# Begin Source File

SOURCE=.\UI_DialogSelLandscape.cpp
# End Source File
# Begin Source File

SOURCE=.\UI_DialogSelObject.cpp
# End Source File
# End Group
# Begin Group "SceneOptions"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\UI_SceneOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\UI_SceneOptions.h
# End Source File
# End Group
# Begin Group "Custom Controls"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CustomControls.cpp
# End Source File
# Begin Source File

SOURCE=.\CustomControls.h
# End Source File
# End Group
# End Group
# Begin Group "Scene"

# PROP Default_Filter ""
# Begin Group "BaseScene"

# PROP Default_Filter ""
# Begin Group "Texture"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Texture.cpp
# End Source File
# Begin Source File

SOURCE=.\Texture.h
# End Source File
# End Group
# Begin Group "Mesh"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\StaticMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\StaticMesh.h
# End Source File
# Begin Source File

SOURCE=.\StaticMeshChunks.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Library.cpp
# End Source File
# Begin Source File

SOURCE=.\Library.h
# End Source File
# Begin Source File

SOURCE=.\ObjectOptionPack.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectOptionPack.h
# End Source File
# Begin Source File

SOURCE=.\Primitives.cpp
# End Source File
# Begin Source File

SOURCE=.\Primitives.h
# End Source File
# Begin Source File

SOURCE=.\Scene.cpp
# End Source File
# Begin Source File

SOURCE=.\Scene.h
# End Source File
# Begin Source File

SOURCE=.\SceneChunks.h
# End Source File
# Begin Source File

SOURCE=.\SceneClassEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneClassEditor.h
# End Source File
# Begin Source File

SOURCE=.\SceneClassList.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneClassList.h
# End Source File
# Begin Source File

SOURCE=.\SceneCopyPaste.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneObject.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneObject.h
# End Source File
# Begin Source File

SOURCE=.\SceneUndo.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneUtil.cpp
# End Source File
# End Group
# Begin Group "SceneClasses"

# PROP Default_Filter ""
# Begin Group "Landscape"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Landscape.cpp
# End Source File
# Begin Source File

SOURCE=.\Landscape.h
# End Source File
# Begin Source File

SOURCE=.\LandscapeEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\LandscapeEditor.h
# End Source File
# End Group
# Begin Group "SObject"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SObject2.cpp
# End Source File
# Begin Source File

SOURCE=.\SObject2.h
# End Source File
# Begin Source File

SOURCE=.\SObject2Editor.cpp
# End Source File
# Begin Source File

SOURCE=.\SObject2Editor.h
# End Source File
# End Group
# Begin Group "SLight"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Light.cpp
# End Source File
# Begin Source File

SOURCE=.\Light.h
# End Source File
# Begin Source File

SOURCE=.\LightEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\LightEditor.h
# End Source File
# End Group
# Begin Group "SndPlane"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SndPlane.cpp
# End Source File
# Begin Source File

SOURCE=.\SndPlane.h
# End Source File
# Begin Source File

SOURCE=.\SndPlaneEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\SndPlaneEditor.h
# End Source File
# End Group
# Begin Group "Sound"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Sound.cpp
# End Source File
# Begin Source File

SOURCE=.\Sound.h
# End Source File
# Begin Source File

SOURCE=.\SoundEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\SoundEditor.h
# End Source File
# End Group
# End Group
# End Group
# Begin Group "Util"

# PROP Default_Filter ""
# Begin Group "Shared"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\IMath.h
# End Source File
# Begin Source File

SOURCE=.\NetDeviceLog.cpp
# End Source File
# Begin Source File

SOURCE=.\NetDeviceLog.h
# End Source File
# End Group
# Begin Group "FS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FileSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\FileSystem.h
# End Source File
# End Group
# Begin Group "D3D Utils"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\D3DUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\D3DUtils.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\MultiObjCheck.h
# End Source File
# Begin Source File

SOURCE=.\XScript.cpp
# End Source File
# Begin Source File

SOURCE=.\XScript.h
# End Source File
# End Group
# Begin Group "Builder"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Builder.cpp
# End Source File
# Begin Source File

SOURCE=.\Builder.h
# End Source File
# Begin Source File

SOURCE=.\BuilderCFModel.cpp
# End Source File
# Begin Source File

SOURCE=.\BuilderCore.cpp
# End Source File
# Begin Source File

SOURCE=.\BuilderFakeCF.cpp
# End Source File
# Begin Source File

SOURCE=.\BuilderLight.cpp
# End Source File
# Begin Source File

SOURCE=.\BuilderLOpt.cpp
# End Source File
# Begin Source File

SOURCE=.\BuilderLTX.cpp
# End Source File
# Begin Source File

SOURCE=.\BuilderRModel.cpp
# End Source File
# Begin Source File

SOURCE=.\BuilderROpt.cpp
# End Source File
# Begin Source File

SOURCE=.\BuilderTexFiles.cpp
# End Source File
# Begin Source File

SOURCE=.\BuilderVis.cpp
# End Source File
# End Group
# End Group
# Begin Group "System Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\MSDev98\VC98\Include\BASETSD.H
# End Source File
# Begin Source File

SOURCE=.\Stdafx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\mainicon.ico
# End Source File
# Begin Source File

SOURCE=.\MenuTemplate.rh
# End Source File
# Begin Source File

SOURCE=.\texselec.bmp
# End Source File
# Begin Source File

SOURCE=.\XRayMapEditor.rc
# End Source File
# End Group
# Begin Group "Precompiled Header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Pch.cpp

!IF  "$(CFG)" == "XRayMapEditor - Win32 Release"

# ADD CPP /Yc

!ELSEIF  "$(CFG)" == "XRayMapEditor - Win32 Debug"

# ADD CPP /Yc"pch.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Pch.h
# End Source File
# End Group
# Begin Group "Scripts"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Debug\Config\default.meshlist
# End Source File
# Begin Source File

SOURCE=.\Debug\Config\default.scenetext
# End Source File
# Begin Source File

SOURCE=.\Debug\config\Library.script
# End Source File
# End Group
# End Target
# End Project
