# Microsoft Developer Studio Project File - Name="XR_3DA" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=XR_3DA - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "XR_3DA.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "XR_3DA.mak" CFG="XR_3DA - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "XR_3DA - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "XR_3DA - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "XR_3DA - Win32 Mixed" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/XR_3DA", WABAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

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
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /G6 /Gz /MT /W3 /GR /GX /Zi /Ot /Og /Ob2 /I "scripting\\" /I ".." /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "ENGINE_BUILD" /Fr /Yu"stdafx.h" /FD -QaxiM -Qipo /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x422 /d "NDEBUG"
# ADD RSC /l 0x422 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib msacm32.lib vfw32.lib /nologo /subsystem:windows /map:"x:\game\XR_3DA.map" /debug /debugtype:both /machine:I386 /force
# SUBTRACT LINK32 /profile /nodefaultlib

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "x:\game\"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /Gz /MTd /W3 /Gm /Gi /GR /GX /ZI /Od /I "scripting\\" /I ".." /D "_DEBUG" /D "DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "ENGINE_BUILD" /Yu"stdafx.h" /FD /Zm200 /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x422 /d "_DEBUG"
# ADD RSC /l 0x422 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib msacm32.lib vfw32.lib /nologo /subsystem:windows /map /debug /machine:I386
# SUBTRACT LINK32 /verbose /profile

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "XR_3DA___Win32_Mixed"
# PROP BASE Intermediate_Dir "XR_3DA___Win32_Mixed"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "x:\game\"
# PROP Intermediate_Dir "Mixed"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /Gr /MT /W3 /GX /O2 /Ob2 /I "scripting\\" /I ".." /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "ENGINE_BUILD" /Yu"stdafx.h" /FD /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /G6 /Gz /MT /W3 /GR /GX /Zi /O2 /Oy- /Ob2 /I "scripting\\" /I ".." /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "ENGINE_BUILD" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x422 /d "NDEBUG"
# ADD RSC /l 0x422 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib msacm32.lib vfw32.lib /nologo /subsystem:windows /profile /map:"x:\game\XR_3DA.map" /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib msacm32.lib vfw32.lib /nologo /subsystem:windows /profile /map:"x:\game\XR_3DA.map" /debug /machine:I386

!ENDIF 

# Begin Target

# Name "XR_3DA - Win32 Release"
# Name "XR_3DA - Win32 Debug"
# Name "XR_3DA - Win32 Mixed"
# Begin Group "General"

# PROP Default_Filter ""
# Begin Group "Resources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\resource.rc
# End Source File
# Begin Source File

SOURCE=.\StartUp_Logo.bmp
# End Source File
# End Group
# Begin Group "StackTracer"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\StackTrace\Array.h

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StackTrace\MapFile.cpp

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StackTrace\MapFile.h

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StackTrace\MapFileEntry.cpp

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StackTrace\MapFileEntry.h

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StackTrace\printStackTrace.cpp

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StackTrace\printStackTrace.h

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StackTrace\StackTrace.cpp

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StackTrace\StackTrace.h

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StackTrace\TextFile.cpp

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StackTrace\TextFile.h

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\defines.cpp
# End Source File
# Begin Source File

SOURCE=.\defines.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# Begin Source File

SOURCE=.\x_ray.cpp
# End Source File
# Begin Source File

SOURCE=.\x_ray.h
# End Source File
# End Group
# Begin Group "Math"

# PROP Default_Filter ""
# Begin Group "CPUinit"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpuid.cpp
# End Source File
# Begin Source File

SOURCE=.\cpuid.h
# End Source File
# End Group
# Begin Group "STLext"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FixedMap.h
# End Source File
# Begin Source File

SOURCE=.\FixedSet.h
# End Source File
# Begin Source File

SOURCE=.\FixedVector.h
# End Source File
# Begin Source File

SOURCE=.\VirtualVector.h
# End Source File
# Begin Source File

SOURCE=.\xr_list.h
# End Source File
# Begin Source File

SOURCE=.\xr_qlist.h
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
# Begin Group "Devices"

# PROP Default_Filter ""
# Begin Group "Execution & 3D"

# PROP Default_Filter ""
# Begin Group "HW"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\HW.cpp
# End Source File
# Begin Source File

