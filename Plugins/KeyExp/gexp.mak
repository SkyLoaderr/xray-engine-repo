# Microsoft Developer Studio Generated NMAKE File, Based on gexp.dsp
!IF "$(CFG)" == ""
CFG=gexp - Win32 NFO Release
!MESSAGE No configuration specified. Defaulting to gexp - Win32 NFO Release.
!ENDIF 

!IF "$(CFG)" != "gexp - Win32 NFO Release" && "$(CFG)" != "gexp - Win32 NFO Hybrid" && "$(CFG)" != "gexp - Win32 KEY Hybrid" && "$(CFG)" != "gexp - Win32 KEY Release" && "$(CFG)" != "gexp - Win32 R3 KEY Release" && "$(CFG)" != "gexp - Win32 R3 NFO Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
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
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "gexp - Win32 NFO Release"

OUTDIR=.\nfoRelease
INTDIR=.\nfoRelease
# Begin Custom Macros
OutDir=.\nfoRelease
# End Custom Macros

ALL : "$(OUTDIR)\nfoexp.dle"


CLEAN :
	-@erase "$(INTDIR)\3dsmtl.obj"
	-@erase "$(INTDIR)\gexp.obj"
	-@erase "$(INTDIR)\gexp.res"
	-@erase "$(INTDIR)\Savemli.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\nfoexp.dle"
	-@erase "$(OUTDIR)\nfoexp.exp"
	-@erase "$(OUTDIR)\nfoexp.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G5 /MT /W3 /GX /O2 /X /I "max2sdk\include" /I "..\..\msdev60\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "NFOEXP" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /x /fo"$(INTDIR)\gexp.res" /i "..\..\msdev60\include" /d "NDEBUG" /d "NFOEXP" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\gexp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=libcmt.lib max2sdk\lib\core.lib max2sdk\lib\util.lib max2sdk\lib\geom.lib max2sdk\lib\gfx.lib max2sdk\lib\mesh.lib version.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib oldnames.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\nfoexp.pdb" /machine:I386 /nodefaultlib /def:".\gexp.def" /out:"$(OUTDIR)\nfoexp.dle" /implib:"$(OUTDIR)\nfoexp.lib" /libpath:"..\..\msdev60\lib" 
DEF_FILE= \
	".\gexp.def"
LINK32_OBJS= \
	"$(INTDIR)\3dsmtl.obj" \
	"$(INTDIR)\gexp.obj" \
	"$(INTDIR)\Savemli.obj" \
	"$(INTDIR)\gexp.res"

"$(OUTDIR)\nfoexp.dle" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "gexp - Win32 NFO Hybrid"

OUTDIR=.\nfoHybrid
INTDIR=.\nfoHybrid
# Begin Custom Macros
OutDir=.\nfoHybrid
# End Custom Macros

ALL : "$(OUTDIR)\nfoexp.dle"


CLEAN :
	-@erase "$(INTDIR)\3dsmtl.obj"
	-@erase "$(INTDIR)\gexp.obj"
	-@erase "$(INTDIR)\gexp.res"
	-@erase "$(INTDIR)\Savemli.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\nfoexp.dle"
	-@erase "$(OUTDIR)\nfoexp.exp"
	-@erase "$(OUTDIR)\nfoexp.ilk"
	-@erase "$(OUTDIR)\nfoexp.lib"
	-@erase "$(OUTDIR)\nfoexp.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /X /I "max2sdk\include" /I "..\..\msdev60\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "NFOEXP" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /x /fo"$(INTDIR)\gexp.res" /i "..\..\msdev60\include" /d "_DEBUG" /d "NFOEXP" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\gexp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=libcmtd.lib winmm.lib oldnames.lib max2sdk\lib\core.lib max2sdk\lib\util.lib max2sdk\lib\geom.lib max2sdk\lib\gfx.lib max2sdk\lib\mesh.lib version.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\nfoexp.pdb" /debug /machine:I386 /nodefaultlib /def:".\gexp.def" /out:"$(OUTDIR)\nfoexp.dle" /implib:"$(OUTDIR)\nfoexp.lib" /libpath:"..\..\msdev60\lib" 
