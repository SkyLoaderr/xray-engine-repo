#include "stdafx.h"

struct lua_State;

#include <stdio.h>
#include "script_class.h"

#include "script_class1.h"
#include "script_class0.h"

#include <typeinfo.h>

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

template <typename TFullList> struct DynamicCast
{
	ASSERT_TYPELIST(TFullList);

	template <typename TList, typename T> struct Helper2
	{
		typedef typename TList::Head Head;

		static void Register(lua_State *L)
		{
			Helper2<TList::Tail,T>::Register(L);
			declare<Loki::SuperSubclassStrict<Head,T>::value>();
		}

		template <bool _1>
		static void declare()
		{
		}

		template <>
		static void declare<true>()
		{
			printf("casting \"%s\" to \"%s\"",typeid(Head).name(),typeid(T).name());
		}
	};

	template <typename T> struct Helper2<Loki::NullType,T>
	{
		static void Register(lua_State *L)
		{
		}
	};

	template <typename TList> struct Helper
	{
		static void Register(lua_State *L)
		{
			Helper<TList::Tail>::Register(L);
			Helper2<TFullList,TList::Head>::Register(L);
		}
	};

	template <> struct Helper<Loki::NullType>
	{
		static void Register(lua_State *L)
		{
		}
	};

	static void Register(lua_State *L)
	{
		Helper<TFullList>::Register(L);
	}
};

void script_test()
{
	Register<Loki::TL::DerivedToFrontAll<script_type_list>::Result>::_Register(0);
	DynamicCast<script_type_list>::Register(0);
}
