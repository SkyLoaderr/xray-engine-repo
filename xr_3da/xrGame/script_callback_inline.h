////////////////////////////////////////////////////////////////////////////
//	Module 		: script_callback_inline.h
//	Created 	: 06.02.2004
//  Modified 	: 28.06.2004
//	Author		: Sergey Zhemeitsev and Dmitriy Iassenev
//	Description : XRay script callbacks inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CScriptCallback::CScriptCallback	() 
{	
	m_lua_function	= 0;
	m_lua_object	= 0;
}

IC	void CScriptCallback::set			(const luabind::functor<void> &lua_function)
{
	
	xr_delete		(m_lua_function);
	m_lua_function	= xr_new<luabind::functor<void> >(lua_function);

}

IC	void CScriptCallback::set			(const luabind::object &lua_object, LPCSTR method)
{
	xr_delete		(m_lua_object);
	m_lua_object	= xr_new<luabind::object>(lua_object);
	m_method_name	= method;
}

IC	void CScriptCallback::clear			()
{
	xr_delete		(m_lua_function);
	xr_delete		(m_lua_object);
}

IC	void CScriptCallback::clear			(bool member)
{
	(member) ? xr_delete(m_lua_object) : xr_delete(m_lua_function);
}

IC	luabind::functor<void> *CScriptCallback::get_function	()
{
	return m_lua_function;
}

IC	luabind::object	*CScriptCallback::get_object	()
{
	return m_lua_object;
}

IC	ref_str	CScriptCallback::get_method	()
{
	return m_method_name;
}

IC	void CScriptCallback::callback		()
{
	if (m_lua_function)
		(*m_lua_function)			();
	if (m_lua_object)
		luabind::call_member<void>	(*m_lua_object,*m_method_name);
}

IC	bool CScriptCallback::assigned		() const
{
	return				(m_lua_function || m_lua_object);
}
