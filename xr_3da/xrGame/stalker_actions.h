////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_actions.h
//	Created 	: 25.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker action classes
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "action_script_base.h"

class CAI_Stalker;

//////////////////////////////////////////////////////////////////////////
// CStalkerActionBase
//////////////////////////////////////////////////////////////////////////

class CStalkerActionBase : public CActionScriptBase<CAI_Stalker> {
protected:
	typedef CActionScriptBase<CAI_Stalker>			inherited;
	typedef CGraphEngine::_solver_condition_type	_condition_type;
	typedef CGraphEngine::_solver_value_type		_value_type;

public:
						CStalkerActionBase	(CAI_Stalker *object, LPCSTR action_name = "");
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDead
//////////////////////////////////////////////////////////////////////////

class CStalkerActionDead : public CStalkerActionBase {
protected:
	typedef CStalkerActionBase inherited;

public:
						CStalkerActionDead	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		execute				();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionFreeNoALife
//////////////////////////////////////////////////////////////////////////

class CStalkerActionFreeNoALife : public CStalkerActionBase {
protected:
	typedef CStalkerActionBase inherited;

public:
						CStalkerActionFreeNoALife	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		execute						();
};

#include "stalker_actions_inline.h"