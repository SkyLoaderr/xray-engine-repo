////////////////////////////////////////////////////////////////////////////
//	Module 		: object_action_aim.cpp
//	Created 	: 14.03.2004
//  Modified 	: 14.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object aim command
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_action_aim.h"

CObjectActionAim::CObjectActionAim	(CInventoryItem *item, CAI_Stalker *owner, u32 type, LPCSTR action_name) :
	inherited		(item,owner,action_name),
	m_type			(type)
{
}

void CObjectActionAim::initialize		()
{
}

void CObjectActionAim::execute			()
{
}

void CObjectActionAim::finalize		()
{
}
