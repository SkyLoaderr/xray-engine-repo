////////////////////////////////////////////////////////////////////////////
//	Module 		: script_action_planner_wrapper.h
//	Created 	: 19.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script action planner wrapper
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "action_planner.h"
#include "script_space.h"

class CLuaGameObject;

typedef CActionPlanner<CLuaGameObject> CScriptActionPlanner;

class CScriptActionPlannerWrapper : public CScriptActionPlanner, public luabind::wrap_base {
public:
	virtual void		reinit						(CLuaGameObject *object, bool clear_all);
	static	void		reinit_static				(CScriptActionPlanner *planner, CLuaGameObject *object, bool clear_all);
	virtual void		update						(u32 time_delta);
	static	void		update_static				(CScriptActionPlanner *planner, u32 time_delta);
};

#include "script_action_planner_wrapper_inline.h"