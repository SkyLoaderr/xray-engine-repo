////////////////////////////////////////////////////////////////////////////
//	Module 		: script_action.h
//	Created 	: 19.03.2004
//  Modified 	: 19.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script action
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "action_base.h"

class CScriptAction : public CActionBase<CLuaGameObject> {
protected:
	typedef CActionBase<CLuaGameObject> inherited;
public:
	IC							CScriptAction	(CLuaGameObject *object = 0, LPCSTR action_name = "") :
									inherited	(object,action_name)
	{
	}

	IC		void				add_condition	(COperatorCondition &condition)
	{
		inherited::add_condition(condition);
	}

	IC		void				remove_condition(COperatorCondition::_condition_type condition)
	{
		inherited::remove_condition(condition);
	}

	IC		void				add_effect		(COperatorCondition &effect)
	{
		inherited::add_effect	(effect);
	}

	IC		void				remove_effect	(COperatorCondition::_condition_type effect)
	{
		inherited::remove_effect(effect);
	}

	virtual	void				reinit			(CLuaGameObject *object, bool clear_all)
	{
		inherited::reinit		(object,clear_all);
	}

	virtual void				initialize		()
	{
		inherited::initialize	();
	}

	virtual void				execute			()
	{
		inherited::execute		();
	}

	virtual void				finalize		()
	{
		inherited::finalize		();
	}

	virtual _edge_value_type	weight			(const CSConditionState &condition0, const CSConditionState &condition1) const
	{
		return					(inherited::weight(condition0,condition1));
	}
};

class CScriptActionWrapper : public CScriptAction {
public:
	luabind::object	m_lua_instance;

					CScriptActionWrapper(const luabind::object &lua_instance, CLuaGameObject *object = 0, LPCSTR action_name = "") : 
						CScriptAction	(object,action_name),
						m_lua_instance	(lua_instance)
	{
	}

	virtual void	reinit				(CLuaGameObject *object, bool clear_all)
	{
		
		luabind::call_member<void>(m_lua_instance,"reinit",object,clear_all);
	}

	static	void	reinit_static		(CScriptAction *action, CLuaGameObject *object, bool clear_all)
	{
		action->CScriptAction::reinit(object,clear_all);
	}

	virtual void	initialize			()
	{
		luabind::call_member<void>(m_lua_instance,"initialize");
	}

	static	void	initialize_static	(CScriptAction *action)
	{
		action->CScriptAction::initialize();
	}

	virtual void	execute				()
	{
		luabind::call_member<void>(m_lua_instance,"execute");
	}

	static	void	execute_static		(CScriptAction *action)
	{
		action->CScriptAction::execute();
	}

	virtual void	finalize			()
	{
		luabind::call_member<void>(m_lua_instance,"finalize");
	}

	static	void	finalize_static		(CScriptAction *action)
	{
		action->CScriptAction::finalize();
	}

	virtual _edge_value_type	weight	(const CSConditionState &condition0, const CSConditionState &condition1)
	{
		return					(luabind::call_member<_edge_value_type>(m_lua_instance,"weight",condition0,condition1));
	}

	virtual _edge_value_type	weight	(const CSConditionState &condition0, const CSConditionState &condition1) const
	{
		return					(const_cast<CScriptActionWrapper*>(this)->weight(condition0,condition1));
	}

	static	_edge_value_type	weight_static	(CScriptAction *action, const CSConditionState &condition0, const CSConditionState &condition1)
	{
		return					(action->CScriptAction::weight(condition0,condition1));
	}
};