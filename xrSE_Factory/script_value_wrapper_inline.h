////////////////////////////////////////////////////////////////////////////
//	Module 		: script_value_wrapper_inline.h
//	Created 	: 16.07.2004
//  Modified 	: 16.07.2004
//	Author		: Dmitriy Iassenev
//	Description : Script value wrapper inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

template <typename _type>
IC	CScriptValueWrapper<_type>::CScriptValueWrapper	(luabind::object &object, LPCSTR name) :
	inherited		(object,name)
{
}

template <typename _type>
virtual	void CScriptValueWrapper<_type>::assign		()
{
	m_object[m_name] = m_value;
}

template <typename _type>
virtual	_type *CScriptValueWrapper<_type>::value		()
{
	return			(&m_value);
}