SOURCE=.\HW.h
# End Source File
# Begin Source File

SOURCE=.\HWCaps.cpp
# End Source File
# Begin Source File

SOURCE=.\HWCaps.h
# End Source File
# End Group
# Begin Group "Primitives Render"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FVF.h
# End Source File
# Begin Source File

SOURCE=.\Primitive.cpp
# End Source File
# Begin Source File

SOURCE=.\Primitive.h
# End Source File
# Begin Source File

SOURCE=.\PrimitivesR.cpp

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# ADD CPP /FAs

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

# ADD BASE CPP /FAs
# ADD CPP /FAs

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PrimitivesR.h
# End Source File
# Begin Source File

SOURCE=.\PrimitivesR_Runtime.h
# End Source File
# Begin Source File

SOURCE=.\SharedPrimitive.cpp
# End Source File
# Begin Source File

SOURCE=.\SharedPrimitive.h
# End Source File
# End Group
# Begin Group "Gamma"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\xr_effgamma.cpp
# End Source File
# Begin Source File

SOURCE=.\xr_effgamma.h
# End Source File
# End Group
# Begin Group "Shaders"

# PROP Default_Filter ""
# Begin Group "Blender"

# PROP Default_Filter ""
# Begin Group "Templates"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Blenders\Blender_Blur.cpp
# End Source File
# Begin Source File

SOURCE=.\Blenders\Blender_Blur.h
# End Source File
# Begin Source File

SOURCE=.\Blenders\Blender_BmmD.cpp
# End Source File
# Begin Source File

SOURCE=.\Blenders\Blender_BmmD.h
# End Source File
# Begin Source File

SOURCE=.\Blenders\Blender_default_aref.cpp
# End Source File
# Begin Source File

SOURCE=.\Blenders\Blender_default_aref.h
# End Source File
# Begin Source File

SOURCE=.\Blenders\Blender_Editor_Selection.cpp
# End Source File
# Begin Source File

SOURCE=.\Blenders\Blender_Editor_Selection.h
# End Source File
# Begin Source File

SOURCE=.\Blenders\Blender_Editor_Wire.cpp
# End Source File
# Begin Source File

SOURCE=.\Blenders\Blender_Editor_Wire.h
# End Source File
# Begin Source File

SOURCE=.\Blenders\Blender_LaEmB.cpp
# End Source File
# Begin Source File

SOURCE=.\Blenders\Blender_LaEmB.h
# End Source File
# Begin Source File

SOURCE=.\Blenders\blender_light.cpp
# End Source File
# Begin Source File

SOURCE=.\Blenders\blender_light.h
# End Source File
# Begin Source File

SOURCE=".\Blenders\Blender_Lm(EbB).cpp"
# End Source File
# Begin Source File

SOURCE=".\Blenders\Blender_Lm(EbB).h"
# End Source File
# Begin Source File

SOURCE=.\Blenders\Blender_LmBmmD.cpp
# End Source File
# Begin Source File

SOURCE=.\Blenders\Blender_LmBmmD.h
# End Source File
# Begin Source File

SOURCE=.\Blenders\Blender_Screen_GRAY.cpp
# End Source File
# Begin Source File

SOURCE=.\Blenders\Blender_Screen_GRAY.h
# End Source File
# Begin Source File

SOURCE=.\Blenders\Blender_Screen_SET.cpp
# End Source File
# Begin Source File

SOURCE=.\Blenders\Blender_Screen_SET.h
# End Source File
# Begin Source File

SOURCE=.\Blenders\Blender_Shadow_Texture.cpp
# End Source File
# Begin Source File

SOURCE=.\Blenders\Blender_Shadow_Texture.h
# End Source File
# Begin Source File

SOURCE=.\Blenders\Blender_Shadow_World.cpp
# End Source File
# Begin Source File

SOURCE=.\Blenders\Blender_Shadow_World.h
# End Source File
# Begin Source File

SOURCE=.\Blenders\Blender_Vertex.cpp
# End Source File
# Begin Source File

SOURCE=.\Blenders\Blender_Vertex.h
# End Source File
# Begin Source File

SOURCE=.\Blenders\Blender_Vertex_aref.cpp
# End Source File
# Begin Source File

