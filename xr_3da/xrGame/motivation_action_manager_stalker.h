////////////////////////////////////////////////////////////////////////////
//	Module 		: motivation_action_manager_stalker.h
//	Created 	: 26.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker motivation action manager class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "motivation_action_manager_script.h"
#include "action_script_base.h"
#include "action_planner_action_script.h"

class CAI_Stalker;

class CMotivationActionManagerStalker : public CMotivationActionManagerScript<CAI_Stalker> {
protected:
	typedef CMotivationActionManagerScript<CAI_Stalker>	inherited;
	typedef CActionScriptBase<CAI_Stalker>				CAction;
	typedef CGraphEngine::_solver_value_type			_value_type;
	typedef CGraphEngine::_solver_condition_type		_condition_type;
	typedef CActionPlannerActionScript<CAI_Stalker>		CActionPlannerAction;


protected:
#ifdef DEBUG
	EStalkerBehaviour		m_stalker_behaviour;
#endif

protected:
			void			add_evaluators						();
			void			add_actions							();
			void			add_motivations						();
#ifdef LOG_ACTION
	virtual LPCSTR			action2string						(const _action_id_type &action_id);
	virtual LPCSTR			property2string						(const _condition_type &property_id);
#endif

public:
							CMotivationActionManagerStalker		();
	virtual					~CMotivationActionManagerStalker	();
			void			init								();
	virtual	void			Load								(LPCSTR section);
	virtual	void			reinit								(CAI_Stalker *object, bool clear_all = false);
	virtual	void			reload								(LPCSTR section);
	virtual	void			update								(u32 time_delta);
#ifdef LOG_ACTION
	virtual	LPCSTR			object_name							() const;
#endif
	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CMotivationActionManagerStalker)
#undef script_type_list
#define script_type_list save_type_list(CMotivationActionManagerStalker)

#include "motivation_action_manager_stalker_inline.h"