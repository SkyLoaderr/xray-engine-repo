////////////////////////////////////////////////////////////////////////////
//	Module 		: script_engine_script.cpp
//	Created 	: 25.12.2002
//  Modified 	: 13.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife Simulator script engine export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_engine.h"
#include "script_space.h"
#include "ai_space.h"
#include "script_debugger.h"

#include <ostream>
#include <luabind/operator.hpp>

using namespace luabind;

void LuaLog(LPCSTR caMessage)
{
	ai().script_engine().script_log	(ScriptStorage::eLuaMessageTypeMessage,"%s",caMessage);
#ifdef USE_DEBUGGER
	if( ai().script_engine().debugger() ){
		ai().script_engine().debugger()->Write(caMessage);
	}
#endif
}

void ErrorLog(LPCSTR caMessage)
{
	ai().script_engine().script_log	(ScriptStorage::eLuaMessageTypeError,"%s",caMessage);
#ifdef USE_DEBUGGER
	if( ai().script_engine().debugger() ){
		ai().script_engine().debugger()->Write(caMessage);
	}
#endif
}

void FlushLogs()
{
#ifdef DEBUG
	FlushLog();
	ai().script_engine().flush_log();
#endif
}

void verify_if_thread_is_running()
{
	THROW2	(ai().script_engine().current_thread(),"coroutine.yield() is called outside the LUA thread!");
}

bool editor()
{
#ifdef XRGAME_EXPORTS
	return		(false);
#else
	return		(true);
#endif
}

#ifdef XRGAME_EXPORTS
CRenderDevice *get_device()
{
	return		(&Device);
}
#endif

int bit_and(int i, int j)
{
	return			(i & j);
}

int bit_or(int i, int j)
{
	return			(i | j);
}

int bit_xor(int i, int j)
{
	return			(i ^ j);
}

int bit_not(int i)
{
	return			(~i);
}

LPCSTR user_name()
{
	return			(Core.UserName);
}

void prefetch_module(LPCSTR file_name)
{
	ai().script_engine().process_file(file_name);
}

struct profile_timer_script {
	CStatTimer					m_timer;
	int							m_recurse_mark;
	
	IC								profile_timer_script	()
	{
		m_recurse_mark			= 0;
	}

	IC								profile_timer_script	(const profile_timer_script &profile_timer)
	{
		*this					= profile_timer;
	}

	IC		profile_timer_script&	operator=				(const profile_timer_script &profile_timer)
	{
		m_timer					= profile_timer.m_timer;
		m_recurse_mark			= profile_timer.m_recurse_mark;
		return					(*this);
	}

	IC		bool					operator<				(const profile_timer_script &profile_timer) const
	{
		return					(m_timer.GetElapsed_ticks() < profile_timer.m_timer.GetElapsed_ticks());
	}

	IC		void					start					()
	{
		if (m_recurse_mark) {
			++m_recurse_mark;
			return;
		}

		++m_recurse_mark;
		m_timer.Begin			();
	}

	IC		void					stop					()
	{
		THROW					(m_recurse_mark);
		--m_recurse_mark;
		
		if (m_recurse_mark)
			return;
		
		m_timer.End				();
	}

	IC		float					time					() const
	{
		return					(float(m_timer.GetElapsed_sec())*1000000.f);
	}
};

IC	profile_timer_script	operator+	(const profile_timer_script &portion0, const profile_timer_script &portion1)
{
	profile_timer_script	result;
	result.m_timer.accum	= portion0.m_timer.accum + portion1.m_timer.accum;
	result.m_timer.count	= portion0.m_timer.count + portion1.m_timer.count;
	return					(result);
}

IC	std::ostream& operator<<(std::ostream &stream, profile_timer_script &timer)
{
	stream					<< timer.time();
	return					(stream);
}

#ifdef XRGAME_EXPORTS
ICF	u32	script_time_global	()	{ return Device.dwTimeGlobal; }
#else
ICF	u32	script_time_global	()	{ return 0; }
#endif

#pragma optimize("s",on)
void CScriptEngine::script_register(lua_State *L)
{
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

	function	(L,	"log",							LuaLog);
	function	(L,	"error_log",					ErrorLog);
	function	(L,	"flush",						FlushLogs);
	function	(L,	"prefetch",						prefetch_module);
	function	(L,	"verify_if_thread_is_running",	verify_if_thread_is_running);
	function	(L,	"editor",						editor);
	function	(L,	"bit_and",						bit_and);
	function	(L,	"bit_or",						bit_or);
	function	(L,	"bit_xor",						bit_xor);
	function	(L,	"bit_not",						bit_not);
	function	(L, "user_name",					user_name);
	function	(L, "time_global",					script_time_global);
#ifdef XRGAME_EXPORTS
	function	(L,	"device",						get_device);
#endif
}
