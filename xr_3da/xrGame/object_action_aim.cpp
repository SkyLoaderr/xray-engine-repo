////////////////////////////////////////////////////////////////////////////
//	Module 		: object_action_aim.cpp
//	Created 	: 14.03.2004
//  Modified 	: 14.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object aim command
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_action_aim.h"
#include "ai/stalker/ai_stalker.h"

CObjectActionAim::CObjectActionAim	(CInventoryItem *item, CAI_Stalker *owner, bool *condition, LPCSTR action_name) :
	inherited		(item,owner,action_name),
	m_condition		(condition)
{
}

void CObjectActionAim::initialize		()
{
	m_start_time	= Level().timeServer();
}

void CObjectActionAim::execute			()
{
	if (Level().timeServer() - m_start_time > m_inertia_time)
		*m_condition = true;
}
