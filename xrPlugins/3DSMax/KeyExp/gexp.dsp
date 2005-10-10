# Microsoft Developer Studio Project File - Name="gexp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=gexp - Win32 NFO Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "gexp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gexp.mak" CFG="gexp - Win32 NFO Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gexp - Win32 NFO Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gexp - Win32 NFO Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gexp - Win32 KEY Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gexp - Win32 KEY Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gexp - Win32 R3 KEY Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gexp - Win32 R3 NFO Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gexp - Win32 R3 NFO Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gexp - Win32 R3 KEY Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Genesis10/Tools/Exporters", GUIAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "gexp - Win32 NFO Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\nfoRelease"
# PROP Intermediate_Dir ".\nfoRelease"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MT /W3 /GX /O2 /I "max2sdk\include" /I "..\..\msdev60\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "NFOEXP" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /x /i "..\..\msdev60\include" /d "NDEBUG" /d "NFOEXP"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 libcmt.lib max2sdk\lib\core.lib max2sdk\lib\util.lib max2sdk\lib\geom.lib max2sdk\lib\gfx.lib max2sdk\lib\mesh.lib version.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib oldnames.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib /out:".\nforelease\nfoexp.dle" /libpath:"..\..\msdev60\lib"
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "gexp - Win32 NFO Hybrid"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\nfoexp_0"
# PROP BASE Intermediate_Dir ".\nfoexp_0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\nfoHybrid"
# PROP Intermediate_Dir ".\nfoHybrid"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MD /W3 /Gm /GX /Zi /Od /I "..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /I "max2sdk\include" /I "..\..\msdev60\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "NFOEXP" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /x /i "..\..\msdev60\include" /d "_DEBUG" /d "NFOEXP"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\..\..\exe\stdplugs\nfoexp.dle"
# ADD LINK32 libcmtd.lib winmm.lib oldnames.lib max2sdk\lib\core.lib max2sdk\lib\util.lib max2sdk\lib\geom.lib max2sdk\lib\gfx.lib max2sdk\lib\mesh.lib version.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib /out:".\nfohybrid\nfoexp.dle" /libpath:"..\..\msdev60\lib"

!ELSEIF  "$(CFG)" == "gexp - Win32 KEY Hybrid"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "gexp___1"
# PROP BASE Intermediate_Dir "gexp___1"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "keyhybrid"
# PROP Intermediate_Dir "keyhybrid"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MDd /W3 /Gm /GX /Zi /Od /I "max2sdk\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "NFOEXP" /FD /c
# SUBTRACT BASE CPP /Fr /YX
# ADD CPP /nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /I "max2sdk\include" /I "..\..\msdev60\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "KEYEXP" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /x /i "..\..\msdev60\include" /d "_DEBUG" /d "KEYEXP"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 winmm.lib max2sdk\lib\core.lib max2sdk\lib\util.lib max2sdk\lib\geom.lib max2sdk\lib\gfx.lib max2sdk\lib\mesh.lib version.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:".\nfohybrid\nfoexp.dle"
# ADD LINK32 libcmtd.lib winmm.lib oldnames.lib max2sdk\lib\core.lib max2sdk\lib\util.lib max2sdk\lib\geom.lib max2sdk\lib\gfx.lib max2sdk\lib\mesh.lib version.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib /out:".\keyhybrid\keyexp.dle" /libpath:"..\..\msdev60\lib"

