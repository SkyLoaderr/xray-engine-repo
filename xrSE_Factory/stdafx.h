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

#include "clsid_game.h"

#define BOOST_THROW_EXCEPTION_HPP_INCLUDED

namespace boost {void __stdcall throw_exception(const std::exception &A);};

#include "smart_cast.h"
