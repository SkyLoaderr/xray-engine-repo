////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_feel.cpp
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Feelings for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "../../inventory_item.h"

BOOL CAI_Stalker::feel_vision_isRelevant(CObject* O)
{
	if (!g_Alive())
		return		FALSE;
	CEntityAlive*	E = dynamic_cast<CEntityAlive*>		(O);
	CInventoryItem*	I = dynamic_cast<CInventoryItem*>	(O);
	if (!E && !I)	return	(FALSE);
//	if (E && (E->g_Team() == g_Team()))			return FALSE;
	return(TRUE);
}

void CAI_Stalker::renderable_Render	()
{
	inherited::renderable_Render		();
	CInventoryOwner::renderable_Render	();
}

void CAI_Stalker::Exec_Look			(float dt)
{
	CSightManager::Exec_Look		(dt);

	// updating rotation matrix
	Fmatrix							mXFORM;
	mXFORM.setHPB					(-NET_Last.o_model,0,0);
	mXFORM.c.set					(Position());
	XFORM().set						(mXFORM);
}

bool CAI_Stalker::bfCheckForNodeVisibility(u32 dwNodeID, bool bIfRayPick)
{
	return				(CVisualMemoryManager::visible(dwNodeID,m_head.current.yaw,ffGetFov()));
}
