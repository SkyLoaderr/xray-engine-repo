////////////////////////////////////////////////////////////////////////////
//	Module 		: object_handler_goap_inline.h
//	Created 	: 11.03.2004
//  Modified 	: 11.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object handler using GOAP (inline functions)
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	bool CObjectHandlerGOAP::firing		() const
{
	return					(m_bFiring);
}

IC	u32 CObjectHandlerGOAP::uid(const u32 id0, const u32 id1) const
{
	VERIFY				(!((id0 << 16) & id1));
	return				((id0 << 16) | id1);
}

IC	bool CObjectHandlerGOAP::object_action	(u32 action_id, CObject *object)
{
	return				((action_id & 0xffff) == object->ID());
}

IC	u32	CObjectHandlerGOAP::current_action_object_id	() const
{
	return				(current_action_id() & 0xffff);
}

IC	u32	CObjectHandlerGOAP::current_action_state_id	() const
{
	return				(current_action_id() >> 16);
}

IC	bool CObjectHandlerGOAP::goal_reached			() const
{
	return				(solution().size() < 2);
}
