////////////////////////////////////////////////////////////////////////////
//	Module 		: object_actions_inline.h
//	Created 	: 12.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object actions inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_handler_goap.h"

//////////////////////////////////////////////////////////////////////////
// CObjectActionBase
//////////////////////////////////////////////////////////////////////////

template <typename _item_type>
IC	CObjectActionBase<_item_type>::CObjectActionBase(_item_type *item, CAI_Stalker *owner, CConditionStorage *storage, LPCSTR action_name) :
	inherited			(owner,action_name),
	m_storage			(storage),
	m_item				(item)
{
}

template <typename _item_type>
IC	void CObjectActionBase<_item_type>::set_property	(_condition_type condition_id, _value_type value)
{
	VERIFY						(m_storage);
	CConditionStorage::iterator	I = m_storage->find(condition_id);
	VERIFY						(m_storage->end() != I);
	(*I).second					= value;
}

template <typename _item_type>
void CObjectActionBase<_item_type>::initialize	()
{
	inherited::initialize	();
	set_property			(CObjectHandlerGOAP::eWorldPropertyAimed1,false);
	set_property			(CObjectHandlerGOAP::eWorldPropertyAimed2,false);
}

//////////////////////////////////////////////////////////////////////////
// CObjectActionMember
//////////////////////////////////////////////////////////////////////////

template <typename _item_type>
IC	CObjectActionMember<_item_type>::CObjectActionMember	(_item_type *item, CAI_Stalker *owner, CConditionStorage *storage, _condition_type condition_id, _value_type value, LPCSTR action_name) :
	inherited			(item,owner,storage,action_name),
	m_condition_id		(condition_id),
	m_value				(value)
{
}

template <typename _item_type>
void CObjectActionMember<_item_type>::execute			()
{
	if (completed())
		set_property		(m_condition_id,m_value);
}