SOURCE=.\Blenders\Blender_Vertex_aref.h
# End Source File
# Begin Source File

SOURCE=.\Blenders\BlenderDefault.cpp
# End Source File
# Begin Source File

SOURCE=.\Blenders\BlenderDefault.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Blenders\Blender.cpp
# End Source File
# Begin Source File

SOURCE=.\Blenders\Blender.h
# End Source File
# Begin Source File

SOURCE=.\Blenders\Blender_CLSID.h
# End Source File
# Begin Source File

SOURCE=.\Blenders\Blender_Recorder.cpp
# End Source File
# Begin Source File

SOURCE=.\Blenders\Blender_Recorder.h
# End Source File
# Begin Source File

SOURCE=.\tss.h
# End Source File
# Begin Source File

SOURCE=.\tss_def.cpp
# End Source File
# Begin Source File

SOURCE=.\tss_def.h
# End Source File
# End Group
# Begin Group "Textures"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Texture.cpp

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# ADD CPP /FAs

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

# ADD BASE CPP /FAs
# ADD CPP /FAs

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Texture.h
# End Source File
# Begin Source File

SOURCE=.\TextureManager.cpp

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# ADD CPP /FAs

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

# ADD BASE CPP /FAs
# ADD CPP /FAs

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TextureManager.h
# End Source File
# Begin Source File

SOURCE=.\TextureManager_Loader.cpp
# End Source File
# Begin Source File

SOURCE=.\TextureManager_Runtime.h
# End Source File
# Begin Source File

SOURCE=.\xr_avi.cpp
# End Source File
# Begin Source File

SOURCE=.\xr_avi.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\SH_Constant.cpp
# End Source File
# Begin Source File

SOURCE=.\SH_Constant.h
# End Source File
# Begin Source File

SOURCE=.\SH_Matrix.cpp
# End Source File
# Begin Source File

SOURCE=.\SH_Matrix.h
# End Source File
# Begin Source File

SOURCE=.\SH_PS.h
# End Source File
# Begin Source File

SOURCE=.\SH_RT.cpp
# End Source File
# Begin Source File

SOURCE=.\SH_RT.h
# End Source File
# Begin Source File

SOURCE=.\SH_Texture.cpp
# End Source File
# Begin Source File

SOURCE=.\SH_Texture.h
# End Source File
# Begin Source File

SOURCE=.\SH_VS.cpp
# End Source File
# Begin Source File

SOURCE=.\SH_VS.h
# End Source File
# Begin Source File

SOURCE=.\Shader.cpp
# End Source File
# Begin Source File

SOURCE=.\Shader.h
# End Source File
# Begin Source File

SOURCE=.\Shader_xrLC.h
# End Source File
# Begin Source File

SOURCE=.\VS_Constants.h
# End Source File
# Begin Source File

SOURCE=.\WaveForm.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\device.cpp
# End Source File
# Begin Source File

SOURCE=.\device.h
# End Source File
# Begin Source File

SOURCE=.\Device_Initialize.cpp
# End Source File
# Begin Source File

SOURCE=.\Device_Misc.cpp
# End Source File
# Begin Source File

SOURCE=.\Stats.cpp
# End Source File
# Begin Source File

SOURCE=.\Stats.h
# End Source File
# Begin Source File

SOURCE=.\Stats_Timer.h
# End Source File
# End Group
# Begin Group "Sound"

# PROP Default_Filter ""
# Begin Group "2D"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\2DSound.cpp
# End Source File
# Begin Source File

SOURCE=.\2DSound.h
# End Source File
# Begin Source File

SOURCE=.\2DSoundRender.cpp
# End Source File
# Begin Source File

SOURCE=.\2DSoundRender.h
# End Source File
# End Group
# Begin Group "3D"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\3DSound.cpp
# End Source File
# Begin Source File

SOURCE=.\3DSound.h
# End Source File
# Begin Source File

SOURCE=.\3DSound_Loader.cpp
# End Source File
# Begin Source File

SOURCE=.\3DSoundRender.cpp
# End Source File
# Begin Source File

SOURCE=.\3DSoundRender.h
# End Source File
# End Group
# Begin Group "2D streamed"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MusicStream.cpp
# End Source File
# Begin Source File

