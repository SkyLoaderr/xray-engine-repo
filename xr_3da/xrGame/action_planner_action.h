////////////////////////////////////////////////////////////////////////////
//	Module 		: action_planner_action.h
//	Created 	: 28.01.2004
//  Modified 	: 10.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Action planner action
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "action_base.h"
#include "action_planner.h"
#include "script_export_space.h"

class CScriptGameObject;

template <typename _object_type>
class CActionPlannerAction : 
	public CActionPlanner<_object_type>,
	public CActionBase<_object_type>
{
protected:
	typedef	CActionPlanner<_object_type>	inherited_planner;
	typedef	CActionBase<_object_type>		inherited_action;
	typedef typename inherited_action::COperatorCondition	COperatorCondition;
	typedef typename inherited_action::_edge_value_type		_edge_value_type;

public:
	IC					CActionPlannerAction	(_object_type *object = 0, LPCSTR action_name = "");
	virtual				~CActionPlannerAction	();
	IC		void		init					();
	virtual	void		Load					(LPCSTR section);
	virtual	void		reinit					(_object_type *object, CPropertyStorage *storage, bool clear_all = false);
	virtual	void		reload					(LPCSTR section);
	virtual	void		update					(u32 time_delta);
	virtual void		initialize				();
	virtual void		execute					();
	virtual void		finalize				();
	virtual bool		completed				() const;
	DECLARE_SCRIPT_REGISTER_FUNCTION
};
typedef CActionPlannerAction<CScriptGameObject> CScriptActionPlannerAction;
add_to_type_list(CScriptActionPlannerAction)
#undef script_type_list
#define script_type_list save_type_list(CScriptActionPlannerAction)

#include "action_planner_action_inline.h"