!ELSEIF  "$(CFG)" == "gexp - Win32 KEY Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "gexp___2"
# PROP BASE Intermediate_Dir "gexp___2"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "keyrelease"
# PROP Intermediate_Dir "keyrelease"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MD /W3 /GX /O2 /I "max2sdk\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "NFOEXP" /FD /c
# SUBTRACT BASE CPP /Fr /YX
# ADD CPP /nologo /G5 /MT /W3 /GX /O2 /I "max2sdk\include" /I "..\..\msdev60\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "KEYEXP" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /x /i "..\..\msdev60\include" /d "NDEBUG" /d "KEYEXP"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 max2sdk\lib\core.lib max2sdk\lib\util.lib max2sdk\lib\geom.lib max2sdk\lib\gfx.lib max2sdk\lib\mesh.lib version.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /machine:I386 /out:".\nforelease\nfoexp.dle"
# SUBTRACT BASE LINK32 /debug
# ADD LINK32 libcmt.lib max2sdk\lib\core.lib max2sdk\lib\util.lib max2sdk\lib\geom.lib max2sdk\lib\gfx.lib max2sdk\lib\mesh.lib version.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib oldnames.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib /out:"c:\3dsmax\plugins\keyexp.dle" /libpath:"..\..\msdev60\lib"
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "gexp - Win32 R3 KEY Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "gexp___Win32_R3_KEY_Release"
# PROP BASE Intermediate_Dir "gexp___Win32_R3_KEY_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "R3KEYRelease"
# PROP Intermediate_Dir "R3KEYRelease"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MT /W3 /GX /O2 /I "max2sdk\include" /I "..\..\msdev60\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "KEYEXP" /FD /c
# SUBTRACT BASE CPP /Fr /YX
# ADD CPP /nologo /G5 /MT /W3 /GX /O2 /I "..\..\msdev60\include" /I "max3sdk\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "KEYEXP" /Fr /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /x /i "..\..\msdev60\include" /d "NDEBUG" /d "KEYEXP"
# ADD RSC /l 0x409 /x /i "..\..\msdev60\include" /d "NDEBUG" /d "KEYEXP"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 libcmt.lib max2sdk\lib\core.lib max2sdk\lib\util.lib max2sdk\lib\geom.lib max2sdk\lib\gfx.lib max2sdk\lib\mesh.lib version.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib oldnames.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib /out:".\keyrelease\keyexp.dle" /libpath:"..\..\msdev60\lib"
# SUBTRACT BASE LINK32 /debug
# ADD LINK32 libcmt.lib r:\3dsmax31\maxsdk\lib\core.lib r:\3dsmax31\maxsdk\lib\maxutil.lib r:\3dsmax31\maxsdk\lib\geom.lib r:\3dsmax31\maxsdk\lib\gfx.lib r:\3dsmax31\maxsdk\lib\mesh.lib version.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib oldnames.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib /out:"c:\3dsmax\plugins\keyexp.dle" /libpath:"..\..\msdev60\lib"
# SUBTRACT LINK32 /incremental:yes /debug

!ELSEIF  "$(CFG)" == "gexp - Win32 R3 NFO Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "gexp___Win32_R3_NFO_Release"
# PROP BASE Intermediate_Dir "gexp___Win32_R3_NFO_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "R3NFORelease"
# PROP Intermediate_Dir "R3NFORelease"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MT /W3 /GX /O2 /I "max2sdk\include" /I "..\..\msdev60\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "NFOEXP" /FD /c
# SUBTRACT BASE CPP /Fr /YX
# ADD CPP /nologo /G5 /MT /W3 /GX /O2 /I "..\..\msdev60\include" /I "max3sdk\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "NFOEXP" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /x /i "..\..\msdev60\include" /d "NDEBUG" /d "NFOEXP"
# ADD RSC /l 0x409 /x /i "..\..\msdev60\include" /d "NDEBUG" /d "NFOEXP"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 libcmt.lib max2sdk\lib\core.lib max2sdk\lib\util.lib max2sdk\lib\geom.lib max2sdk\lib\gfx.lib max2sdk\lib\mesh.lib version.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib oldnames.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib /out:".\nforelease\nfoexp.dle" /libpath:"..\..\msdev60\lib"
# SUBTRACT BASE LINK32 /debug
# ADD LINK32 libcmt.lib max3sdk\lib\core.lib max3sdk\lib\maxutil.lib max3sdk\lib\geom.lib max3sdk\lib\gfx.lib max3sdk\lib\mesh.lib version.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib oldnames.lib /nologo /subsystem:windows /dll /machine:I386 /out:".\R3nforelease\nfoexp.dle" /libpath:"..\..\msdev60\lib"
# SUBTRACT LINK32 /debug /nodefaultlib