SOURCE=.\MusicStream.h
# End Source File
# Begin Source File

SOURCE=.\xr_streamsnd.cpp
# End Source File
# Begin Source File

SOURCE=.\xr_streamsnd.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\xr_cda.cpp
# End Source File
# Begin Source File

SOURCE=.\xr_cda.h
# End Source File
# Begin Source File

SOURCE=.\xr_snd_defs.h
# End Source File
# Begin Source File

SOURCE=.\xr_sndman.cpp
# End Source File
# Begin Source File

SOURCE=.\xr_sndman.h
# End Source File
# End Group
# Begin Group "Input"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\fcontroller.cpp
# End Source File
# Begin Source File

SOURCE=.\FController.h
# End Source File
# Begin Source File

SOURCE=.\Xr_input.cpp
# End Source File
# Begin Source File

SOURCE=.\xr_input.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\xrSyncronize.h
# End Source File
# End Group
# Begin Group "Engine"

# PROP Default_Filter ""
# Begin Group "Collision Core"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cl_collector.cpp
# End Source File
# Begin Source File

SOURCE=.\cl_collector.h
# End Source File
# Begin Source File

SOURCE=.\cl_intersect.h
# End Source File
# Begin Source File

SOURCE=.\xrCDB.h
# End Source File
# Begin Source File

SOURCE=.\xrXRC.cpp
# End Source File
# Begin Source File

SOURCE=.\xrXRC.h
# End Source File
# End Group
# Begin Group "LTX"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Xr_ini.cpp
# End Source File
# Begin Source File

SOURCE=.\xr_ini.h
# End Source File
# Begin Source File

SOURCE=.\xr_tokens.cpp
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
# Begin Group "Console"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\log.cpp
# End Source File
# Begin Source File

SOURCE=.\log.h
# End Source File
# Begin Source File

SOURCE=.\xr_ioc_cmd.cpp
# End Source File
# Begin Source File

SOURCE=.\xr_ioc_cmd.h
# End Source File
# Begin Source File

SOURCE=.\XR_IOConsole.cpp
# End Source File
# Begin Source File

SOURCE=.\XR_IOConsole.h
# End Source File
# End Group
# Begin Group "Core"

# PROP Default_Filter ""
# Begin Group "Memory manager"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FMemoryManager.cpp
# End Source File
# Begin Source File

SOURCE=.\FMemoryManager.h
# End Source File
# Begin Source File

SOURCE=.\LocalAllocator.cpp
# End Source File
# Begin Source File

SOURCE=.\LocalAllocator.h
# End Source File
# End Group
# Begin Group "File System"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FS.cpp
# End Source File
# Begin Source File

SOURCE=.\FS.h
# End Source File
# Begin Source File

SOURCE=.\LocatorAPI.cpp
# End Source File
# Begin Source File

SOURCE=.\LocatorAPI.h
# End Source File
# Begin Source File

SOURCE=.\LzHuf.cpp
# End Source File
# Begin Source File

SOURCE=.\lzhuf.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\clsid.cpp
# End Source File
# Begin Source File

SOURCE=.\clsid.h
# End Source File
# Begin Source File

SOURCE=.\Engine.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine.h
# End Source File
# Begin Source File

SOURCE=.\EngineAPI.cpp
# End Source File
# Begin Source File

SOURCE=.\EngineAPI.h
# End Source File
# Begin Source File

SOURCE=.\EventAPI.cpp
# End Source File
# Begin Source File

SOURCE=.\EventAPI.h
# End Source File
# Begin Source File

SOURCE=.\ScriptAPI.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptAPI.h
# End Source File
# Begin Source File

SOURCE=.\ScriptAPI_Export.cpp
# End Source File
# Begin Source File

SOURCE=.\xrCPU_Pipe.h
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
# Begin Group "Scripting"

# PROP Default_Filter ""
# Begin Group "HDR"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Scripting\Code.h
# End Source File
# Begin Source File

SOURCE=.\Scripting\FIXED.H
# End Source File
# Begin Source File

SOURCE=.\Scripting\INTERNAL.H
# End Source File
# Begin Source File

SOURCE=.\Scripting\Seer.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Scripting\Callasm.c

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# ADD CPP /Gd
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

