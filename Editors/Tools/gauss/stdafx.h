// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
// Third generation by Oles.

#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_

#pragma once
#define ENGINE_API 

#define WIN32_LEAN_AND_MEAN			// Exclude rarely-used stuff from Windows headers
#define STRICT						// Enable strict syntax
#define IDIRECTPLAY2_OR_GREATER

#define NOGDICAPMASKS
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOKEYSTATES
#define NOCLIPBOARD
#define NODRAWTEXT
#define NONLS
#define NOMEMMGR
#define NOMETAFILE
#define NOMINMAX
#define NOSERVICE
#define NOCOMM
#define NOHELP
#define NOPROFILER
#define NOMCX

#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <msacm.h>
#include <tchar.h>
#include <Commdlg.h>

#define DIRECTSOUND_VERSION 0x0700
#define DIRECTINPUT_VERSION 0x0700

#include <d3d8.h>
#include <d3dx8.h>
#include <dinput.h>
#include <dsound.h>
#include <dplobby.h>
#include <dplay.h>

#include <malloc.h>
#ifdef DEBUG
#include <crtdbg.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <process.h>

#define IC				__forceinline

// Warnings
#pragma warning (disable : 4786 )		// too long names
#pragma warning (disable : 4503 )		// decorated name length exceeded
#pragma warning (disable : 4251 )		// object needs DLL interface

// stl
#include <algorithm>
#include <numeric>
#include <limits>
#include <vector>
#include <stack>
#include <list>
#include <set>
#include <map>
#include <deque>
#include <string>
#include <sys\utime.h>
using namespace std;

// Property-set support
#define PropertyG(a)	__declspec( property( get=a ) )
#define PropertyP(a)	__declspec( property( put=a ) )
#define PropertyGP(a,b)	__declspec( property( get=a, put=b ) )

enum TMsgDlgType { mtWarning, mtError, mtInformation, mtConfirmation, mtCustom };
enum TMsgDlgBtn { mbYes, mbNo, mbOK, mbCancel, mbAbort, mbRetry, mbIgnore, mbAll, mbNoToAll, mbYesToAll, mbHelp };
typedef TMsgDlgBtn TMsgDlgButtons[mbHelp];

// Our headers
#define NO_XR_NETWORK
#define	NO_XR_VERTEX

#ifdef DEBUG
#define R_ASSERT	_ASSERT
#define VERIFY		_ASSERT
#else
#define R_ASSERT
#define VERIFY  
#endif
#define NODEFAULT 

#define _FREE(x)		{ if(x) { free(x); (x)=NULL; } }
#define _DELETE(a)      {delete(a); (a)=NULL;}
#define _DELETEARRAY(a) {delete[](a); (a)=NULL;}

#define	AnsiString string

#include "clsid.h"
#include "vector.h"
#include "fixedvector.h"
#include "math.h"

#include "Log.h"
#include "Engine.h"
// TODO: reference additional headers your program requires here

#pragma comment( lib, "imagehlp.lib"	)
#pragma comment( lib, "winmm.lib"		)

#pragma comment( lib, "d3d8.lib"		)
#pragma comment( lib, "d3dx8.lib"		)
#pragma comment( lib, "dxerr8.lib"		)

#pragma comment( lib, "dsound.lib"		)
#pragma comment( lib, "dinput.lib"		)
#pragma comment( lib, "dxguid.lib"		)

#define DEFINE_SVECTOR(type,sz,lst,it)\
	typedef svector<type,sz> lst;\
	typedef lst::iterator it;

DEFINE_VECTOR(bool,BOOLVec,BOOLIt);
DEFINE_VECTOR(BYTE,BYTEVec,BYTEIt);
DEFINE_VECTOR(WORD,WORDVec,WORDIt);
DEFINE_VECTOR(DWORD,DWORDVec,DWORDIt);
DEFINE_VECTOR(int,INTVec,INTIt);
DEFINE_VECTOR(float,FloatVec,FloatIt);
DEFINE_VECTOR(Fplane,PlaneVec,PlaneIt);      
DEFINE_VECTOR(Fvector2,Fvector2Vec,Fvector2It);
DEFINE_VECTOR(Fvector,FvectorVec,FvectorIt);
DEFINE_VECTOR(AnsiString,AStringVec,AStringIt);
DEFINE_VECTOR(LPCSTR,LPCSTRVec,LPCSTRIt);

struct st_Version{
    union{
        struct{
            int f_age;
            int res0;
        };
        __int64 ver;
    };
    st_Version   (){reset();}
    int size	(){return sizeof(st_Version);}
    bool operator == (st_Version& v)	{return v.f_age==f_age;}
    void reset	(){ver=0;}
};

#ifdef _LW_SHADER
	#define _EDITOR_FILE_NAME_ "lw_shader"
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
