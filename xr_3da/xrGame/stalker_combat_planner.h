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

private:
	CCoverPoint			*m_last_cover;
	u32					m_last_level_time;
	u16					m_last_enemy_id;

protected:
			void		add_evaluators			();
			void		add_actions				();
	IC		void		update_cover			();
			void		react_on_grenades		();
			void		react_on_member_death	();

public:
						CStalkerCombatPlanner	(CAI_Stalker *object = 0, LPCSTR action_name = "");
	virtual				~CStalkerCombatPlanner	();
	virtual	void		setup					(CAI_Stalker *object, CPropertyStorage *storage);
	virtual void		update					();
	virtual void		initialize				();
	virtual void		finalize				();

public:
	IC		void		last_cover				(CCoverPoint *last_cover);
	IC		CCoverPoint	*last_cover				() const;
};

#include "stalker_combat_planner_inline.h"