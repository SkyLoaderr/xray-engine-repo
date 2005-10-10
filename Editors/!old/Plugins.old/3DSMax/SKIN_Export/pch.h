//-----------------------------------------------------------------------------
// File: Pch.h
//
// Desc: Header file to precompile
//
// Copyright (C) 1998-2000 Microsoft Corporation. All Rights Reserved.
//-----------------------------------------------------------------------------
#ifndef __PCH__H
#define __PCH__H

#define WIN32_LEAN_AND_MEAN			// Exclude rarely-used stuff from Windows headers
#define STRICT						// Enable strict syntax

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
#include <commctrl.h>
#include <direct.h>
#include <commdlg.h>

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
#include <assert.h>

#include "d3d8.h"
#include "d3dx8.h"

#ifndef DEBUG
#pragma inline_depth	( 255 )
#pragma inline_recursion( on  )
#pragma intrinsic		(abs, fabs, fmod, sin, cos, tan, asin, acos, atan, sqrt, exp, log, log10, strcpy, strcat)
#endif

// Warnings
#pragma warning (disable : 4786 )		// too long names
#pragma warning (disable : 4251 )		// object needs DLL interface

// stl
#include <algorithm>
#include <numeric>
#include <vector>
#include <stack>
#include <list>
#include <set>
#include <map>
#include <deque>
#include <string>

template <class T>
T min(T a, T b) { return std::_MIN(a,b); }
template <class T>
T max(T a, T b) { return std::_MAX(a,b); }
using std::string;

#include "Max.h"
#include "resource.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"
#include "phyexp.h"
#include "bipexp.h"
#include "stdmat.h"

#define IC			__forceinline
#define VERIFY		assert
#define R_ASSERT	assert
#define ENGINE_API

// Our headers
#define NO_XR_LIGHT
#define NO_XR_MATERIAL
#define NO_XR_3DBUFFER
#define NO_XR_3DLISTENER
#define NO_XR_VERTEX
#define NODEFAULT R_ASSERT(0)
#include "math.h"
#include "vector.h"
#include "defines.h"
#include "fs.h"
#include "log.h"

#endif