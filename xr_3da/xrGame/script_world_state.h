////////////////////////////////////////////////////////////////////////////
//	Module 		: script_world_state.h
//	Created 	: 19.03.2004
//  Modified 	: 19.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script world state
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_world_property.h"

class CScriptWorldState : public CGraphEngine::CWorldState {
protected:
	typedef CGraphEngine::CWorldState inherited;
public:
	IC	const COperatorCondition	*property			(COperatorCondition::_condition_type condition) const;
	IC	void						remove_condition	(COperatorCondition::_condition_type condition);
	IC	bool						includes			(inherited &condition) const;
	IC	void						clear				();
};

#include "script_world_state_inline.h"