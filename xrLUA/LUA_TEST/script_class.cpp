#include "stdafx.h"

struct lua_State;

#include <stdio.h>
#include "script_class.h"

#include "script_class1.h"
#include "script_class0.h"

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

void script_test()
{
	Register<Loki::TL::DerivedToFrontAll<script_type_list>::Result>::_Register(0);
}