DEF_FILE= \
	".\gexp.def"
LINK32_OBJS= \
	"$(INTDIR)\3dsmtl.obj" \
	"$(INTDIR)\gexp.obj" \
	"$(INTDIR)\Savemli.obj" \
	"$(INTDIR)\gexp.res"

"$(OUTDIR)\nfoexp.dle" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "gexp - Win32 KEY Hybrid"

OUTDIR=.\keyhybrid
INTDIR=.\keyhybrid
# Begin Custom Macros
OutDir=.\keyhybrid
# End Custom Macros

ALL : "$(OUTDIR)\keyexp.dle"


CLEAN :
	-@erase "$(INTDIR)\3dsmtl.obj"
	-@erase "$(INTDIR)\gexp.obj"
	-@erase "$(INTDIR)\gexp.res"
	-@erase "$(INTDIR)\Savemli.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\keyexp.dle"
	-@erase "$(OUTDIR)\keyexp.exp"
	-@erase "$(OUTDIR)\keyexp.ilk"
	-@erase "$(OUTDIR)\keyexp.lib"
	-@erase "$(OUTDIR)\keyexp.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /X /I "max2sdk\include" /I "..\..\msdev60\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "KEYEXP" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /x /fo"$(INTDIR)\gexp.res" /i "..\..\msdev60\include" /d "_DEBUG" /d "KEYEXP" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\gexp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=libcmtd.lib winmm.lib oldnames.lib max2sdk\lib\core.lib max2sdk\lib\util.lib max2sdk\lib\geom.lib max2sdk\lib\gfx.lib max2sdk\lib\mesh.lib version.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\keyexp.pdb" /debug /machine:I386 /nodefaultlib /def:".\gexp.def" /out:"$(OUTDIR)\keyexp.dle" /implib:"$(OUTDIR)\keyexp.lib" /libpath:"..\..\msdev60\lib" 
DEF_FILE= \
	".\gexp.def"
LINK32_OBJS= \
	"$(INTDIR)\3dsmtl.obj" \
	"$(INTDIR)\gexp.obj" \
	"$(INTDIR)\Savemli.obj" \
	"$(INTDIR)\gexp.res"

"$(OUTDIR)\keyexp.dle" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "gexp - Win32 KEY Release"

OUTDIR=.\keyrelease
INTDIR=.\keyrelease
# Begin Custom Macros
OutDir=.\keyrelease
# End Custom Macros

ALL : "$(OUTDIR)\keyexp.dle"


CLEAN :
	-@erase "$(INTDIR)\3dsmtl.obj"
	-@erase "$(INTDIR)\gexp.obj"
	-@erase "$(INTDIR)\gexp.res"
	-@erase "$(INTDIR)\Savemli.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\keyexp.dle"
	-@erase "$(OUTDIR)\keyexp.exp"
	-@erase "$(OUTDIR)\keyexp.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G5 /MT /W3 /GX /O2 /X /I "max2sdk\include" /I "..\..\msdev60\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "KEYEXP" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /x /fo"$(INTDIR)\gexp.res" /i "..\..\msdev60\include" /d "NDEBUG" /d "KEYEXP" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\gexp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=libcmt.lib max2sdk\lib\core.lib max2sdk\lib\util.lib max2sdk\lib\geom.lib max2sdk\lib\gfx.lib max2sdk\lib\mesh.lib version.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib oldnames.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\keyexp.pdb" /machine:I386 /nodefaultlib /def:".\gexp.def" /out:"$(OUTDIR)\keyexp.dle" /implib:"$(OUTDIR)\keyexp.lib" /libpath:"..\..\msdev60\lib" 
DEF_FILE= \
	".\gexp.def"
LINK32_OBJS= \
	"$(INTDIR)\3dsmtl.obj" \
	"$(INTDIR)\gexp.obj" \
	"$(INTDIR)\Savemli.obj" \
	"$(INTDIR)\gexp.res"

"$(OUTDIR)\keyexp.dle" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "gexp - Win32 R3 KEY Release"

OUTDIR=.\R3KEYRelease
INTDIR=.\R3KEYRelease
# Begin Custom Macros
OutDir=.\R3KEYRelease
# End Custom Macros

