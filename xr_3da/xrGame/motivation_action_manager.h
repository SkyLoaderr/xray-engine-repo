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

template <typename _object_type>
class CMotivationActionManager :
	public CMotivationManager<CMotivation,_object_type>,
	public CActionPlanner<_object_type>
{
protected:
	typedef CMotivationManager<CMotivation,_object_type>	CSMotivationManager;
	typedef CActionPlanner<_object_type>					CSActionPlanner;
	typedef CMotivationAction<_object_type>					CSMotivationAction;

public:
	virtual			~CMotivationActionManager	();
	virtual void	reinit						(_object_type *object, bool clear_all = false);
	virtual void	Load						(LPCSTR section);
	virtual void	reload						(LPCSTR section);
	virtual void	update						(u32 time_delta);
};

#include "motivation_action_manager_inline.h"