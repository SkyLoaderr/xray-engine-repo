////////////////////////////////////////////////////////////////////////////
//	Module 		: motivation_action_manager.h
//	Created 	: 26.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Motivation action manager class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "motivation_manager.h"
#include "action_planner.h"
#include "motivation.h"
#include "motivation_action.h"

template <
	typename _object_type,
	template <typename _object_type> class _motivation_type = CMotivation,
	template <typename _object_type> class _motivation_action_type = CMotivationAction
>
class CMotivationActionManager :
	public CMotivationManager<_object_type,_motivation_type>,
	public CActionPlanner<_object_type>
{
protected:
	typedef CMotivationManager<_object_type,_motivation_type>	CSMotivationManager;
	typedef CActionPlanner<_object_type>						CSActionPlanner;
	typedef CMotivationAction<_object_type>						CSMotivationAction;

	using CSActionPlanner::add_condition;

protected:
	IC		void	add_condition				(CWorldState &goal, _condition_type condition_id, _value_type value);

public:
	IC				CMotivationActionManager	();
	virtual			~CMotivationActionManager	();
	virtual void	reinit						(_object_type *object, bool clear_all = false);
	virtual void	Load						(LPCSTR section);
	virtual void	reload						(LPCSTR section);
	virtual void	update						(u32 time_delta);
	virtual void	clear						();
	IC		void	clear_motivations			();
	IC		void	clear_actions				();
};

#include "motivation_action_manager_inline.h"