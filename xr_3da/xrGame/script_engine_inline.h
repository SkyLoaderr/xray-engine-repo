////////////////////////////////////////////////////////////////////////////
//	Module 		: script_engine_inline.h
//	Created 	: 01.04.2004
//  Modified 	: 01.04.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script Engine inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	void CScriptEngine::add_script_processor		(LPCSTR processor_name, CScriptProcessor *script_processor)
{
	CScriptProcessorStorage::const_iterator	I = m_script_processors.find(processor_name);
	VERIFY									(I == m_script_processors.end());
	m_script_processors.insert				(std::make_pair(processor_name,script_processor));
}

CScriptProcessor *CScriptEngine::script_processor	(LPCSTR processor_name) const
{
	CScriptProcessorStorage::const_iterator	I = m_script_processors.find(processor_name);
	if ((I != m_script_processors.end()))
		return								((*I).second);
	return									(0);
}

IC	void CScriptEngine::set_current_thread			(CScriptStackTracker *new_thread)
{
#ifdef DEBUG
	VERIFY									((!m_current_thread && new_thread) || (m_current_thread && !new_thread));
#endif
	m_current_thread						= new_thread;
}

IC	CScriptStackTracker *CScriptEngine::current_thread			()
{
	return									(m_current_thread);
}

IC	CScriptStackTracker	&CScriptEngine::script_stack_tracker	()
{
	if (!current_thread())
		return								(*this);
	return									(*current_thread());
}

IC	void CScriptEngine::reload_modules		(bool flag)
{
	m_reload_modules						= flag;
}

template <typename _result_type>
IC	bool CScriptEngine::functor(LPCSTR function_to_call, luabind::functor<_result_type> &lua_function)
{
	if (!xr_strlen(function_to_call))
		return				(false);

	string256				name_space, function;
	LPCSTR					I = function_to_call, J = 0;
	for ( ; ; J=I,++I) {
		I					= strchr(I,'.');
		if (!I)
			break;
	}
	strcpy					(name_space,"_G");
	if (!J)
		strcpy				(function,function_to_call);
	else {
		Memory.mem_copy		(name_space,function_to_call, u32(J - function_to_call)*sizeof(char));
		name_space[u32(J - function_to_call)] = 0;
		strcpy				(function,J + 1);
	}

	if	(!object(name_space,function,LUA_TFUNCTION))
		return				(false);

	luabind::object			lua_namespace	= ai().script_engine().name_space(name_space);
	lua_function			= luabind::object_cast<luabind::functor<_result_type> >(lua_namespace[function]);

	return					(true);
}
