#include "stdafx.h"

void xrServer::Perform_destroy	(CSE_Abstract* tpSE_Abstract, u32 mode)
{
	NET_Packet			P;
	P.w_begin			(M_EVENT);
	P.w_u32				(Device.dwTimeGlobal-NET_Latency*2);
	P.w_u16				(GE_DESTROY);
	P.w_u16				(tpSE_Abstract->ID);
	//Msg					("*** SERVER-destroy: %s, ID=%d",tpSE_Abstract->s_name, tpSE_Abstract->ID);

	SendBroadcast		(0xffffffff,P,mode);
	entities.erase		(tpSE_Abstract->ID);
	entity_Destroy		(tpSE_Abstract);
}

void xrServer::SLS_Clear		()
{
	// Don't do anything if game in ALife mode
	if (game->type == GAME_SINGLE) {
		game_sv_Single *tpGame	= dynamic_cast<game_sv_Single*>(game);
		if (tpGame && tpGame->m_tpALife)
			return;
	}

	// Collect entities
	u32					mode	= net_flags(TRUE,TRUE);
	xrS_entities::iterator		I=entities.begin(),E=entities.end();
	for ( ; I!=E; I++)
		Perform_destroy			(I->second,mode);
}
