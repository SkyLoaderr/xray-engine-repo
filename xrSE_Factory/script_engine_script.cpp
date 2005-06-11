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
		return					((float(m_accumulated)*CPU::cycles2microsec));
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
#ifdef XRGAME_EXPORTS
	function	(L,	"device",						get_device);
#endif
}
