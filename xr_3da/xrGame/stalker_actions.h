////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_actions.h
//	Created 	: 25.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker action classes
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "action_base.h"

class CAI_Stalker;

//////////////////////////////////////////////////////////////////////////
// CStalkerActionBase
//////////////////////////////////////////////////////////////////////////

class CStalkerActionBase : public CActionBase<CAI_Stalker> {
protected:
	typedef CActionBase<CAI_Stalker>				inherited;
	typedef CGraphEngine::CSolverConditionStorage	CConditionStorage;
	typedef CGraphEngine::_solver_condition_type	_condition_type;
	typedef CGraphEngine::_solver_value_type		_value_type;

protected:
	CConditionStorage	*m_storage;

public:
						CStalkerActionBase	(CAI_Stalker *object, CConditionStorage *storage, LPCSTR action_name = "");
	IC		void		set_property		(_condition_type condition_id, _value_type value);
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDead
//////////////////////////////////////////////////////////////////////////

class CStalkerActionDead : public CStalkerActionBase {
protected:
	typedef CStalkerActionBase inherited;

public:
						CStalkerActionDead	(CAI_Stalker *object, CConditionStorage *storage, LPCSTR action_name = "");
	virtual void		execute				();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionFreeNoALife
//////////////////////////////////////////////////////////////////////////

class CStalkerActionFreeNoALife : public CStalkerActionBase {
protected:
	typedef CStalkerActionBase inherited;

public:
						CStalkerActionFreeNoALife	(CAI_Stalker *object, CConditionStorage *storage, LPCSTR action_name = "");
	virtual void		execute						();
};

#include "stalker_actions_inline.h"