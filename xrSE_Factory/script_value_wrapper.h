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
class CScriptValueWrapper : public CScriptValue {
private:
	typedef CScriptValue inherited;

private:
	_type			m_value;

public:
	IC				CScriptValueWrapper	(luabind::object &object, LPCSTR name);
	virtual	void	assign				();
	virtual	_type	*value				();
};

#include "script_value_wrapper_inline.h"