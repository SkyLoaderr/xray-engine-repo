#include "stdafx.h"
#include "hudmanager.h"


void xrServer::OnCL_Disconnected	(IClient* CL)
{
	Level().HUD()->outMessage(0xffffffff,"SERVER","Player '%s' disconnected",CL->Name);

	NET_Packet			P;

	// Signal to everybody about disconnect
	P.w_begin			(M_PLIST_REMOVE);
	P.w_u32				(CL->ID);
	SendBroadcast		(CL->ID,P,net_flags(TRUE,TRUE));

	// Collect entities
	xrS_entities::iterator	I=entities.begin(),E=entities.end();
	for (; I!=E; I++)
	{
		xrServerEntity*	E	= I->second;
		if (E->owner == CL)	{
			P.w_begin			(M_EVENT);
			P.w_u32				(Device.dwTimeGlobal);
			P.w_u16				(GE_DESTROY);
			P.w_u16				(E->ID);

			SendBroadcast		(CL->ID,P,net_flags(TRUE,TRUE));
			entities.erase		(E->ID);
			entity_Destroy		(E);
		}
	}
}
