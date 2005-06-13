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

#include <ostream>

using namespace luabind;

void profiler_log(LPCSTR msg)
{
	printf						("%s\n",msg);
}

struct profile_timer_script {
	u64							m_start_time;
	u64							m_accumulated;
	int							m_recurse_mark;
	
	IC								profile_timer_script	()
	{
		m_start_time			= 0;
		m_accumulated			= 0;
		m_recurse_mark			= 0;
	}

	IC								profile_timer_script	(const profile_timer_script &profile_timer)
	{
		*this					= profile_timer;
	}

	IC		profile_timer_script&	operator=				(const profile_timer_script &profile_timer)
	{
//		VERIFY					(!profile_timer.m_start_time);
//		VERIFY					(!profile_timer.m_recurse_mark);
		m_start_time			= profile_timer.m_start_time;
		m_accumulated			= profile_timer.m_accumulated;
		m_recurse_mark			= profile_timer.m_recurse_mark;
		return					(*this);
	}

	IC		bool					operator<				(const profile_timer_script &profile_timer) const
	{
//		VERIFY					(!profile_timer.m_start_time);
//		VERIFY					(!profile_timer.m_recurse_mark);
//		VERIFY					(!m_start_time);
//		VERIFY					(!m_recurse_mark);
		return					(m_accumulated < profile_timer.m_accumulated);
	}

	IC		void					start					()
	{
		u64						temp = CPU::GetCycleCount();
		if (!m_recurse_mark) {
			VERIFY				(!m_start_time);
			m_start_time		= temp;
		}
		++m_recurse_mark;
	}

	IC		void					stop					()
	{
		u64						temp = CPU::GetCycleCount();
		
		VERIFY					(m_recurse_mark);
		--m_recurse_mark;
		
		VERIFY					(temp > m_start_time);
		VERIFY					(m_start_time);
		
		if (m_recurse_mark)
			return;
		
		m_accumulated			+= temp - m_start_time;
		m_start_time			= 0;
	}

	IC		float					time					() const
	{
		return					((float(m_accumulated)*CPU::cycles2microsec) + 1000.f);
	}
};

IC	profile_timer_script	operator+	(const profile_timer_script &portion0, const profile_timer_script &portion1)
{
	profile_timer_script	result;
	result.m_accumulated	= portion0.m_accumulated + portion1.m_accumulated;
	return					(result);
}

IC	std::ostream& operator<<(std::ostream &stream, profile_timer_script &portion)
{
	stream					<< (float(portion.m_accumulated)*CPU::cycles2microsec);
	return					(stream);
}

IC	void	debug_break	()
{
	printf("debug_break called\n");
}

void profiler_test	()
{
	lua_State				*L = 0;
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

	function				(L,"log",profiler_log);
	function				(L,"debug_break",debug_break);

	module(L)[
		class_<profile_timer_script>("profile_timer")
			.def(constructor<>())
			.def(constructor<profile_timer_script&>())
			.def(const_self + profile_timer_script())
			.def(const_self < profile_timer_script())
			.def(tostring(self))
			.def("start",&profile_timer_script::start)
			.def("stop",&profile_timer_script::stop)
			.def("time",&profile_timer_script::time)
	];

	lua_dofile				(L,"x:\\profiler.script");
	lua_dostring			(L,"setup_hook()");
	lua_dofile				(L,"x:\\profile_test.script");
//	lua_dostring			(L,"clear_hook()");
	lua_dostring			(L,"stats()");

	lua_close				(L);
}