#pragma once
#include "PHReqComparer.h"

class CPHScriptCondition:
	public CPHCondition,
	public CPHReqComparerV
{
	luabind::functor<bool>			*m_lua_function;

							CPHScriptCondition				(const CPHScriptCondition &func) {m_lua_function=func.m_lua_function;}	;

public:
							CPHScriptCondition				(const luabind::functor<bool> &func)	;
	virtual					~CPHScriptCondition				()										;
	virtual bool 			is_true							()										;
	virtual bool 			obsolete						()								const	;
	virtual bool			compare							(const	CPHReqComparerV* v)		const	{return v->compare(this);}
	virtual bool			compare							(const	CPHScriptCondition*v)	const	{return v->m_lua_function==m_lua_function;}
	///virtual bool			is_equal						(CPHReqBase* v)							;
	//virtual bool			is_relative						(CPHReqBase* v)							;

};

class CPHScriptAction :
	public CPHAction,
	public CPHReqComparerV
{
	bool	b_obsolete							   ;
	luabind::functor<void>			*m_lua_function;
public:
							CPHScriptAction					(const luabind::functor<void> &func)	;
	virtual					~CPHScriptAction				()										;
	virtual void 			run								()										;
	virtual bool 			obsolete						()								const	;
	virtual bool			compare							(const	CPHReqComparerV* v)		const	{return v->compare(this);}
	virtual bool			compare							(const	CPHScriptAction* v)		const	{return *m_lua_function==*(v->m_lua_function);}
};


class CPHScriptObjectCondition:
	public CPHCondition,
	public CPHReqComparerV
{
	luabind::object					*m_lua_object;
	shared_str						m_method_name;
public:
							CPHScriptObjectCondition		(const luabind::object &lua_object, LPCSTR method)	;
	virtual					~CPHScriptObjectCondition		()													;
	virtual bool 			is_true							()													;
	virtual bool 			obsolete						()										const		;
	virtual bool			compare							(const	luabind::object* v)				const		{return *m_lua_object==*v;}
	virtual bool			compare							(const	CPHReqComparerV* v)				const		{return v->compare(this);}
	virtual bool			compare							(const	CPHScriptObjectCondition* v)	const		{return m_method_name==v->m_method_name&&*m_lua_object==*(v->m_lua_object);}
};

class CPHScriptObjectAction :
	public CPHAction,
	public CPHReqComparerV
{
	bool	b_obsolete							   ;
	luabind::object					*m_lua_object;
	shared_str						m_method_name;
public:
							CPHScriptObjectAction			(const luabind::object &lua_object, LPCSTR method)	;
	virtual					~CPHScriptObjectAction			()													;
	virtual void 			run								()													;
	virtual bool 			obsolete						()										const		;
	virtual bool			compare							(const	luabind::object* v)				const		{return *m_lua_object==*v;}
	virtual bool			compare							(const	CPHReqComparerV* v)				const		{return v->compare(this);}
	virtual bool			compare							(const	CPHScriptObjectAction* v)		const		{return m_method_name==v->m_method_name&&*m_lua_object==*(v->m_lua_object);}
};
//////////////////////////////////////////////////////////////////////////////////////////

class CPHScriptObjectConditionN:
	public CPHCondition,
	public CPHReqComparerV
{
	CScriptCallbackEx<bool>			m_callback;
public:
	CPHScriptObjectConditionN			( const luabind::object &object,const luabind::functor<bool> &functor)	;
	virtual					~CPHScriptObjectConditionN		()													;
	virtual bool 			is_true							()													;
	virtual bool 			obsolete						()										const		;
	virtual bool			compare							(const	luabind::object* v)				const		{return m_callback==(*v);}
	virtual bool			compare							(const	CPHReqComparerV* v)				const		{return v->compare(this);}
	virtual bool			compare							(const	CPHScriptObjectConditionN* v)	const		{return m_callback==v->m_callback;}
};

class CPHScriptObjectActionN :
	public CPHAction,
	public CPHReqComparerV
{
	bool	b_obsolete							   ;
	CScriptCallbackEx<void>			m_callback;
public:
	CPHScriptObjectActionN			( const luabind::object &object,const luabind::functor<void> &functor);
	virtual					~CPHScriptObjectActionN			()													;
	virtual void 			run								()													;
	virtual bool 			obsolete						()									const			;
	virtual bool			compare							(const	luabind::object* v)			const			{return m_callback==*v;}
	virtual bool			compare							(const	CPHReqComparerV* v)			const			{return v->compare(this);}
	virtual bool			compare							(const	CPHScriptObjectActionN* v)	const			{return m_callback==v->m_callback;}
};

class CPHSriptReqObjComparer :
	public CPHReqComparerV
{
	luabind::object					*m_lua_object;

public:
								CPHSriptReqObjComparer	(const luabind::object & lua_object)			{m_lua_object= xr_new<luabind::object>(lua_object);}
		virtual		bool		compare					(const	CPHScriptObjectCondition* v)	const	{return v->compare(m_lua_object);}
		virtual		bool		compare					(const	CPHScriptObjectAction* v)		const	{return v->compare(m_lua_object);}
		virtual		bool		compare					(const	CPHScriptObjectConditionN* v)	const	{return v->compare(m_lua_object);}
		virtual		bool		compare					(const	CPHScriptObjectActionN* v)		const	{return v->compare(m_lua_object);}
};