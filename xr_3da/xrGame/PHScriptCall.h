#pragma once

class CPHScriptCondition:
	public CPHCondition
{
	luabind::functor<bool>			*m_lua_function;
public:
							CPHScriptCondition				(const luabind::functor<bool> &func)	;
	virtual					~CPHScriptCondition				()										;
	virtual bool 			is_true							()										;
	virtual bool 			obsolete						()										;

};

class CPHScriptAction :
	public CPHAction
{
	bool	b_obsolete							   ;
	luabind::functor<void>			*m_lua_function;
public:
							CPHScriptAction					(const luabind::functor<void> &func)	;
	virtual					~CPHScriptAction				()										;
	virtual void 			run								()										;
	virtual bool 			obsolete						()										;
};


class CPHScriptObjectCondition:
	public CPHCondition
{
	luabind::object					*m_lua_object;
	shared_str						m_method_name;
public:
							CPHScriptObjectCondition		(const luabind::object &lua_object, LPCSTR method)	;
	virtual					~CPHScriptObjectCondition		()													;
	virtual bool 			is_true							()													;
	virtual bool 			obsolete						()													;

};

class CPHScriptObjectAction :
	public CPHAction
{
	bool	b_obsolete							   ;
	luabind::object					*m_lua_object;
	shared_str						m_method_name;
public:
							CPHScriptObjectAction			(const luabind::object &lua_object, LPCSTR method)	;
	virtual					~CPHScriptObjectAction			()													;
	virtual void 			run								()													;
	virtual bool 			obsolete						()													;
};

