////////////////////////////////////////////////////////////////////////////
//	Module 		: object_action_base_inline.h
//	Created 	: 12.03.2004
//  Modified 	: 12.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object base action inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CObjectActionBase::CObjectActionBase(CGameObject *item, CAI_Stalker *owner, LPCSTR action_name) :
	inherited			(owner,action_name)
{
	m_item				= item;
}
