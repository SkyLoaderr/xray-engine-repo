////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_combat_planner.h
//	Created 	: 25.03.2004
//  Modified 	: 27.09.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker combat planner
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "action_planner_action_script.h"

class CAI_Stalker;

class CStalkerCombatPlanner : public CActionPlannerActionScript<CAI_Stalker> {
private:
	typedef CActionPlannerActionScript<CAI_Stalker> inherited;

public:
						CStalkerCombatPlanner	(CAI_Stalker *object = 0, LPCSTR action_name = "");
	virtual				~CStalkerCombatPlanner	();
	virtual	void		reinit					(CAI_Stalker *object, CPropertyStorage *storage, bool clear_all = false);
	virtual	void		reload					(LPCSTR section);
			void		add_evaluators			();
			void		add_actions				();
};
