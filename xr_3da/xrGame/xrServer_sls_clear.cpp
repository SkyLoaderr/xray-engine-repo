#include "stdafx.h"

void xrServer::SLS_Clear	()
{
	NET_Packet		P;
	u32				mode		= net_flags(TRUE,TRUE);

	// Collect entities
	xrS_entities::iterator	I=entities.begin(),E=entities.end();
	for (; I!=E; I++)
	{
		xrServerEntity*	E	= I->second;

		P.w_begin			(M_EVENT);
		P.w_u32				(Device.dwTimeGlobal);
		P.w_u16				(GE_DESTROY);
		P.w_u16				(E->ID);

		SendBroadcast		(0xffffffff,P,mode);
		entities.erase		(E->ID);
		entity_Destroy		(E);
	}
}
