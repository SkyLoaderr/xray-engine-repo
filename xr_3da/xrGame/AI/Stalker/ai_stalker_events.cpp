////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_events.cpp
//	Created 	: 26.02.2003
//  Modified 	: 26.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Events handling for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "../../weapon.h"
#include "../../targetcs.h"
#include "../../customdetector.h"
#include "../../bolt.h"
#include "../../pda.h"

IC BOOL BE	(BOOL A, BOOL B)
{
	bool a = !!A;
	bool b = !!B;
	return a==b;
}

void CAI_Stalker::OnEvent		(NET_Packet& P, u16 type)
{
	inherited::OnEvent		(P,type);

	u16 id;
	switch (type)
	{
	case GE_TRADE_BUY:
	case GE_OWNERSHIP_TAKE:
		{
			P.r_u16		(id);
			CObject* O	= Level().Objects.net_Find	(id);

#ifndef SILENCE
			Msg("Trying to take - %s (%d)", O->cName(),O->ID());
#endif
			if (m_inventory.Take(dynamic_cast<CGameObject*>(O),true)) {
				O->H_SetParent(this);
#ifndef SILENCE
				Msg("TAKE - %s (%d)", O->cName(),O->ID());
#endif
			}
			else {
				DropItemSendMessage(O);
#ifndef SILENCE
				Msg("TAKE - can't take! - Dropping for valid server information %s (%d)", O->cName(),O->ID());
#endif
			}
		}
		break;
	}
}

void CAI_Stalker::feel_touch_new				(CObject* O)
{
	if (!g_Alive())		return;
	if (Remote())		return;

	// Now, test for game specific logical objects to minimize traffic
	CInventoryItem		*I	= dynamic_cast<CInventoryItem*>	(O);

	if (I && I->Useful()) {
#ifndef SILENCE
		Msg("Taking item %s (%d)!",I->cName(),I->ID());
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
void CAI_Stalker::ReceivePdaMessage(u16 who, EPdaMsg msg, int info_index)
{
	EPdaMsg pda_msg = ePdaMsgAccept;

	if(GetPDA()->NeedToAnswer(who))
	{
		switch(msg)
		{
		case ePdaMsgTrade:
			pda_msg = ePdaMsgAccept;
			break;
		case ePdaMsgNeedHelp:
			pda_msg = ePdaMsgDecline;
			break;
		case ePdaMsgGoAway:
			pda_msg = ePdaMsgDeclineRude;
			break;
		}

		//реакция на сообщение
		SendPdaMessage(who, pda_msg, info_index);
	}
}