!ELSEIF  "$(CFG)" == "gexp - Win32 R3 NFO Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "gexp___Win32_R3_NFO_Debug"
# PROP BASE Intermediate_Dir "gexp___Win32_R3_NFO_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "R3NFODebug"
# PROP Intermediate_Dir "R3NFODebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MT /W3 /GX /O2 /I "..\..\msdev60\include" /I "max3sdk\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "NFOEXP" /FD /c
# SUBTRACT BASE CPP /Fr /YX
# ADD CPP /nologo /G5 /MDd /W3 /GX /ZI /Od /I "..\..\msdev60\include" /I "max3sdk\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "NFOEXP" /Fr /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /x /i "..\..\msdev60\include" /d "NDEBUG" /d "NFOEXP"
# ADD RSC /l 0x409 /i "..\..\msdev60\include" /d "NDEBUG" /d "NFOEXP"
# SUBTRACT RSC /x
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 libcmt.lib max3sdk\lib\core.lib max3sdk\lib\maxutil.lib max3sdk\lib\geom.lib max3sdk\lib\gfx.lib max3sdk\lib\mesh.lib version.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib oldnames.lib /nologo /subsystem:windows /dll /machine:I386 /out:".\R3nforelease\nfoexp.dle" /libpath:"..\..\msdev60\lib"
# SUBTRACT BASE LINK32 /debug /nodefaultlib
# ADD LINK32 max3sdk\lib\core.lib max3sdk\lib\maxutil.lib max3sdk\lib\geom.lib max3sdk\lib\gfx.lib max3sdk\lib\mesh.lib version.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib oldnames.lib msvcrtd.lib /nologo /subsystem:windows /dll /incremental:yes /debug /machine:I386 /out:"C:\3DSMAX\Plugins\nfoexp.dle" /libpath:"..\..\msdev60\lib"
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "gexp - Win32 R3 KEY Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "gexp___Win32_R3_KEY_Debug"
# PROP BASE Intermediate_Dir "gexp___Win32_R3_KEY_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "gexp___Win32_R3_KEY_Debug"
# PROP Intermediate_Dir "gexp___Win32_R3_KEY_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MT /W3 /GX /O2 /I "..\..\msdev60\include" /I "max3sdk\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "KEYEXP" /Fr /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /G5 /MDd /W3 /Gi /GX /ZI /Od /I "..\..\msdev60\include" /I "max3sdk\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "KEYEXP" /Fr /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /x /i "..\..\msdev60\include" /d "NDEBUG" /d "KEYEXP"
# ADD RSC /l 0x409 /x /i "..\..\msdev60\include" /d "NDEBUG" /d "KEYEXP"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 libcmt.lib r:\3dsmax31\maxsdk\lib\core.lib r:\3dsmax31\maxsdk\lib\maxutil.lib r:\3dsmax31\maxsdk\lib\geom.lib r:\3dsmax31\maxsdk\lib\gfx.lib r:\3dsmax31\maxsdk\lib\mesh.lib version.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib oldnames.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib /out:"c:\3dsmax\plugins\keyexp.dle" /libpath:"..\..\msdev60\lib"
# SUBTRACT BASE LINK32 /incremental:yes /debug
# ADD LINK32 r:\3dsmax31\maxsdk\lib\core.lib r:\3dsmax31\maxsdk\lib\maxutil.lib r:\3dsmax31\maxsdk\lib\geom.lib r:\3dsmax31\maxsdk\lib\gfx.lib r:\3dsmax31\maxsdk\lib\mesh.lib version.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib oldnames.lib msvcrtd.lib /nologo /subsystem:windows /dll /incremental:yes /debug /machine:I386 /nodefaultlib /out:"r:\3dsmax31\plugins\keyexp.dle" /libpath:"..\..\msdev60\lib"

!ENDIF 

# Begin Target

# Name "gexp - Win32 NFO Release"
# Name "gexp - Win32 NFO Hybrid"
# Name "gexp - Win32 KEY Hybrid"
# Name "gexp - Win32 KEY Release"
# Name "gexp - Win32 R3 KEY Release"
# Name "gexp - Win32 R3 NFO Release"
# Name "gexp - Win32 R3 NFO Debug"
# Name "gexp - Win32 R3 KEY Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\3dsmtl.cpp
# End Source File
# Begin Source File

SOURCE=.\gexp.cpp
# End Source File
# Begin Source File

SOURCE=.\gexp.def
# End Source File
# Begin Source File

SOURCE=.\gexp.rc

!IF  "$(CFG)" == "gexp - Win32 NFO Release"

!ELSEIF  "$(CFG)" == "gexp - Win32 NFO Hybrid"

!ELSEIF  "$(CFG)" == "gexp - Win32 KEY Hybrid"

# ADD BASE RSC /l 0x422
# ADD RSC /l 0x422
# SUBTRACT RSC /x

!ELSEIF  "$(CFG)" == "gexp - Win32 KEY Release"

!ELSEIF  "$(CFG)" == "gexp - Win32 R3 KEY Release"

# ADD BASE RSC /l 0x422
# ADD RSC /l 0x422
# SUBTRACT RSC /x

!ELSEIF  "$(CFG)" == "gexp - Win32 R3 NFO Release"

# ADD BASE RSC /l 0x422
# ADD RSC /l 0x422
# SUBTRACT RSC /x

!ELSEIF  "$(CFG)" == "gexp - Win32 R3 NFO Debug"

# ADD BASE RSC /l 0x422
# SUBTRACT BASE RSC /x
# ADD RSC /l 0x422
# SUBTRACT RSC /x

!ELSEIF  "$(CFG)" == "gexp - Win32 R3 KEY Debug"

# ADD BASE RSC /l 0x422
# SUBTRACT BASE RSC /x
# ADD RSC /l 0x422
# SUBTRACT RSC /x

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Savemli.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\3dseres.h
# End Source File
# Begin Source File

SOURCE=.\3dsimp.h
# End Source File
# Begin Source File

SOURCE=.\3dsires.h
# End Source File
# Begin Source File

SOURCE=.\Cfile.h
# End Source File
# Begin Source File

SOURCE=.\gexp.h
# End Source File
# Begin Source File

SOURCE=.\Kfio.h
# End Source File
# Begin Source File

SOURCE=.\Mtldef.h
# End Source File
# Begin Source File

SOURCE=.\Ofile.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\gexp.mak
# End Source File
# End Target
# End Project
