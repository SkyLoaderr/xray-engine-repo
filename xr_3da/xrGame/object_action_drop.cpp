////////////////////////////////////////////////////////////////////////////
//	Module 		: object_action_drop.cpp
//	Created 	: 14.03.2004
//  Modified 	: 14.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object drop command
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_action_drop.h"

CObjectActionDrop::CObjectActionDrop	(CInventoryItem *item, CAI_Stalker *owner, LPCSTR action_name) :
	inherited		(item,owner,action_name)
{
}

void CObjectActionDrop::initialize		()
{
}

void CObjectActionDrop::execute			()
{
}

void CObjectActionDrop::finalize		()
{
}
