////////////////////////////////////////////////////////////////////////////
//	Module 		: object_handler_goap_inline.h
//	Created 	: 11.03.2004
//  Modified 	: 11.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object handler using GOAP (inline functions)
////////////////////////////////////////////////////////////////////////////

#pragma once

//#include "object_action_base.h"

IC	bool CObjectHandlerGOAP::firing		() const
{
	return					(m_bFiring);
}

IC	CObjectHandlerGOAP::_condition_type CObjectHandlerGOAP::uid(const u32 id0, const u32 id1) const
{
	VERIFY				(!((id0 << 16) & id1));
	return				((id0 << 16) | id1);
}

IC	bool CObjectHandlerGOAP::object_action	(u32 action_id, CObject *object)
{
	return				((action_id >> 16) == object->ID());
}

IC	u32	CObjectHandlerGOAP::current_action_object_id	() const
{
	return				(action_object_id(current_action_id()));
}

IC	u32	CObjectHandlerGOAP::current_action_state_id	() const
{
	return				(action_state_id(current_action_id()));
}

IC	u32	CObjectHandlerGOAP::action_object_id		(_condition_type action_id) const
{
	return				(action_id >> 16);
}

IC	u32	CObjectHandlerGOAP::action_state_id			(_condition_type action_id) const
{
	return				(action_id & 0xffff);
}

IC	bool CObjectHandlerGOAP::goal_reached			() const
{
	return				(solution().size() < 2);
}

IC	void CObjectHandlerGOAP::add_condition			(CActionBase<CAI_Stalker> *action, u16 id, EWorldProperties property, _value_type value)
{
	action->add_condition(CWorldProperty(uid(id,property),value));
}

IC	void CObjectHandlerGOAP::add_effect				(CActionBase<CAI_Stalker> *action, u16 id, EWorldProperties property, _value_type value)
{
	action->add_effect	(CWorldProperty(uid(id,property),value));
}

IC	CObjectHandlerGOAP::EWorldProperties CObjectHandlerGOAP::object_property(MonsterSpace::EObjectAction object_action) const
{
	switch (object_action) {
		case eObjectActionSwitch1		: return(eWorldPropertySwitch1);
		case eObjectActionSwitch2		: return(eWorldPropertySwitch2);
		case eObjectActionAim1			: return(eWorldPropertyAiming1);
		case eObjectActionAim2			: return(eWorldPropertyAiming2);
		case eObjectActionFire1			: return(eWorldPropertyFiring1);
		case eObjectActionFire2			: return(eWorldPropertyFiring2);
		case eObjectActionIdle			: return(eWorldPropertyIdle);
		case eObjectActionStrapped		: return(eWorldPropertyIdleStrap);
		case eObjectActionDrop			: return(eWorldPropertyDropped);
		case eObjectActionActivate		: return(eWorldPropertyIdle);
		case eObjectActionDeactivate	: return(eWorldPropertyNoItemsIdle);
		default							: NODEFAULT;
	}
#ifdef DEBUG
	return	(eWorldPropertyDummy);
#endif
}

IC	void CObjectHandlerGOAP::set_aimed	(u32 type, bool value)
{
	(type ? m_aimed2 : m_aimed1) = value;
}
