////////////////////////////////////////////////////////////////////////////
//	Module 		: script_engine_inline.h
//	Created 	: 01.04.2004
//  Modified 	: 01.04.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script Engine inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#ifdef XRGAME_EXPORTS
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
#endif

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
	if (!xr_strlen(function_to_call))
		return				(false);

	string256				name_space, function;

	parse_script_namespace	(function_to_call,name_space,function);
	add_file				(name_space);
	process					();

	if	(!object(name_space,function,LUA_TFUNCTION))
		return				(false);

	luabind::object			lua_namespace	= ai().script_engine().name_space(name_space);
	lua_function			= luabind::object_cast<luabind::functor<_result_type> >(lua_namespace[function]);

	return					(true);
}

template <typename _result_type>
IC	luabind::functor<_result_type> CScriptEngine::create_object_creator	(LPCSTR class_name, LPCSTR arguments)
{
	string1024				function_body, function_name, _class_name;
	strcpy					(_class_name,class_name);
	LPSTR					I;
	while (0!=(I = strchr(_class_name,'.')))
		*I					= '_';
	
	sprintf					(function_name,"create_%s",_class_name);
	if (!xr_strlen(arguments))
		sprintf				(function_body,"function %s(object_factory) object_factory:set_instance(%s()) end",function_name,class_name);
	else
		sprintf				(function_body,"function %s(object_factory,%s) object_factory:set_instance(%s(%s)) end",function_name,arguments,class_name,arguments);
	lua_dostring			(lua(),function_body);
	luabind::functor<_result_type>	result;
	R_ASSERT				(functor(function_name,result));
	return					(result);
}
