// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__81632403_DFD8_4A42_A4D3_0AFDD8EA0D25__INCLUDED_)
#define AFX_STDAFX_H__81632403_DFD8_4A42_A4D3_0AFDD8EA0D25__INCLUDED_7

#pragma once

#include <xrCore.h>

#pragma warning(disable:4995)
#include <d3dx9.h>
#include <commctrl.h>
#pragma warning(default:4995)

#define ENGINE_API
#define ECORE_API
#define XR_EPROPS_API
#include "clsid.h"
#include "defines.h"
#include "cl_log.h"
#include "xrCDB.h"
#include "_d3d_extensions.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>

#ifdef AI_COMPILER
#	include "smart_cast.h"
#endif
// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__81632403_DFD8_4A42_A4D3_0AFDD8EA0D25__INCLUDED_7)
