////////////////////////////////////////////////////////////////////////////
//	Module 		: script_action_planner_action_wrapper.cpp
//	Created 	: 29.03.2004
//  Modified 	: 29.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script action planner action wrapper
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_action_planner_action_wrapper.h"

void CScriptActionPlannerActionWrapper::reinit		(CLuaGameObject *object, CPropertyStorage *storage, bool clear_all)
{
	luabind::call_member<void>				(m_lua_instance,"reinit",object,storage,clear_all);
}

void CScriptActionPlannerActionWrapper::reinit_static	(CScriptActionPlannerAction *planner, CLuaGameObject *object, CPropertyStorage *storage, bool clear_all)
{
	planner->CScriptActionPlannerAction::reinit	(object,storage,clear_all);
}
