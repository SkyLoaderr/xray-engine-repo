// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__81632403_DFD8_4A42_A4D3_0AFDD8EA0D25__INCLUDED_)
#define AFX_STDAFX_H__81632403_DFD8_4A42_A4D3_0AFDD8EA0D25__INCLUDED_

#pragma once

#include <xrCore.h>

#include <d3dx8.h>
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>

#define RGBA_GETALPHA(rgb)      ((rgb) >> 24)
#define RGBA_GETRED(rgb)        (((rgb) >> 16) & 0xff)
#define RGBA_GETGREEN(rgb)      (((rgb) >> 8) & 0xff)
#define RGBA_GETBLUE(rgb)       ((rgb) & 0xff)

#ifndef	NDEBUG
#define X_TRY 
#define X_CATCH if (0)
#else
#define X_TRY try
#define X_CATCH catch(...)
#endif

#define ENGINE_API				// fake, to enable sharing with engine
#include "clsid.h"
#include "defines.h"
#include "log.h"
#include "fs.h"

#include "_d3d_extensions.h"
#include "communicate.h"
extern b_params	g_params;

#include "build.h"

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__81632403_DFD8_4A42_A4D3_0AFDD8EA0D25__INCLUDED_)
