////////////////////////////////////////////////////////////////////////////
//	Module 		: object_action_show.cpp
//	Created 	: 14.03.2004
//  Modified 	: 14.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object show command
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_action_show.h"

CObjectActionShow::CObjectActionShow	(CInventoryItem *item, CAI_Stalker *owner, LPCSTR action_name) :
	inherited		(item,owner,action_name)
{
}

void CObjectActionShow::initialize		()
{
}

void CObjectActionShow::execute			()
{
}

void CObjectActionShow::finalize		()
{
}
