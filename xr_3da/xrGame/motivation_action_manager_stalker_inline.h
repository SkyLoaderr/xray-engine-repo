////////////////////////////////////////////////////////////////////////////
//	Module 		: motivation_action_manager_stalker_inline.h
//	Created 	: 26.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker motivation action manager class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	void CMotivationActionManagerStalker::add_condition	(CWorldState &goal, _condition_type condition_id, _value_type value)
{
	goal.add_condition		(CWorldProperty(condition_id,value));
}
