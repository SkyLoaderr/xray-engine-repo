////////////////////////////////////////////////////////////////////////////
//	Module 		: script_action_planner_action_wrapper.h
//	Created 	: 29.03.2004
//  Modified 	: 29.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script action planner action wrapper
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "action_planner_action.h"
#include "script_game_object.h"

class CScriptActionPlannerActionWrapper : public CScriptActionPlannerAction, public luabind::wrap_base {
public:
	virtual void			reinit								(CScriptGameObject *object, CPropertyStorage *storage, bool clear_all = false);
	static	void			reinit_static						(CScriptActionPlannerAction *planner, CScriptGameObject *object, CPropertyStorage *storage, bool clear_all = false);
};

#include "script_action_planner_action_wrapper_inline.h"