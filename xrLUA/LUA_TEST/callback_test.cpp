#include "stdafx.h"

#pragma warning(push)
#pragma warning(disable:4995)
#pragma warning(disable:4530)
#include "xrCore.h"
#pragma comment(lib,"x:/xrCore.lib")
#pragma warning(pop)

// Lua
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#pragma comment(lib,"x:/xrLUA.lib")

// Lua-bind
#pragma warning(disable:4995)
#pragma warning(disable:4530)
#pragma warning(disable:4244)
#include "luabind/luabind.hpp"
#include "luabind/adopt_policy.hpp"
#include "luabind/dependency_policy.hpp"
#include "luabind/return_reference_to_policy.hpp"
#include "luabind/out_value_policy.hpp"
#include "luabind/discard_result_policy.hpp"
#include "luabind/iterator_policy.hpp"
#include <luabind/operator.hpp>
#pragma warning(default:4244)
#pragma warning(default:4995)
#pragma warning(default:4530)

const u32 callback_count = 2;

namespace callback_test {
lua_State *L			 = 0;

IC	u32 _callback_count()
{
	return	(callback_count);
}

IC	lua_State *lua()
{
	return			(L);
}

#include "intrusive_ptr.h"

struct thread_reference_intrusive_base : public intrusive_base {
	int			m_thread_reference;

	IC			thread_reference_intrusive_base	() : m_thread_reference(LUA_REFNIL)
	{
	}

	IC			thread_reference_intrusive_base	(int reference) : m_thread_reference(reference)
	{
	}

	template <typename T>
	IC	void	_release	(T*object)
	{
		if (m_thread_reference != LUA_REFNIL)
			luabind::detail::unref	(lua(),m_thread_reference);
		xr_delete			(object);
	}
};

typedef intrusive_ptr<thread_reference_intrusive_base,thread_reference_intrusive_base> trib;
trib	current_thread_reference = 0;


struct CCallbackWrapper {
public:
	typedef luabind::functor<void> functor;

public:
	trib		m_ref;
	functor		*m_function;

public:
	
	IC	CCallbackWrapper	()
	{
		m_function	= 0;
		m_ref		= 0;
	}

	IC	CCallbackWrapper	(const functor &function)
	{
		m_function	= xr_new<functor>(function);
		m_ref		= current_thread_reference;
	}

	IC	CCallbackWrapper &operator=	(const CCallbackWrapper &wrapper)
	{
		m_function	= wrapper.m_function ? xr_new<functor>(*wrapper.m_function) : 0;
		m_ref		= wrapper.m_ref;
		return		(*this);
	}

	IC	~CCallbackWrapper	()
	{
		clear		();
	}

	IC	void	clear		()
	{
		xr_delete	(m_function);
		m_ref		= 0;
	}

	IC	functor *function	()
	{
		return		(m_function);
	}
};

//CCallbackWrapper g_callback[callback_count];
luabind::functor<void> *g_callback[callback_count];

void set_callback(int index, const luabind::functor<void> &function)
{
//	g_callback[index]		= CCallbackWrapper(function);
	g_callback[index]		= xr_new<luabind::functor<void> >(function);
}

void callback_test	()
{
	L						= lua_open();

	if (!L) {
		lua_error			(L);
		return;
	}

	luaopen_base			(L);
	luaopen_string			(L);
	luaopen_math			(L);
	luaopen_table			(L);
	luaopen_debug			(L);

	luabind::open			(L);

	luabind::function		(L,"set_callback",&set_callback);
	luabind::function		(L,"callback_count",&_callback_count);

	LPCSTR					s = "callback.main()";
	lua_dofile				(L,"x:\\callback.script");
	for (int i=0; i<1000; ++i) {
		int					top = lua_gettop(L);
		lua_State			*t = lua_newthread(L);
//		current_thread_reference = xr_new<thread_reference_intrusive_base>(luabind::detail::ref(L));
//		int thread_reference = luabind::detail::ref(L);
//		luabind::detail::getref(L,thread_reference);
//		int					temp = luaL_ref	(L,LUA_REGISTRYINDEX);
		int					err = luaL_loadbuffer(t,s,xr_strlen(s),"@_thread_main");
		if (err) {
			printf			("ERROR : %s\n",lua_tostring(L,-1));
			break;
		}
		lua_resume			(t,0);
//		current_thread_reference = 0;
//		luabind::detail::unref(L,thread_reference);
//		xr_delete			(thread_reference);
		lua_settop			(L,top);
		lua_setgcthreshold	(L,0);
		for (int j=0; j<1; ++j) {
			lua_setgcthreshold	(L,0);
			for (int k=0; k<callback_count; ++k) {
//				if (g_callback[k].function())
//					(*(g_callback[k].function()))	(k*1+0,k*1+1,k*1+2,k*1+3);
				if (g_callback[k] && g_callback[k]->is_valid())
					(*(g_callback[k]))	(k*1+0,k*1+1,k*1+2,k*1+3);
			}
		}
		lua_setgcthreshold	(L,0);
//		luabind::detail::getref(L,thread_reference);
	}

	for (int k=0; k<callback_count; ++k)
//		g_callback[k].clear	();
		xr_delete(g_callback[k]);

	lua_setgcthreshold		(L,0);
	printf					("Lua is closed!\n");
	lua_close				(L);
}
}
