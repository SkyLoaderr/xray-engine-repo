////////////////////////////////////////////////////////////////////////////
//	Module 		: script_value_wrapper.h
//	Created 	: 16.07.2004
//  Modified 	: 16.07.2004
//	Author		: Dmitriy Iassenev
//	Description : Script value wrapper
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_value.h"

template <typename _type>
class CScriptValueWrapperImpl : public CScriptValue {
private:
	typedef CScriptValue inherited;

protected:
	_type			m_value;

public:
	IC				CScriptValueWrapperImpl	(luabind::object object, LPCSTR name);
	virtual	void	assign					();
	virtual	_type	*value					();
};

template <typename _type>
class CScriptValueWrapper : public CScriptValueWrapperImpl<_type> {
private:
	typedef CScriptValueWrapperImpl<_type> inherited;

public:
	IC				CScriptValueWrapper		(luabind::object object, LPCSTR name);
};

template <>
class CScriptValueWrapperImpl<bool> : public CScriptValue {
private:
	typedef CScriptValue inherited;

protected:
	BOOL			m_value;

public:
	IC				CScriptValueWrapperImpl	(luabind::object object, LPCSTR name) : inherited(object,name)
	{
		m_value		= luabind::object_cast<bool>(object[name]);
	}

	virtual	void	assign					()
	{
		m_object[*m_name]	= !!m_value;
	}

	virtual	BOOL	*value					()
	{
		return		(&m_value);
	}
};

template <>
class CScriptValueWrapperImpl<ref_str> : public CScriptValue {
private:
	typedef CScriptValue inherited;

protected:
	ref_str			m_value;

public:
	IC				CScriptValueWrapperImpl	(luabind::object object, LPCSTR name) : inherited(object,name)
	{
		m_value		= luabind::object_cast<LPCSTR>(object[name]);
	}

	virtual	void	assign					()
	{
		if (!*m_value)
			m_value			= ref_str("");
		m_object[*m_name]	= *m_value;
	}

	virtual	ref_str	*value					()
	{
		return		(&m_value);
	}
};

#include "script_value_wrapper_inline.h"