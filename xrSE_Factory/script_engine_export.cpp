////////////////////////////////////////////////////////////////////////////
//	Module 		: script_engine_export.cpp
//	Created 	: 01.04.2004
//  Modified 	: 22.06.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script Engine export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_engine.h"

#pragma warning(push)
#pragma warning(disable:4005)
#include "script_engine_export.h"
#pragma warning(pop)

template <typename TList> struct Register
{
	ASSERT_TYPELIST(TList);

	static void _Register(lua_State *L)
	{
		Register<TList::Tail>::_Register(L);
		TList::Head::script_register(L);
	}
};

template <> struct Register<Loki::NullType>
{
	static void _Register(lua_State *L)
	{
	}
};

void CScriptEngine::export_classes()
{
	Register<Loki::TL::DerivedToFrontAll<script_type_list>::Result>::_Register(lua());
}
