////////////////////////////////////////////////////////////////////////////
//	Module 		: action_planner_action_script.h
//	Created 	: 07.07.2004
//  Modified 	: 07.07.2004
//	Author		: Dmitriy Iassenev
//	Description : Action planner action with script support
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "action_planner_action.h"
//#include "script_game_object.h"

class CScriptGameObject;

template <typename _object_type>
class CActionPlannerActionScript : public CScriptActionPlannerAction {
protected:
	typedef CScriptActionPlannerAction inherited;

public:
	_object_type	*m_object;

public:
	IC				CActionPlannerActionScript	(const xr_vector<COperatorCondition> &conditions, const xr_vector<COperatorCondition> &effects, _object_type *object = 0, LPCSTR action_name = "");
	IC				CActionPlannerActionScript	(_object_type *object = 0, LPCSTR action_name = "");
	virtual			~CActionPlannerActionScript	();
	virtual	void	reinit						(_object_type *object, CPropertyStorage *storage, bool clear_all);
	virtual	void	reinit						(CScriptGameObject *object, CPropertyStorage *storage, bool clear_all);
};

#include "action_planner_action_script_inline.h"