////////////////////////////////////////////////////////////////////////////
//	Module 		: script_engine_inline.h
//	Created 	: 01.04.2004
//  Modified 	: 01.04.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script Engine inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	void CScriptEngine::add_script_process		(LPCSTR process_name, CScriptProcess *script_process)
{
	CScriptProcessStorage::const_iterator	I = m_script_processes.find(process_name);
	VERIFY									(I == m_script_processes.end());
	m_script_processes.insert				(std::make_pair(process_name,script_process));
}

CScriptProcess *CScriptEngine::script_process	(LPCSTR process_name) const
{
	CScriptProcessStorage::const_iterator	I = m_script_processes.find(process_name);
	if ((I != m_script_processes.end()))
		return								((*I).second);
	return									(0);
}

IC	void CScriptEngine::set_current_thread		(CScriptStackTracker *new_thread)
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

IC	void CScriptEngine::parse_script_namespace(LPCSTR function_to_call, LPSTR name_space, LPSTR function)
{
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
}

template <typename _result_type>
IC	bool CScriptEngine::functor(LPCSTR function_to_call, luabind::functor<_result_type> &lua_function)
{
	luabind::object			object;
	if (!function_object(function_to_call,object))
		return				(false);

	lua_function			= luabind::object_cast<luabind::functor<_result_type> >(object);

	return					(true);
}
