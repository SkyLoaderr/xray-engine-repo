////////////////////////////////////////////////////////////////////////////
//	Module 		: object_action_base_inline.h
//	Created 	: 12.03.2004
//  Modified 	: 12.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object base action inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

template <typename _item_type>
IC	CObjectActionBase<_item_type>::CObjectActionBase(_item_type *item, CAI_Stalker *owner, LPCSTR action_name) :
	inherited			(owner,action_name)
{
	m_item				= item;
}