# ADD BASE CPP /Gd
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /Gd
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Scripting\Compiler.c

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# ADD CPP /Gd
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

# ADD BASE CPP /Gd
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /Gd
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Scripting\Consts.c

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# ADD CPP /Gd
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

# ADD BASE CPP /Gd
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /Gd
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Scripting\DECLARE.C

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# ADD CPP /Gd
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

# ADD BASE CPP /Gd
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /Gd
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Scripting\DICT.C

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# ADD CPP /Gd
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

# ADD BASE CPP /Gd
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /Gd
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Scripting\EXPR.C

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# ADD CPP /Gd
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

# ADD BASE CPP /Gd
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /Gd
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Scripting\HEADERS.C

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# ADD CPP /Gd
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

# ADD BASE CPP /Gd
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /Gd
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Scripting\INITIAL.C

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# ADD CPP /Gd
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

# ADD BASE CPP /Gd
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /Gd
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Scripting\Instance.c

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# ADD CPP /Gd
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

# ADD BASE CPP /Gd
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /Gd
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Scripting\PARSER.C

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# ADD CPP /Gd
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

# ADD BASE CPP /Gd
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /Gd
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Scripting\Pmem.c

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# ADD CPP /Gd
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

# ADD BASE CPP /Gd
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /Gd
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Scripting\PREPROC.C

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# ADD CPP /Gd
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

# ADD BASE CPP /Gd
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /Gd
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Scripting\PROTECT.C

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# ADD CPP /Gd
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

# ADD BASE CPP /Gd
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /Gd
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Scripting\Script.cpp

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Scripting\Script.h
# End Source File
# Begin Source File

SOURCE=.\Scripting\Script_int.c

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# ADD CPP /Gd
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

# ADD BASE CPP /Gd
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /Gd
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Scripting\VARS.C

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# ADD CPP /Gd
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

# ADD BASE CPP /Gd
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /Gd
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Scripting\Vcpucode.c

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# ADD CPP /Gd
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

# ADD BASE CPP /Gd
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /Gd
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Scripting\Vcpudeco.c

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# ADD CPP /Gd
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

# ADD BASE CPP /Gd
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /Gd
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# End Group
# Begin Group "Client - Server"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\NET_Client.cpp
# End Source File
# Begin Source File

SOURCE=.\NET_Client.h
# End Source File
# Begin Source File

SOURCE=.\NET_Compressor.cpp
# End Source File
# Begin Source File

SOURCE=.\NET_Compressor.h
# End Source File
# Begin Source File

SOURCE=.\net_messages.h
# End Source File
# Begin Source File

SOURCE=.\NET_Server.cpp
# End Source File
# Begin Source File

SOURCE=.\NET_Server.h
# End Source File
# Begin Source File

SOURCE=.\NET_Shared.h
# End Source File
# Begin Source File

SOURCE=.\NET_utils.h
# End Source File
# End Group
# Begin Group "Imaging"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Image.cpp
# End Source File
# Begin Source File

SOURCE=.\Image.h
# End Source File
# Begin Source File

SOURCE=.\tga.cpp
# End Source File
# Begin Source File

SOURCE=.\tga.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\FTimer.h
# End Source File
# Begin Source File

SOURCE=.\Properties.h
# End Source File
# Begin Source File

SOURCE=.\pure.cpp
# End Source File
# Begin Source File

SOURCE=.\pure.h
# End Source File
# Begin Source File

SOURCE=.\ReferenceCount.cpp
# End Source File
# Begin Source File

SOURCE=.\ReferenceCount.h
# End Source File
# Begin Source File

SOURCE=.\std_classes.h
# End Source File
# End Group
# Begin Group "Game API"

# PROP Default_Filter ""
# Begin Group "Objects"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\xr_object.cpp
# End Source File
# Begin Source File

SOURCE=.\xr_object.h
# End Source File
# Begin Source File

SOURCE=.\xr_object_list.cpp
# End Source File
# Begin Source File

SOURCE=.\xr_object_list.h
# End Source File
# Begin Source File

SOURCE=.\xrSheduler.cpp
# End Source File
# Begin Source File

SOURCE=.\xrSheduler.h
# End Source File
# End Group
# Begin Group "Demo"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FDemoPlay.cpp
# End Source File
# Begin Source File

