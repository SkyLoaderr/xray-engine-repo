#include "stdafx.h"

void xrServer::OnCL_Disconnected	(IClient* CL)
{
	Level().HUD()->outMessage(0xffffffff,"SERVER","Player '%s' disconnected",CL->Name);

	// Signal to everybody about disconnect
	P.w_begin		(M_PLIST_REMOVE);
	P.w_u32			(CL->ID);
	SendBroadcast	(0xffffffff,P,net_flags(TRUE,TRUE));

	// Collect entities
	svector<u16,256>	IDs;
	xrS_entities::iterator	I=entities.begin(),E=entities.end();
	for (; I!=E; I++)
	{
		xrServerEntity*	E	= I->second;
		if (E->owner == CL)	{
			IDs.push_back	(E->ID);
			entity_Destroy	(E);
		}
	}

	// Destroy from the map and create message
	NET_Packet			P;
	P.w_begin			(M_DESTROY);
	P.w_u16				(u16(IDs.size()));
	for (int i=0; i<int(IDs.size()); i++)
	{
		u16		ID			= IDs[i];
		P.w_u16				(ID);
		entities.erase		(ID);
	}

	// Send this to all other clients
	SendBroadcast		(CL->ID,P,net_flags(TRUE));
}

