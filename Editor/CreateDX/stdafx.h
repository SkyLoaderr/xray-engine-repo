// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
// Third generation by Oles.

#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_

#pragma once
#define ENGINE_API 

//#define WIN32_LEAN_AND_MEAN			// Exclude rarely-used stuff from Windows headers
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
using namespace std;

// Property-set support
#define PropertyG(a)	__declspec( property( get=a ) )
#define PropertyP(a)	__declspec( property( put=a ) )
#define PropertyGP(a,b)	__declspec( property( get=a, put=b ) )

// CLASS ID type
typedef unsigned __int64	CLASS_ID;
#define MK_CLSID(a,b,c,d,e,f,g,h) \
	CLASS_ID(	((CLASS_ID(a)<<CLASS_ID(24))|(CLASS_ID(b)<<CLASS_ID(16))|(CLASS_ID(c)<<CLASS_ID(8))|(CLASS_ID(d)))<<CLASS_ID(32) | \
				((CLASS_ID(e)<<CLASS_ID(24))|(CLASS_ID(f)<<CLASS_ID(16))|(CLASS_ID(g)<<CLASS_ID(8))|(CLASS_ID(h))) )

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
#define _RELEASE(a)     { if (a){ a->Release(); a=NULL;}}

#define Log //
#define Msg //

struct Fvector{};
struct Fcolor{};

#include "FS.h"
// TODO: reference additional headers your program requires here

#define MAX_FOLDER_NAME    255
#define MAX_OBJ_NAME       64
#define MAX_OBJCLS_NAME    64
#define MAX_CLASS_SCRIPT   4096
#define MAX_LINK_NAME      64
#define MAX_LTX_ADD        16384
#define MAX_ADD_FILES_TEXT 1024

#define	AnsiString string

#define DEFINE_SVECTOR(type,sz,lst,it)\
	typedef svector<type,sz> lst;\
	typedef lst::iterator it;
#define DEFINE_VECTOR(type,lst,it)\
	typedef vector<type> lst;\
	typedef lst::iterator it;

DEFINE_VECTOR(bool,BOOLVec,BOOLIt);
DEFINE_VECTOR(BYTE,BYTEVec,BYTEIt);
DEFINE_VECTOR(WORD,WORDVec,WORDIt);
DEFINE_VECTOR(DWORD,DWORDVec,DWORDIt);
DEFINE_VECTOR(int,INTVec,INTIt);
DEFINE_VECTOR(float,FloatVec,FloatIt);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