SOURCE=.\FDemoPlay.h
# End Source File
# Begin Source File

SOURCE=.\FDemoRecord.cpp
# End Source File
# Begin Source File

SOURCE=.\FDemoRecord.h
# End Source File
# End Group
# Begin Group "HUD"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CustomHUD.cpp
# End Source File
# Begin Source File

SOURCE=.\CustomHUD.h
# End Source File
# Begin Source File

SOURCE=.\XR_Animator.cpp

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\XR_Animator.h
# End Source File
# End Group
# Begin Group "Environment"

# PROP Default_Filter ""
# Begin Group "Effects"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\xr_efflensflare.cpp
# End Source File
# Begin Source File

SOURCE=.\xr_efflensflare.h
# End Source File
# Begin Source File

SOURCE=.\xr_effsun.cpp
# End Source File
# Begin Source File

SOURCE=.\xr_effsun.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Environment.cpp
# End Source File
# Begin Source File

SOURCE=.\Environment.h
# End Source File
# End Group
# Begin Group "Cameras"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CameraBase.cpp
# End Source File
# Begin Source File

SOURCE=.\CameraBase.h
# End Source File
# Begin Source File

SOURCE=.\CameraDebug.cpp
# End Source File
# Begin Source File

SOURCE=.\CameraDebug.h
# End Source File
# Begin Source File

SOURCE=.\CameraDefs.h

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CameraFirstEye.cpp
# End Source File
# Begin Source File

SOURCE=.\CameraFirstEye.h
# End Source File
# Begin Source File

SOURCE=.\cameralook.cpp
# End Source File
# Begin Source File

SOURCE=.\cameralook.h
# End Source File
# Begin Source File

SOURCE=.\CameraManager.cpp
# End Source File
# Begin Source File

SOURCE=.\CameraManager.h
# End Source File
# Begin Source File

SOURCE=.\Effector.cpp
# End Source File
# Begin Source File

SOURCE=.\Effector.h
# End Source File
# Begin Source File

SOURCE=.\EffectorFall.cpp
# End Source File
# Begin Source File

SOURCE=.\EffectorFall.h
# End Source File
# End Group
# Begin Group "Collision"

# PROP Default_Filter ""
# Begin Group "Physics"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Physics\PhysicsObject.cpp
# End Source File
# Begin Source File

SOURCE=.\Physics\PhysicsObject.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\xr_area.cpp
# End Source File
# Begin Source File

SOURCE=.\xr_area.h
# End Source File
# Begin Source File

SOURCE=.\xr_area_collide.cpp

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\xr_area_debug.cpp

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\xr_area_movement.cpp
# End Source File
# Begin Source File

SOURCE=.\xr_area_query.cpp
# End Source File
# Begin Source File

SOURCE=.\xr_area_raypick.cpp
# End Source File
# Begin Source File

SOURCE=.\xr_collide_defs.h
# End Source File
# Begin Source File

SOURCE=.\xr_collide_form.cpp
# End Source File
# Begin Source File

SOURCE=.\xr_collide_form.h
# End Source File
# End Group
# Begin Group "Animator"

# PROP Default_Filter ""
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

SOURCE=.\ObjectAnimator.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectAnimator.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\xr_creator.cpp
# End Source File
# Begin Source File

SOURCE=.\xr_creator.h
# End Source File
# Begin Source File

SOURCE=.\xr_level_controller.cpp
# End Source File
# Begin Source File

SOURCE=.\xr_level_controller.h
# End Source File
# Begin Source File

SOURCE=.\xrLevel.h
# End Source File
# End Group
# Begin Group "Render"

# PROP Default_Filter ""
# Begin Group "Visuals"

# PROP Default_Filter ""
# Begin Group "ShadowForm"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FShadowForm.cpp

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FShadowForm.h
# End Source File
# End Group
# Begin Group "Skeleton"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BodyInstance.cpp

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# ADD CPP /FAs

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

# ADD BASE CPP /FAs
# ADD CPP /FAs

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\BodyInstance.h
# End Source File
# Begin Source File

SOURCE=.\SkeletonCalculate.cpp
# End Source File
# Begin Source File

SOURCE=.\SkeletonX.cpp

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# ADD CPP /FAcs

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

