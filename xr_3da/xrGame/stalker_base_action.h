////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_base_action.h
//	Created 	: 25.03.2004
//  Modified 	: 27.09.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker base action
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
						CStalkerActionBase			(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize					();
	virtual void		finalize					();
};
