#include "stdafx.h"

void xrServer::Perform_destroy	(xrServerEntity* tpServerEntity, u32 mode)
{
	NET_Packet			P;
	P.w_begin			(M_EVENT);
	P.w_u32				(Device.dwTimeGlobal-NET_Latency*2);
	P.w_u16				(GE_DESTROY);
	P.w_u16				(tpServerEntity->ID);
	Msg					("*** SERVER-destroy: %s, ID=%d",tpServerEntity->s_name, tpServerEntity->ID);

	SendBroadcast		(0xffffffff,P,mode);
	entities.erase		(tpServerEntity->ID);
	entity_Destroy		(tpServerEntity);
}

void xrServer::SLS_Clear		()
{
	u32					mode	= net_flags(TRUE,TRUE);
	// Collect entities
	xrS_entities::iterator		I=entities.begin(),E=entities.end();
	for ( ; I!=E; I++)
		Perform_destroy			(I->second,mode);
}
