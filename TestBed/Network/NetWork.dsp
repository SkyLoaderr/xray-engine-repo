# Microsoft Developer Studio Project File - Name="NetWork" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=NetWork - Win32 Debug_Client
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "NetWork.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "NetWork.mak" CFG="NetWork - Win32 Debug_Client"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "NetWork - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "NetWork - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "NetWork - Win32 Debug_Server" (based on "Win32 (x86) Console Application")
!MESSAGE "NetWork - Win32 Debug_Client" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/TestBed/Network", LCEAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "NetWork - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "NetWork - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /Gi /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "NetWork - Win32 Debug_Server"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "NetWork___Win32_Debug_Server"
# PROP BASE Intermediate_Dir "NetWork___Win32_Debug_Server"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "NetWork___Win32_Debug_Server"
# PROP Intermediate_Dir "NetWork___Win32_Debug_Server"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /Gi /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /Gi /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "SERVER" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"Debug/net_server.exe" /pdbtype:sept

!ELSEIF  "$(CFG)" == "NetWork - Win32 Debug_Client"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "NetWork___Win32_Debug_Client"
# PROP BASE Intermediate_Dir "NetWork___Win32_Debug_Client"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "NetWork___Win32_Debug_Client"
# PROP Intermediate_Dir "NetWork___Win32_Debug_Client"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /Gi /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /Gi /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"Debug/net_client.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "NetWork - Win32 Release"
# Name "NetWork - Win32 Debug"
# Name "NetWork - Win32 Debug_Server"
# Name "NetWork - Win32 Debug_Client"
# Begin Group "Common"

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

SOURCE=.\ClientServer.txt
# End Source File
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

SOURCE=.\NET_Messages.h
# End Source File
# Begin Source File

SOURCE=.\NET_Server.cpp
# End Source File
# Begin Source File

SOURCE=.\NET_Server.h
# End Source File
# Begin Source File

SOURCE=.\NET_utils.h
# End Source File
# Begin Source File

SOURCE=.\NetWork.cpp
# End Source File
# Begin Source File

SOURCE=.\NetWork_server.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\xrSyncronize.h
# End Source File
# End Target
# End Project
