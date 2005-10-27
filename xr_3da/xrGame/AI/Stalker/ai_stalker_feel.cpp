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
#include "../../memory_manager.h"
#include "../../visual_memory_manager.h"
#include "../../sight_manager.h"
#include "../../stalker_movement_manager.h"

BOOL CAI_Stalker::feel_vision_isRelevant(CObject* O)
{
	if (!g_Alive())
		return		FALSE;
	CEntityAlive*	E = smart_cast<CEntityAlive*>		(O);
	CInventoryItem*	I = smart_cast<CInventoryItem*>	(O);
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
	sight().Exec_Look				(dt);
}

bool CAI_Stalker::bfCheckForNodeVisibility(u32 dwNodeID, bool bIfRayPick)
{
	return							(memory().visual().visible(dwNodeID,movement().m_head.current.yaw,ffGetFov()));
}

BOOL CAI_Stalker::feel_touch_contact	(CObject *O)
{
	CGameObject						*game_object = smart_cast<CGameObject*>(O);
	if (!game_object)
		return						(FALSE);

	return							(game_object->feel_touch_on_contact(this));
}

BOOL CAI_Stalker::feel_touch_on_contact	(CObject *O)
{
	if ((O->spatial.type | STYPE_VISIBLEFORAI) != O->spatial.type)
		return	(FALSE);

	return		(inherited::feel_touch_on_contact(O));
}