ALL : "$(OUTDIR)\keyexp.dle"


CLEAN :
	-@erase "$(INTDIR)\3dsmtl.obj"
	-@erase "$(INTDIR)\gexp.obj"
	-@erase "$(INTDIR)\gexp.res"
	-@erase "$(INTDIR)\Savemli.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\keyexp.dle"
	-@erase "$(OUTDIR)\keyexp.exp"
	-@erase "$(OUTDIR)\keyexp.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G5 /MT /W3 /GX /O2 /X /I "..\..\msdev60\include" /I "max3sdk\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "KEYEXP" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /x /fo"$(INTDIR)\gexp.res" /i "..\..\msdev60\include" /d "NDEBUG" /d "KEYEXP" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\gexp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=libcmt.lib max3sdk\lib\core.lib max3sdk\lib\maxutil.lib max3sdk\lib\geom.lib max3sdk\lib\gfx.lib max3sdk\lib\mesh.lib version.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib oldnames.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\keyexp.pdb" /machine:I386 /nodefaultlib /def:".\gexp.def" /out:"$(OUTDIR)\keyexp.dle" /implib:"$(OUTDIR)\keyexp.lib" /libpath:"..\..\msdev60\lib" 
DEF_FILE= \
	".\gexp.def"
LINK32_OBJS= \
	"$(INTDIR)\3dsmtl.obj" \
	"$(INTDIR)\gexp.obj" \
	"$(INTDIR)\Savemli.obj" \
	"$(INTDIR)\gexp.res"

"$(OUTDIR)\keyexp.dle" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "gexp - Win32 R3 NFO Release"

OUTDIR=.\R3NFORelease
INTDIR=.\R3NFORelease
# Begin Custom Macros
OutDir=.\R3NFORelease
# End Custom Macros

ALL : "$(OUTDIR)\nfoexp.dle"


CLEAN :
	-@erase "$(INTDIR)\3dsmtl.obj"
	-@erase "$(INTDIR)\gexp.obj"
	-@erase "$(INTDIR)\gexp.res"
	-@erase "$(INTDIR)\Savemli.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\nfoexp.dle"
	-@erase "$(OUTDIR)\nfoexp.exp"
	-@erase "$(OUTDIR)\nfoexp.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G5 /MT /W3 /GX /O2 /X /I "..\..\msdev60\include" /I "max3sdk\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "NFOEXP" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /x /fo"$(INTDIR)\gexp.res" /i "..\..\msdev60\include" /d "NDEBUG" /d "NFOEXP" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\gexp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=libcmt.lib max3sdk\lib\core.lib max3sdk\lib\maxutil.lib max3sdk\lib\geom.lib max3sdk\lib\gfx.lib max3sdk\lib\mesh.lib version.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib oldnames.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\nfoexp.pdb" /machine:I386 /nodefaultlib /def:".\gexp.def" /out:"$(OUTDIR)\nfoexp.dle" /implib:"$(OUTDIR)\nfoexp.lib" /libpath:"..\..\msdev60\lib" 
DEF_FILE= \
	".\gexp.def"
LINK32_OBJS= \
	"$(INTDIR)\3dsmtl.obj" \
	"$(INTDIR)\gexp.obj" \
	"$(INTDIR)\Savemli.obj" \
	"$(INTDIR)\gexp.res"

"$(OUTDIR)\nfoexp.dle" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("gexp.dep")
!INCLUDE "gexp.dep"
!ELSE 
!MESSAGE Warning: cannot find "gexp.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "gexp - Win32 NFO Release" || "$(CFG)" == "gexp - Win32 NFO Hybrid" || "$(CFG)" == "gexp - Win32 KEY Hybrid" || "$(CFG)" == "gexp - Win32 KEY Release" || "$(CFG)" == "gexp - Win32 R3 KEY Release" || "$(CFG)" == "gexp - Win32 R3 NFO Release"
SOURCE=.\3dsmtl.cpp

"$(INTDIR)\3dsmtl.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\gexp.cpp

"$(INTDIR)\gexp.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\gexp.rc

"$(INTDIR)\gexp.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\Savemli.cpp

"$(INTDIR)\Savemli.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

