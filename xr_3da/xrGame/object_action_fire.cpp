////////////////////////////////////////////////////////////////////////////
//	Module 		: object_action_fire.cpp
//	Created 	: 14.03.2004
//  Modified 	: 14.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object fire command
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_action_fire.h"

CObjectActionFire::CObjectActionFire	(CInventoryItem *item, CAI_Stalker *owner, u32 type, LPCSTR action_name) :
	inherited		(item,owner,action_name),
	m_type			(type)
{
}

void CObjectActionFire::initialize		()
{
}

void CObjectActionFire::execute			()
{
}

void CObjectActionFire::finalize		()
{
}
