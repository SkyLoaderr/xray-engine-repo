// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
// 15th generation by Oles.

#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_

#pragma once

#include <xrCore.h>

#pragma warning(disable:4995)
#include <d3d9.h>
#include <dplay8.h>
#pragma warning(default:4995)

// you must define ENGINE_BUILD then building the engine itself
// and not define it if you are about to build DLL
#ifndef NO_ENGINE_API
	#ifdef	ENGINE_BUILD
		#define DLL_API			__declspec(dllimport)
		#define ENGINE_API		__declspec(dllexport)
	#else
		#define DLL_API			__declspec(dllexport)
		#define ENGINE_API		__declspec(dllimport)
	#endif
#else
	#define ENGINE_API
	#define DLL_API
#endif // NO_ENGINE_API

// Our headers
#include "engine.h"
#include "defines.h"
#ifndef NO_XRLOG
#include "log.h"
#endif
#include "device.h"
#include "fs.h"
#include "properties.h"
#include "xrXRC.h"

#include "sound.h"

// TODO: reference additional headers your program requires here
#pragma comment( lib, "x:\\xrCore.lib"	)
#pragma comment( lib, "x:\\xrCDB.lib"	)
#pragma comment( lib, "x:\\xrSound.lib"	)
#pragma comment( lib, "x:\\xrLUA.lib"	)

#pragma comment( lib, "winmm.lib"		)

#pragma comment( lib, "d3d9.lib"		)
#pragma comment( lib, "dinput.lib"		)
#pragma comment( lib, "dxguid.lib"		)


#define LUABIND_NO_EXCEPTIONS
#define LUABIND_DONT_COPY_STRINGS
#define BOOST_THROW_EXCEPTION_HPP_INCLUDED
namespace boost {	ENGINE_API	void __stdcall throw_exception(const exception &A);	};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
