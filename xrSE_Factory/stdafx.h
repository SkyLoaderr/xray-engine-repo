////////////////////////////////////////////////////////////////////////////
//	Module 		: stdafx.h
//	Created 	: 18.06.2004
//  Modified 	: 18.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Precompiled header creator
////////////////////////////////////////////////////////////////////////////

#pragma once

#include <xrCore.h>

#define WIN32_LEAN_AND_MEAN

#define ENGINE_API
#define ECORE_API
#define DLL_API					__declspec(dllexport)
#define TIXML_USE_STL

#include "clsid_game.h"

#define BOOST_THROW_EXCEPTION_HPP_INCLUDED

namespace boost {void __stdcall throw_exception(const std::exception &A);};

#include "smart_cast.h"

#if XRAY_EXCEPTIONS
#	define	THROW(xpr)				if (!(xpr)) {throw __FILE__LINE__"\""#xpr"\"";}
#	define	THROW2(xpr,msg0)		if (!(xpr)) {throw *shared_str().sprintf("%s \"%s\" : %s",__FILE__LINE__,#xpr,msg0 ? msg0 : "");}
#	define	THROW3(xpr,msg0,msg1)	if (!(xpr)) {throw *shared_str().sprintf("%s \"%s\" : %s, %s",__FILE__LINE__,#xpr,msg0 ? msg0 : "",msg1 ? msg1 : "");}
#else
#	define	THROW					VERIFY
#	define	THROW2					VERIFY2
#	define	THROW3					VERIFY3
#endif
