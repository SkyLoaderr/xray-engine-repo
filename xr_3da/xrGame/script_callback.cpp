////////////////////////////////////////////////////////////////////////////
//	Module 		: script_callback.cpp
//	Created 	: 06.02.2004
//  Modified 	: 28.06.2004
//	Author		: Sergey Zhemeitsev and Dmitriy Iassenev
//	Description : XRay script callbacks
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_callback.h"

CScriptCallback::~CScriptCallback	()
{
	xr_delete(m_lua_function);
	xr_delete(m_lua_object);
}
