////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_events.cpp
//	Created 	: 26.02.2003
//  Modified 	: 26.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Events handling for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "../../pda.h"
#include "../../inventory.h"
#include "../../xrmessages.h"
#include "../../shootingobject.h"

#define SILENCE

void CAI_Stalker::OnEvent		(NET_Packet& P, u16 type)
{
	inherited::OnEvent			(P,type);
	CInventoryOwner::OnEvent	(P,type);

	switch (type)
	{
		case GE_TRADE_BUY :
		case GE_OWNERSHIP_TAKE : {

			u16 id;
			P.r_u16		(id);
			CObject* O	= Level().Objects.net_Find	(id);

			//. hack
			if (!O)
				return;

#ifndef SILENCE
			Msg("Trying to take - %s (%d)", *O->cName(),O->ID());
#endif
			if (inventory().Take(dynamic_cast<CGameObject*>(O),true, false)) { //GetScriptControl()
				O->H_SetParent(this);
				if (!inventory().ActiveItem() && GetScriptControl() && dynamic_cast<CShootingObject*>(O))
					CObjectHandler::set_goal	(eObjectActionIdle,dynamic_cast<CGameObject*>(O));

#ifndef SILENCE
				Msg("TAKE - %s (%d)", *O->cName(),O->ID());
#endif
			}
			else {
//				DropItemSendMessage(O);
				NET_Packet				P;
				u_EventGen				(P,GE_OWNERSHIP_REJECT,ID());
				P.w_u16					(u16(O->ID()));
				u_EventSend				(P);

#ifndef SILENCE
				Msg("TAKE - can't take! - Dropping for valid server information %s (%d)", *O->cName(),O->ID());
#endif
			}
			break;
		}
		case GE_TRADE_SELL :
		case GE_OWNERSHIP_REJECT : {
			u16 id;
			P.r_u16		(id);
			CObject* O	= Level().Objects.net_Find	(id);

			//. hack
			if (!O)
				return;

			if (inventory().Drop(dynamic_cast<CGameObject*>(O)) && !O->getDestroy()) {
				O->H_SetParent	(0);
				feel_touch_deny	(O,2000);
			}
			break;
		}
	}
}

void CAI_Stalker::feel_touch_new				(CObject* O)
{
	if (!g_Alive())		return;
	if (Remote())		return;
	if ((O->spatial.type | STYPE_VISIBLEFORAI) != O->spatial.type) return;

	// Now, test for game specific logical objects to minimize traffic
	CInventoryItem		*I	= dynamic_cast<CInventoryItem*>	(O);

	if (I && I->Useful()) {
#ifndef SILENCE
		Msg("Taking item %s (%d)!",*I->cName(),I->ID());
#endif
		NET_Packet		P;
		u_EventGen		(P,GE_OWNERSHIP_TAKE,ID());
		P.w_u16			(u16(I->ID()));
		u_EventSend		(P);
	}
}

void CAI_Stalker::DropItemSendMessage(CObject *O)
{
	if (!O || !O->H_Parent() || (this != O->H_Parent()))
		return;

#ifndef SILENCE
	Msg("Dropping item!");
#endif
	// We doesn't have similar weapon - pick up it
	NET_Packet				P;
	u_EventGen				(P,GE_OWNERSHIP_REJECT,ID());
	P.w_u16					(u16(O->ID()));
	u_EventSend				(P);
}

/////////////////////////
//PDA functions
void CAI_Stalker::ReceivePdaMessage(u16 who, EPdaMsg msg, INFO_INDEX info_index)
{
	CInventoryOwner::ReceivePdaMessage(who, msg, info_index);
}