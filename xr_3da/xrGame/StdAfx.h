// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__8822E545_09B4_4016_9034_721F7BF4B88B__INCLUDED_)
#define AFX_STDAFX_H__8822E545_09B4_4016_9034_721F7BF4B88B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include "..\stdafx.h"

#pragma warning( 4 : 4018 )
#pragma warning( 4 : 4244 )

#include "clsid_game.h"

#include "..\xr_object.h"
#include "..\xr_creator.h"
#include "..\sound.h"
#include "..\xr_ini.h"
#include "..\xr_input.h"
#include "..\x_ray.h"
#include "..\std_classes.h"
#include "..\GameFont.h"

#include "..\NET_Server.h"
#include "..\NET_Client.h"

#include "..\render.h"
#include "..\3dsound.h"
#include "..\fmesh.h"
#include "..\BodyInstance.h"

#define FUNCCALL __cdecl
#define _CALLING __cdecl

#include <ode/ode.h>

#include "Level.h"

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__8822E545_09B4_4016_9034_721F7BF4B88B__INCLUDED_)
