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

class CAI_Stalker;

class CMotivationActionManagerStalker : public CMotivationActionManagerScript<CAI_Stalker> {
protected:
	typedef CMotivationActionManagerScript<CAI_Stalker>	inherited;
	typedef CActionScriptBase<CAI_Stalker>				CAction;
	typedef CGraphEngine::_solver_value_type			_value_type;
	typedef CGraphEngine::_solver_condition_type		_condition_type;

	using inherited::add_condition;

protected:
	IC		void			add_condition						(CWorldState &goal, _condition_type condition_id, _value_type value);
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
};

#include "motivation_action_manager_stalker_inline.h"