# ADD BASE CPP /FAcs
# ADD CPP /FAcs

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SkeletonX.h
# End Source File
# End Group
# Begin Group "Ordinal (normal)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FVisual.cpp
# End Source File
# Begin Source File

SOURCE=.\FVisual.h
# End Source File
# End Group
# Begin Group "Progressive"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FProgressive.cpp
# End Source File
# Begin Source File

SOURCE=.\FProgressive.h
# End Source File
# Begin Source File

SOURCE=.\FProgressiveFixedVisual.cpp
# End Source File
# Begin Source File

SOURCE=.\FProgressiveFixedVisual.h
# End Source File
# End Group
# Begin Group "Hierrarhy"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FHierrarhyVisual.cpp
# End Source File
# Begin Source File

SOURCE=.\FHierrarhyVisual.h
# End Source File
# End Group
# Begin Group "FastSprite"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FastSprite.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\FastSprite.h
# End Source File
# Begin Source File

SOURCE=.\FastSpriteRail.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\FastSpriteRail.h
# End Source File
# End Group
# Begin Group "Detail patches"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FDetailPatch.cpp
# End Source File
# Begin Source File

SOURCE=.\FDetailPatch.h
# End Source File
# End Group
# Begin Group "Cached"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FCached.cpp

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# ADD CPP /FAs

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

# ADD BASE CPP /FAs
# ADD CPP /FAs

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FCached.h
# End Source File
# End Group
# Begin Group "Particles"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\psvisual.cpp

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# ADD CPP /FAs

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psvisual.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\FBasicVisual.cpp
# End Source File
# Begin Source File

SOURCE=.\FBasicVisual.h
# End Source File
# Begin Source File

SOURCE=.\fmesh.cpp
# End Source File
# Begin Source File

SOURCE=.\Fmesh.h
# End Source File
# Begin Source File

SOURCE=.\FShotMarkVisual.cpp
# End Source File
# Begin Source File

SOURCE=.\FShotMarkVisual.h
# End Source File
# End Group
# Begin Group "Wallmarks"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\WallmarksEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\WallmarksEngine.h
# End Source File
# End Group
# Begin Group "Visibility"

# PROP Default_Filter ""
# Begin Group "Sector - portal"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Portal.cpp
# End Source File
# Begin Source File

SOURCE=.\Portal.h
# End Source File
# End Group
# Begin Group "HOM occlusion"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\HOM.cpp

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# ADD CPP /FAs

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\HOM.h
# End Source File
# Begin Source File

SOURCE=.\occRasterizer.cpp
# End Source File
# Begin Source File

SOURCE=.\occRasterizer.h
# End Source File
# Begin Source File

SOURCE=.\occRasterizer_core.cpp
# End Source File
# End Group
# Begin Group "Frustum"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Frustum.cpp
# End Source File
# Begin Source File

SOURCE=.\Frustum.h
# End Source File
# End Group
# End Group
# Begin Group "Particle system"

# PROP Default_Filter ""
# Begin Group "Particle Sorter"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ParticleSorter.cpp
# End Source File
# Begin Source File

SOURCE=.\ParticleSorter.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Particle.h
# End Source File
# Begin Source File

SOURCE=.\ParticleSystem.h
# End Source File
# Begin Source File

SOURCE=.\PSLibrary.cpp
# End Source File
# Begin Source File

SOURCE=.\PSLibrary.h
# End Source File
# Begin Source File

SOURCE=.\PSObject.cpp
# End Source File
# Begin Source File

SOURCE=.\PSObject.h
# End Source File
# Begin Source File

SOURCE=.\PSystem.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\PSystem.h
# End Source File
# Begin Source File

SOURCE=.\xrParticlesLib.cpp
# End Source File
# Begin Source File

SOURCE=.\xrParticlesLib.h
# End Source File
# End Group
# Begin Group "Detail"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DetailFormat.h
# End Source File
# Begin Source File

SOURCE=.\DetailManager.cpp
# End Source File
# Begin Source File

SOURCE=.\DetailManager.h
# End Source File
# Begin Source File

SOURCE=.\DetailManager_soft.cpp
# End Source File
# Begin Source File

SOURCE=.\DetailManager_VS.cpp
# End Source File
# End Group
# Begin Group "Lighting"

