////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_feel.cpp
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Feelings for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "../ai_monsters_misc.h"
#include "../../hudmanager.h"
#include "../../actor.h"

//#undef SILENCE

BOOL CAI_Stalker::feel_vision_isRelevant(CObject* O)
{
	CEntityAlive*	E = dynamic_cast<CEntityAlive*>		(O);
	CInventoryItem*	I = dynamic_cast<CInventoryItem*>	(O);
	if (!E && !I)	return	(FALSE);
	if (E && (E->g_Team() == g_Team()))			return FALSE;
	return(TRUE);
}

void CAI_Stalker::renderable_Render	()
{
	inherited::renderable_Render	();
	if(m_inventory.ActiveItem())
		m_inventory.ActiveItem()->renderable_Render();
}

void CAI_Stalker::Exec_Look(float dt)
{
	CSightManager::Exec_Look		(dt);

	// updating rotation matrix
	Fmatrix							mXFORM;
	mXFORM.setHPB					(-NET_Last.o_model,0,0);
	mXFORM.c.set					(Position());
	XFORM().set						(mXFORM);
}
