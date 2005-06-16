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

#pragma warning(disable:4251)
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

#pragma warning(disable:4996)
#pragma warning(disable:4995)

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

/**
#include <hash_map>
#include "../src/lobject.h"
#include "../src/lstring.h"

__declspec(dllimport) int  luaV_strcmp				(const TString *ls, const TString *rs);
__declspec(dllimport) void lua_push_string_object	(lua_State *L, TString *string);

class lua_string_holder {
public:
	lua_State		*m_state;
	TString			*m_object;
	int				m_reference;

public:
	inline	lua_string_holder	(lua_State *state, LPCSTR string)
	{
		VERIFY					(state);
		m_state					= state;
		m_object				= luaS_new(m_state,string);
		reference				();
	}

	inline	lua_string_holder	(lua_State *state, TString *object)
	{
		VERIFY					(state);
		m_state					= state;
		VERIFY					(object);
		m_object				= object;
		reference				();
	}

	inline	lua_string_holder	(const lua_string_holder &object)
	{
		*this					= object;
	}

	inline	lua_string_holder&	operator=	(const lua_string_holder &object)
	{
		m_state					= object.m_state;
		m_object				= object.m_object;
		reference				();
		return					(*this);
	}

	inline	~lua_string_holder	()
	{
		printf					("- %08X\n",*(int*)(void**)&m_object);
		luaL_unref				(m_state,LUA_REGISTRYINDEX,m_reference);
	}

	inline	void	reference	()
	{
		printf					("+ %08X\n",*(int*)(void**)&m_object);
		lua_push_string_object	(m_state,m_object);
		m_reference				= luaL_ref(m_state,LUA_REGISTRYINDEX);
	}
};

struct TString_hash_compare {
	const static size_t bucket_size = 4;
	const static size_t min_buckets = 8;

	inline	size_t	operator()	(const lua_string_holder &str) const
	{
		return		(str.m_object->tsv.hash);
	}
	
	inline	bool	operator()	(const lua_string_holder &str0, const lua_string_holder &str1) const
	{
		return		(luaV_strcmp(str0.m_object,str1.m_object) < 0);
	}
};

typedef std::hash_map<lua_string_holder,int,TString_hash_compare> lua_string_map;
/**/

lua_State *open_lua()
{
	lua_State				*L = 0;
	L						= lua_open();

	if (!L) {
		lua_error			(L);
		return				(0);
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
			.def_readwrite("m_recurse_mark",&profile_timer_script::m_recurse_mark)
			.def(constructor<>())
			.def(constructor<profile_timer_script&>())
			.def(const_self + profile_timer_script())
			.def(const_self < profile_timer_script())
			.def(tostring(self))
			.def("start",&profile_timer_script::start)
			.def("stop",&profile_timer_script::stop)
			.def("time",&profile_timer_script::time)
	];

	return					(L);
}

struct test_class {
	int		m_reference_count;
	int		m_lua_reference;
};

#include <boost/intrusive_ptr.hpp>

typedef boost::intrusive_ptr<test_class>	test_ptr;

void intrusive_ptr_add_ref(test_class *p)
{
	p->m_reference_count++;
}

void intrusive_ptr_release(test_class *p)
{
	p->m_reference_count--;
	if (!p->m_reference_count)
		printf	("we are completely unreferenced!\n");
}

void profiler_test	()
{
	test_ptr	a = xr_new<test_class>();
	test_ptr	b = a;
	a			= 0;
	b			= 0;

/**
	lua_string_map			strings;
	
//	strings.insert			(std::make_pair(lua_string_holder(L,"test0"),0));
//	strings.insert			(std::make_pair(lua_string_holder(L,"test1"),1));
//	strings.insert			(std::make_pair(lua_string_holder(L,"test2"),2));
//	strings.insert			(std::make_pair(lua_string_holder(L,"test3"),3));
//	strings.insert			(std::make_pair(lua_string_holder(L,"test4"),4));
//	strings.insert			(std::make_pair(lua_string_holder(L,"test5"),5));

	strings[lua_string_holder(L,"test0")]	= 0;
	strings[lua_string_holder(L,"test1")]	= 1;
	strings[lua_string_holder(L,"test2")]	= 2;
	strings[lua_string_holder(L,"test3")]	= 3;
	strings[lua_string_holder(L,"test4")]	= 4;
	strings[lua_string_holder(L,"test5")]	= 5;

	strings[lua_string_holder(L,"test3")] = 100;
/**/

	lua_State				*L1 = open_lua();
	lua_State				*L2 = open_lua();

	lua_dofile				(L1,"x:\\profiler.script");
	lua_dostring			(L1,"setup_hook()");
	lua_dofile				(L1,"x:\\profile_test.script");
	lua_dostring			(L1,"stats()");

	lua_dofile				(L2,"x:\\profiler.script");
	lua_dostring			(L2,"setup_hook()");
	lua_dofile				(L2,"x:\\profile_test.script");
	lua_dostring			(L2,"stats()");

/**
	lua_setgcthreshold		(L,0);
	typedef xr_vector<TString*>	old_strings_type;
	old_strings_type		old_strings;

	{
		lua_string_map::const_iterator	I = strings.begin();
		lua_string_map::const_iterator	E = strings.end();
		for ( ; I != E; ++I) {
			printf				("[%s][%d]\n",getstr((*I).first.m_object),(*I).second);
			old_strings.push_back	((*I).first.m_object);
		}
	}

	strings.clear			();

	lua_dofile				(L,"x:\\profile_test.script");
	lua_dofile				(L,"x:\\profile_test.script");
	lua_dofile				(L,"x:\\profile_test.script");
	lua_dofile				(L,"x:\\profile_test.script");
	lua_dofile				(L,"x:\\profile_test.script");
	lua_dofile				(L,"x:\\profile_test.script");
	lua_dofile				(L,"x:\\profile_test.script");

	lua_setgcthreshold		(L,0);

	{
		old_strings_type::const_iterator	I = old_strings.begin();
		old_strings_type::const_iterator	E = old_strings.end();
		for ( ; I != E; ++I)
			printf				("[%s]\n",getstr(*I));
	}
/**/

	lua_close				(L1);
	lua_close				(L2);
}