# PROP Default_Filter ""
# Begin Group "Lights manager"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FLightsController.cpp

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# ADD CPP /FAs

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

# ADD BASE CPP /FAs
# ADD CPP /FAs

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FLightsController.h
# End Source File
# Begin Source File

SOURCE=.\light.h
# End Source File
# End Group
# Begin Group "DLight"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\LightPPA.cpp
# End Source File
# Begin Source File

SOURCE=.\LightPPA.h
# End Source File
# End Group
# Begin Group "Projector"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\LightProjector.cpp
# End Source File
# Begin Source File

SOURCE=.\LightProjector.h
# End Source File
# End Group
# Begin Group "Shadows"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\LightShadows.cpp
# End Source File
# Begin Source File

SOURCE=.\LightShadows.h
# End Source File
# End Group
# Begin Group "LightTrack"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\LightTrack.cpp
# End Source File
# Begin Source File

SOURCE=.\LightTrack.h
# End Source File
# End Group
# End Group
# Begin Group "Glows manager"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GlowManager.cpp
# End Source File
# Begin Source File

SOURCE=.\GlowManager.h
# End Source File
# End Group
# Begin Group "RTarget"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FStaticRender_RenderTarget.cpp
# End Source File
# Begin Source File

SOURCE=.\FStaticRender_RenderTarget.h
# End Source File
# End Group
# Begin Group "Model Pool"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ModelPool.cpp
# End Source File
# Begin Source File

SOURCE=.\ModelPool.h
# End Source File
# End Group
# Begin Group "Font output"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\XR_CustomFont.cpp
# End Source File
# Begin Source File

SOURCE=.\XR_CustomFont.h
# End Source File
# Begin Source File

SOURCE=.\XR_GameFont.cpp
# End Source File
# Begin Source File

SOURCE=.\XR_GameFont.h
# End Source File
# Begin Source File

SOURCE=.\XR_HUDFont.cpp
# End Source File
# Begin Source File

SOURCE=.\XR_HUDFont.h
# End Source File
# Begin Source File

SOURCE=.\XR_SmallFont.cpp
# End Source File
# Begin Source File

SOURCE=.\XR_SmallFont.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\DetailModel.cpp
# End Source File
# Begin Source File

SOURCE=.\DetailModel.h
# End Source File
# Begin Source File

SOURCE=.\FStaticRender.cpp

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# ADD CPP /FAs

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

# ADD BASE CPP /FAs
# ADD CPP /FAs

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FStaticRender.h
# End Source File
# Begin Source File

SOURCE=.\FStaticRender_Cached.cpp
# End Source File
# Begin Source File

SOURCE=.\FStaticRender_DetectSector.cpp
# End Source File
# Begin Source File

SOURCE=.\FStaticRender_Loader.cpp
# End Source File
# Begin Source File

SOURCE=.\FStaticRender_SceneGraph.cpp
# End Source File
# Begin Source File

SOURCE=.\FStaticRender_Screenshot.cpp
# End Source File
# Begin Source File

SOURCE=.\FStaticRender_Types.h
# End Source File
# Begin Source File

SOURCE=.\Render.h
# End Source File
# Begin Source File

SOURCE=.\SceneGraph.h
# End Source File
# Begin Source File

SOURCE=.\tempobject.cpp
# End Source File
# Begin Source File

SOURCE=.\tempobject.h
# End Source File
# End Group
# Begin Group "Interface"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\XR_Menu.cpp

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\XR_Menu.h
# End Source File
# Begin Source File

SOURCE=.\XR_Options.cpp

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\XR_Options.h
# End Source File
# Begin Source File

SOURCE=.\XR_Quit.cpp

!IF  "$(CFG)" == "XR_3DA - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "XR_3DA - Win32 Mixed"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\XR_Quit.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ClientServer.txt
# End Source File
# Begin Source File

SOURCE=".\Effects description.txt"
# End Source File
# Begin Source File

SOURCE=X:\game\engine.log
# End Source File
# Begin Source File

SOURCE=.\features.txt
# End Source File
# Begin Source File

SOURCE=.\plan_last_month.txt
# End Source File
# Begin Source File

SOURCE=.\TODO.txt
# End Source File
# End Target
# End Project
